/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #65 $
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
#include <iostream>
#include <sstream>
#include <fstream>

#include <sys/types.h>
#include <stdio.h>      /* tolower */

#include "ClientToServerCmd.hpp"

#include "AbstractServer.hpp"
#include "AbstractClientEnv.hpp"
#include "Log.hpp"
#include "Str.hpp"
#include "User.hpp"

using namespace std;
using namespace boost;
using namespace ecf;

bool UserCmd::equals(ClientToServerCmd* rhs) const
{
   UserCmd* the_rhs = dynamic_cast< UserCmd* > ( rhs );
   if ( !the_rhs ) return false;
   return user_ == the_rhs->user();
}

bool UserCmd::authenticate(AbstractServer* as, STC_Cmd_ptr& cmd) const
{
   // The user should NOT be empty. Rather than asserting and killing the server, fail authentication
   // ECFLOW-577 and ECFLOW-512. When user_ empty ??
   if (!user_.empty() && as->authenticateReadAccess(user_,passwd_)) {

      // Does this user command require write access
      if ( isWrite() ) {
         // command requires write access. Check user has write access
         if ( as->authenticateWriteAccess(user_) ) {
            return true;
         }
         std::string msg = "[ authentication failed ] User ";
         msg += user_;
         msg += " has no *write* access. Please see your administrator.";
         throw std::runtime_error( msg );
      }
      else {
         // read request, and we have read access
         return true;
      }
   }

   std::string msg = "[ authentication failed ] User '";
   msg += user_;
   msg += "' is not allowed any access.";
   throw std::runtime_error( msg );

   return false;
}

bool UserCmd::do_authenticate(AbstractServer* as, STC_Cmd_ptr&, const std::string& path) const
{
   if (!user_.empty() && as->authenticateReadAccess(user_,passwd_,path)) {

      // Does this user command require write access
      if ( isWrite() ) {
         // command requires write access. Check user has write access, add access to suite/node/path
         if ( as->authenticateWriteAccess(user_,path) ) {
            return true;
         }
         std::string msg = "[ authentication failed ] User ";
         msg += user_;
         msg += " has no *write* access. path(";msg += path; msg += ")Please see your administrator.";
         throw std::runtime_error( msg );
      }
      else {
         // read request, and we have read access
         return true;
      }
   }

   std::string msg = "[ authentication failed ] User '";
   msg += user_;
   msg += "' is not allowed any access. path(";
   msg += path;
   msg += ")";
   throw std::runtime_error( msg );

   return false;
}

bool UserCmd::do_authenticate(AbstractServer* as, STC_Cmd_ptr&, const std::vector<std::string>& paths) const
{
   if (!user_.empty() && as->authenticateReadAccess(user_,passwd_,paths)) {

      // Does this user command require write access
      if ( isWrite() ) {
         // command requires write access. Check user has write access
         if ( as->authenticateWriteAccess(user_,paths) ) {
            return true;
         }
         std::string msg = "[ authentication failed ] User ";
         msg += user_;
         msg += " has no *write* access to paths(";
         for(size_t i=0; i < paths.size(); ++i) { msg += paths[i];msg += ",";}
         msg += ") Please see your administrator.";
         throw std::runtime_error( msg );
      }
      else {
         // read request, and we have read access
         return true;
      }
   }

   std::string msg = "[ authentication failed ] User '";
   msg += user_;
   msg += "' is not allowed any access. paths(";
   for(size_t i=0; i < paths.size(); ++i) { msg += paths[i];msg += ",";}
   msg += ")";
   throw std::runtime_error( msg );

   return false;
}

void UserCmd::setup_user_authentification(const std::string& user, const std::string& passwd)
{
   user_ = user;
   passwd_ = passwd;
   assert(!user_.empty());
}

void UserCmd::setup_user_authentification(AbstractClientEnv& clientEnv)
{
   setup_user_authentification(User::login_name(),clientEnv.get_user_password());
}

void UserCmd::setup_user_authentification()
{
   if (user_.empty())  user_ = User::login_name();
   assert(!user_.empty());
}

void UserCmd::prompt_for_confirmation(const std::string& prompt)
{
   cout << prompt;
   char reply[256];
   cin.getline (reply,256);
   if (reply[0] != 'y' && reply[0] != 'Y') {
      exit(1);
   }
}

std::ostream& UserCmd::user_cmd(std::ostream& os, const std::string& the_cmd) const
{
   return os << the_cmd << " :" << user_;
}

//#define DEBUG_ME 1

void UserCmd::split_args_to_options_and_paths(
          const std::vector<std::string>& args,
          std::vector<std::string>& options,
          std::vector<std::string>& paths,
          bool treat_colon_in_path_as_path)
{
   // ** ECFLOW-137 **, if the trigger expression have a leading '/' then it gets parsed into the paths
   //                   vector and not options
   // This is because boost program options does *NOT* seem to preserve the leading quotes around the
   // trigger/complete expression,  i.e "/suite/t2 == complete"   is read as /suite/t2 == complete
   // However in paths we do not expect to see any spaces

   // Note: Node names are not allowed ':', hence ':' in a node path is always to reference a node attribute, event,limit in this case
   // However we need to deal with special situations:
   //   o --alter add inlimit /path/to/node/withlimit:limit_name 10  /s1  # TREAT /path/to/node/withlimit:limit_name as a OPTION
   //   o --change trigger "/suite/task:a" /path/to/a/node                # TREAT "/suite/task:a" as a OPTION
   //   o --force=clear /suite/task:ev                                    # TREAT /suite/task:ev as a PATH
   // We need to distinguish between the two, hence use treat_colon_in_path_as_path
   size_t vec_size = args.size();
   if (treat_colon_in_path_as_path ) {
      for(size_t i = 0; i < vec_size; i++) {
         if (args[i].empty()) continue;
         if (args[i][0] == '/' && args[i].find(" ") == std::string::npos) {
            // --force=clear /suite/task:ev  -> treat '/suite/task:ev' as a path
            paths.push_back(args[i]);
         }
         else {
            options.push_back(args[i]);
         }
      }
   }
   else {
      // Treat ':' is path as a option, TREAT '/path/to/node/withlimit:limit_name' as a option
      for(size_t i = 0; i < vec_size; i++) {
         if (args[i].empty()) continue;
         if (args[i][0] == '/' && args[i].find(" ") == std::string::npos && args[i].find(":") == std::string::npos) {
            paths.push_back(args[i]);
         }
         else {
            options.push_back(args[i]);
         }
      }
   }
#ifdef DEBUG_ME
   std::cout << "split_args_to_options_and_paths\n";
   for(size_t i = 0; i < args.size(); ++i) { std::cout << "args[" << i << "]=" <<args[i] << "\n"; }
   for(size_t i = 0; i < options.size(); ++i) { std::cout << "options[" << i << "]=" << options[i] << "\n"; }
   for(size_t i = 0; i < paths.size(); ++i) { std::cout << "paths[" << i << "]=" << paths[i] << "\n"; }
#endif
}

int UserCmd::time_out_for_load_sync_and_get() { return 600; }

