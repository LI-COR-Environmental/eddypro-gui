/***************************************************************************
  dlrawfiledesctab.cpp
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
 ***************************************************************************/

#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QToolButton>
#include <QPushButton>
#include <QScrollArea>
#include <QDebug>

#include "dlproject.h"
#include "dbghelper.h"
#include "rawfilesettingsdialog.h"
#include "variable_delegate.h"
#include "variable_model.h"
#include "variable_view.h"
#include "dlrawfiledesctab.h"

DlRawfileDescTab::DlRawfileDescTab(QWidget *parent, DlProject *dlProject) :
    QWidget(parent),
    dlProject_(dlProject),
    rawSettingsDialog(0),
    fieldSepTOB1Flag_(false)
{
    DEBUG_FUNC_NAME

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

    QToolButton *addButton = new QToolButton();
    addButton->setObjectName(QStringLiteral("plusButton"));
    addButton->setToolTip(tr("<b>+</b> Add a variable."));

    QToolButton *removeButton = new QToolButton();
    removeButton->setObjectName(QStringLiteral("minusButton"));
    removeButton->setToolTip(tr("<b>-</b> Remove a variable."));

    QVBoxLayout *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addStretch();

    rawSettingsButton = new QPushButton(tr("Raw File Settings..."));
    rawSettingsButton->setProperty("mdButton", true);
    rawSettingsButton->setStyleSheet(QStringLiteral("QPushButton {margin-top: 15px}"));

    QGridLayout *varLayout = new QGridLayout;
    varLayout->addWidget(variableView_, 0, 0, -1, 1);
    varLayout->addLayout(buttonsLayout, 0, 1, 1, 1);
    varLayout->setColumnStretch(0, 1);
    varLayout->setColumnStretch(1, 0);
    varLayout->setSizeConstraint(QLayout::SetNoConstraint);

    QGroupBox *varGroup = new QGroupBox(tr("Data Columns Info"));
    varGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    varGroup->setFlat(true);
    varGroup->setLayout(varLayout);

    QScrollArea* varScrollArea = new QScrollArea;
    varScrollArea->setWidget(varGroup);
    varScrollArea->setWidgetResizable(true);

    QGridLayout *mainlayout = new QGridLayout(this);
    mainlayout->setRowMinimumHeight(0, 18);
    mainlayout->addWidget(rawSettingsButton, 1, 0, 1, 1, Qt::AlignTop);
    mainlayout->addWidget(varScrollArea, 0, 1, -1, -1);
    mainlayout->setColumnStretch(1, 1);
    mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
    setLayout(mainlayout);

    connect(variableModel_, SIGNAL(modified()),
            this, SLOT(modelModified()));

    connect(dlProject, SIGNAL(projectModified()),
            this, SLOT(updateModels()));

    connect(dlProject, SIGNAL(projectChanged()),
            this, SLOT(updateModels()));

    connect(addButton, SIGNAL(clicked()),
            variableView_, SLOT(addVar()));
    connect(removeButton, SIGNAL(clicked()),
            variableView_, SLOT(removeVar()));

    connect(rawSettingsButton, SIGNAL(clicked()), this, SLOT(showRawSettingsDialog()));

    // to trigger table editing with single click without altering the
    // editTriggers property, because that way the column selection
    // clicking on the header trigger the first cell editor
    connect(variableView_, SIGNAL(clicked(const QModelIndex &)),
            variableView_, SLOT(edit(const QModelIndex &)));
}

DlRawfileDescTab::~DlRawfileDescTab()
{
    DEBUG_FUNC_NAME
    delete rawSettingsDialog;
}

void DlRawfileDescTab::reset()
{
    DEBUG_FUNC_NAME

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
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    variableModel_->flush();

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void DlRawfileDescTab::rawfileViewRefresh()
{
    variableView_->clearSelection();
}

void DlRawfileDescTab::modelModified()
{
    dlProject_->setModified(true);
}

void DlRawfileDescTab::updateModels()
{
    DEBUG_FUNC_NAME

    AnemDescList *adl = dlProject_->anems();
    IrgaDescList *idl = dlProject_->irgas();

    QStringList models;
    int k = 0;
    foreach (const AnemDesc& anem, *adl)
    {
        ++k;
        const QString aModel = anem.model();
        if (AnemDesc::isGoodAnemometer(anem))
        {
            models << tr("Sonic ") + QString::number(k) + QStringLiteral(": ") + aModel;
        }
    }

    k = 0;
    foreach (const IrgaDesc& irga, *idl)
    {
        ++k;
        const QString iModel = irga.model();
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
    DEBUG_FUNC_NAME
    qDebug() << "dlProject_->fieldSep()" << dlProject_->fieldSep();
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
    DEBUG_FUNC_NAME

    QStringList modelList;
    modelList << tr("Other");
    variableModel_->setInstrModels(modelList);
}

// NOTE: not used
void DlRawfileDescTab::onlineHelpTrigger_1()
{
//    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#")));
}
