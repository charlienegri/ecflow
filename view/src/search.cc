//=============================================================================================
// Name        : 
// Author      : 
// Revision    : $Revision: #5 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description : 
//=============================================================================================
//=============================================================================================

#include "search.h"
#include "host.h"
#include "runnable.h"
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include "gui.h"
#include "extent.h"
#include "flags.h"
#include "result.h"
#include <X11/IntrinsicP.h>


extern "C" {
#include "xec.h"
}

search& search::instance()
{
  static search *m = new search();
  return *m;
}

search::search():
  name_(0)
  , timed_since_(0)
  , timed_from_(86400*3) // 3 days
{
	create(gui::top());
}

search::~search()
{
}


void search::searchCB(Widget,XtPointer)
{
  if(name_)  XtFree(name_);
  name_ = 0;
  
  if(XmToggleButtonGetState(what_)) {
    char* p = XmTextGetString(what_text_);        
    if(*p) 
      name_ = XtNewString(p);
    else 
      name_ = 0;
    XtFree(p);
  }

  type_flags_.clear();
  if(XmToggleButtonGetState(type_))
    scan(type_rowcol_,type_flags_);
  
  status_flags_.clear();
  if(XmToggleButtonGetState(status_))
    scan(status_rowcol_,status_flags_);
  
  special_flags_.clear();
  if(XmToggleButtonGetState(special_))
    scan(special_rowcol_,special_flags_);
  
  if(XmToggleButtonGetState(timed_)) {
    char *since = XmTextGetString(timed_text_since_);
    char *from  = XmTextGetString(timed_text_from_);
    if(since) 
      timed_since_ = atoi(since);
    if(from) 
      timed_from_ = atoi(from);
    fprintf (stdout, "# from: %d\tsince: %d\n", timed_from_, timed_since_);
    XtFree(since);
    XtFree(from);
  } else {
    timed_from_  = 86400;
    timed_since_ = 0;
  }

  if(XmToggleButtonGetState(misc_)) {
    icas_ = XmToggleButtonGetState(icase_);
    rege_ = XmToggleButtonGetState(toggle11_);
    glob_ = XmToggleButtonGetState(toggle12_);
    subs_ = XmToggleButtonGetState(toggle13_);
  } else { icas_ = true; subs_ = true; rege_ = glob_ = false; }

  result::clear();
  searchable::look_for(*this, !XmToggleButtonGetState(where_));
  result::show();
}

void search::closeCB(Widget,XtPointer)  
{
  XtUnmanageChild(form_);
}

void search::mapCB(Widget,XtPointer)
{
   searchable::parent(where_rowcol_);
}

void search::raise()
{
  XtManageChild(form_);
  XMapRaised(XtDisplay(_xd_rootwidget),XtWindow(_xd_rootwidget));
}

void search::show()
{
  instance().raise();
}

#include <sys/types.h>
#include <regex.h>
#include <boost/algorithm/string.hpp>    
#include <fnmatch.h>

void search::next(node& n)
{
  bool ok = true;
  ok &= check(n,type_flags_);	
  if (ok) ok &= check(n,status_flags_);
  if (ok) ok &= check(n,special_flags_);	
  if (ok && name_) {
    if (subs_) {
      bool res = n.match(name_);
      if (icas_) 
    	res |= !strncasecmp(name_, n.name().c_str(), strlen(name_));      
      else 	
	res |= !strncmp(name_, n.name().c_str(), strlen(name_));
      ok &= res;
    } else if (glob_) {
	std::string path (n.name());
	std::string pattern (name_);    
	if (name_[0] == '/') 
	  path = n.full_name();
	if (icas_) {
	  boost::algorithm::to_lower(path);
	  boost::algorithm::to_lower(pattern);
	}
	ok &= 0==fnmatch(pattern.c_str(), path.c_str(), 0);
      } else if (rege_) {
	regex_t exp; int flags = REG_EXTENDED;
	std::string path (n.name());
	std::string pattern (name_);    
	if (name_[0] == '/') 
	  path = n.full_name();
	if (icas_) 
	  flags &= REG_ICASE;
	int rv = regcomp(&exp, pattern.c_str(), flags);
	if (rv!=0) 
	  std::cerr << "regcomp failed %d" << rv << "\n";
	else
	  ok &= 0==regexec(&exp, path.c_str(), 0, NULL, 0);
	regfree(&exp);
    } else if (icas_) { // ignore case
      ok &= strcasestr(n.name().c_str(),name_) != 0; 
    } else { // case sensitive, accept longest names
       	ok &= n.match(name_);
    } 
      //    }
  }
  if (ok && XmToggleButtonGetState(timed_))
  if (n.type() == NODE_TASK || n.type() == NODE_FAMILY || n.type() == NODE_SUITE) {
    boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
    int diff = (now - n.status_time()).total_seconds();
    ok &= (-1 < timed_since_ && timed_since_ < diff) && diff < timed_from_;
  }
  if(ok) result::show(n);
}

bool search::check(node& n,array<flags*>& f)
{
  int c = f.count();
  bool ok = (c == 0);
  for(int i = 0; i < c ; i++)
    ok = ok || f[i]->eval(&n);
  return ok;
}


void search::whatCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(what_))
    XtManageChild(what_text_);
  else
    XtUnmanageChild(what_text_);
}

void search::whereCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(where_))
    XtManageChild(where_rowcol_);
  else
    XtUnmanageChild(where_rowcol_);
}

void search::statusCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(status_))
    XtManageChild(status_rowcol_);
  else
    XtUnmanageChild(status_rowcol_);
}

void search::typeCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(type_))
    XtManageChild(type_rowcol_);
  else
    XtUnmanageChild(type_rowcol_);
}

void search::specialCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(special_))
    XtManageChild(special_rowcol_);
  else
    XtUnmanageChild(special_rowcol_);
}

void search::timedCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(timed_)) {
    XtManageChild(timed_rowcol_);
    XtManageChild(timed_text_from_);
    XtManageChild(timed_text_since_);
  } else {
    XtUnmanageChild(timed_rowcol_);
    XtUnmanageChild(timed_text_from_);
    XtUnmanageChild(timed_text_since_);
  }
}

void search::radioCB(Widget w, XtPointer data) {
  rege_ = XmToggleButtonGetState(toggle11_);
  glob_ = XmToggleButtonGetState(toggle12_);
  subs_ = XmToggleButtonGetState(toggle13_);
}

void search::miscCB(Widget,XtPointer)
{
  if(XmToggleButtonGetState(misc_)) {
    XmToggleButtonSetState(icase_, True, False);
    // XmToggleButtonSetState(toggle12_, True, False); // glob
    XmToggleButtonSetState(toggle13_, True, False); // substring
    XtManageChild(misc_rowcol_); 
    XtManageChild(fname_); 
    XtManageChild(icase_);
  } else {
    XtUnmanageChild(misc_rowcol_); 
    XtUnmanageChild(icase_);
    XtUnmanageChild(fname_); 
  }
}

void search::scan(Widget w,array<flags*>& f)
{
  f.clear();
  CompositeWidget c = (CompositeWidget)w;
  for(unsigned int i = 0 ; i < c->composite.num_children; i++) {
    Widget p = c->composite.children[i];
    if(XmIsToggleButton(p)) {
      flags *s = (flags*)xec_GetUserData(p);
      if(s && XmToggleButtonGetState(p)) {
	f.add(s);
      }
    }
  }
}
/*
^[^#]\s*\w+\s*:(?<data>.*?)$

*/
