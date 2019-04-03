/***************************************************************************
  mainwidget.cpp
  --------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
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

#include "mainwidget.h"

#include <QDebug>
#include <QMainWindow>
#include <QStackedLayout>

#include "advoutputoptions.h"
#include "advspectraloptions.h"
#include "basicsettingspage.h"
#include "configstate.h"
#include "dlproject.h"
#include "ecproject.h"
#include "projectpage.h"
#include "runpage.h"
#include "welcomepage.h"
#include "widget_utils.h"

MainWidget::MainWidget(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* configState) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(configState),
    mainWidgetLayout(nullptr),
    welcomePage_(nullptr),
    projectPage_(nullptr),
    basicSettingsPage_(nullptr),
    advancedSettingsPage_(nullptr),
    runPage_(nullptr)
{
    // stacked widget # 0
    welcomePage_ = new WelcomePage(this, ecProject_, configState_);
    welcomePage_->setSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::Expanding);

    // stacked widget # 1
    projectPage_ = new ProjectPage(this, dlProject_, ecProject_, configState_);
    projectPage_->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);

    // stacked widget # 2
    basicSettingsPage_ = new BasicSettingsPage(this, dlProject_, ecProject_, configState_);
    basicSettingsPage_->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);

    // stacked widget # 3
    advancedSettingsPage_ = new AdvancedSettingsPage(this, dlProject_, ecProject_, configState_);
    advancedSettingsPage_->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);

    // stacked widget # 4
    runPage_ = new RunPage(this, ecProject_, configState_);
    runPage_->setSizePolicy(QSizePolicy::Ignored,
                            QSizePolicy::Ignored);

    mainWidgetLayout = new QStackedLayout(this);
    mainWidgetLayout->addWidget(welcomePage_);
    mainWidgetLayout->addWidget(projectPage_);
    mainWidgetLayout->addWidget(basicSettingsPage_);
    mainWidgetLayout->addWidget(advancedSettingsPage_);
    mainWidgetLayout->addWidget(runPage_);

    setLayout(mainWidgetLayout);

    // from MainWindow
    connect(dynamic_cast<QMainWindow*>(parent), SIGNAL(updateMetadataReadRequest()),
            basicSettingsPage_, SLOT(updateMetadataRead()));
    connect(this, SIGNAL(showSetPrototypeRequest()),
            basicSettingsPage_, SLOT(showSetPrototype()));

    connect(dynamic_cast<QMainWindow*>(parent), SIGNAL(checkMetadataOutputRequest()),
            advancedSettingsPage_->advancedSettingPages(), SIGNAL(checkMetadataOutputRequest()));

    connect(projectPage_, SIGNAL(updateMetadataReadRequest()),
            basicSettingsPage_, SLOT(updateMetadataRead()));
    connect(projectPage_, &ProjectPage::mdCleanupRequest,
            this, &MainWidget::mdCleanupRequest);
    connect(projectPage_, &ProjectPage::requestBasicSettingsClear,
            basicSettingsPage_, &BasicSettingsPage::clearSelectedItems);
    connect(projectPage_, &ProjectPage::setOutputBiometRequest,
            advancedPage()->advancedSettingPages()->outputOptions(),
            &AdvOutputOptions::setOutputBiomet);

    connect(basicSettingsPage_, &BasicSettingsPage::saveSilentlyRequest,
            this, &MainWidget::saveSilentlyRequest);

    connect(welcomePage_, &WelcomePage::openProjectRequest,
            this, &MainWidget::openProjectRequest);
    connect(welcomePage_, &WelcomePage::newProjectRequest,
            this, &MainWidget::newProjectRequest);
    connect(welcomePage_, &WelcomePage::checkUpdatesRequest,
            this, &MainWidget::checkUpdatesRequest);
}

void MainWidget::setCurrentPage(Defs::CurrPage page)
{
    if (mainWidgetLayout->currentIndex() != static_cast<int>(page))
    {
        mainWidgetLayout->currentWidget()->setSizePolicy(QSizePolicy::Ignored,
                                                         QSizePolicy::Ignored);
    }

    mainWidgetLayout->setCurrentIndex(static_cast<int>(page));

    mainWidgetLayout->currentWidget()->setSizePolicy(QSizePolicy::Expanding,
                                                     QSizePolicy::Expanding);
    adjustSize();

}

Defs::CurrPage MainWidget::currentPage()
{
    return static_cast<Defs::CurrPage>(mainWidgetLayout->currentIndex());
}

bool MainWidget::smartFluxCloseRequest()
{
    return WidgetUtils::okToCloseSmartFlux(this);
}

// update smartflux bar visibility on all the subpages
void MainWidget::updateSmartfluxBarStatus()
{
    welcomePage_->updateSmartfluxBar();
    welcomePage_->updateSmartfluxCheckBox();

    projectPage_->setSmartfluxUI();
    projectPage_->updateSmartfluxBar();

    basicSettingsPage_->updateSmartfluxBar();
    advancedSettingsPage_->updateSmartfluxBar();
    runPage_->updateSmartfluxBar();
}
