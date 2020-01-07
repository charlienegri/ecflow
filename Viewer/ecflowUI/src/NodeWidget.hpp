/***************************** LICENSE START ***********************************

 Copyright 2009-2020 ECMWF and INPE. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef NODEWIDGET_HPP_
#define NODEWIDGET_HPP_

#include <QString>
#include <QWidget>

#include "DashboardWidget.hpp"
#include "Viewer.hpp"
#include "VInfo.hpp"

class QStackedLayout;
class QWidget;

class AbstractNodeModel;
class AttributeFilter;
class IconFilter;
class NodeFilterDef;
class NodeStateFilter;
class VModelData;
class NodePathWidget;
class NodeViewBase;
class ServerFilter;

class NodeWidget : public DashboardWidget
{
Q_OBJECT

public:
	void active(bool);
	bool active() const;
	NodeViewBase* view() const {return view_;}
	QWidget* widget();
	VInfo_ptr currentSelection();
	//void currentSelection(VInfo_ptr info);
	void reload();
    void populateDialog() {}
    QList<QAction*> dockTitleActions() {return dockActions_;}

public Q_SLOTS:
	void setCurrentSelection(VInfo_ptr);

protected Q_SLOTS:
	void slotInfoPanelAction();
    void slotSelectionChangedInBc(VInfo_ptr info);

protected:
	explicit NodeWidget(const std::string& type,ServerFilter* serverFilter,QWidget* parent=0);
	virtual ~NodeWidget();

	void updateActionState(VInfo_ptr);
    bool broadcastSelection() const {return broadcastSelection_;}

	ServerFilter* serverFilter_;

	AbstractNodeModel* model_;	
	NodeViewBase* view_;

	IconFilter* icons_;
	AttributeFilter* atts_;

	NodeFilterDef* filterDef_;
	NodeStateFilter *states_;

private:
	void createActions();
	QList<QAction*> dockActions_;
	QMap<QString,QAction*> dockActionMap_;
	QList<QAction*> infoPanelActions_;
    bool broadcastSelection_;
};

#endif
