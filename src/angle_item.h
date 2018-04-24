/***************************************************************************
  angle_item.h
  -------------------
  Copyright (C) 2012-2018, LI-COR Biosciences
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

#ifndef ANGLE_ITEM_H
#define ANGLE_ITEM_H

#include <QColor>

struct AngleItem
{
    explicit AngleItem(double angle = 30.0,
                       Qt::CheckState included = Qt::Checked,
                       QColor color = QColor("#aae6ff"))
        : angle_(angle),
          included_(included),
          color_(color)
    {
    }

    double angle_;
    Qt::CheckState included_;
    QColor color_;
};

#endif // ANGLE_ITEM_H
