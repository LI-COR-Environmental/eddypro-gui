/***************************************************************************
  irga_view.cpp
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
#include <QAction>

#include "dbghelper.h"
#include "irga_model.h"
#include "irga_view.h"

// Constructor
IrgaView::IrgaView(QWidget *parent) :
    IrgaTableView(parent),
    addAction_(0),
    removeAction_(0),
    clearAction_(0)
{
    // create context menu actions
    addAction_ = new QAction(tr("&Add Irga"), this);
    addAction_->setShortcut(QKeySequence(tr("Ctrl+Insert")));
    connect(addAction_, SIGNAL(triggered()), this, SLOT(addIrga()));

    removeAction_ = new QAction(tr("&Remove Irga"), this);
    removeAction_->setShortcut(QKeySequence(tr("Ctrl+Delete")));
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeIrga()));

    clearAction_ = new QAction(tr("&Clear Selection"), this);
    clearAction_->setShortcut(QKeySequence(tr("Escape")));
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(clearSelection()));

    addAction(addAction_);
    addAction(removeAction_);
    addAction(clearAction_);
}

IrgaView::~IrgaView()
{
    DEBUG_FUNC_NAME
}

// Create and show context menu
void IrgaView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
}

// Add a new blank irga after the selected or after the last if there is
// no selection
void IrgaView::addIrga()
{
    DEBUG_FUNC_NAME

    int currCol = currentIndex().column();
    int lastCol = irgaCount();

    if (currCol == -1)
        currCol = lastCol;
    else
        ++currCol;

    ((IrgaModel *)model())->insertColumns(currCol, 1, QModelIndex());
    updateGeometries();
    ((IrgaModel *)model())->flush();
    setCurrentIndex(model()->index(IrgaModel::MANUFACTURER, currCol));
}

// Remove the selected irga or the last if there is no selection
void IrgaView::removeIrga()
{
    DEBUG_FUNC_NAME

    int currCol = currentIndex().column();
    int lastCol = irgaCount() - 1;

    if (currCol == -1)
        currCol = lastCol;

    ((IrgaModel *)model())->removeColumns(currCol, 1, QModelIndex());
    updateGeometries();
    clearSelection();
}

int IrgaView::irgaCount()
{
    return ((IrgaModel *)model())->columnCount(QModelIndex());
}
