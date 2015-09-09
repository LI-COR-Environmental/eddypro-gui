/***************************************************************************
  splitterhandle.cpp
  -------------------
  Copyright (C) 2013-2015, LI-COR Biosciences
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

#include "splitterhandle.h"

#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QTime>

SplitterHandle::SplitterHandle(Qt::Orientation orientation, QSplitter *parent)
    : QSplitterHandle(orientation, parent)
{
}

// set the height of the handle to 3 px
QSize SplitterHandle::sizeHint() const
{
    return QSize(this->width(), 5);
}

void SplitterHandle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QRadialGradient gradient(QPointF(rect().width() / 2.0, rect().height() / 2.0),
                             rect().width() * 3.0 / 4.0,
                             QPointF(rect().width() / 2.0, rect().height() / 2.0));
    gradient.setColorAt(0, QColor("#007FBD"));
    gradient.setColorAt(0.5, QColor(255,255,255,0));
    gradient.setColorAt(1, QColor(255,255,255,0));

    painter.fillRect(int(rect().width() / 2.0) - 10, 0, int(rect().width() / 2.0) + 10, 1, QBrush(gradient));
    painter.fillRect(int(rect().width() / 2.0) + 10, 0, rect().width(), 1, Qt::white);
    painter.fillRect(0, 1, rect().width(), 2, Qt::white);
    painter.fillRect(0, 2, rect().width(), 3, QBrush(gradient));
    painter.fillRect(0, 3, rect().width(), 4, Qt::white);
    painter.fillRect(int(rect().width() / 2.0) - 10, 4, int(rect().width() / 2.0) + 10, 5, QBrush(gradient));
    painter.fillRect(int(rect().width() / 2.0) + 10, 4, rect().width(), 5, Qt::white);
}

void SplitterHandle::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    // NOTE: commented out because of design review
//    buzz();

    // TODO: finish splitter tooltip management
//    tooltip_ = new QDialog(0);
////    tooltip_->resize(QSize(50, 30));

////    Qt::WindowFlags winFflags = Qt::ToolTip;
////    tooltip_->setWindowFlags(winFflags);

//    QPropertyAnimation *animation = new QPropertyAnimation(tooltip_, "geometry");
//    animation->setDuration(500);
//    animation->setDirection(QAbstractAnimation::Forward);
//    QRect startRect(QPoint(500, 500), QPoint(600, 600));
//    QRect endRect(QPoint(520, 520),QPoint(620, 620));
//    animation->setStartValue(startRect);
//    animation->setEndValue(endRect);
//    animation->start();
}

// NOTE: not used
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
