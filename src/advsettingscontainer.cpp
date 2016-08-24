/***************************************************************************
  advsettingscontainer.cpp
  -------------------
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#include "advsettingscontainer.h"

#include <QDebug>
#include <QStackedLayout>

#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "advoutputoptions.h"
#include "advprocessingoptions.h"
#include "advspectraloptions.h"
#include "advstatisticaloptions.h"

AdvSettingsContainer::AdvSettingsContainer(QWidget *parent,
                                           DlProject *dlProject,
                                           EcProject *ecProject,
                                           ConfigState *config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config)
{
    processingOptions_ = new AdvProcessingOptions(this, dlProject_, ecProject_, configState_);
    spectralOptions_ = new AdvSpectralOptions(this, dlProject_, ecProject_, configState_);
    statisticalOptions_ = new AdvStatisticalOptions(this, ecProject_);
    outputOptions_ = new AdvOutputOptions(this, ecProject_, configState_);

    mainLayout_ = new QStackedLayout(this);
    mainLayout_->addWidget(processingOptions_);
    mainLayout_->addWidget(statisticalOptions_);
    mainLayout_->addWidget(spectralOptions_);
    mainLayout_->addWidget(outputOptions_);
    mainLayout_->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout_->setSpacing(0);
    mainLayout_->setContentsMargins(15, 15, 0, 10);
    setLayout(mainLayout_);

    connect(spectralOptions_, &AdvSpectralOptions::updateOutputsRequest,
            outputOptions_, &AdvOutputOptions::updateOutputs);

    connect(this, &AdvSettingsContainer::checkMetadataOutputRequest,
            outputOptions_, &AdvOutputOptions::checkMetadataOutput);
}

AdvSettingsContainer::~AdvSettingsContainer()
{
    DEBUG_FUNC_NAME
}

void AdvSettingsContainer::setCurrentPage(int page)
{
    mainLayout_->setCurrentIndex(page);
}
