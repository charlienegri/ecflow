/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// Name        :
// Author      : Avi
// Revision    : $Revision: #14 $
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
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

#include "Task.hpp"
#include "Family.hpp"
#include "Suite.hpp"
#include "Defs.hpp"
#include "Jobs.hpp"
#include "JobsParam.hpp"
#include "CalendarUpdateParams.hpp"

using namespace std;
using namespace ecf;
using namespace boost::posix_time;
using namespace boost::gregorian;

BOOST_AUTO_TEST_SUITE( NodeTestSuite )

BOOST_AUTO_TEST_CASE( test_day_time_combination )
{
   cout << "ANode:: ...test_day_time_combination\n";
   // See ECFLOW-337 , this is where the job was being run twice in a week instead of once.
   //                  i.e because the day was still free past midnight.

   // Create the suite, starting on a sunday
   Defs  defs;
   suite_ptr suite = defs.add_suite("s1");
   boost::posix_time::ptime the_time = boost::posix_time::ptime(date(2015,6,7),time_duration(0,0,0)); //sunday
   suite->addClock( ClockAttr(the_time) );
   task_ptr t1 = suite->add_task("t1");
   t1->addDay( DayAttr(DayAttr::MONDAY) );
   t1->addTime( ecf::TimeAttr(ecf::TimeSlot(10,0)) );

   defs.beginAll();

   CalendarUpdateParams calUpdateParams( hours(1) );
   boost::posix_time::ptime expected_time = boost::posix_time::ptime(date(2015,6,8),time_duration(10,0,0)); //Monday & 10
   //cout << "expected_time =  " << expected_time << "\n";

   int submitted = 0;
   for(int m=1; m < 120; m++) {  // run for 5 days

      Jobs jobs(&defs);
      JobsParam jobsParam;
      jobs.generate(jobsParam);

      if (jobsParam.submitted().size() ) {
         submitted++;
         //cout << "submitted at " << suite->calendar().suiteTime() << "\n";

         BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time,"\nExpected to submit at " << expected_time << " only, but also found " << suite->calendar().suiteTime());

         Node::Requeue_args args(true /* reset repeats*/,
                                  0    /* clear_suspended_in_child_nodes */,
                                  true /* reset_next_time_slot_ */,
                                  true /* reset relative duration */);
         t1->requeue(args);
      }

      defs.updateCalendar(calUpdateParams);
   }
   BOOST_CHECK_MESSAGE( submitted == 1 ,"Expected one submission but found " << submitted);
}



BOOST_AUTO_TEST_CASE( test_date_time_combination )
{
   // See ECFLOW-337
   cout << "ANode:: ...test_date_time_combination\n";

   // Create the suite, starting on a sunday
   Defs  defs;
   suite_ptr suite = defs.add_suite("s1");
   boost::posix_time::ptime the_time = boost::posix_time::ptime(date(2015,6,7),time_duration(0,0,0)); //sunday
   suite->addClock( ClockAttr(the_time) );
   task_ptr t1 = suite->add_task("t1");
   t1->addDate( DateAttr(8,6,2015) );   // Monday
   t1->addTime( ecf::TimeAttr(ecf::TimeSlot(10,0)) );

   defs.beginAll();

   CalendarUpdateParams calUpdateParams( hours(1) );
   boost::posix_time::ptime expected_time = boost::posix_time::ptime(date(2015,6,8),time_duration(10,0,0)); // Monday & 10
   //cout << "expected_time =  " << expected_time << "\n";

   int submitted = 0;
   for(int m=1; m < 100; m++) {

      Jobs jobs(&defs);
      JobsParam jobsParam;
      jobs.generate(jobsParam);

      if (jobsParam.submitted().size() ) {
         submitted++;
         //cout << "submitted at " << suite->calendar().suiteTime() << "\n";

         BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time,"\nExpected to submit at " << expected_time << " only, but also found " << suite->calendar().suiteTime());

         Node::Requeue_args args(true /* reset repeats*/,
                                 0    /* clear_suspended_in_child_nodes */,
                                 true /* reset_next_time_slot_ */,
                                 true /* reset relative duration */);
         t1->requeue(args);
      }

      defs.updateCalendar(calUpdateParams);
   }
   BOOST_CHECK_MESSAGE( submitted == 1 ,"Expected one submission but found " << submitted);
}


