//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VIEWER_SRC_CASESENSITIVEBUTTON_HPP_
#define VIEWER_SRC_CASESENSITIVEBUTTON_HPP_

#include <QToolButton>

#include <map>

class CaseSensitiveButton : public QToolButton
{
Q_OBJECT

public:
	explicit CaseSensitiveButton(QWidget* parent=0);

protected Q_SLOTS:
	void slotClicked(bool);

Q_SIGNALS:
	void changed(bool);

private:
	std::map<bool,QString> tooltip_;
};

#endif /* VIEWER_SRC_CASESENSITIVEBUTTON_HPP_ */
