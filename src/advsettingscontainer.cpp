/***************************************************************************
  advsettingscontainer.cpp
  -------------------
  Copyright (C) 2011-2014, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include <QStackedLayout>
#include <QDebug>

#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "advstatisticaloptions.h"
#include "advprocessingoptions.h"
#include "advoutputoptions.h"
#include "advspectraloptions.h"
#include "advsettingscontainer.h"

AdvSettingsContainer::AdvSettingsContainer(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config),
    processingOptions_(0),
    statisticalOptions_(0),
    outputOptions_(0),
    spectralOptions_(0)
{
    processingOptions_ = new AdvProcessingOptions(this, dlProject_, ecProject_, configState_);
    spectralOptions_ = new AdvSpectralOptions(this, dlProject_, ecProject_, configState_);
    statisticalOptions_ = new AdvStatisticalOptions(this, ecProject_);
    outputOptions_ = new AdvOutputOptions(this, ecProject_, configState_);

    mainLayout = new QStackedLayout(this);
    mainLayout->addWidget(processingOptions_);
    mainLayout->addWidget(spectralOptions_);
    mainLayout->addWidget(statisticalOptions_);
    mainLayout->addWidget(outputOptions_);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(15, 15, 0, 10);
    setLayout(mainLayout);

    connect(spectralOptions_, SIGNAL(updateOutputsRequest(int)),
            outputOptions_, SLOT(updateOutputs(int)));

    connect(this, SIGNAL(checkMetadataOutputRequest()),
            outputOptions_, SLOT(checkMetadataOutput()));
}

AdvSettingsContainer::~AdvSettingsContainer()
{
    DEBUG_FUNC_NAME
}

void AdvSettingsContainer::setCurrentPage(int page)
{
    mainLayout->setCurrentIndex(page);
}
