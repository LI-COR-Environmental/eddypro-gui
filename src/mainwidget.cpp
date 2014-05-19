/***************************************************************************
  mainwidget.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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

#include <QDebug>
#include <QStackedLayout>
#include <QMainWindow>

#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "welcomepage.h"
#include "basicsettingspage.h"
#include "projectpage.h"
#include "advancedsettingspage.h"
#include "advsettingscontainer.h"
#include "advprocessingoptions.h"
#include "advoutputoptions.h"
#include "advspectraloptions.h"
#include "runpage.h"
#include "configstate.h"
#include "mainwidget.h"

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

    connect(mainWidgetLayout, SIGNAL(currentChanged(int)),
            this, SLOT(fadeInWidget(int)));

    // from MainWindow
    connect(static_cast<QMainWindow*>(parent), SIGNAL(updateMetadataReadRequest()),
            basicSettingsPage_, SLOT(updateMetadataRead()));
    connect(static_cast<QMainWindow*>(parent), SIGNAL(checkMetadataOutputRequest()),
            advancedSettingsPage_->advancedSettingPages(), SIGNAL(checkMetadataOutputRequest()));

    connect(projectPage_, SIGNAL(updateMetadataReadRequest()),
            basicSettingsPage_, SLOT(updateMetadataRead()));
    connect(projectPage_, SIGNAL(updateRawFilenameFormatRequest()),
            basicSettingsPage_, SLOT(updateRawFilenameFormat()));
    connect(projectPage_, SIGNAL(requestBasicSettingsClear()),
            basicSettingsPage_, SLOT(clearSelectedItems()));
    connect(projectPage_, SIGNAL(setOutputBiometRequest()),
            advancedPage()->advancedSettingPages()->outputOptions(),
            SLOT(setOutputBiomet()));

    connect(welcomePage_, SIGNAL(openProjectRequest(QString)), this, SIGNAL(openProjectRequest(QString)));
    connect(welcomePage_, SIGNAL(newProjectRequest()), this, SIGNAL(newProjectRequest()));
    connect(welcomePage_, SIGNAL(checkUpdatesRequest()), this, SIGNAL(checkUpdatesRequest()));
}

MainWidget::~MainWidget()
{
}

void MainWidget::setCurrentPage(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME
    if (mainWidgetLayout->currentIndex() != page)
    {
        mainWidgetLayout->currentWidget()->setSizePolicy(QSizePolicy::Ignored,
                                                         QSizePolicy::Ignored);
    }

    mainWidgetLayout->setCurrentIndex(page);

    mainWidgetLayout->currentWidget()->setSizePolicy(QSizePolicy::Expanding,
                                                     QSizePolicy::Expanding);
    adjustSize();

}

Defs::CurrPage MainWidget::currentPage()
{
    return static_cast<Defs::CurrPage>(mainWidgetLayout->currentIndex());
}

void MainWidget::fadeInWidget(int index)
{
    DEBUG_FUNC_NAME

    if (fadingOn)
    {
        if (faderWidget)
            faderWidget->close();
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

    basicSettingsPage_->updateSmartfluxBar();
    advancedSettingsPage_->updateSmartfluxBar();
    runPage_->updateSmartfluxBar();
}
