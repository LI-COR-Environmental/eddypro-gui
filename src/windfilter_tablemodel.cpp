/***************************************************************************
  angle_tablemodel.cpp
  -------------------
  Copyright (C) 2012-2018, LI-COR Biosciences
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

#include "windfilter_tablemodel.h"

#include <QDebug>

#include "defs.h"
#include "widget_utils.h"

namespace {
// private constants
const int MaxColumns = 2;
const double MinDegree = 0.0;
const double MaxDegree = 360.0;
const double DefaultSectorWidth = 10.0;
const double MaxSectors = 16;

int colorHueDecStep_ = 1;
} // namespace unnamed

WindFilterTableModel::WindFilterTableModel(QObject *parent, QList<SectorItem> *sectors)
    : QAbstractTableModel(parent),
      sectors_(sectors)
{}

Qt::ItemFlags WindFilterTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
    {
        switch (index.column())
        {
            case START_ANGLE:
            case END_ANGLE:
                    theFlags |= Qt::ItemIsSelectable
                              | Qt::ItemIsEditable
                              | Qt::ItemIsEnabled;
                break;
            default: Q_ASSERT(false);
        }
    }
    return theFlags;
}

QVariant WindFilterTableModel::data(const QModelIndex &index, int role) const
{
    auto column = index.column();

    if (!index.isValid()
        || index.row() < 0
        || index.row() >= sectors_->count()
        || column < 0
        || column >= MaxColumns)
    {
        return QVariant();
    }

    const SectorItem &item = sectors_->at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch (column)
        {
            case START_ANGLE:
                return QVariant(QString::number(item.startAngle_, 'f', 1)
                                + QStringLiteral(" [%1]").arg(Defs::DEGREE));
            case END_ANGLE:
                return QVariant(QString::number(item.endAngle_, 'f', 1)
                                + QStringLiteral(" [%1]").arg(Defs::DEGREE));
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::EditRole)
    {
        switch (column)
        {
            case START_ANGLE:
            {
                return QVariant(QString::number(item.startAngle_, 'f', 1));
            }
            case END_ANGLE:
            {
                return QVariant(QString::number(item.endAngle_, 'f', 1));
            }
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::DecorationRole)
    {
        switch (column)
        {
            case START_ANGLE:
            case END_ANGLE:
            {
                return item.color_;
            }
            default: Q_ASSERT(false);
        }
    }
    return QVariant();
}

QVariant WindFilterTableModel::headerData(int section,
        Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case START_ANGLE:
                return tr("Start Angle");
            case END_ANGLE:
                return tr("End Angle");
            default: Q_ASSERT(false);
        }
    }
    else if (orientation == Qt::Vertical)
    {
        return section + 1;
    }
    return QVariant();
}

int WindFilterTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : sectors_->count();
}

int WindFilterTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MaxColumns;
}

bool WindFilterTableModel::setData(const QModelIndex &index,
                         const QVariant &value, int role)
{
    auto row = index.row();
    auto column = index.column();
    auto numSectors = sectors_->count();

    if (!index.isValid()
        || (role != Qt::EditRole)
        || row < 0
        || row >= numSectors
        || column < 0
        || column >= MaxColumns)
    {
        return false;
    }

    auto currentItem = sectors_->value(row);

    if (role == Qt::EditRole)
    {
        switch (column)
        {
            case START_ANGLE:
            {
                // update only if new value is not equal to the old value
                if (value == currentItem.startAngle_) return false;

                bool ok;
                double startAngle = value.toDouble(&ok);
                currentItem.startAngle_ = startAngle;
                break;
            }
            case END_ANGLE:
            {
                if (value == currentItem.endAngle_) return false;

                bool ok;
                double startAngle = value.toDouble(&ok);
                currentItem.endAngle_ = startAngle;
                break;
            }
            default:
                Q_ASSERT(false);
                return false;
        }
    }

    sectors_->replace(row, currentItem);

    emit modified();

    // whole column may have changed
//    emit dataChanged(index, index);
    emit dataChanged(index.sibling(row, START_ANGLE),
                     index.sibling(row, END_ANGLE));

    return true;
}

bool WindFilterTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    return false;
}

bool WindFilterTableModel::insertRows(int row, int count, const QModelIndex&)
{
    if (sectors_->count() >= MaxSectors) return false;
    if (angleSum() >= MaxDegree) return false;
    if (count != 1) return false;

    if ((row < 0) || (row >= sectors_->count()))
        row = sectors_->count();

    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (auto i = 0; i < count; ++i)
    {
        auto sector = SectorItem(startAngleToBeInserted(),
                                 endAngleToBeInserted(),
                                 WidgetUtils::getColor(colorHueDecStep_));
        sectors_->insert(row, sector);
    }
    ++colorHueDecStep_;
    endInsertRows();

    emit modified();
    return true;
}

bool WindFilterTableModel::removeRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false;
    if ((row < 0) || (row >= sectors_->count())) return false;

    auto currAngleSum = angleSum();
    if (currAngleSum >= 0.0)
    {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        for (auto i = 0; i < count; ++i)
            sectors_->removeAt(row);
        endRemoveRows();
        ++colorHueDecStep_;
        emit modified();
        return true;
    }
    return false;
}

// Clear the model
void WindFilterTableModel::clear()
{
    for (auto i = 0; i < sectors_->count(); ++i)
    {
        removeRows(i, 1);
    }
}

// Reset the model
void WindFilterTableModel::flush()
{
    beginResetModel();
    endResetModel();
}

double WindFilterTableModel::angleSum()
{
    auto sum = 0.0;
    for (auto row = 0; row < sectors_->count(); ++row)
    {
        auto value = sectors_->value(row).endAngle_ - sectors_->value(row).startAngle_;
        sum += value;
    }
    return sum;
}

double WindFilterTableModel::startAngleToBeInserted()
{
    return angleSum();
}

double WindFilterTableModel::endAngleToBeInserted()
{
    auto endAngleToBeInserted = startAngleToBeInserted() + DefaultSectorWidth;

    if (endAngleToBeInserted >= MaxDegree)
        endAngleToBeInserted = MaxDegree;

    return endAngleToBeInserted;
}
