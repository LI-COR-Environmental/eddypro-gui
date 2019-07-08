/***************************************************************************
  clickablelabel.cpp
  ------------------
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
