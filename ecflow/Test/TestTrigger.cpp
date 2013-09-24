//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #29 $ 
//
// Copyright 2009-2012 ECMWF. 
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
//============================================================================
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>

#include "ServerTestHarness.hpp"

#include "Defs.hpp"
#include "Suite.hpp"
#include "Family.hpp"
#include "Task.hpp"
#include "DurationTimer.hpp"
#include "Str.hpp"


using namespace std;
using namespace ecf;
namespace fs = boost::filesystem;

BOOST_AUTO_TEST_SUITE( TestSuite )

// In the test case we will dynamically create all the test data.
// The data is created dynamically so that we can stress test the server
// This test does not have any time dependencies in the def file.
BOOST_AUTO_TEST_CASE( test_triggers_and_meters )
{
	DurationTimer timer;
	cout << "Test:: ...test_triggers_and_meters " << flush;
   TestClean clean_at_start_and_end;

 	//# Note: we have to use relative paths, since these tests are relocatable
	//suite test_triggers_and_meters
	//	edit SLEEPTIME 1
	//	edit ECF_INCLUDE $ECF_HOME/includes
	//	family family
	//   	task model
	//        	meter file 0 100 100
	//      task t0
	//          trigger model:file ge 10
	//      task t1
	//          trigger model:file ge 20
	//      task t2
	//          trigger model:file ge 30
	//      ....
 	//  	endfamily
	//endsuite
	int taskSize = 9; // on linux 1024 tasks take ~4 seconds for job submission
	std::string meterName = "file";
	std::string taskName = "model";
  	Defs theDefs;
 	{
		std::auto_ptr< Suite > suite( new Suite( "test_triggers_and_meters" ) );
 		std::auto_ptr< Family > fam( new Family( "family" ) );
		std::auto_ptr< Task > taskModel( new Task(taskName ));
		taskModel->addMeter( Meter(meterName,0,100,100) ); // ServerTestHarness will add correct ecf
 		fam->addTask( taskModel );
		fam->addVerify( VerifyAttr(NState::COMPLETE,1) );

  		for(int i=0; i < taskSize; i++) {
  			std::auto_ptr< Task > task( new Task( "t" + boost::lexical_cast<std::string>(i*10 + 10) ) );
  			task->addVerify( VerifyAttr(NState::COMPLETE,1) );
   		task->add_trigger(  taskName + Str::COLON() + meterName + " ge " + boost::lexical_cast<std::string>(i*10 + 10) );
  			fam->addTask( task );
 		}
 		suite->addFamily( fam );
		suite->addVerify( VerifyAttr(NState::COMPLETE,1) );
		theDefs.addSuite( suite );
 	}

 	// The test harness will create corresponding directory structure & default ecf file
   ServerTestHarness serverTestHarness(false/*do log file verification*/,false/* dont do standard verification */);
	serverTestHarness.run(theDefs,ServerTestHarness::testDataDefsLocation( "test_triggers_and_meters.def"));

	cout << timer.duration() << " update-calendar-count(" << serverTestHarness.serverUpdateCalendarCount() << ")\n";
}

BOOST_AUTO_TEST_SUITE_END()
