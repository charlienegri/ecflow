//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "ServerSettingsItemWidget.hpp"

#include "Node.hpp"
#include "ServerHandler.hpp"
#include "VNode.hpp"

#include <QPushButton>

#include <assert.h>

//========================================================
//
// ServerSettingsItemWidget
//
//========================================================

ServerSettingsItemWidget::ServerSettingsItemWidget(QWidget *parent) : QWidget(parent)
{
	setupUi(this);

	connect(buttonBox_,SIGNAL(clicked(QAbstractButton*)),
			this,SLOT(slotClicked(QAbstractButton *)));

	QPushButton* applyPb=buttonBox_->button(QDialogButtonBox::Apply);
    assert(applyPb);
	applyPb->setEnabled(false);

	connect(editor_,SIGNAL(changed()),
		this,SLOT(slotEditorChanged()));
}

QWidget* ServerSettingsItemWidget::realWidget()
{
	return this;
}

void ServerSettingsItemWidget::reload(VInfo_ptr info)
{
	clearContents();

	enabled_=true;
	info_=info;

	if(info_ && info_.get() && info_->isServer() && info_->server())
	{
		editor_->edit(info_->server()->conf()->guiProp(),
				QString::fromStdString(info_->server()->name()));
	}
	else
	{

	}
}

void ServerSettingsItemWidget::clearContents()
{
	InfoPanelItem::clear();
	//TODO: properly set gui state
}

void  ServerSettingsItemWidget::slotEditorChanged()
{
	QPushButton* applyPb=buttonBox_->button(QDialogButtonBox::Apply);
	assert(applyPb);
	applyPb->setEnabled(true);
}

void ServerSettingsItemWidget::slotClicked(QAbstractButton* button)
{
	if(!enabled_)
		return;

	switch(buttonBox_->standardButton(button))
	{
	case QDialogButtonBox::Apply:
		{
			if(editor_->applyChange())
			{
				if(info_ && info_->server())
					info_->server()->conf()->saveSettings();
			}
		}
		break;
	default:
		break;
	}
}

static InfoPanelItemMaker<ServerSettingsItemWidget> maker1("server_settings");
