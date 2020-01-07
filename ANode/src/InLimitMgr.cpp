/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #28 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <assert.h>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

#include "InLimitMgr.hpp"
#include "Limit.hpp"
#include "Node.hpp"
#include "Memento.hpp"
#include "Ecf.hpp"
#include "Str.hpp"
#include "Extract.hpp"

using namespace ecf;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////

InLimitMgr& InLimitMgr::operator=(const InLimitMgr& rhs)
{
   if (this != &rhs) {
      inLimitVec_ = rhs.inLimitVec_;
      node_ = NULL;
   }
   return *this;
}

std::ostream& InLimitMgr::print(std::ostream& os) const
{
	BOOST_FOREACH(const InLimit& i, inLimitVec_)     { i.print(os); }
 	return os;
}

bool InLimitMgr::operator==(const InLimitMgr& rhs) const
{
	if (inLimitVec_.size() != rhs.inLimitVec_.size()) {
#ifdef DEBUG
		if (Ecf::debug_equality()) {
			std::cout << "InLimitMgr::operator==  inLimitVec_.size() != rhs.inLimitVec_.size() " << node_->debugNodePath() << "\n";
		}
#endif
		return false;
	}
	for(size_t i = 0; i < inLimitVec_.size(); ++i) {
 		if (!(inLimitVec_[i] == rhs.inLimitVec_[i] )) {
#ifdef DEBUG
			if (Ecf::debug_equality()) {
				std::cout << "InLimitMgr::operator==  (!(inLimitVec_[i] == rhs.inLimitVec_[i] )) " << node_->debugNodePath() << "\n";
			}
#endif
			return false;
		}
	}
	return true;
}

void InLimitMgr::addInLimit(const InLimit& l )
{
	if (!findInLimitByNameAndPath(l)) {
		inLimitVec_.push_back( l );
		return;
	}
 	throw std::runtime_error( "Add InLimit failed: Duplicate InLimit see node " + node_->debugNodePath() );
}

bool InLimitMgr::deleteInlimit(const std::string& name)
{
   //cout << "InLimitMgr::deleteInlimit: " << name << "\n";
   if (name.empty()) {
      inLimitVec_.clear();
      return true;
   }

   string path_to_limit; // This can be empty
   string limit_name;
   (void)Extract::pathAndName( name, path_to_limit, limit_name  ); // already checked for empty name
   //cout << "   path_to_limit:" << path_to_limit << "\n";
   //cout << "   limit_name:" <<  limit_name << "\n";


   for(size_t i = 0; i < inLimitVec_.size(); i++) {
      //cout << "   " << i << ": " << inLimitVec_[i].pathToNode() << "  :  " << inLimitVec_[i].name() << "\n";
      if (path_to_limit.empty()) {
         if (inLimitVec_[i].name() == limit_name ) {
            inLimitVec_.erase( inLimitVec_.begin() + i );
            return true;
         }
      }
      else {
         if (inLimitVec_[i].name() == limit_name && inLimitVec_[i].pathToNode() == path_to_limit) {
            inLimitVec_.erase( inLimitVec_.begin() + i );
            return true;
         }
      }
   }
   throw std::runtime_error("InLimitMgr::deleteInlimit: Can not find inlimit: " + name);
}

Limit* InLimitMgr::findLimitViaInLimit(const InLimit& theInLimit) const
{
   // Use in *test* only
	size_t theSize = inLimitVec_.size();
	for(size_t i = 0; i < theSize; i++) {
 		if (inLimitVec_[i].name() == theInLimit.name() && inLimitVec_[i].pathToNode() == theInLimit.pathToNode()) {
 			resolveInLimit(inLimitVec_[i]);
 			return inLimitVec_[i].limit() ;
		}
 	}
	return NULL;
}

bool InLimitMgr::findInLimitByNameAndPath(const InLimit& theInLimit) const
{
	size_t theSize = inLimitVec_.size();
	for(size_t i = 0; i < theSize; i++) {
 		if (inLimitVec_[i].name() == theInLimit.name() && inLimitVec_[i].pathToNode() == theInLimit.pathToNode()) {
 			return true;
		}
 	}
	return false;
}

