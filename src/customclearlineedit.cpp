/***************************************************************************
  customclearlineedit.cpp
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

#include "customclearlineedit.h"

#include <QAction>
#include <QDebug>
#include <QIcon>

CustomClearLineEdit::CustomClearLineEdit(QWidget *parent) :
    CustomLineEdit(parent)
{
    setDefaultIcon(QIcon(QStringLiteral(":/icons/clear-line")));

    connect(action(), &QAction::triggered,
            this, &QLineEdit::clear);

    // init
    updateAction(QString());
}

void CustomClearLineEdit::setDisconnectedAction() const
{
    disconnect(action(), &QAction::triggered,
               this, &QLineEdit::clear);
}

void CustomClearLineEdit::updateAction(const QString &text)
{
    action()->setVisible(!text.isEmpty());

    // NOTE: without this the icon would be only grayed out
    if (text.isEmpty())
    {
        setIcon(QIcon());
    }
    else
    {
        setIcon(defaultIcon());
    }
}
