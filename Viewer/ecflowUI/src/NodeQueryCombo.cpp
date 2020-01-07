//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include "NodeQueryCombo.hpp"

#include "NodeQuery.hpp"
#include "NodeQueryHandler.hpp"

#include <QVariant>

NodeQueryCombo::NodeQueryCombo(QWidget* parent) : QComboBox(parent)
{
	for(std::vector<NodeQuery*>::const_iterator it=NodeQueryHandler::instance()->items().begin();
		it != NodeQueryHandler::instance()->items().end(); ++it)
	{
		addItem(QString::fromStdString((*it)->name()));
	}

	connect(this,SIGNAL(currentIndexChanged(int)),
			this,SLOT(slotCurrentChanged(int)));
}

void NodeQueryCombo::slotCurrentChanged(int current)
{
	if(current != -1)
		Q_EMIT changed(itemData(current).toString());
}









