#ifndef CHILD_HPP_
#define CHILD_HPP_
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #5 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description : Specifies the different kinds of child commands
//               These are specified in the job file, and communicate with the server
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "User.hpp"

namespace ecf {

class Child : private boost::noncopyable {
public:
	enum CmdType    { INIT, EVENT, METER, LABEL, WAIT, ABORT, COMPLETE };

	enum ZombieType {
	   USER,
	   ECF,
	   PATH ,
	   NOT_SET };

	static std::string to_string(ZombieType);
	static bool valid_zombie_type( const std::string& );
	static ZombieType zombie_type( const std::string&);

	static std::string to_string(const std::vector<Child::CmdType>&);
	static std::string to_string( Child::CmdType );
	static std::vector<Child::CmdType> child_cmds(const std::string&);
	static Child::CmdType child_cmd(const std::string&);

	/// Expect a , separated string
	static bool valid_child_cmds( const std::string& );
	static bool valid_child_cmd( const std::string& );

   static std::vector<Child::CmdType> list();

private:
	Child();
};
}
#endif
