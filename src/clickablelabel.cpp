/***************************************************************************
  clickablelabel.cpp
  -------------------
  Label with click signal
  -------------------
  Copyright (C) 2011-2017, LI-COR Biosciences
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

#include "clickablelabel.h"

#include <QMouseEvent>

ClickableLabel::ClickableLabel(QWidget *parent) :
    QLabel(parent)
{
}

void ClickableLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        emit pressed();
    }
    else if (event->button() ==  Qt::LeftButton)
    {
        emit pressed();
    }
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (contentsRect().contains(event->pos()))
    {
        if (event->buttons() == Qt::LeftButton)
        {
            emit released();
        }
        else if (event->button() ==  Qt::LeftButton)
        {
            emit released();
        }
    }
}

void ClickableLabel::mouseMoveEvent(QMouseEvent* event)
{
    if (contentsRect().contains(event->pos())
        && (event->buttons() == Qt::LeftButton
            || event->button() ==  Qt::LeftButton))
    {
        emit left();
    }
}
