//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VIEWER_SRC_CUSTOMTABWIDGET_HPP_
#define VIEWER_SRC_CUSTOMTABWIDGET_HPP_

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{
public:
	explicit CustomTabWidget(QWidget* parent=0);

	void setCustomIcon(int index, QPixmap pix);

protected:
	QSize maxIconSize() const;
};


#endif /* VIEWER_SRC_CUSTOMTABWIDGET_HPP_ */
