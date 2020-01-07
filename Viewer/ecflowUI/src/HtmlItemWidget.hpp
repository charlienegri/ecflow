//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef HTMLITEMWIDGET_HPP
#define HTMLITEMWIDGET_HPP

#include <QWidget>

#include "ui_HtmlItemWidget.h"

class HtmlItemWidget : public QWidget, protected Ui::HtmlItemWidget
{
Q_OBJECT

public:
    explicit HtmlItemWidget(QWidget *parent=0);
    ~HtmlItemWidget();

protected Q_SLOTS:
    void on_searchTb__clicked();
    void on_fontSizeUpTb__clicked();
    void on_fontSizeDownTb__clicked();

Q_SIGNALS:
    void editorFontSizeChanged();

protected:
    void removeSpacer();
};

#endif // HTMLITEMWIDGET_HPP
