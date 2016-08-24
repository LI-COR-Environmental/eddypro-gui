/***************************************************************************
  angle_tableview.cpp
  -------------------
  Copyright (C) 2012-2016, LI-COR Biosciences
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

#include "angle_tableview.h"

#include <QMouseEvent>

AngleTableView::AngleTableView(QWidget *parent) :
    QTableView(parent)
{
}

void AngleTableView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    if (!item.isValid())
        clearSelection();

    QAbstractItemView::mousePressEvent(event);
}

QSize AngleTableView::sizeHint() const
{
    return QSize(200, 200);
}

int AngleTableView::sizeHintForRow(int row) const
{
    Q_UNUSED(row)
    return 17;
}
