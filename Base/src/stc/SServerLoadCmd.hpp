#ifndef SSERVER_LOAD_CMD_HPP_
#define SSERVER_LOAD_CMD_HPP_
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #4 $ 
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

#include "ServerToClientCmd.hpp"

///================================================================================
/// Paired with CtsCmd::SERVER_LOAD
/// Client---(CtsCmd::SERVER_LOAD)---->Server-----(SServerLoadCmd)--->client:
///================================================================================
class SServerLoadCmd : public ServerToClientCmd {
public:
   explicit SServerLoadCmd(const std::string& log_file_path) : log_file_path_(log_file_path) {}
   SServerLoadCmd() : ServerToClientCmd() {}

   void init(const std::string& s) { log_file_path_ = s;}
   const std::string& log_file_path() const { return log_file_path_; }
   virtual std::ostream& print(std::ostream& os) const;
   virtual bool equals(ServerToClientCmd*) const;
   virtual bool handle_server_response( ServerReply& server_reply, Cmd_ptr cts_cmd, bool debug ) const;

private:
   std::string log_file_path_;

   friend class boost::serialization::access;
   template<class Archive>
   void serialize( Archive & ar, const unsigned int /*version*/ ) {
      ar & boost::serialization::base_object< ServerToClientCmd >( *this );
      ar & log_file_path_;
   }
};

std::ostream& operator<<(std::ostream& os, const SServerLoadCmd&);

#endif
