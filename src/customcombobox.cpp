/***************************************************************************
  customcombobox.cpp
  ------------------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
    auto item = new QStandardItem(text);
    item->setFlags(item->flags() & ~(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable));
    item->setData(QStringLiteral("parent"), Qt::AccessibleDescriptionRole);

    QFont font = item->font();
    font.setBold(true);
    font.setItalic(true);
    item->setFont(font);

    QBrush b;
    b.setColor(QColor(QStringLiteral("#2986f5")));
    item->setForeground(b);

    auto itemModel = dynamic_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}

void CustomComboBox::addChildItem(const QString& text, const QVariant& userData)
{
    auto item = new QStandardItem(text);
    item->setData(userData, Qt::UserRole);
    item->setData(QStringLiteral("child"), Qt::AccessibleDescriptionRole);

    auto itemModel = dynamic_cast<QStandardItemModel*>(model());
    itemModel->appendRow(item);
}

void CustomComboBox::addChildItems(const QStringList& texts)
{
    for (const QString& text : texts)
    {
        addChildItem(text);
    }
}
