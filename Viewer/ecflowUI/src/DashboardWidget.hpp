/***************************** LICENSE START ***********************************

 Copyright 2009-2020 ECMWF and INPE. This software is distributed under the terms
 of the Apache License version 2.0. In applying this license, ECMWF does not
 waive the privileges and immunities granted to it by virtue of its status as
 an Intergovernmental Organization or submit itself to any jurisdiction.

 ***************************** LICENSE END *************************************/

#ifndef DASHBOARDWIDGET_HPP_
#define DASHBOARDWIDGET_HPP_

#include <string>
#include <QDockWidget>
#include <QWidget>
#include <QAction>
#include <QIcon>

#include "VInfo.hpp"

class DashboardDockTitleWidget;
class NodePathWidget;
class ServerFilter;
class VComboSettings;

class DashboardWidget : public QWidget
{
Q_OBJECT

public:
	DashboardWidget(const std::string& type, QWidget* parent=0);
    virtual ~DashboardWidget() {}

    virtual void populateDockTitleBar(DashboardDockTitleWidget*)=0;
    virtual void populateDialog()=0;
    virtual void reload()=0;
	virtual void rerender()=0;
    virtual bool initialSelectionInView() {return false;}
	virtual VInfo_ptr currentSelection() {return VInfo_ptr(); }
    QAction* detachedAction() const {return detachedAction_;}
    QAction* maximisedAction() const {return maximisedAction_;}
    virtual QList<QAction*> dockTitleActions() {return QList<QAction*>();}

    bool detached() const;
    void setDetached(bool b);
    bool isMaximised() const;
    void resetMaximised();
    void setEnableMaximised(bool st);
    bool isInDialog() const {return inDialog_;}

    virtual void writeSettings(VComboSettings*);
    virtual void readSettings(VComboSettings*);
    virtual void writeSettingsForDialog() {}
    virtual void readSettingsForDialog() {}

	const std::string type() const {return type_;}
	void id(const std::string& id) {id_=id;}

public Q_SLOTS:
	virtual void setCurrentSelection(VInfo_ptr)=0;

Q_SIGNALS:
    void titleUpdated(QString,QString type=QString());
    void selectionChanged(VInfo_ptr);
    void maximisedChanged(DashboardWidget*);
    void popInfoPanel(VInfo_ptr,QString);
    void dashboardCommand(VInfo_ptr,QString);

protected Q_SLOTS:
    void slotDetachedToggled(bool);
    void slotMaximisedToggled(bool);

protected:
    virtual void detachedChanged()=0;
    void setInDialog(bool);

    std::string id_;
	std::string type_;
	bool acceptSetCurrent_;
    QAction *detachedAction_;
    QAction *maximisedAction_;
    bool ignoreMaximisedChange_;
    NodePathWidget* bcWidget_;

private:
    bool inDialog_;
};


#endif
