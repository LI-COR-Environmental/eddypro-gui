/***************************************************************************
  advsettingscontainer.cpp
  ------------------------
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#include "advsettingscontainer.h"

#include <QDebug>
#include <QStackedLayout>

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

void AdvSettingsContainer::setCurrentPage(int page)
{
    mainLayout_->setCurrentIndex(page);
}
