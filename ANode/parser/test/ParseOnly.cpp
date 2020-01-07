//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision$
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
// Description :
//============================================================================
#include <string>
#include <iostream>
#include <fstream>

#include "Defs.hpp"
#include "PrintStyle.hpp"

using namespace std;
using namespace ecf;

int main(int argc, char* argv[])
{
//   cout << "argc = " << argc << "\n";
//   for(int i = 0; i < argc; i++) {
//      cout << "arg " << i << ":" << argv[i] << "\n";
//   }

   if (argc != 2) {
      cout << "Expect single argument which is path to a defs file\n";
      return 1;
   }

   std::string path = argv[1];

   Defs defs;
   std::string errorMsg,warningMsg;
   if (!defs.restore(path,errorMsg,warningMsg)) {
      cout << errorMsg << "\n";
      cout << warningMsg << "\n";
      return 1;
   }

//   PrintStyle style(PrintStyle::MIGRATE);
//   cout << defs;
   return 0;
}
