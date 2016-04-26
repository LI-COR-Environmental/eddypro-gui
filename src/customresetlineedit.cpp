/***************************************************************************
  customresetlineedit.cpp
  -------------------
  Copyright (C) 2014, LI-COR Biosciences
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
 ***************************************************************************/

#include "customresetlineedit.h"

#include <QAction>
#include <QIcon>

CustomResetLineEdit::CustomResetLineEdit(QWidget *parent) :
    CustomLineEdit(parent),
    stdText_(QString())
{
    setDefaultIcon(QIcon(QStringLiteral(":/icons/reset-line")));

    connect(action(), &QAction::triggered,
            this, [=](){ setText(stdText_); });
}

void CustomResetLineEdit::setDefaultText(const QString &text)
{
    stdText_ = text;
    setText(stdText_);
    action()->setVisible(false);
}

// NOTE: never used
QString CustomResetLineEdit::defaultText() const
{
    return stdText_;
}

void CustomResetLineEdit::updateAction(const QString &text)
{
    bool isTextChanged = (text != stdText_);

    action()->setVisible(isTextChanged);

    // NOTE: without this the icon will be only grayed out
    if (isTextChanged)
    {
        setIcon(defaultIcon());
    }
    else
    {
        setIcon(QIcon());
    }
}
