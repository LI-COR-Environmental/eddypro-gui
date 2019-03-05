/***************************************************************************
  irga_view.cpp
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
