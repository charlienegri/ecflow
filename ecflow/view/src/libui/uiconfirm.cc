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
#include <Xm/MessageB.h>
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>


#include "uiconfirm.h"

confirm_shell_p confirm_shell = (confirm_shell_p) NULL;



void confirm_shell_c::create (Widget parent, char *widget_name)
{
  /* Widget children[6];      // Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget button20 = (Widget)NULL;
	Widget button21 = (Widget)NULL;
	Widget button22 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "confirm_shell";

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	XtSetArg(al[ac], XmNtransient, TRUE); ac++;
	confirm_shell = XmCreateDialogShell ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = confirm_shell;
	XtSetArg(al[ac], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); ac++;
	XtSetArg(al[ac], XmNdialogType, XmDIALOG_QUESTION); ac++;
	XtSetArg(al[ac], XmNmessageAlignment, XmALIGNMENT_CENTER); ac++;
	XtSetArg(al[ac], XmNdefaultButtonType, XmDIALOG_CANCEL_BUTTON); ac++;
	form_ = XmCreateMessageBox ( confirm_shell, "form_", al, ac );
	ac = 0;
	button20 = XmMessageBoxGetChild ( form_, XmDIALOG_CANCEL_BUTTON );
	button21 = XmMessageBoxGetChild ( form_, XmDIALOG_HELP_BUTTON );
	label_ = XmMessageBoxGetChild ( form_, XmDIALOG_MESSAGE_LABEL );
	button22 = XmMessageBoxGetChild ( form_, XmDIALOG_OK_BUTTON );
	XtSetArg(al[ac], XmNsensitive, FALSE); ac++;
	XtSetValues ( button21,al, ac );
	ac = 0;
	XtAddCallback (form_, XmNhelpCallback,&confirm_shell_c:: helpCB, (XtPointer) this);
	XtAddCallback (form_, XmNcancelCallback,&confirm_shell_c:: cancelCB, (XtPointer) this);
	XtAddCallback (form_, XmNokCallback,&confirm_shell_c:: okCB, (XtPointer) this);
}

void confirm_shell_c::helpCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	confirm_shell_p instance = (confirm_shell_p) client_data;
	instance->helpCB ( widget, call_data );
}

void confirm_shell_c::cancelCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	confirm_shell_p instance = (confirm_shell_p) client_data;
	instance->cancelCB ( widget, call_data );
}

void confirm_shell_c::okCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	confirm_shell_p instance = (confirm_shell_p) client_data;
	instance->okCB ( widget, call_data );
}

