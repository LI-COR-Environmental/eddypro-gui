/***************************************************************************
  clicklabel.cpp
  --------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
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

#include "clicklabel.h"

#include <QApplication>

ClickLabel::ClickLabel(QWidget* parent, Qt::WindowFlags flags)
    : QLabel(parent, flags),
      headerData_(NoHeader)
{
}

ClickLabel::ClickLabel(const QString& text, QWidget* parent, Qt::WindowFlags flags)
    : QLabel(text, parent, flags),
      headerData_(NoHeader)
{
}

ClickLabel::ClickLabel(const ClickLabel& clabel)
    : headerData_(NoHeader)
{
    Q_UNUSED(clabel)
}

ClickLabel &ClickLabel::operator=(const ClickLabel &clabel)
{
    if (this != &clabel)
    {
        headerData_ = clabel.headerData_;
    }
    return *this;
}

void ClickLabel::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    time.start();
}

void ClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    if (time.elapsed() < QApplication::doubleClickInterval())
        emit clicked();
}

void ClickLabel::setHeaderData(HeaderData data)
{
    headerData_ = data;
}
