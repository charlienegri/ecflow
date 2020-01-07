/***************************** LICENSE START ***********************************

 Copyright 2009-2020 ECMWF and INPE. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef TABLENODEWIDGET_HPP_
#define TABLENODEWIDGET_HPP_

#include "ui_TableNodeWidget.h"

#include "NodeWidget.hpp"

class NodeStateFilter;
class TableNodeSortModel;
class VParamFilterMenu;
class VSettings;

class TableNodeWidget : public NodeWidget, protected Ui::TableNodeWidget
{
Q_OBJECT

public:
    TableNodeWidget(ServerFilter* servers,bool interactive,QWidget* parent=0);
	~TableNodeWidget();

    void populateDockTitleBar(DashboardDockTitleWidget* tw);
    void rerender();

    void writeSettings(VComboSettings*);
    void readSettings(VComboSettings*);

protected Q_SLOTS:
	void on_actionBreadcrumbs_triggered(bool b);
    void slotSelectionChangedInView(VInfo_ptr info);
    void slotSelectionAutoScrollChanged(bool b);

protected:
    void detachedChanged() {}

private:
    TableNodeSortModel *sortModel_;
    VParamFilterMenu *stateFilterMenu_;
    QAction* acAutoScroll_;
};

#endif

