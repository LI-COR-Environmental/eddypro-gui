/***************************************************************************
  irga_view.cpp
  -------------
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

#include "irga_view.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>

#include "irga_model.h"

// Constructor
IrgaView::IrgaView(QWidget *parent) :
    IrgaTableView(parent),
    addAction_(nullptr),
    removeAction_(nullptr),
    clearAction_(nullptr)
{
    // create context menu actions
    addAction_ = new QAction(tr("&Add Gas Analyzer"), this);
    addAction_->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    addAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    removeAction_ = new QAction(tr("&Remove Gas Analyzer"), this);
    removeAction_->setShortcut(QKeySequence(QKeySequence::ZoomOut));
    removeAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    clearAction_ = new QAction(tr("&Clear Selection"), this);
    clearAction_->setShortcut(QKeySequence(tr("Escape")));
    clearAction_->setShortcutContext(Qt::WidgetWithChildrenShortcut);

    connect(addAction_, &QAction::triggered, this, &IrgaView::addIrga);
    connect(removeAction_, &QAction::triggered, this, &IrgaView::removeIrga);
    connect(clearAction_, &QAction::triggered, this, &IrgaView::clearSelection);

    addAction(addAction_);
    addAction(removeAction_);
    addAction(clearAction_);
}

// Create and show context menu
void IrgaView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(addAction_);
    menu.addAction(removeAction_);
    menu.addSeparator();
    menu.addAction(clearAction_);
    menu.exec(event->globalPos());
}

// Add a new blank irga after the selected or after the last if there is
// no selection
void IrgaView::addIrga()
{
    int currCol = currentIndex().column();
    int lastCol = irgaCount();

    if (currCol == -1)
        currCol = lastCol;
    else
        ++currCol;

    // cast the model(), but it's not stricly necessary because
    // model() already returns the setModel() assigned to the view instance
    auto concreteModel = dynamic_cast<IrgaModel *>(model());

    concreteModel->insertColumns(currCol, 1);
    updateGeometries();
    concreteModel->flush();
    setCurrentIndex(concreteModel->index(IrgaModel::MANUFACTURER, currCol));
}

// Remove the selected irga or the last if there is no selection
void IrgaView::removeIrga()
{
    int currCol = currentIndex().column();
    int lastCol = irgaCount() - 1;

    if (currCol == -1)
        currCol = lastCol;

    dynamic_cast<IrgaModel *>(model())->removeColumns(currCol, 1);
    updateGeometries();
    clearSelection();
}

int IrgaView::irgaCount()
{
    return dynamic_cast<IrgaModel *>(model())->columnCount(QModelIndex());
}
