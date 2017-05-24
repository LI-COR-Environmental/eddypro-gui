/***************************************************************************
  angle_tablemodel.cpp
  -------------------
  Copyright (C) 2012-2017, LI-COR Biosciences
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

#include "angle_tablemodel.h"

#include <QDebug>

#include "dbghelper.h"
#include "defs.h"
#include "widget_utils.h"

namespace {
// private constants
const int MaxColumns = 1;
const double MinDegrees = 0.0;
const double MaxDegrees = 360.0;
const double InitialAngle = 30.0;

static int colorHueDecStep_ = 1;
} // namespace unnamed


// FIXME: include angle checkbox with only 1 angle doesn't work
AngleTableModel::AngleTableModel(QObject *parent, QList<AngleItem> *angles)
    : QAbstractTableModel(parent),
      angles_(angles),
      offset_(0.0),
      skipPruning_(false)
{}

Qt::ItemFlags AngleTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
    {
        switch (index.column())
        {
            case DEGREES:
                    theFlags |= Qt::ItemIsSelectable
                              | Qt::ItemIsEditable
                              | Qt::ItemIsEnabled
                              | Qt::ItemIsUserCheckable;
                break;
            default: Q_ASSERT(false);
        }
    }
    return theFlags;
}

QVariant AngleTableModel::data(const QModelIndex &index, int role) const
{
//    DEBUG_FUNC_NAME

    int column = index.column();

    if (!index.isValid()
        || index.row() < 0
        || index.row() >= angles_->count()
        || column < 0
        || column >= MaxColumns)
    {
        return QVariant();
    }

    const AngleItem &item = angles_->at(index.row());

    if (role == Qt::DisplayRole)
    {
        switch (column)
        {
            case DEGREES:
                return QVariant(QString::number(item.angle_, 'f', 1)
                                + QStringLiteral(" [%1]").arg(Defs::DEGREE));
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::EditRole)
    {
        switch (column)
        {
            case DEGREES:
            {
                return QVariant(QString::number(item.angle_, 'f', 1));
            }
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::DecorationRole)
    {
        switch (column)
        {
            case DEGREES:
            {
                return item.color_;
            }
            default: Q_ASSERT(false);
        }
    }
    if (role == Qt::CheckStateRole)
    {
        switch (column)
        {
            case DEGREES:
                return item.included_;
            default: Q_ASSERT(false);
        }
    }
    return QVariant();
}

QVariant AngleTableModel::headerData(int section,
        Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case DEGREES:
                return tr("Degrees");
            default: Q_ASSERT(false);
        }
    }
    else if (orientation == Qt::Vertical)
    {
        return section + 1;
    }
    return QVariant();
}

int AngleTableModel::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : angles_->count();
}

int AngleTableModel::columnCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : MaxColumns;
}

bool AngleTableModel::setData(const QModelIndex &index,
                         const QVariant &value, int role)
{
    DEBUG_FUNC_NAME
    int row = index.row();
    int column = index.column();
    int numAngles = angles_->count();

    if (!index.isValid()
        || (role != Qt::EditRole && role != Qt::CheckStateRole)
        || row < 0
        || row >= numAngles
        || column < 0
        || column >= MaxColumns)
    {
        qDebug() << "setData init tests failed";
        return false;
    }
    qDebug() << "setData init tests passed";

    AngleItem item = angles_->value(row);
    int nextRow = (row + 1) % numAngles;

    AngleItem nextItem = angles_->value(nextRow);

    if (role == Qt::EditRole)
    {
        qDebug() << "EditRole";
        switch (column)
        {
            case DEGREES:
            {
                bool ok;
                double angle = value.toDouble(&ok);
                double currAngle = item.angle_;
                double currAngleSum = angleSum();
                double followingAngle = nextItem.angle_;
                qDebug() << "angle" << angle;
                qDebug() << "currAngle" << currAngle;
                qDebug() << "currAngleSum" << currAngleSum;
                qDebug() << "followingAngle" << followingAngle;

                if (angle > MaxDegrees)
                    angle = MaxDegrees;

                if (numAngles > 1)
                {
                    if (!ok || angle < MinDegrees)
                        return false;

                    // equal to 360 degrees
                    if (qFuzzyCompare(currAngleSum, MaxDegrees))
                    {
                        qDebug() << "currAngleSum" << currAngleSum;

                        // eat following
                        if (angle <= (currAngle + followingAngle))
                        {
                            qDebug() << "eat following";
                            nextItem.angle_ = followingAngle - (angle - currAngle);
                        }
                        // eat all following but no more
                        else
                        {
                            qDebug() << "eat all";
                            angle = currAngle + followingAngle;
                            nextItem.angle_ = 0.0;
                        }

                        qDebug() << "row" << row;
                        // set offset on first angle
                        if (row == (numAngles - 1))
                        {
                            qDebug() << "last row";
                            setOffset(offset_ + (followingAngle - nextItem.angle_));
                        }
                    }
                    // lesser than 360 degrees
                    else if (currAngleSum < MaxDegrees)
                    {
                        // fill up to 360
                        if (angle > (MaxDegrees - (currAngleSum - currAngle)))
                        {
                            angle = MaxDegrees - (currAngleSum - currAngle);
                            qDebug() << "angleSum()" << currAngleSum;
                            qDebug() << "max ANGLE" << angle;
                        }
                        else
                        {
                            qDebug() << "normal ANGLE" << angle;
                        }
                    }
                    // greater than 360 degrees
                    else
                    {
                        qDebug() << "angleSum()" << currAngleSum;
                        qDebug() << "to be prevented";
                    }
                }
                // one angle only
                else
                {
                    nextItem.angle_ = angle;
                }
                item.angle_ = angle;
                break;
            }
            default: Q_ASSERT(false);
        }
    }

    if (role == Qt::CheckStateRole)
    {
        qDebug() << "CheckStateRole";
        switch (column)
        {
            case DEGREES:
            {
                qDebug() << "1 item.included_" << item.included_;
                Qt::CheckState included = static_cast<Qt::CheckState>(value.toUInt());
                item.included_ = included;
                qDebug() << "2 item.included_" << item.included_;
                qDebug() << "2 included" << included;
                break;
            }
            default: Q_ASSERT(false);
        }
    }

    angles_->replace(row, item);
    angles_->replace(nextRow, nextItem);

    if (!skipPruning_)
        pruneEmptyRows();

    emit modified();
    emit dataChanged(index, index);

    return true;
}

bool AngleTableModel::insertRows(int row, int count, const QModelIndex&)
{
    DEBUG_FUNC_NAME

    if (count != 1) return false;
    if ((row < 0) || (row >= angles_->count()))
        row = angles_->count();

    if (angleSum() < MaxDegrees)
    {
        beginInsertRows(QModelIndex(), row, row + count - 1);
        for (int i = 0; i < count; ++i)
        {
            angles_->insert(row,
                            AngleItem(angleToBeInserted(),
                                      Qt::Checked,
                                      WidgetUtils::getColor(colorHueDecStep_)));
        }
        endInsertRows();
        ++colorHueDecStep_;
        emit modified();
        return true;
    }
    return false;
}

bool AngleTableModel::removeRows(int row, int count, const QModelIndex&)
{
    DEBUG_FUNC_NAME

    if (count != 1) return false;
    if ((row < 0) || (row >= angles_->count())) return false;

    double currAngleSum = angleSum();
    if (currAngleSum >= 0.0)
    {
        beginRemoveRows(QModelIndex(), row, row + count - 1);
        for (int i = 0; i < count; ++i)
            angles_->removeAt(row);
        endRemoveRows();
        ++colorHueDecStep_;
        if (angles_->count() == 0)
            setOffset(0.0);
        emit modified();
        return true;
    }
    return false;
}

void AngleTableModel::pruneEmptyRows()
{
    DEBUG_FUNC_NAME
    int rows = angles_->count();
    qDebug() << "rows" << rows;

    if (rows > 0)
    {
        for (int i = rows - 1; i >= 0; --i)
        {
            qDebug() << "i" << i;
            qDebug() << "value" << angles_->at(i).angle_;
            if (angles_->at(i).angle_ == 0.0)
            {
                qDebug() << "remove";
                removeRows(i, 1);
            }
        }
    }
}

// Clear the model
void AngleTableModel::clear()
{
    DEBUG_FUNC_NAME

    for (int i = 0; i < angles_->count(); ++i)
    {
        removeRows(i, 1);
    }
}

// Reset the model
void AngleTableModel::flush()
{
    beginResetModel();
    endResetModel();
}

double AngleTableModel::angleSum()
{
    DEBUG_FUNC_NAME
    double sum = 0.0;
    for (int row = 0; row < angles_->count(); ++row)
    {
        double value = angles_->value(row).angle_;
        sum += value;
        qDebug() << "row" << row;
        qDebug() << "value" << value;
    }
    qDebug() << "sum" << sum;
    return sum;
}

double AngleTableModel::angleToBeInserted()
{
    double angleAvailable = MaxDegrees - angleSum();
    double angleToBeInserted = angleAvailable;

    if (angleAvailable >= InitialAngle)
        angleToBeInserted = InitialAngle;

    return angleToBeInserted;
}

void AngleTableModel::setOffset(double alpha)
{
    DEBUG_FUNC_NAME
    qDebug() << "alpha" << alpha;

    if (!qFuzzyCompare(alpha, offset_))
    {
        offset_ = alpha;
        emit offsetChanged(alpha);
    }
}

void AngleTableModel::setSkipPruning(bool skip)
{
    DEBUG_FUNC_NAME

    if (skip != skipPruning_)
    {
        skipPruning_ = skip;
    }
}
