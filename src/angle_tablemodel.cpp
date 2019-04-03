/***************************************************************************
  angle_tablemodel.cpp
  --------------------
  Copyright © 2012-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#include "angle_tablemodel.h"

#include <QDebug>

#include "defs.h"
#include "widget_utils.h"

namespace {
// private constants
const int MaxColumns = 1;
const double MinDegree = 0.0;
const double MaxDegree = 360.0;
const double DefaultSectorWidth = 30.0;

int colorHueDecStep_ = 1;
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
        return false;
    }

    AngleItem item = angles_->value(row);
    int nextRow = (row + 1) % numAngles;

    AngleItem nextItem = angles_->value(nextRow);

    if (role == Qt::EditRole)
    {
        switch (column)
        {
            case DEGREES:
            {
                bool ok;
                double angle = value.toDouble(&ok);
                double currAngle = item.angle_;
                double currAngleSum = angleSum();
                double followingAngle = nextItem.angle_;

                if (angle > MaxDegree)
                    angle = MaxDegree;

                if (numAngles > 1)
                {
                    if (!ok || angle < MinDegree)
                        return false;

                    // equal to 360 degrees
                    if (qFuzzyCompare(currAngleSum, MaxDegree))
                    {
                        // eat following
                        if (angle <= (currAngle + followingAngle))
                        {
                            nextItem.angle_ = followingAngle - (angle - currAngle);
                        }
                        // eat all following but no more
                        else
                        {
                            angle = currAngle + followingAngle;
                            nextItem.angle_ = 0.0;
                        }

                        // set offset on first angle
                        if (row == (numAngles - 1))
                        {
                            setOffset(offset_ + (followingAngle - nextItem.angle_));
                        }
                    }
                    // lesser than 360 degrees
                    else if (currAngleSum < MaxDegree)
                    {
                        // fill up to 360
                        if (angle > (MaxDegree - (currAngleSum - currAngle)))
                        {
                            angle = MaxDegree - (currAngleSum - currAngle);
                        }
                        else
                        {
                            qDebug() << "normal ANGLE" << angle;
                        }
                    }
                    // greater than 360 degrees
                    else
                    {
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
        switch (column)
        {
            case DEGREES:
            {
                auto included = static_cast<Qt::CheckState>(value.toUInt());
                item.included_ = included;
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

bool AngleTableModel::setHeaderData(int, Qt::Orientation, const QVariant &, int)
{
    return false;
}

bool AngleTableModel::insertRows(int row, int count, const QModelIndex&)
{
    if (count != 1) return false;
    if ((row < 0) || (row >= angles_->count()))
        row = angles_->count();

    if (angleSum() < MaxDegree)
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
    int rows = angles_->count();

    if (rows > 0)
    {
        for (int i = rows - 1; i >= 0; --i)
        {
            if (angles_->at(i).angle_ == 0.0)
            {
                removeRows(i, 1);
            }
        }
    }
}

// Clear the model
void AngleTableModel::clear()
{
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
    double sum = 0.0;
    for (int row = 0; row < angles_->count(); ++row)
    {
        double value = angles_->value(row).angle_;
        sum += value;
    }
    return sum;
}

double AngleTableModel::angleToBeInserted()
{
    double angleAvailable = MaxDegree - angleSum();
    double angleToBeInserted = angleAvailable;

    if (angleAvailable >= DefaultSectorWidth)
        angleToBeInserted = DefaultSectorWidth;

    return angleToBeInserted;
}

void AngleTableModel::setOffset(double alpha)
{
    if (!qFuzzyCompare(alpha, offset_))
    {
        offset_ = alpha;
        emit offsetChanged(alpha);
    }
}

void AngleTableModel::setSkipPruning(bool skip)
{
    if (skip != skipPruning_)
    {
        skipPruning_ = skip;
    }
}
