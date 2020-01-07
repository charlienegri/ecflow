#ifndef DEFS_DELTA_HPP_
#define DEFS_DELTA_HPP_
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #23 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
// class DefsDelta: holds a list of incremental changes made in the server.
//                  This class is created in the server, and transferred to the client
//                  The client use it to syncronize with the server, without the need
//                  for asking for the full defs.
//
// When the client request the incremental changes, it also passes its client_state_change_no.
// The client_state_change number is used in the server side to determine what's changed.
// These changes are collated with add()  and transferred to the client.
// The client then calls  incremental_sync() which will apply the changes to the client defs
// so bringing it in sync with the server defs.
//
// Note:: updating state_change_no() on the *client side*  a no-op() it has no effect
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <boost/serialization/level.hpp>
#include <boost/serialization/tracking.hpp>
#include "Memento.hpp"

class DefsDelta : private boost::noncopyable {
public:
   ///=========================================================================
	/// *Server side*
   explicit DefsDelta(unsigned int client_state_change_no)
	: client_state_change_no_(client_state_change_no),
	  server_state_change_no_(0),
	  server_modify_change_no_(0) {}

	/// This class can be re-used hence init() should reset all data members
 	void init(unsigned int client_state_change_no);

	/// Add the compound memento, ie. store all memento's for a *given* node.
 	void add(compound_memento_ptr);

   void set_server_state_change_no( unsigned int s) { server_state_change_no_ =  s ; }
   void set_server_modify_change_no( unsigned int s) { server_modify_change_no_ =  s ; }
   unsigned int get_server_state_change_no() const  { return server_state_change_no_;  }
   unsigned int get_server_modify_change_no() const { return server_modify_change_no_; }

 	///=========================================================================
 	/// *Client side*
	/// Applies the mementos to the client defs and record all changed nodes.
   /// Can raise std::runtime_error.
 	/// Note:: updating state_change_no() on the *client side*  has *no effect*
	bool incremental_sync(defs_ptr client_def, std::vector<std::string>& changed_nodes) const;


	/// =========================================================================
	// The client state change number. Used to determine what's changed
	unsigned int client_state_change_no() const { return  client_state_change_no_;}

	/// return the number of compound mementos
	size_t size() const { return compound_mementos_.size(); }

private:
	unsigned int client_state_change_no_;  // *no* need to persist since only used on server side

   unsigned int server_state_change_no_;
   unsigned int server_modify_change_no_;
	std::vector<compound_memento_ptr> compound_mementos_;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive & ar, const unsigned int /*version*/ ) {
      ar & server_state_change_no_;
      ar & server_modify_change_no_;
   	ar & compound_mementos_;
  	}
};

// This should ONLY be added to objects that are *NOT* serialised through a pointer
BOOST_CLASS_IMPLEMENTATION(DefsDelta, boost::serialization::object_serializable)
BOOST_CLASS_TRACKING(DefsDelta,boost::serialization::track_never);

#endif
