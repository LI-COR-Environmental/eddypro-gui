/***************************************************************************
  hovercheckbox.cpp
  -------------------
  Checkbox with hover effect
  -------------------
  Copyright (C) 2016, LI-COR Biosciences
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

#include "hovercheckbox.h"

#include <QStyleOptionButton>
#include <QStylePainter>

HoverCheckBox::HoverCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    isHover = false;
    isPressed = false;
}

void HoverCheckBox::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QStylePainter p(this);
    QStyleOptionButton opt;

    initStyleOption(&opt);

    if (isHover)
    {
        opt.state |= QStyle::State_MouseOver;
    }
    if (isPressed)
    {
        opt.state |= QStyle::State_Sunken;
    }
    p.drawControl(QStyle::CE_CheckBox, opt);
}

void HoverCheckBox::setHover()
{
    isHover = true;
    update();
}

void HoverCheckBox::setPressed()
{
    isPressed = true;
    update();
}

void HoverCheckBox::clearPressed()
{
    isPressed = false;
    update();
}

void HoverCheckBox::clearStates()
{
    isHover = false;
    isPressed = false;
    update();
}
