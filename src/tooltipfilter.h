/***************************************************************************
  tooltipfilter.h
  -------------------
  Filter to catch tooltips
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#ifndef TOOLTIPFILTER_H
#define TOOLTIPFILTER_H

#include <QObject>

class TooltipFilter : public QObject
{
    Q_OBJECT

    enum class TooltipType { NoTooltip, StdTooltip, CustomTooltip };
    bool tooltipOn_;
    TooltipType tooltipType_;

public:
    TooltipFilter(bool tooltipOn, QObject* parent = nullptr);
    void setTooltipAvailable(bool available);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void updateTooltipRequest(const QString&);
};

#endif // TOOLTIPFILTER_H
