/***************************************************************************
  hovercheckbox.cpp
  -----------------
  Copyright © 2016-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
