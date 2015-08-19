/***************************************************************************
  advmenudelegate.h
  -------------------
  Copyright (C) 2014-2015, LI-COR Biosciences
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

#ifndef ADVMENUDELEGATE_H
#define ADVMENUDELEGATE_H

#include <QStyledItemDelegate>

class AdvMenuDelegate : public QStyledItemDelegate
{
public:
    explicit AdvMenuDelegate(QObject* parent = nullptr);
    virtual ~AdvMenuDelegate();

    enum datarole {
        TextRole = Qt::UserRole + 100,
        IconRole = Qt::UserRole + 101
    };

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const;

    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
};

#endif  // ADVMENUDELEGATE_H