void InLimitMgr::get_memento( compound_memento_ptr& comp) const
{
#ifdef DEBUG_MEMENTO
	std::cout << "InLimitMgr::get_memento " << node_->debugNodePath() << "\n";
#endif

 	BOOST_FOREACH(const InLimit& l, inLimitVec_ )  { comp->add( boost::make_shared<NodeInLimitMemento>(  l) ); }
}


bool InLimitMgr::inLimit() const
{
	// Check in we are in limit.
	// ** WE need to do a lookahead. hence we pass down inlimit tokens **
	// In the case we have multiple inlimits
	// then we are only in limit if _ALL_ are in limit. This is like
	// a logical AND.
 	if (!inLimitVec_.empty()) {

 		resolveInLimitReferences();

 		int inlimitsWithLimits = 0;
		int inlimitCount = 0;
		size_t theSize = inLimitVec_.size();
		for(size_t i = 0; i < theSize; i++ ) {
 			Limit* limit = inLimitVec_[i].limit();
			if (limit) {
				inlimitsWithLimits++;
				if (limit->inLimit( inLimitVec_[i].tokens() )) {
					inlimitCount++;
 				}
			}
		}

		return  (inlimitsWithLimits == inlimitCount ) ;
	}

 	return true;
}

void InLimitMgr::incrementInLimit( std::set<Limit*>& limitSet,const std::string& task_path) const
{
//	cout << "InLimitMgr::incrementInLimit " << node_->absNodePath() << endl;

	// *NOTE* each limit is incremented if within LIMIT, and that
	//        has not previously been updated.
	//  we could have the same in limit at the task and family level.
	//  in this case the task takes priority.
	//  suite suite
	//    family family
	//       inlimit limitname 12
	//       task t1
 	//          inlimit limitname 4
	//    endfamily
	//  endsuite
	//
	// In this case the limit <limitname> is incremented by 4 _only_
	//
	// Note: It is illegal for a node to have the same inlimit but with
	//       different tokens:
	//
	//       task t1
 	//          inlimit limitname 4
 	//          inlimit limitname 2    // illegal and trapped by parser

	resolveInLimitReferences();

	BOOST_FOREACH(const InLimit& inlimit, inLimitVec_) {
		Limit* limit = inlimit.limit();
		if (limit && limitSet.find(limit) == limitSet.end()) {
			limitSet.insert(limit);
 			// cout << "InLimitMgr::incrementInLimit " << node_->absNodePath() << " LIMIT incremented " << endl;
			limit->increment( inlimit.tokens(), task_path);
		}
	}
}

void InLimitMgr::decrementInLimit( std::set<Limit*>& limitSet,const std::string& task_path) const
{
	// *NOTE* each limit is incremented if within LIMIT, and that
	//        has not previously been updated.
	//  we could have the same in limit at the task and family level.
	//  in this case the task takes priority.
	//  suite suite
	//    family family
	//       inlimit limitname 12
	//       task t1
 	//          inlimit limitname 4
	//    endfamily
	//  endsuite
	//
	// In this case the limit <limitname> is incremented by 4 _only_
	//
	// Note: It is illegal for a node to have the same inlimit but with
	//       different tokens:
	//
	//       task t1
 	//          inlimit limitname 4
 	//          inlimit limitname 2    // illegal and trapped by parser
	resolveInLimitReferences();

	BOOST_FOREACH(const InLimit& inlimit, inLimitVec_) {
		Limit* limit = inlimit.limit();
		if (limit && limitSet.find(limit) == limitSet.end()) {
			limitSet.insert(limit);
 			// cout << "InLimitMgr::incrementInLimit " << node_->absNodePath() << " LIMIT incremented " << endl;
			limit->decrement( inlimit.tokens(), task_path);
		}
	}
}


//#define DEBUG_WHY 1

