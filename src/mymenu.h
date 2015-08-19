/***************************************************************************
  mymenu.h
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

#ifndef MYMENU_H
#define MYMENU_H

#include <QDebug>
#include <QHelpEvent>
#include <QMenu>
#include <QToolTip>

#include "tooltipfilter.h"

class MyMenu : public QMenu
{
    Q_OBJECT

public:
    explicit MyMenu(QWidget* parent = nullptr)
        : QMenu(parent)
    {
    }

    bool event(QEvent* e)
    {
        if (QEvent::ToolTip == e->type())
        {
            // show action tooltip instead of widget tooltip
            QHelpEvent *he = static_cast<QHelpEvent*>(e);
            QAction *action = he ? actionAt(he->pos()) : 0;
            if (action)
            {
                if (action->property("show-tooltip").toBool())
                {
                    QToolTip::showText(he->globalPos(), action->toolTip(), this);
                }
                else
                {
                    QToolTip::hideText();
                }
                return true;
            }
        }
        return QMenu::event(e);
    }
};

#endif // MYMENU_H
