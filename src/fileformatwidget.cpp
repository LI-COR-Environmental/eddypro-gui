/***************************************************************************
  fileformatwidget.cpp
  -------------------
  Variation of lineedit and browse widget
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#include "customdroplineedit.h"
#include "fileformatwidget.h"

#include "QPushButton"

FileFormatWidget::FileFormatWidget()
{
    button()->setText(tr("Set..."));
}

void FileFormatWidget::onButtonClick()
{
}

bool FileFormatWidget::isReadOnly() const
{
    return lineEdit()->isReadOnly();
}

void FileFormatWidget::setReadOnly(bool on)
{
    lineEdit()->setReadOnly(on);
}