static void add_consumed_paths(Limit* limit, std::stringstream& ss)
{
   ss << "(";
   const std::set<std::string>& consumed_paths = limit->paths();
   int count = 0;
   for (std::set<std::string>::const_iterator i = consumed_paths.begin(); i!=consumed_paths.end(); ++i) {
      if ( 4 == count) { ss << "..."; break; }
      ss << (*i) << ",";
      count++;
   }
   ss << ")";
}

bool InLimitMgr::why(std::vector<std::string>& vec, bool html) const
{
   // Note: if this correspond to a leaf node, like a task. Then it may not be
   // sufficient to just check in limits at this level  *BUT*
   // Node::top_down_why() and Node::bottom_up_why() will traverse the hierarchy
   // so we don't need to replicate it here.

#ifdef DEBUG_WHY
	std::cout << "InLimitMgr::why " << node_->debugNodePath() << " top_down:" << top_down << "\n";
#endif
	bool why_found = false;
 	if (!inLimit()){
#ifdef DEBUG_WHY
 		std::cout << "   InLimitMgr::why " << node_->debugNodePath() << " NOT in limit\n";
#endif
  		for(size_t i = 0; i < inLimitVec_.size(); i++ ) {
	 		Limit* limit = inLimitVec_[i].limit();
			if (limit &&  !limit->inLimit(inLimitVec_[i].tokens())) {
				std::stringstream ss;
				if (  inLimitVec_[i].pathToNode().empty()) {
					ss << "limit " << limit->name() << " is full";
				}
				else {
				   if (html) {
				      std::stringstream s;
				      s << "[limit]" << inLimitVec_[i].pathToNode() << Str::COLON() << limit->name();
				      ss << Node::path_href_attribute(s.str()) << " is full";
				   }
				   else ss << "limit " <<  inLimitVec_[i].pathToNode() << Str::COLON() << limit->name() << " is full";
				}

            // show node paths that have consumed a limit, Only show first 5, Otherwise string may be too long
            add_consumed_paths(limit,ss);

				vec.push_back(ss.str());
            why_found = true;
			}
		}
	}
 	return why_found;
}

void InLimitMgr::check(std::string& errorMsg, std::string& warningMsg,bool reportErrors, bool reportWarnings) const
{
   size_t theSize = inLimitVec_.size();
   for(size_t i = 0; i < theSize; i++) {
      (void)find_limit(inLimitVec_[i], errorMsg, warningMsg,  reportErrors,   reportWarnings) ;
   }
}

void InLimitMgr::resolveInLimit(InLimit& inLimit,std::string& errorMsg, std::string& warningMsg,bool reportErrors, bool reportWarnings) const
{
//		cout << "Inlimit " << inLimit.toString() << "\n";

	/// if limit pointer already setup use them. These are shared ptr backed, Hence if deleted beneath use should return 0;
	if (inLimit.limit()) {
//		cout << "InLimitMgr::resolveInLimit " << inLimit.toString() << " Reusing limit ptr \n";
		return;
	}

	/// Find the limit referenced by the InLimit. i.e. Link inLimit to its LIMIT
	/// The return value can be NULL
	limit_ptr referencedLimit = find_limit(inLimit,errorMsg,warningMsg,reportErrors,reportWarnings);
	inLimit.limit(  referencedLimit );
}

void InLimitMgr::auto_add_inlimit_externs(Defs* defs) const
{
   std::string errorMsg;
   std::string warningMsg;
   size_t theSize = inLimitVec_.size();
   for(size_t i = 0; i < theSize; i++) {
      limit_ptr referencedLimit = find_limit(inLimitVec_[i],errorMsg,warningMsg,false,false);
      if (!referencedLimit.get()) {
         if (inLimitVec_[i].pathToNode().empty()) defs->add_extern( inLimitVec_[i].name() );
         else                                     defs->add_extern( inLimitVec_[i].pathToNode() + ":" + inLimitVec_[i].name());
      }
   }
}

