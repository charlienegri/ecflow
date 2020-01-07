//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef TEXTFORMAT_HPP
#define TEXTFORMAT_HPP

#include <QColor>
#include <QList>
#include <QString>

class QAction;

namespace Viewer
{
    QString formatShortCut(QString);
    QString formatShortCut(QAction*);
    void addShortCutToToolTip(QList<QAction*>);
    QString formatBoldText(QString,QColor);
    QString formatItalicText(QString,QColor);
    QString formatText(QString,QColor);
    QString formatTableThText(QString txt,QColor col);
    QString formatTableTrBg(QString txt,QColor col);
    QString formatTableTrText(QString txt);
    QString formatTableTdText(QString txt,QColor col);
    QString formatTableTdText(QString txt);
    QString formatTableTdBg(QString txt,QColor col);
    QString formatTableRow(QString col1Text,QString col2Text,QColor bg,QColor fg, bool boldCol1);
    QString formatTableRow(QString col1Text,QString col2Text, bool boldCol1);

} //namespace Viewer

#endif // TEXTFORMAT_HPP
