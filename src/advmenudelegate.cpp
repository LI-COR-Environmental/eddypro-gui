/***************************************************************************
  advmenudelegate.cpp
  -------------------
  Copyright (C) 2014-2017, LI-COR Biosciences
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

#include "advmenudelegate.h"

#include <QApplication>
#include <QPainter>

AdvMenuDelegate::AdvMenuDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

AdvMenuDelegate::~AdvMenuDelegate()
{
}

QSize AdvMenuDelegate::sizeHint(const QStyleOptionViewItem& option,
                          const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return QSize(195, 68);
}
void AdvMenuDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QRect r = option.rect;

    if (option.state & QStyle::State_Selected)
    {
        QPen fontSelectedPen(Qt::white, 1, Qt::SolidLine);
        painter->setPen(fontSelectedPen);
    }

    auto icon = qvariant_cast<QIcon>(index.data(IconRole));

//#if defined(Q_OS_DARWIN)
//    auto pixmap = icon.pixmap(icon.actualSize(QSize(42, 42)));
//    auto pixmap = icon.pixmap(QSize(42, 42));
//    pixmap.setDevicePixelRatio(2.0);
//#endif
//    icon.addPixmap(pixmap, QIcon::Normal, QIcon::On);

    if (!icon.isNull())
    {
        icon.paint(painter, r.adjusted(8, 0, 8, 0), Qt::AlignVCenter | Qt::AlignLeft);
    }

#if defined(Q_OS_DARWIN)
    auto fontSize = 13;
#elif defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    auto fontSize = 10;
#endif

    auto font = QFont(QStringLiteral("Open Sans"), fontSize, QFont::Normal);
    QFontMetrics fm(font);
    auto text = qvariant_cast<QString>(index.data(TextRole));

    painter->setFont(font);

    auto text_width = fm.width(text);

    // manage text wrapping, centering the item text
    if (text_width <= 185)
    {
        painter->drawText(r.adjusted(54,
                                     (r.height() - fm.height()) / 2,
                                     44,
                                     (r.height() - fm.height()) / 2), text);
    }
    else
    {
        painter->drawText(r.adjusted(54,
                                     (r.height() - fm.height()*2 - fm.leading()) / 2,
                                     44,
                                     (r.height() - fm.height()*2 - fm.leading()) / 2), text);
    }

    painter->restore();
}
