/***************************************************************************
  customcombobox.cpp
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

#include "customcombobox.h"

#include <QModelIndex>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStyleOptionViewItem>
#include <QVariant>

CustomComboBox::CustomComboBox(QWidget *parent) :
    QComboBox(parent)
{
}

// it should override the ItemDelegate sizeHint
//QSize CustomComboBox::sizeHint(const QStyleOptionViewItem& option,
//                               const QModelIndex& index) const
//{
//    QString type = index.data(Qt::AccessibleDescriptionRole).toString();
//    if (type == QLatin1String("separator"))
//    {
//        return QSize(5, 5);
//    }
//    return itemDelegate()->sizeHint(option, index);
//}

void CustomComboBox::addSeparator()
{
    insertSeparator(count());
}

void CustomComboBox::paint(QPainter* painter,
                           const QStyleOptionViewItem& option,
                           const QModelIndex& index) const
{
    QString type = index.data(Qt::AccessibleDescriptionRole).toString();
    if (type == QLatin1String("separator"))
    {
        itemDelegate()->paint(painter, option, index);
        int y = (option.rect.top() + option.rect.bottom()) / 2;
        painter->setPen(option.palette.color(QPalette::Active, QPalette::Dark));
        painter->drawLine(option.rect.left(), y, option.rect.right(), y);
    }
    else
    {
        itemDelegate()->paint(painter, option, index);
    }

    if (type == QLatin1String("parent"))
    {
        QStyleOptionViewItem parentOption = option;
        parentOption.state |= QStyle::State_Enabled;
        itemDelegate()->paint(painter, parentOption, index);
    }
    else if (type == QLatin1String("child"))
    {
        QStyleOptionViewItem childOption = option;
//        int indent = option.fontMetrics.width(QString(4, QLatin1Char(' ')));
//        childOption.rect.adjust(indent, 0, 0, 0);
        childOption.textElideMode = Qt::ElideNone;
        itemDelegate()->paint(painter, childOption, index);
    }
}

void CustomComboBox::addParentItem(const QString& text)
{
    QStandardItem* item = new QStandardItem(text);
    item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable));
    item->setData(QStringLiteral("parent"), Qt::AccessibleDescriptionRole);

    QFont font = item->font();
    font.setBold(true);
    font.setItalic(true);
    item->setFont(font);

    QBrush b;
    b.setColor(QColor(QStringLiteral("#2986f5")));
    item->setForeground(b);

    QStandardItemModel* itemModel = static_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}

void CustomComboBox::addChildItem(const QString& text, const QVariant& userData)
{
//    QStandardItem* item = new QStandardItem(text + QString(4, QLatin1Char(' ')));
    QStandardItem* item = new QStandardItem(text);
    item->setData(userData, Qt::UserRole);
    item->setData(QStringLiteral("child"), Qt::AccessibleDescriptionRole);

    QStandardItemModel* itemModel = static_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}

void CustomComboBox::addChildItems(const QStringList& texts)
{
    for (const QString& text : texts)
    {
        addChildItem(text);
    }
}
