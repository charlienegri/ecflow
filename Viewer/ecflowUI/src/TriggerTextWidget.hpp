//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef TRIGGERTEXTWIDGET_HPP
#define TRIGGERTEXTWIDGET_HPP

#include <QTextBrowser>

class TriggerTableItem;

class TriggerTextWidget : public QTextBrowser
{
public:
    explicit TriggerTextWidget(QWidget *parent=0);
    void reload(TriggerTableItem* item);

private:
    QString makeHtml(TriggerTableItem *ti,QString directTitle,QString modeText) const;
};

#endif

