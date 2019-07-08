/***************************************************************************
  mystyle.h
  -------------------
  Copyright (C) 2013-2017, LI-COR Biosciences
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

#ifndef MYSTYLE_H
#define MYSTYLE_H

#include "proxystyle.h"

class MyStyle : public ProxyStyle
{
    public:
        explicit MyStyle(const QString& style)
            : ProxyStyle(style) {}

        int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr,
                        const QWidget* widget = nullptr) const;
};

#endif // MYSTYLE_H

int MyStyle::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    // TODO: to verify
    // hack for the menu icons size
#if defined(Q_OS_WIN)
    if (metric == QStyle::PM_SmallIconSize)
        return 16;
#endif

//    if (metric == PM_SplitterWidth)
//        return 10;

    // TODO: general, customize
//    if (metric == PM_DockWidgetSeparatorExtent)
//        return 7;

//    if (metric == PM_DockWidgetHandleExtent)
//        return 7;

    if (metric == PM_DockWidgetTitleBarButtonMargin)
        return 0;

    if (metric == PM_DockWidgetTitleMargin)
        return 0;

    return ProxyStyle::pixelMetric(metric, option, widget);
}
