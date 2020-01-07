//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef PROPERTYDIALOG_INC_
#define PROPERTYDIALOG_INC_

#include "ui_PropertyDialog.h"

#include <QDialog>

class QAbstractButton;

class PropertyEditor;
class VProperty;

class PropertyDialog : public QDialog, private Ui::PropertyDialog
{

Q_OBJECT    
    
public:
    explicit PropertyDialog(QWidget *parent=0);
    ~PropertyDialog() {}

    bool isConfigChanged() const {return configChanged_;}
    void showPage(QString);

    //Called from VConfigLoader
    static void load(VProperty*);

public Q_SLOTS:
    void accept();
    void reject();
    void slotChangePage(QListWidgetItem *current, QListWidgetItem *previous);
    void slotButton(QAbstractButton*);

Q_SIGNALS:
	void configChanged();

private:
    void build();
    void addPage(QWidget *w,QPixmap pix,QString txt);
    void manageChange(bool);
    void apply();

    void closeEvent(QCloseEvent * event);
    void readSettings();
    void writeSettings();

    QList<PropertyEditor*> editors_;
    bool configChanged_;

    static VProperty* prop_;

};

#endif

