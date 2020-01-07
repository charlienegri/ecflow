//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef MESSAGEITEMWIDGET_HPP_
#define MESSAGEITEMWIDGET_HPP_

#include "InfoPanelItem.hpp"

#include "ui_MessageItemWidget.h"

class LogModel;

class MessageItemWidget : public QWidget, public InfoPanelItem, protected Ui::MessageItemWidget
{
    Q_OBJECT
public:
	explicit MessageItemWidget(QWidget *parent=0);

	void reload(VInfo_ptr);
	QWidget* realWidget();
    void clearContents();

	//From VInfoPresenter
	void infoReady(VReply*);
	void infoFailed(VReply*);
	void infoProgress(VReply*);

    void nodeChanged(const VNode*, const std::vector<ecf::Aspect::Type>&) {}
    void defsChanged(const std::vector<ecf::Aspect::Type>&) {}

protected Q_SLOTS:
    void on_actionCopyEntry__triggered();
    void on_actionCopyRow__triggered();

protected:
    void updateState(const ChangeFlags&) {}	
    void toClipboard(QString txt) const;

    LogModel* model_;
};

#endif

