/***************************************************************************
  tooltipfilter.cpp
  -----------------
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
        auto w = dynamic_cast<QWidget *>(obj);
        auto helpEvent = dynamic_cast<QHelpEvent *>(event);
        QPoint pos = helpEvent->globalPos();
        QString itemTooltip = w->toolTip();
        QRect rect = w->rect();

        // case of instrument tables inside a scrollarea viewport
        if (w->objectName() == QLatin1String("qt_scrollarea_viewport"))
        {
            auto view = qobject_cast<QAbstractItemView*>(obj->parent());
            if (!view)
            {
                return false;
            }
            w = view;

            QPoint position = helpEvent->pos();

            QModelIndex index = view->indexAt(position);
            if (!index.isValid())
            {
                return false;
            }
            rect = view->visualRect(index);
            itemTooltip = view->model()->data(index, Qt::ToolTipRole).toString();
        }

        switch (tooltipType_)
        {
            case TooltipType::NoTooltip:
                QToolTip::hideText();
                return true;
            case TooltipType::StdTooltip:
                QToolTip::showText(pos, itemTooltip, w, rect);
                return true;
            case TooltipType::CustomTooltip:
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
