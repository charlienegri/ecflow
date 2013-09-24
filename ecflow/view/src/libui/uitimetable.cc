/*
** Generated by X-Designer
*/
/*
**LIBS: -lXm -lXt -lX11
*/

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleBG.h>

extern "C" {
#include "SimpleTime.h"
#include "Hyper.h"
}
#define XmCreateDrawnButton CreateHyper
#include <xdclass.h>

class timetable_form_c: public xd_XmForm_c {
public:
	virtual void create (Widget parent, char *widget_name = NULL);
protected:
	Widget timetable_form;
	Widget file_;
	Widget from_;
	Widget to_;
	Widget all_;
	Widget tasks_;
	Widget by_name_;
	Widget by_time_;
	Widget time_;
	Widget menu_;
	Widget label_;
	Widget set_from_;
	Widget set_to_;
	Widget set_both_;
	Widget tools_;
public:
	static void optionsCB( Widget, XtPointer, XtPointer );
	virtual void optionsCB( Widget, XtPointer ) = 0;
	static void resetCB( Widget, XtPointer, XtPointer );
	virtual void resetCB( Widget, XtPointer ) = 0;
	static void setBothCB( Widget, XtPointer, XtPointer );
	virtual void setBothCB( Widget, XtPointer ) = 0;
	static void setToCB( Widget, XtPointer, XtPointer );
	virtual void setToCB( Widget, XtPointer ) = 0;
	static void setFromCB( Widget, XtPointer, XtPointer );
	virtual void setFromCB( Widget, XtPointer ) = 0;
	static void activateCB( Widget, XtPointer, XtPointer );
	virtual void activateCB( Widget, XtPointer ) = 0;
	static void updateCB( Widget, XtPointer, XtPointer );
	virtual void updateCB( Widget, XtPointer ) = 0;
	static void loadCB( Widget, XtPointer, XtPointer );
	virtual void loadCB( Widget, XtPointer ) = 0;
	static void mergeCB( Widget, XtPointer, XtPointer );
	virtual void mergeCB( Widget, XtPointer ) = 0;
};

typedef timetable_form_c *timetable_form_p;
class timetable;
typedef timetable *timetable_p;

timetable_p timetable_form = (timetable_p) NULL;



