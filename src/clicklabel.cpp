/***************************************************************************
  clicklabel.cpp
  -------------------
  QLabel class with clicked signal
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

#include <QApplication>

#include "clicklabel.h"

ClickLabel::ClickLabel(QWidget* parent, Qt::WindowFlags flags)
    : QLabel(parent, flags),
      headerData_(NoHeader)
{
}

ClickLabel::ClickLabel(const QString& text, QWidget* parent, Qt::WindowFlags flags)
    : QLabel(text, parent, flags)
{
}

ClickLabel::ClickLabel(const ClickLabel& clabel)
    : QLabel()
{
    Q_UNUSED(clabel)
}

ClickLabel::~ClickLabel()
{}

void ClickLabel::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    time.start();
}

void ClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    if (time.elapsed() < qApp->doubleClickInterval())
        emit clicked();
}

void ClickLabel::setHeaderData(HeaderData data)
{
    headerData_ = data;
}
