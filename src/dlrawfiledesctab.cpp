/***************************************************************************
  dlrawfiledesctab.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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
 ***************************************************************************/

#include "dlrawfiledesctab.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>

#include "dbghelper.h"
#include "dlproject.h"
#include "rawfilesettingsdialog.h"
#include "variable_delegate.h"
#include "variable_model.h"
#include "variable_view.h"

DlRawfileDescTab::DlRawfileDescTab(QWidget* parent, DlProject* dlProject) :
    QWidget(parent),
    dlProject_(dlProject),
    rawSettingsDialog(nullptr),
    fieldSepTOB1Flag_(false)
{
    variableView_ = new VariableView(this);
    variableModel_ = new VariableModel(variableView_, dlProject_->variables());
    variableDelegate_ = new VariableDelegate(variableView_);

    variableView_->setModel(variableModel_);
    variableView_->setItemDelegate(variableDelegate_);
    variableView_->setSelectionMode(QAbstractItemView::SingleSelection);
    variableView_->setSelectionBehavior(QAbstractItemView::SelectColumns);
    variableView_->setCornerButtonEnabled(false);
    variableView_->setWordWrap(false);

    QHeaderView *hHeaderView = variableView_->horizontalHeader();
    hHeaderView->setDefaultSectionSize(175);
    hHeaderView->setSectionResizeMode(QHeaderView::Fixed);
    hHeaderView->setSectionsClickable(true);
    hHeaderView->setHighlightSections(true);
    hHeaderView->setSectionsMovable(false);

    variableView_->setMinimumHeight(298);
    variableView_->resizeRowsToContents();
    variableView_->resizeColumnsToContents();
    for (int i = 0; i < variableModel_->rowCount(); i++)
    {
        variableView_->verticalHeader()->resizeSection(i, 20);
    }

    auto addButton = new QToolButton;
    addButton->setObjectName(QStringLiteral("plusButton"));
    addButton->setToolTip(tr("<b>+</b> Add a variable."));

    auto removeButton = new QToolButton;
    removeButton->setObjectName(QStringLiteral("minusButton"));
    removeButton->setToolTip(tr("<b>-</b> Remove a variable."));

    auto buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addStretch();

    rawSettingsButton = new QPushButton(tr("Raw File Settings..."));
    rawSettingsButton->setProperty("mdButton", true);
    rawSettingsButton->setStyleSheet(QStringLiteral("QPushButton {margin-top: 15px}"));

    clearCustomVarsButton = new QPushButton(tr("Clear Custom Variables"));
    clearCustomVarsButton->setProperty("mdButton", true);
    clearCustomVarsButton->setStyleSheet(QStringLiteral("QPushButton {margin-top: 15px}"));

    auto varLayout = new QGridLayout;
    varLayout->addWidget(variableView_, 0, 0, -1, 1);
    varLayout->addLayout(buttonsLayout, 0, 1, 1, 1);
    varLayout->setColumnStretch(0, 1);
    varLayout->setColumnStretch(1, 0);
    varLayout->setRowStretch(2, 0);
    varLayout->setSizeConstraint(QLayout::SetNoConstraint);

    auto varGroup = new QGroupBox(tr("Data Columns Info"));
    varGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    varGroup->setFlat(true);
    varGroup->setLayout(varLayout);

    auto varScrollArea = new QScrollArea;
    varScrollArea->setWidget(varGroup);
    varScrollArea->setWidgetResizable(true);

    auto mainlayout = new QGridLayout(this);
    mainlayout->setRowMinimumHeight(0, 18);
    mainlayout->addWidget(rawSettingsButton, 1, 0, 1, 1, Qt::AlignTop);
    mainlayout->addWidget(clearCustomVarsButton, 2, 0, 1, 1, Qt::AlignTop);
    mainlayout->addWidget(varScrollArea, 0, 1, -1, -1);
    mainlayout->setRowStretch(3, 1);
    mainlayout->setColumnStretch(1, 1);
    mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
    setLayout(mainlayout);

    connect(variableModel_, &VariableModel::modified, [=]()
            { dlProject_->setModified(true); });

    connect(dlProject, &DlProject::projectModified,
            this, &DlRawfileDescTab::updateModels);

    connect(dlProject, &DlProject::projectChanged,
            this, &DlRawfileDescTab::updateModels);

    connect(addButton, &QToolButton::clicked,
            variableView_, &VariableView::addVar);
    connect(removeButton, &QToolButton::clicked,
            variableView_, &VariableView::removeVar);

    connect(rawSettingsButton, &QPushButton::clicked,
            this, &DlRawfileDescTab::showRawSettingsDialog);

    connect(clearCustomVarsButton, &QPushButton::clicked,[=]()
            { variableDelegate_->clearCustomVariableBuffer(); });

    // to trigger table editing with single click without altering the
    // editTriggers property, because that way the column selection
    // clicking on the header trigger the first cell editor
    connect(variableView_, SIGNAL(clicked(QModelIndex)),
            variableView_, SLOT(edit(QModelIndex)));
}

DlRawfileDescTab::~DlRawfileDescTab()
{
    delete rawSettingsDialog;
}

void DlRawfileDescTab::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    clearInstrModels();
    variableModel_->flush();

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void DlRawfileDescTab::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    variableModel_->flush();

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void DlRawfileDescTab::modelModified()
{
    dlProject_->setModified(true);
}

void DlRawfileDescTab::updateModels()
{
    AnemDescList *adl = dlProject_->anems();
    IrgaDescList *idl = dlProject_->irgas();

    QStringList models;
    int k = 0;
    for (const auto &anem : *adl)
    {
        ++k;
        const QString& aModel = anem.model();
        if (AnemDesc::isGoodAnemometer(anem))
        {
            models << tr("Sonic ") + QString::number(k) + QStringLiteral(": ") + aModel;
        }
    }

    k = 0;
    for (const auto &irga : *idl)
    {
        ++k;
        const QString& iModel = irga.model();
        if (IrgaDesc::isGoodIrga(irga))
        {
            models << tr("Irga ") + QString::number(k) + QStringLiteral(": ") + iModel;
        }
    }

    // NOTE: evaluate if necessary
    models << tr("Other");

    variableModel_->setInstrModels(models);
}

void DlRawfileDescTab::showRawSettingsDialog()
{
    if (!rawSettingsDialog)
    {
        rawSettingsDialog = new RawFileSettingsDialog(this, dlProject_);
    }
    rawSettingsDialog->refresh();
    rawSettingsDialog->show();
    rawSettingsDialog->raise();
    rawSettingsDialog->activateWindow();
}

void DlRawfileDescTab::clearInstrModels()
{
    QStringList modelList;
    modelList << tr("Other");
    variableModel_->setInstrModels(modelList);
}

void DlRawfileDescTab::onlineHelpTrigger_1()
{
//    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/")));
}
