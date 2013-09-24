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
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>


#include "uiedit_variable.h"

edit_variable_p edit_variable_form = (edit_variable_p) NULL;



void edit_variable_form_c::create (Widget parent, char *widget_name)
{
	Widget children[2];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget button1 = (Widget)NULL;
	Widget form1 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "edit_variable_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	edit_variable_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = edit_variable_form;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	tools_ = XmCreateRowColumn ( edit_variable_form, "tools_", al, ac );
	ac = 0;
	button1 = XmCreatePushButton ( tools_, "Apply", al, ac );
	form1 = XmCreateForm ( edit_variable_form, "form1", al, ac );
	name_ = XmCreateLabel ( form1, "name", al, ac );
	value_ = XmCreateTextField ( form1, "value", al, ac );

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( tools_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, tools_); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( form1,al, ac );
	ac = 0;
	XtAddCallback (button1, XmNactivateCallback,&edit_variable_form_c:: applyCB, (XtPointer) this);
	children[ac++] = button1;
	XtManageChildren(children, ac);
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 2); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 2); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftOffset, 2); ac++;
	XtSetValues ( name_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopOffset, 2); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomOffset, 2); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNleftOffset, 2); ac++;
	XtSetArg(al[ac], XmNleftWidget, name_); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightOffset, 2); ac++;
	XtSetValues ( value_,al, ac );
	ac = 0;
	XtAddCallback (value_, XmNvalueChangedCallback,&edit_variable_form_c:: changedCB, (XtPointer) this);
	children[ac++] = name_;
	children[ac++] = value_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = tools_;
	children[ac++] = form1;
	XtManageChildren(children, ac);
	ac = 0;
}

void edit_variable_form_c::changedCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_variable_form_p instance = (edit_variable_form_p) client_data;
	instance->changedCB ( widget, call_data );
}

void edit_variable_form_c::applyCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_variable_form_p instance = (edit_variable_form_p) client_data;
	instance->applyCB ( widget, call_data );
}

