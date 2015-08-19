/***************************************************************************
  mainwidget.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#include "mainwidget.h"

#include <QDebug>
#include <QMainWindow>
#include <QStackedLayout>

#include "advoutputoptions.h"
#include "advspectraloptions.h"
#include "basicsettingspage.h"
#include "configstate.h"
#include "dbghelper.h"
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
    mainWidgetLayout(0),
    welcomePage_(0),
    projectPage_(0),
    basicSettingsPage_(0),
    advancedSettingsPage_(0),
    runPage_(0),
    faderWidget(0),
    fadingOn(true)
{
    DEBUG_FUNC_NAME

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

    connect(mainWidgetLayout, &QStackedLayout::currentChanged,
            this, &MainWidget::fadeInWidget);

    // from MainWindow
    connect(static_cast<QMainWindow*>(parent), SIGNAL(updateMetadataReadRequest()),
            basicSettingsPage_, SLOT(updateMetadataRead()));
    connect(this, SIGNAL(showSetPrototypeRequest()),
            basicSettingsPage_, SLOT(showSetPrototype()));

    connect(static_cast<QMainWindow*>(parent), SIGNAL(checkMetadataOutputRequest()),
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

MainWidget::~MainWidget()
{
}

void MainWidget::setCurrentPage(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME
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

void MainWidget::fadeInWidget(int index)
{
    DEBUG_FUNC_NAME

    if (fadingOn)
    {
        if (faderWidget)
        {
            faderWidget->close();
        }
        faderWidget = new FaderWidget(mainWidgetLayout->widget(index));
        faderWidget->setFadeDuration(200);
        faderWidget->start();
    }
}

// update smartflux bar visibility on all the subpages
void MainWidget::updateSmartfluxBarStatus()
{
    DEBUG_FUNC_NAME

    welcomePage_->updateSmartfluxBar();
    welcomePage_->updateSmartfluxCheckBox();

    projectPage_->setSmartfluxUI();
    projectPage_->updateSmartfluxBar();

    basicSettingsPage_->updateSmartfluxBar();
    advancedSettingsPage_->updateSmartfluxBar();
    runPage_->updateSmartfluxBar();
}
