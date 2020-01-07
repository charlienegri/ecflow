//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================
#ifndef OUTPUTFETCHINFO_HPP
#define OUTPUTFETCHINFO_HPP

#include <QWidget>

#include "VInfo.hpp"
#include "VReply.hpp"

class QLabel;
class QTextEdit;

class OutputFetchInfo : public QWidget
{
public:
    OutputFetchInfo(QWidget* parent=0);
    void setInfo(VReply*,VInfo_ptr);
    void clearInfo();

private:
    QString buildList(QStringList,bool ordered=false);

    QLabel* label_;
    QTextEdit* te_;

};

#endif // OUTPUTFETCHINFO_HPP

