/***************************************************************************
  splitterhandle.cpp
  ------------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
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
    return {this->width(), 5};
}

void SplitterHandle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QRadialGradient gradient(QPointF(rect().width() / 2.0, rect().height() / 2.0),
                             rect().width() * 3.0 / 4.0,
                             QPointF(rect().width() / 2.0, rect().height() / 2.0));
    gradient.setColorAt(0, QColor(0, 127, 189)); // #007FBD
    gradient.setColorAt(0.5, QColor(255, 255, 255, 0));
    gradient.setColorAt(1, QColor(255, 255, 255, 0));

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

// NOTE: not used anymore
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