limit_ptr InLimitMgr::find_limit(const InLimit& inLimit, std::string& errorMsg, std::string& warningMsg,bool reportErrors, bool reportWarnings) const
{
   if (inLimit.pathToNode().empty()) {

      // cout << "inLimit.pathToNode().empty() search " << debugType() << " " << node_->absNodePath() << "\n";
      limit_ptr referencedLimit = node_->findLimitUpNodeTree( inLimit.name() );
      if ( referencedLimit.get() )  return referencedLimit;

      if (reportWarnings) {

         // See if the name is defined, as an extern, in which case *DONT* warn:
         // This is client side specific, since server does not have externs.
         if (node_->defs()->find_extern(inLimit.name(),Str::EMPTY())) {
            return referencedLimit; // this is empty/NULL
         }

         std::stringstream ss;
         ss << "Warning: ";
         ss << node_->debugType() << " " << node_->absNodePath() << " has a " << inLimit.toString() << ", which can not be found on the parent nodes\n";
         warningMsg += ss.str();
      }
      return referencedLimit; // this is empty/NULL
   }

   // *FIND* the node referenced by the In-Limit, this should hold the Limit.
   // cout << "Inlimit path not empty \n";
   string warning_message;
   node_ptr referenceNode = node_->findReferencedNode( inLimit.pathToNode(), inLimit.name(), warning_message);
   if (!referenceNode.get()) {
       /// Could not find the node which *HOLDS* the limit
       if (reportWarnings) {

          // OK a little bit of duplication, since findReferencedNode, will also look for externs
          // See if the Path:name is defined as an extern, in which case *DONT* warn:
          // This is client side specific, since server does not have externs.
          if (node_->defs()->find_extern(inLimit.pathToNode(),inLimit.name())) {
             return limit_ptr();
          }

          std::stringstream ss;
          ss << "Warning: " << node_->debugType() << " " << node_->absNodePath() << " has a " << inLimit.toString() << ", which can not be found\n";
          warningMsg += ss.str();
       }
       return limit_ptr();
   }

   // *FOUND* the node which should hold the Limit.
   limit_ptr referencedLimit = referenceNode->find_limit( inLimit.name() );
   if (!referencedLimit.get()) {

      // See if the name is defined, as an extern, in which case *DONT* warn:
      // This is client side specific, since server does not have externs.
      if (node_->defs()->find_extern(inLimit.pathToNode(),inLimit.name())) {
         return limit_ptr();
      }

      if (reportWarnings) {
         std::stringstream ss;
         ss << node_->debugType() << " " << node_->absNodePath() << " has a " << inLimit.toString() << " :";
         ss << "The referenced " << referenceNode->debugType() << " '" << referenceNode->absNodePath() << "' does not define the limit " << inLimit.name() << "\n";
         warning_message += ss.str();
         warningMsg += "Warning: ";
         warningMsg +=  warning_message;
         warningMsg += "\n";
      }
      return referencedLimit; // this is empty/NULL
   }

   // *FOUND* the referenced LIMIT. inlimit tokens must be less than limit.
   // ECFLOW-713 make this a warning and not error, it allows job control over multiple suites, at load/begin time
   if ( inLimit.tokens() > referencedLimit->theLimit() ) {
      if (reportWarnings) {
         // in limit exceeds the LIMIT value
         std::stringstream ss; ss << "Warning: ";
         ss << node_->debugType() << " " << node_->absNodePath() << " has a " << inLimit.toString() << " reference\n";
         ss << " with value '" << inLimit.tokens() << "' which exceeds '" << referencedLimit->theLimit() << "' defined on the Limit\n";
         warningMsg += ss.str();
      }
   }
   return referencedLimit;
}

void InLimitMgr::resolveInLimit(InLimit& inLimit) const
{
   // Used in *test* only
	std::string errorMsg;
	std::string warningMsg;
	resolveInLimit(inLimit, errorMsg, warningMsg, false, false );
}

void InLimitMgr::resolveInLimitReferences() const
{
   size_t theSize = inLimitVec_.size();
   if (theSize > 0) {
      std::string warningMsg;
      std::string errorMsg;
      for(size_t i = 0; i < theSize; i++) {
         resolveInLimit(inLimitVec_[i], errorMsg, warningMsg,  false,   false) ;
      }
   }
}

