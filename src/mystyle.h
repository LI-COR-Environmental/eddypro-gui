/***************************************************************************
  mystyle.h
  -------------------
  Copyright (C) 2013-2015, LI-COR Biosciences
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
                        const QWidget* widget = nullptr) const
        {
            // TODO: to verify
            // hack for the menu icons size
            if (metric == QStyle::PM_SmallIconSize)
                return 16;

//            if (metric == PM_SplitterWidth)
//                return 10;

            // TODO: general, customize
//            if (metric == PM_DockWidgetSeparatorExtent)
//                return 7;

//            if (metric == PM_DockWidgetHandleExtent)
//                return 7;

//            if (metric == PM_DockWidgetTitleBarButtonMargin)
//                return 0;

//            if (metric == PM_DockWidgetTitleMargin)
//                return 0;

            return ProxyStyle::pixelMetric(metric, option, widget);
        }

    // TODO: general, implement a checkbox with rich text to have the indicator hover effect
//    void drawControl(ComplexControl control, const QStyleOptionComplex * option, QPainter * painter, const QWidget * widget = 0 ) const
//    {
//        switch (element) {

//            case CE_CheckBoxLabel:
//                if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
//                    uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);

//                    if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
//                        alignment |= Qt::TextHideMnemonic;
//                    QPixmap pix;
//                    QRect textRect = btn->rect;
//                    if (!btn->icon.isNull()) {
//                        pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
//                        proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
//                        if (btn->direction == Qt::RightToLeft)
//                            textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
//                        else
//                            textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
//                    }
//                    if (!btn->text.isEmpty()) {
//                        proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic,
//                            btn->palette, btn->state & State_Enabled, btn->text, QPalette::WindowText);
//                    }
//                }
//                break;
//        }
//    }
};

#endif // MYSTYLE_H
