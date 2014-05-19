/***************************************************************************
  splitterhandle.cpp
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QDialog>
#include <QApplication>
#include <QTime>

#include "splitterhandle.h"

SplitterHandle::SplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
{
}

QSize SplitterHandle::sizeHint() const
{
    return QSize(this->width(), 2);
}

void SplitterHandle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    QRadialGradient gradient(QPointF(rect().width()/2, rect().height()/2),
                             rect().width() * 3 / 4,
                             QPointF(rect().width()/2, rect().height()/2));
    gradient.setColorAt(0, QColor("#007FBD"));
    gradient.setColorAt(0.5, QColor(255,255,255,0));
    gradient.setColorAt(1, QColor(255,255,255,0));

    painter.fillRect(event->rect(), QBrush(gradient));
}

void SplitterHandle::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    buzz();
}

void SplitterHandle::buzz()
{
    int xp = x();
    int yp = y();
    QTime t;

    t.start();
    for (int i = 8; i > 0;)
    {
        QApplication::processEvents();
        if (t.elapsed() >= 1)
        {
            int delta = i >> 2;
            int dir = i & 3;
            int dx = ((dir == 1) || (dir == 2)) ? delta : -delta;
            int dy = (dir < 2) ? delta : -delta;
            move(xp + dx, yp + dy);
            t.restart();
            i--;
        }
    }
    move(xp, yp);
}
