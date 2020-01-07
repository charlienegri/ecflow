//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "CustomCommandDialog.hpp"


CustomCommandDialog::CustomCommandDialog(QWidget *parent)
{
    setupUi(this);

    // when the user clicks the 'Run' button, we close the dialog with ACCEPT
    connect(commandDesigner_->runButton(), SIGNAL(clicked()), this, SLOT(accept()));

}
