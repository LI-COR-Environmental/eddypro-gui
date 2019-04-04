/***************************************************************************
  dlinstrtab.cpp
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
 ***************************************************************************/

#include "dlinstrtab.h"

#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QToolButton>

#include "anem_delegate.h"
#include "anem_model.h"
#include "anem_view.h"
#include "dlproject.h"
#include "irga_delegate.h"
#include "irga_model.h"
#include "irga_view.h"

DlInstrTab::DlInstrTab(QWidget *parent, DlProject *dlProject) :
    QWidget(parent),
    dlProject_(dlProject)
{
    anemView_ = new AnemView(this);
    anemModel_ = new AnemModel(anemView_, dlProject_->anems());
    anemDelegate_ = new AnemDelegate(anemView_);

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

    auto addAnemButton = new QToolButton;
    addAnemButton->setObjectName(QStringLiteral("plusButton"));
    addAnemButton->setToolTip(tr("<b>+</b> Add a new anemometer."));

    auto removeAnemButton = new QToolButton;
    removeAnemButton->setObjectName(QStringLiteral("minusButton"));
    removeAnemButton->setToolTip(tr("<b>-</b> Remove the currently selected anemometer."));

    auto anemButtonsLayout = new QVBoxLayout;
    anemButtonsLayout->addWidget(addAnemButton);
    anemButtonsLayout->addWidget(removeAnemButton);
    anemButtonsLayout->addStretch();

    auto anemLayout = new QHBoxLayout;
    anemLayout->addWidget(anemView_);
    anemLayout->addLayout(anemButtonsLayout);
    anemLayout->setSizeConstraint(QLayout::SetNoConstraint);

    auto anemGroup = new QGroupBox(tr("Anemometers Info"));
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

    auto addIrgaButton = new QToolButton;
    addIrgaButton->setObjectName(QStringLiteral("plusButton"));
    addIrgaButton->setToolTip(tr("<b>+</b> Add a new gas analyzer."));

    auto removeIrgaButton = new QToolButton;
    removeIrgaButton->setObjectName(QStringLiteral("minusButton"));
    removeIrgaButton->setToolTip(tr("<b>-</b>Remove the currently selected gas analyzer."));

    auto irgaButtonsLayout = new QVBoxLayout;
    irgaButtonsLayout->addWidget(addIrgaButton);
    irgaButtonsLayout->addWidget(removeIrgaButton);
    irgaButtonsLayout->addStretch();

    auto irgaLayout = new QHBoxLayout;
    irgaLayout->addWidget(irgaView_);
    irgaLayout->addLayout(irgaButtonsLayout);
    irgaLayout->setSizeConstraint(QLayout::SetNoConstraint);

    auto irgaGroup = new QGroupBox(tr("Gas Analyzers Info"));
    irgaGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    irgaGroup->setFlat(true);
    irgaGroup->setToolTip(tr("<b>Gas analyzers info:</b> Describe gas analyzers used to collect the data you want to process."));
    irgaGroup->setMinimumHeight(378);
    irgaGroup->setLayout(irgaLayout);

    auto instrLayout = new QHBoxLayout;
    instrLayout->addWidget(anemGroup);
    instrLayout->addWidget(irgaGroup);
    instrLayout->setSpacing(0);
    instrLayout->setContentsMargins(0, 0, 0, 0);

    auto instrContainer = new QFrame;
    instrContainer->setFrameStyle(QFrame::NoFrame);
    instrContainer->setObjectName(QStringLiteral("instrContainer"));
    instrContainer->setLayout(instrLayout);

    auto instrumentScrollArea = new QScrollArea;
    instrumentScrollArea->setWidget(instrContainer);
    instrumentScrollArea->setWidgetResizable(true);

    auto mainlayout = new QHBoxLayout(this);
    mainlayout->addWidget(instrumentScrollArea);
    mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
    setLayout(mainlayout);

    connect(anemModel_, &AnemModel::modified, [=]()
            { dlProject_->setModified(true); });
    connect(addAnemButton, &QToolButton::clicked,
            anemView_, &AnemView::addAnem);
    connect(removeAnemButton, &QToolButton::clicked,
            anemView_, &AnemView::removeAnem);

    connect(irgaModel_, &IrgaModel::modified, [=]()
            { dlProject_->setModified(true); });
    connect(addIrgaButton, &QToolButton::clicked,
            irgaView_, &IrgaView::addIrga);
    connect(addIrgaButton, &QToolButton::clicked,
            this, &DlInstrTab::updateScrollBars);
    connect(removeIrgaButton, &QToolButton::clicked,
            irgaView_, &IrgaView::removeIrga);

    // NOTE: to trigger table editing with single click without altering the
    // editTriggers property, because that way the column selection
    // clicking on the header triggers the first cell editor
    connect(anemView_, SIGNAL(clicked(QModelIndex)),
            anemView_, SLOT(edit(QModelIndex)));
    connect(irgaView_, SIGNAL(clicked(QModelIndex)),
            irgaView_, SLOT(edit(QModelIndex)));
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

// NOTE: verify if it's still necessary
void DlInstrTab::updateScrollBars()
{
    QWidgetList wl = anemView_->scrollBarWidgets(Qt::AlignBottom);

    if (!wl.isEmpty())
    {
        wl.at(0)->update();
    }
}
