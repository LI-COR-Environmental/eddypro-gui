/***************************************************************************
  advancedsettingspage.cpp
  ------------------------
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
#include <QListWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>

#include "alia.h"
#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "advprocessingoptions.h"
#include "advoutputoptions.h"
#include "advspectraloptions.h"
#include "advstatisticaloptions.h"
#include "advsettingscontainer.h"
#include "smartfluxbar.h"
#include "planarfitsettingsdialog.h"
#include "timelagsettingsdialog.h"
#include "advancedsettingspage.h"

const QString AdvancedSettingsPage::PAGE_TITLE_0 = QT_TR_NOOP(QStringLiteral("Processing Options"));
const QString AdvancedSettingsPage::PAGE_TITLE_1 = QT_TR_NOOP(QStringLiteral("Statistical Analysis"));
const QString AdvancedSettingsPage::PAGE_TITLE_2 = QT_TR_NOOP(QStringLiteral("Output Files"));
const QString AdvancedSettingsPage::PAGE_TITLE_3 = QT_TR_NOOP(QStringLiteral("Computation Options"));
const QString AdvancedSettingsPage::PAGE_TITLE_4 = QT_TR_NOOP(QStringLiteral("Spectral Corrections"));

AdvancedSettingsPage::AdvancedSettingsPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config),
    advancedSettingPages_(0)
{
    DEBUG_FUNC_NAME

    createMenu();
    createIcons();

    QWidget* resetButtonContainer = new QWidget;
    resetButtonContainer->setProperty("buttonContainer", true);

    QPushButton *resetButton = new QPushButton;
    resetButton->setProperty("mdButton", true);
    resetButton->setText(tr("Restore Default Values"));
    resetButton->setToolTip(tr("<b>Restore Default Values</b>: Resets all the Advanced Settings to the default settings."));

    QHBoxLayout* resetButtonContainerLayout = new QHBoxLayout;
    resetButtonContainerLayout->addWidget(resetButton, 1, Qt::AlignCenter);
    resetButtonContainer->setLayout(resetButtonContainerLayout);

    advancedSettingPages_ = new AdvSettingsContainer(this, dlProject_, ecProject_, configState_);
    menuWidget->setCurrentRow(0);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);
    smartfluxBar_->setVisible(false);
    smartfluxBar_->setToolTip(tr("EddyPro is in SMARTFlux configuration mode (Ctrl+F to exit)"));

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(smartfluxBar_, 0, 0, 1, -1);
    mainLayout->addWidget(menuWidget, 1, 0, 1, 1);
    mainLayout->addWidget(advancedSettingPages_, 1, 1, 2, 1);
    mainLayout->addWidget(resetButtonContainer, 2, 0, 1, 1);
    mainLayout->setRowStretch(1, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetButtonCLicked()));

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));
}

AdvancedSettingsPage::~AdvancedSettingsPage()
{ ; }

void AdvancedSettingsPage::createMenu()
{
    DEBUG_FUNC_NAME

    menuWidget = new QListWidget();
    menuWidget->setSelectionRectVisible(false);
    qDebug() << "isSelectionRectVisible" << menuWidget->isSelectionRectVisible();
    qDebug() << "selectionMode" << menuWidget->selectionMode();
    // to reduce the select decoration
    menuWidget->setObjectName(QStringLiteral("advSettingsMenu"));
    menuWidget->setIconSize(QSize(185, 48));
    menuWidget->setSpacing(0);
    menuWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(menuWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(changePage(int)));
}

void AdvancedSettingsPage::createIcons()
{
    QListWidgetItem *tlButton = new QListWidgetItem(menuWidget);
    tlButton->setIcon(QIcon(QStringLiteral(":/icons/adv-menu-processing")));
    tlButton->setText(PAGE_TITLE_0);

    QListWidgetItem *rsButton = new QListWidgetItem(menuWidget);
    rsButton->setIcon(QIcon(QStringLiteral(":/icons/adv-menu-spectra")));
    rsButton->setText(PAGE_TITLE_4);

    QListWidgetItem *mfButton = new QListWidgetItem(menuWidget);
    mfButton->setIcon(QIcon(QStringLiteral(":/icons/adv-menu-statistic")));
    mfButton->setText(PAGE_TITLE_1);

    QListWidgetItem *saButton = new QListWidgetItem(menuWidget);
    saButton->setIcon(QIcon(QStringLiteral(":/icons/adv-menu-output")));
    saButton->setText(PAGE_TITLE_2);
}

void AdvancedSettingsPage::changePage(int index)
{
    advancedSettingPages_->setCurrentPage(index);
}

void AdvancedSettingsPage::resetButtonCLicked()
{
    DEBUG_FUNC_NAME
    if (Alia::queryEcReset_1() == QMessageBox::Yes)
    {
        advancedSettingPages_->processingOptions()->reset();
        advancedSettingPages_->statisticalOptions()->reset();
        advancedSettingPages_->spectralOptions()->reset();
        advancedSettingPages_->outputOptions()->reset();
        advancedSettingPages_->processingOptions()->updateAngleOfAttack(ecProject_->generalColMasterSonic());
        ecProject_->setModified(true);
    }
}

void AdvancedSettingsPage::updateSmartfluxBar()
{
    DEBUG_FUNC_NAME
    qDebug() << configState_->project.smartfluxMode;
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);

    advancedSettingPages_->processingOptions()->setSmartfluxUI();
    advancedSettingPages_->processingOptions()->getPlanarFitSettingsDialog()->setSmartfluxUI();
    advancedSettingPages_->processingOptions()->getTimeLagSettingsDialog()->setSmartfluxUI();

    advancedSettingPages_->spectralOptions()->setSmartfluxUI();
    advancedSettingPages_->outputOptions()->setSmartfluxUI();
}
