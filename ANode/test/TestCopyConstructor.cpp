/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #10 $
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
#include <stdlib.h>
#include <iostream>
#include <boost/test/unit_test.hpp>

#include "MyDefsFixture.hpp"
#include "Ecf.hpp"

using namespace std;
using namespace ecf;

BOOST_AUTO_TEST_SUITE( NodeTestSuite )

BOOST_AUTO_TEST_CASE( test_copy_constructors )
{
   cout << "ANode:: ...test_copy_constructors\n";
   MyDefsFixture theDefsFixture;

   DebugEquality debug_equality; // only as affect in DEBUG build
   Defs copy = Defs(theDefsFixture.defsfile_);
   BOOST_CHECK_MESSAGE( copy == theDefsFixture.defsfile_,"copy constructor failed");
}

BOOST_AUTO_TEST_SUITE_END()
