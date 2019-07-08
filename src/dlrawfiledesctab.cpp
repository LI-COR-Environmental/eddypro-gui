/***************************************************************************
  dlrawfiledesctab.cpp
  --------------------
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
 ***************************************************************************/

#include "dlrawfiledesctab.h"

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QPushButton>
#include <QScrollArea>
#include <QToolButton>

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
