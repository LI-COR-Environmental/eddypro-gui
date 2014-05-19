/***************************************************************************
  variable_view.cpp
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

#include <QAction>

#include "dbghelper.h"
#include "variable_model.h"
#include "variable_view.h"

VariableView::VariableView(QWidget *parent) :
    VariableTableView(parent),
    addAction_(0),
    removeAction_(0),
    clearAction_(0)
{
    // create context menu actions
    addAction_ = new QAction(tr("&Add Variable"), this);
    addAction_->setShortcut(QKeySequence(tr("Ctrl+Insert")));
    connect(addAction_, SIGNAL(triggered()), this, SLOT(addVar()));

    removeAction_ = new QAction(tr("&Remove Variable"), this);
    removeAction_->setShortcut(QKeySequence(tr("Ctrl+Delete")));
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeVar()));

    clearAction_ = new QAction(tr("&Clear Selection"), this);
    clearAction_->setShortcut(QKeySequence(tr("Escape")));
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(clearSelection()));

    addAction(addAction_);
    addAction(removeAction_);
    addAction(clearAction_);
}

VariableView::~VariableView()
{
    DEBUG_FUNC_NAME
}

// Create and show context menu
void VariableView::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
}

// Add a new blank anemometer after the selected or after the first if there is
// no selection
void VariableView::addVar()
{
    int currCol = currentIndex().column();
    int lastCol = varCount();

    if (currCol == -1)
        currCol = lastCol;
    else
        ++currCol;

    ((VariableModel *)model())->insertColumns(currCol, 1, QModelIndex());
    updateGeometries();
    ((VariableModel *)model())->flush();
    setCurrentIndex(model()->index(0, currCol));
}

// Remove the selected anemometer or the last if there is no selection
void VariableView::removeVar()
{
    int currCol = currentIndex().column();
    int lastCol = varCount() - 1;

    if (currCol == -1)
        currCol = lastCol;

    model()->removeColumns(currCol, 1, QModelIndex());
    updateGeometries();
    clearSelection();
}

int VariableView::varCount()
{
    return model()->columnCount(QModelIndex());
}
