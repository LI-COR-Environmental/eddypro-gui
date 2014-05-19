/***************************************************************************
  anem_view.cpp
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
#include <QDebug>

#include "dbghelper.h"
#include "anem_model.h"
#include "anem_view.h"

// Constructor
AnemView::AnemView(QWidget *parent) :
    AnemTableView(parent),
    addAction_(0),
    removeAction_(0),
    clearAction_(0)
{
    // create context menu actions
    addAction_ = new QAction(tr("&Add Anemometer"), this);
    addAction_->setShortcut(QKeySequence(tr("Ctrl+Insert")));
    connect(addAction_, SIGNAL(triggered()), this, SLOT(addAnem()));

    removeAction_ = new QAction(tr("&Remove Anemometer"), this);
    removeAction_->setShortcut(QKeySequence(tr("Ctrl+Delete")));
    connect(removeAction_, SIGNAL(triggered()), this, SLOT(removeAnem()));

    clearAction_ = new QAction(tr("&Clear Selection"), this);
    connect(clearAction_, SIGNAL(triggered()), this, SLOT(clearSelection()));

    addAction(addAction_);
    addAction(removeAction_);
    addAction(clearAction_);
}

AnemView::~AnemView()
{
    DEBUG_FUNC_NAME
}

// Add a new blank anemometer after the selected or after the last if there is
// no selection
void AnemView::addAnem()
{
    DEBUG_FUNC_NAME

    int currCol = currentIndex().column();
    int lastCol = anemCount();

    qDebug() << "currCol (1):" << currCol
             << "lastCol" << lastCol;

    if (currCol == -1)
        currCol = lastCol;
    else
        ++currCol;

    qDebug() << "currCol (2):" << currCol;

    ((AnemModel *)model())->insertColumns(currCol, 1, QModelIndex());
    updateGeometries();
    ((AnemModel *)model())->flush();
    setCurrentIndex(model()->index(AnemModel::MANUFACTURER, currCol));
}

// Remove the selected anemometer or the last if there is no selection
void AnemView::removeAnem()
{
    DEBUG_FUNC_NAME

    int currCol = currentIndex().column();
    int lastCol = anemCount() - 1;

    qDebug() << "currCol (1):" << currCol
             << "lastCol" << lastCol;

    if (currCol == -1)
        currCol = lastCol;

    qDebug() << "currCol (2):" << currCol;

    qDebug() << "currentIndex().row()" << currentIndex().row();
    qDebug() << "currentIndex().column()" << currentIndex().column();

    model()->removeColumns(currCol, 1);
    updateGeometries();
    clearSelection();
}

int AnemView::anemCount()
{
    return ((AnemModel *)model())->columnCount(QModelIndex());
}
