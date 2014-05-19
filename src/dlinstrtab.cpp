/***************************************************************************
  dlinstrtab.cpp
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

#include <QDebug>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QToolButton>
#include <QScrollArea>

#include "dbghelper.h"
#include "dlproject.h"
#include "anem_delegate.h"
#include "anem_model.h"
#include "anem_view.h"
#include "irga_delegate.h"
#include "irga_model.h"
#include "irga_view.h"
#include "dlinstrtab.h"

DlInstrTab::DlInstrTab(QWidget *parent, DlProject *dlProject) :
    QWidget(parent),
    dlProject_(dlProject)
{
    DEBUG_FUNC_NAME

    anemView_ = new AnemView(this);
    anemModel_ = new AnemModel(anemView_, dlProject_->anems());
    anemDelegate_ = new AnemDelegate(anemView_);

    // NOTE: test if delegate clipping is necessary
    anemDelegate_->setClipping(false);
    anemView_->setModel(anemModel_);
    anemView_->setItemDelegate(anemDelegate_);
    anemView_->setSelectionMode(QAbstractItemView::SingleSelection);
    anemView_->setSelectionBehavior(QAbstractItemView::SelectColumns);
    anemView_->setCornerButtonEnabled(false);
    anemView_->setWordWrap(false);


    anemView_->horizontalHeader()->setDefaultSectionSize(130);
    anemView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    anemView_->horizontalHeader()->setSectionsClickable(true);
    anemView_->horizontalHeader()->setHighlightSections(true);
    anemView_->horizontalHeader()->setSectionsMovable(false);
    anemView_->resizeRowsToContents();
    anemView_->resizeColumnsToContents();

    for (int i = 0; i < anemModel_->rowCount(); i++)
    {
        anemView_->verticalHeader()->resizeSection(i, 20);
    }

    QToolButton *addAnemButton = new QToolButton();
    addAnemButton->setObjectName(QStringLiteral("plusButton"));
    addAnemButton->setToolTip(tr("<b>+</b> Add a new anemometer."));

    QToolButton *removeAnemButton = new QToolButton();
    removeAnemButton->setObjectName(QStringLiteral("minusButton"));
    removeAnemButton->setToolTip(tr("<b>-</b> Remove the currently selected anemometer."));

    QVBoxLayout *anemButtonsLayout = new QVBoxLayout;
    anemButtonsLayout->addWidget(addAnemButton);
    anemButtonsLayout->addWidget(removeAnemButton);
    anemButtonsLayout->addStretch();

    QHBoxLayout *anemLayout = new QHBoxLayout;
    anemLayout->addWidget(anemView_);
    anemLayout->addLayout(anemButtonsLayout);
    anemLayout->setSizeConstraint(QLayout::SetNoConstraint);

    QGroupBox *anemGroup = new QGroupBox(tr("Anemometers Info"));
    anemGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    anemGroup->setFlat(true);
    anemGroup->setToolTip(tr("<b>Anemometers info:</b> Describe anemometers used at the flux station to collect data you want to process."));
    anemGroup->setLayout(anemLayout);
    anemGroup->setMinimumHeight(378);

    irgaView_ = new IrgaView(this);
    irgaModel_ = new IrgaModel(irgaView_, dlProject_->irgas());
    irgaDelegate_ = new IrgaDelegate(irgaView_);
    irgaView_->setModel(irgaModel_);
    irgaView_->setItemDelegate(irgaDelegate_);

    irgaView_->setSelectionMode(QAbstractItemView::SingleSelection);
    irgaView_->setSelectionBehavior(QAbstractItemView::SelectColumns);
    irgaView_->setCornerButtonEnabled(false);
    irgaView_->setWordWrap(false);

    irgaView_->horizontalHeader()->setDefaultSectionSize(145);
    irgaView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    irgaView_->horizontalHeader()->setSectionsClickable(true);
    irgaView_->horizontalHeader()->setHighlightSections(true);
    irgaView_->horizontalHeader()->setSectionsMovable(false);
    irgaView_->setMinimumWidth(320);
    irgaView_->resizeRowsToContents();
    irgaView_->resizeColumnsToContents();
    irgaView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    for (int i = 0; i < irgaModel_->rowCount(); i++)
    {
        irgaView_->verticalHeader()->resizeSection(i, 20);
    }

    QToolButton *addIrgaButton = new QToolButton();
    addIrgaButton->setObjectName(QStringLiteral("plusButton"));
    addIrgaButton->setToolTip(tr("<b>+</b> Add a new gas analyzer."));

    QToolButton *removeIrgaButton = new QToolButton();
    removeIrgaButton->setObjectName(QStringLiteral("minusButton"));
    removeIrgaButton->setToolTip(tr("<b>-</b>Remove the currently selected gas analyzer."));

    QVBoxLayout *irgaButtonsLayout = new QVBoxLayout;
    irgaButtonsLayout->addWidget(addIrgaButton);
    irgaButtonsLayout->addWidget(removeIrgaButton);
    irgaButtonsLayout->addStretch();

    QHBoxLayout *irgaLayout = new QHBoxLayout;
    irgaLayout->addWidget(irgaView_);
    irgaLayout->addLayout(irgaButtonsLayout);
    irgaLayout->setSizeConstraint(QLayout::SetNoConstraint);

    QGroupBox *irgaGroup = new QGroupBox(tr("Gas Analyzers Info"));
    irgaGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    irgaGroup->setFlat(true);
    irgaGroup->setToolTip(tr("<b>Gas analyzers info:</b> Describe gas analyzers used to collect the data you want to process."));
    irgaGroup->setMinimumHeight(378);
    irgaGroup->setLayout(irgaLayout);

    QHBoxLayout *instrLayout = new QHBoxLayout;
    instrLayout->addWidget(anemGroup);
    instrLayout->addWidget(irgaGroup);
    instrLayout->setSpacing(0);
    instrLayout->setContentsMargins(0, 0, 0, 0);

    QFrame* instrContainer = new QFrame();
    instrContainer->setFrameStyle(QFrame::NoFrame);
    instrContainer->setObjectName(QStringLiteral("instrContainer"));
    instrContainer->setLayout(instrLayout);

    QScrollArea* instrumentScrollArea = new QScrollArea;
    instrumentScrollArea->setWidget(instrContainer);
    instrumentScrollArea->setWidgetResizable(true);

    QHBoxLayout *mainlayout = new QHBoxLayout(this);
    mainlayout->addWidget(instrumentScrollArea);
    mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
    setLayout(mainlayout);

    connect(anemModel_, SIGNAL(modified()),
            this, SLOT(anemModelModified()));
    connect(addAnemButton, SIGNAL(clicked()),
            anemView_, SLOT(addAnem()));
    connect(removeAnemButton, SIGNAL(clicked()),
            anemView_, SLOT(removeAnem()));

    connect(irgaModel_, SIGNAL(modified()),
            this, SLOT(irgaModelModified()));
    connect(addIrgaButton, SIGNAL(clicked()),
            irgaView_, SLOT(addIrga()));
    connect(addIrgaButton, SIGNAL(clicked()),
            this, SLOT(updateScrollBars()));
    connect(removeIrgaButton, SIGNAL(clicked()),
            irgaView_, SLOT(removeIrga()));

    // NOTE: to trigger table editing with single click without altering the
    // editTriggers property, because that way the column selection
    // clicking on the header trigger the first cell editor
    connect(anemView_, SIGNAL(clicked(const QModelIndex &)),
            anemView_, SLOT(edit(const QModelIndex &)));
    connect(irgaView_, SIGNAL(clicked(const QModelIndex &)),
            irgaView_, SLOT(edit(const QModelIndex &)));
}

DlInstrTab::~DlInstrTab()
{
    DEBUG_FUNC_NAME
}

void DlInstrTab::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    anemModel_->flush();
    irgaModel_->flush();

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void DlInstrTab::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    anemModel_->flush();
    irgaModel_->flush();

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void DlInstrTab::anemViewRefresh()
{
    anemView_->clearSelection();
}

void DlInstrTab::anemModelModified()
{
    dlProject_->setModified(true);
}

void DlInstrTab::irgaViewRefresh()
{
    irgaView_->clearSelection();
}

void DlInstrTab::irgaModelModified()
{
    dlProject_->setModified(true);
}

void DlInstrTab::updateScrollBars()
{
    DEBUG_FUNC_NAME

    QWidgetList wl = anemView_->scrollBarWidgets(Qt::AlignBottom);

    if (!wl.isEmpty())
        wl.at(0)->update();
}
