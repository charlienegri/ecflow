//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef MANUALITEMWIDGET_HPP_
#define MANUALITEMWIDGET_HPP_

#include "InfoPanelItem.hpp"
#include "CodeItemWidget.hpp"

class ManualItemWidget : public CodeItemWidget, public InfoPanelItem
{
public:
	explicit ManualItemWidget(QWidget *parent=0);
	~ManualItemWidget();

	void reload(VInfo_ptr);
	QWidget* realWidget();
    void clearContents();

    //From VInfoPresenter
    void infoReady(VReply*);
    void infoFailed(VReply*);
    void infoProgress(VReply*);

    void nodeChanged(const VNode*, const std::vector<ecf::Aspect::Type>&) {}
    void defsChanged(const std::vector<ecf::Aspect::Type>&) {}

protected:
    void updateState(const ChangeFlags&);
    void reloadRequested();
};

#endif