BOOST_AUTO_TEST_CASE( test_day_time_combination_in_hierarchy )
{
   cout << "ANode:: ...test_day_time_combination_in_hierarchy\n";
   // See ECFLOW-833. Also See Note: ACore/doc/TimeDependencies.ddoc
   // ***** This behaviour is not intuitive *****

   // Create the suite, starting on a sunday
   Defs  defs;
   suite_ptr suite = defs.add_suite("s1");
   boost::posix_time::ptime the_time = boost::posix_time::ptime(date(2015,6,7),time_duration(0,0,0)); //sunday
   suite->addClock( ClockAttr(the_time) );
   family_ptr f1 = suite->add_family("f1");
   f1->addDay( DayAttr(DayAttr::MONDAY) );
   task_ptr t1 = f1->add_task("t1");
   t1->addTime( ecf::TimeAttr(ecf::TimeSlot(10,0)) );

   defs.beginAll();

   CalendarUpdateParams calUpdateParams( hours(1) );
   boost::posix_time::ptime expected_time1 = boost::posix_time::ptime(date(2015,6,8),time_duration(0,0,0)); //Monday & midnight
   boost::posix_time::ptime expected_time2 = boost::posix_time::ptime(date(2015,6,8),time_duration(10,0,0)); //Monday & 10
   //cout << "expected_time =  " << expected_time << "\n";

   int submitted = 0;
   for(int m=1; m < 120; m++) {  // run for 5 days

      Jobs jobs(&defs);
      JobsParam jobsParam;
      jobs.generate(jobsParam);

      if (jobsParam.submitted().size() ) {
         submitted++;
         //cout << "submitted at " << suite->calendar().suiteTime() << "\n";

         // Unexpected, since time has been free'd on the sunday, and will stay this way for the following day
         // i.e one a time is free, it stays free until re-queued.
         if ( submitted == 1)
            BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time1,"\nExpected to submit at " << expected_time1 << " only, but also found " << suite->calendar().suiteTime());

         if ( submitted == 2)
            BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time2,"\nExpected to submit at " << expected_time2 << " only, but also found " << suite->calendar().suiteTime());

         Node::Requeue_args args(true /* reset repeats*/,
                                 0    /* clear_suspended_in_child_nodes */,
                                 true /* reset_next_time_slot_ */,
                                 true /* reset relative duration */);
         t1->requeue(args);
      }

      defs.updateCalendar(calUpdateParams);
   }
   BOOST_CHECK_MESSAGE( submitted == 2 ,"Expected two submission but found " << submitted);
}

BOOST_AUTO_TEST_CASE( test_date_time_combination_in_hierarchy )
{
   // See ECFLOW-833, See Note: ACore/doc/TimeDependencies.ddoc
   // ***** This behaviour is not intuitive *****
   cout << "ANode:: ...test_date_time_combination_in_hierarchy\n";

   // Create the suite, starting on a sunday
   Defs  defs;
   suite_ptr suite = defs.add_suite("s1");
   boost::posix_time::ptime the_time = boost::posix_time::ptime(date(2015,6,7),time_duration(0,0,0)); //sunday
   suite->addClock( ClockAttr(the_time) );
   family_ptr f1 = suite->add_family("f1");
   f1->addDate( DateAttr(8,6,2015) );   // Monday
   task_ptr t1 = f1->add_task("t1");
   t1->addTime( ecf::TimeAttr(ecf::TimeSlot(10,0)) );

   defs.beginAll();

   CalendarUpdateParams calUpdateParams( hours(1) );
   boost::posix_time::ptime expected_time1 = boost::posix_time::ptime(date(2015,6,8),time_duration(0,0,0)); // Monday & midnight
   boost::posix_time::ptime expected_time2 = boost::posix_time::ptime(date(2015,6,8),time_duration(10,0,0)); // Monday & 10
   //cout << "expected_time =  " << expected_time << "\n";

   int submitted = 0;
   for(int m=1; m < 100; m++) {

      Jobs jobs(&defs);
      JobsParam jobsParam;
      jobs.generate(jobsParam);

      if (jobsParam.submitted().size() ) {
         submitted++;
         //cout << "submitted at " << suite->calendar().suiteTime() << "\n";

         // Unexpected, since time has been free'd on the sunday, and will stay this way for the following day
         // i.e one a time is free, it stays free until re-queued.
         if ( submitted == 1)
            BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time1,"\nExpected to submit at " << expected_time1 << " only, but also found " << suite->calendar().suiteTime());

         if ( submitted == 2)
            BOOST_CHECK_MESSAGE( suite->calendar().suiteTime() == expected_time2,"\nExpected to submit at " << expected_time2 << " only, but also found " << suite->calendar().suiteTime());

         Node::Requeue_args args(true /* reset repeats*/,
                                 0    /* clear_suspended_in_child_nodes */,
                                 true /* reset_next_time_slot_ */,
                                 true /* reset relative duration */);
         t1->requeue(args);
      }

      defs.updateCalendar(calUpdateParams);
   }
   BOOST_CHECK_MESSAGE( submitted == 2 ,"Expected one submission but found " << submitted);
}

BOOST_AUTO_TEST_SUITE_END()