void timetable_form_c::create (Widget parent, char *widget_name)
{
	Widget children[6];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget form2 = (Widget)NULL;
	Widget form1 = (Widget)NULL;
	Widget radioBox1 = (Widget)NULL;
	Widget radioBox2 = (Widget)NULL;
	Widget scrolledWin1 = (Widget)NULL;
	Widget scrollbar1 = (Widget)NULL;
	Widget scrollbar2 = (Widget)NULL;
	Widget separator2 = (Widget)NULL;
	Widget button2 = (Widget)NULL;
	Widget button3 = (Widget)NULL;
	Widget button4 = (Widget)NULL;
	Widget button5 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "timetable_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	timetable_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = timetable_form;
	form2 = XmCreateForm ( timetable_form, "form2", al, ac );
	form1 = XmCreateForm ( form2, "form1", al, ac );
	file_ = XmCreateTextField ( form1, "file_", al, ac );
	XtSetArg(al[ac], XmNcolumns, 19); ac++;
	from_ = XmCreateText ( form1, "from_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNcolumns, 19); ac++;
	to_ = XmCreateText ( form1, "to_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_TIGHT); ac++;
	radioBox1 = XmCreateRadioBox ( form1, "radioBox1", al, ac );
	ac = 0;
	all_ = XmCreateToggleButtonGadget ( radioBox1, "All", al, ac );
	tasks_ = XmCreateToggleButtonGadget ( radioBox1, "Tasks only", al, ac );
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_TIGHT); ac++;
	radioBox2 = XmCreateRadioBox ( form1, "radioBox2", al, ac );
	ac = 0;
	by_name_ = XmCreateToggleButtonGadget ( radioBox2, "Sort by name", al, ac );
	by_time_ = XmCreateToggleButtonGadget ( radioBox2, "Sort by time", al, ac );
	XtSetArg(al[ac], XmNscrollingPolicy, XmAUTOMATIC); ac++;
	scrolledWin1 = XmCreateScrolledWindow ( form2, "scrolledWin1", al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNhorizontalScrollBar, &scrollbar1 ); ac++;
	XtSetArg(al[ac], XmNverticalScrollBar, &scrollbar2 ); ac++;
	XtGetValues(scrolledWin1, al, ac );
	ac = 0;
#define XmCreateDrawingArea CreateTime
	time_ = XmCreateDrawingArea ( scrolledWin1, "time_", al, ac );
	ac = 0;
	menu_ = XmCreatePopupMenu ( time_, "menu_", al, ac );
	XtSetArg(al[ac], XmNx, 2); ac++;
	XtSetArg(al[ac], XmNy, 2); ac++;
	label_ = XmCreateLabel ( menu_, "timeline_label", al, ac );
	ac = 0;
	separator2 = XmCreateSeparator ( menu_, "separator2", al, ac );
	XtSetArg(al[ac], XmNx, 2); ac++;
	XtSetArg(al[ac], XmNy, 22); ac++;
	set_from_ = XmCreatePushButton ( menu_, "Set start of display range to this date", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNx, 2); ac++;
	XtSetArg(al[ac], XmNy, 44); ac++;
	set_to_ = XmCreatePushButton ( menu_, "Set end of display range to this date", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNx, 2); ac++;
	XtSetArg(al[ac], XmNy, 66); ac++;
	set_both_ = XmCreatePushButton ( menu_, "Set display range to these dates", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNx, 2); ac++;
	XtSetArg(al[ac], XmNy, 88); ac++;
	button2 = XmCreatePushButton ( menu_, "Reset display range", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNspacing, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	tools_ = XmCreateRowColumn ( form2, "tools_", al, ac );
	ac = 0;
	button3 = XmCreatePushButton ( tools_, "Merge", al, ac );
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
	XtSetArg(al[ac], XmNdefaultButtonShadowThickness, 0); ac++;
	button4 = XmCreatePushButton ( tools_, "Load", al, ac );
	ac = 0;
	button5 = XmCreatePushButton ( tools_, "Update", al, ac );

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( form2,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, tools_); ac++;
	XtSetValues ( form1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, form1); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( scrolledWin1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( tools_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, radioBox1); ac++;
	XtSetValues ( file_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, to_); ac++;
	XtSetValues ( from_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( to_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, radioBox2); ac++;
	XtSetValues ( radioBox1,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNrightWidget, from_); ac++;
	XtSetValues ( radioBox2,al, ac );
	ac = 0;
	XtAddCallback (file_, XmNactivateCallback,&timetable_form_c:: activateCB, (XtPointer) this);
	XtAddCallback (from_, XmNactivateCallback,&timetable_form_c:: activateCB, (XtPointer) this);
	XtAddCallback (to_, XmNactivateCallback,&timetable_form_c:: activateCB, (XtPointer) this);
	XtAddCallback (all_, XmNvalueChangedCallback,&timetable_form_c:: optionsCB, (XtPointer) this);
	XtAddCallback (tasks_, XmNvalueChangedCallback,&timetable_form_c:: optionsCB, (XtPointer) this);
	children[ac++] = all_;
	children[ac++] = tasks_;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (by_name_, XmNvalueChangedCallback,&timetable_form_c:: optionsCB, (XtPointer) this);
	XtAddCallback (by_time_, XmNvalueChangedCallback,&timetable_form_c:: optionsCB, (XtPointer) this);
	children[ac++] = by_name_;
	children[ac++] = by_time_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = file_;
	children[ac++] = from_;
	children[ac++] = to_;
	children[ac++] = radioBox1;
	children[ac++] = radioBox2;
	XtManageChildren(children, ac);
	ac = 0;
	XtAddCallback (set_from_, XmNactivateCallback,&timetable_form_c:: setFromCB, (XtPointer) this);
	XtAddCallback (set_to_, XmNactivateCallback,&timetable_form_c:: setToCB, (XtPointer) this);
	XtAddCallback (set_both_, XmNactivateCallback,&timetable_form_c:: setBothCB, (XtPointer) this);
	XtAddCallback (button2, XmNactivateCallback,&timetable_form_c:: resetCB, (XtPointer) this);
	children[ac++] = label_;
	children[ac++] = separator2;
	children[ac++] = set_from_;
	children[ac++] = set_to_;
	children[ac++] = set_both_;
	children[ac++] = button2;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = time_;
	XtManageChildren(children, ac);
	ac = 0;
	XmScrolledWindowSetAreas(scrolledWin1, scrollbar1, scrollbar2, time_ );
	XtAddCallback (button3, XmNactivateCallback,&timetable_form_c:: mergeCB, (XtPointer) this);
	XtAddCallback (button4, XmNactivateCallback,&timetable_form_c:: loadCB, (XtPointer) this);
	XtAddCallback (button5, XmNactivateCallback,&timetable_form_c:: updateCB, (XtPointer) this);
	children[ac++] = button3;
	children[ac++] = button4;
	children[ac++] = button5;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = form1;
	children[ac++] = scrolledWin1;
	children[ac++] = tools_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = form2;
	XtManageChildren(children, ac);
	ac = 0;
}

void timetable_form_c::optionsCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->optionsCB ( widget, call_data );
}

void timetable_form_c::resetCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->resetCB ( widget, call_data );
}

void timetable_form_c::setBothCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->setBothCB ( widget, call_data );
}

void timetable_form_c::setToCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->setToCB ( widget, call_data );
}

void timetable_form_c::setFromCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->setFromCB ( widget, call_data );
}

void timetable_form_c::activateCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->activateCB ( widget, call_data );
}

void timetable_form_c::updateCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->updateCB ( widget, call_data );
}

void timetable_form_c::loadCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->loadCB ( widget, call_data );
}

void timetable_form_c::mergeCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	timetable_form_p instance = (timetable_form_p) client_data;
	instance->mergeCB ( widget, call_data );
}

