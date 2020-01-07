#ifndef MISC_ATTRS_HPP_
#define MISC_ATTRS_HPP_
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #235 $
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
#include <ostream>

#include <boost/noncopyable.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>         // no need to include <vector>

#include "ZombieAttr.hpp"
#include "VerifyAttr.hpp"
#include "NodeFwd.hpp"

class MiscAttrs : private boost::noncopyable {
public:
   explicit MiscAttrs(Node* node) : node_(node) {}
   MiscAttrs(const MiscAttrs& rhs) : node_(NULL),zombies_(rhs.zombies_),verifys_(rhs.verifys_) {}
   MiscAttrs() : node_(NULL) {}

   // needed by node serialisation
   void set_node(Node* n) { node_ = n; }

   void begin();

   // standard functions: ==============================================
   std::ostream& print(std::ostream&) const;
   bool operator==(const MiscAttrs& ) const;

   // Access functions: ======================================================
   const std::vector<VerifyAttr>&      verifys()  const { return verifys_;}
   const std::vector<ZombieAttr>&      zombies()  const { return zombies_; }

   // Add functions: ===============================================================
   void addVerify( const VerifyAttr& );  // for testing and verification Can throw std::runtime_error
   void addZombie( const ZombieAttr& );  // will throw std::runtime_error if duplicate

   // Delete functions: can throw std::runtime_error ===================================
   // if name argument is empty, delete all attributes of that type
   // Can throw std::runtime_error of the attribute can not be found
   void delete_zombie(const ecf::Child::ZombieType);
   void deleteZombie(const std::string& type); // string must be one of [ user | ecf | path ]

   // Change functions: ================================================================
   /// returns true the change was made else false, Can throw std::runtime_error for parse errors

   // Find functions: ============================================================
   bool findVerify(const VerifyAttr& ) const;
   const ZombieAttr& findZombie( ecf::Child::ZombieType ) const;

   void verification(std::string& errorMsg) const;

   bool empty() const { return (zombies_.empty() && verifys_.empty()) ? true : false; }

private:
   std::vector<ZombieAttr>::const_iterator zombie_begin() const { return zombies_.begin();}
   std::vector<ZombieAttr>::const_iterator zombie_end() const { return zombies_.end();}
   std::vector<VerifyAttr>::const_iterator verify_begin() const { return verifys_.begin();}
   std::vector<VerifyAttr>::const_iterator verify_end() const { return verifys_.end();}

private:
   Node*        node_; // *NOT* persisted must be set by the parent class
   friend class Node;

private:
   std::vector<ZombieAttr> zombies_;
   std::vector<VerifyAttr> verifys_;     // used for statistics and test verification

private:
   friend class boost::serialization::access;
   template<class Archive>
   void serialize(Archive & ar, const unsigned int /*version*/) {
      ar & zombies_;
      ar & verifys_;
   }
};
#endif
