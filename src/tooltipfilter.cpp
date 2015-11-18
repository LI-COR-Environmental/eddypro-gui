/***************************************************************************
  tooltipfilter.cpp
  -------------------
  Filter to catch tooltips
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#include "tooltipfilter.h"

#include <QDebug>
#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QAbstractItemView>

TooltipFilter::TooltipFilter(bool tooltipOn, QObject *parent)
    : QObject(parent),
      tooltipOn_(tooltipOn),
      tooltipType_(TooltipType::StdTooltip)
{
    if (!tooltipOn_)
        tooltipType_ = TooltipType::NoTooltip;
}

bool TooltipFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
//        qDebug() << "QEvent::ToolTip";
//        qDebug() << "obj" << obj;
//        qDebug() << "obj->parent" << obj->parent();

        QWidget *w = static_cast<QWidget *>(obj);
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QPoint pos = helpEvent->globalPos();
        QString itemTooltip = w->toolTip();
        QRect rect = w->rect();

        // case of instrument tables inside a scrollarea viewport
        if (w->objectName() == QLatin1String("qt_scrollarea_viewport"))
        {
            QAbstractItemView* view = qobject_cast<QAbstractItemView*>(obj->parent());
            if (!view)
            {
                return false;
            }
            w = view;

            QPoint position = helpEvent->pos();

            QModelIndex index = view->indexAt(position);
            if (!index.isValid())
            {
//                qDebug() << "no valid index";
                return false;
            }
//            qDebug() << "index" << index;
            rect = view->visualRect(index);
            itemTooltip = view->model()->data(index, Qt::ToolTipRole).toString();
//            qDebug() << "itemTooltip" << itemTooltip;
        }

        switch (tooltipType_)
        {
            case TooltipType::NoTooltip:
                qDebug() << "NoTooltip";
                QToolTip::hideText();
                return true;
            case TooltipType::StdTooltip:
                qDebug() << "StdTooltip" << itemTooltip;
                QToolTip::showText(pos, itemTooltip, w, rect);
                return true;
            case TooltipType::CustomTooltip:
                qDebug() << "CustomTooltip";
                break;
            default:
                QToolTip::hideText();
                emit updateTooltipRequest(itemTooltip);
                return true;
        }
    }

    return QObject::eventFilter(obj, event);
}

void TooltipFilter::setTooltipAvailable(bool available)
{
    tooltipOn_ = available;

    if (tooltipOn_)
        tooltipType_ = TooltipType::StdTooltip;
    else
        tooltipType_ = TooltipType::NoTooltip;
}
