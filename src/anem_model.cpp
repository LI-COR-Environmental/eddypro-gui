/***************************************************************************
  anem_model.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#include "anem_model.h"

#include <QApplication>
#include <QDebug>

#include "dbghelper.h"
#include "defs.h"
#include "stringutils.h"
#include "widget_utils.h"

AnemModel::AnemModel(QObject *parent, AnemDescList *list) :
    QAbstractTableModel(parent),
    list_(list)
{
}

AnemModel::~AnemModel()
{
//    DEBUG_FUNC_NAME
}

// Reset the model
void AnemModel::flush()
{
//    DEBUG_FUNC_NAME
    beginResetModel();
    endResetModel();
}

// Return data at index
QVariant AnemModel::data(const QModelIndex& index, int role) const
{
    // row is the var field
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return QVariant();
    if (column >= list_->count()) return QVariant();

    // column is the entry in the QList
    const AnemDesc& anemDesc = list_->at(column);

    QVariant nullStrValue = QString();

    // row is the anem field
    if (role == Qt::DisplayRole)
    {
        switch (row)
        {
            case MANUFACTURER:
                return QVariant(anemDesc.manufacturer());
            case MODEL:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::campbellModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::gillModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::metekModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::youngModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_4())
                {
                    if (anemDesc.model() == AnemDesc::getANEM_MODEL_STRING_12())
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.model());
                }
            case SWVERSION:
                return QVariant(anemDesc.swVersion());
            case ID:
                return QVariant(anemDesc.id());
            case HEIGHT:
                return QVariant(QString::number(anemDesc.height(), 'f', 2) + QStringLiteral(" [m]"));
            case WINDFORMAT:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3())
                {
                    if (anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_0()
                        || anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_1())
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0())
                {
                    if (anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_0())
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.windFormat(), AnemDesc::allWindFormatStringList()))
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.windFormat());
                }
            case NORTHALIGNMENT:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_4())
                {
                    if (anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_2())
                    {
                        return QVariant(anemDesc.northAlignment());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_0()
                        || anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_1())
                    {
                        return QVariant(anemDesc.northAlignment());
                    }
                    else
                    {
                        const_cast<AnemModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.northAlignment());
                }
            case NORTHOFFSET:
                return QVariant(QString::number(anemDesc.northOffset(), 'f', 1) + tr(" [%1]").arg(Defs::DEGREE));
            case NSEPARATION:
                if (column == 0)
                    return QVariant(QStringLiteral("Reference"));
                else
                    return QVariant(QString::number(anemDesc.nSeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case ESEPARATION:
                if (column == 0)
                    return QVariant(QStringLiteral("Reference"));
                else
                    return QVariant(QString::number(anemDesc.eSeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case VSEPARATION:
                if (column == 0)
                    return QVariant(QStringLiteral("Reference"));
                else
                    return QVariant(QString::number(anemDesc.vSeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case VPATHLENGTH:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(anemDesc.vPathLength(), 'f', 4) + QStringLiteral(" [cm]"));
                }
            case HPATHLENGTH:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(anemDesc.hPathLength(), 'f', 4) + QStringLiteral(" [cm]"));
                }
            case TAU:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(anemDesc.tau(), 'f', 4) + QStringLiteral(" [s]"));
                }
            default:
                return QVariant();
        }
    }
    else if (role == Qt::EditRole)
    {
        switch (row)
        {
            case MANUFACTURER:
                return QVariant(anemDesc.manufacturer());
            case MODEL:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::campbellModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::gillModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::metekModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.model(),
                                                         AnemDesc::youngModelStringList()))
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_4())
                {
                    if (anemDesc.model() == AnemDesc::getANEM_MODEL_STRING_12())
                    {
                        return QVariant(anemDesc.model());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.model());
                }
            case SWVERSION:
                return QVariant(anemDesc.swVersion());
            case ID:
                return QVariant(anemDesc.id());
            case HEIGHT:
                return QVariant(anemDesc.height());
            case WINDFORMAT:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3())
                {
                    if (anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_0()
                        || anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_1())
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0())
                {
                    if (anemDesc.windFormat() == AnemDesc::getANEM_WIND_FORMAT_STRING_0())
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (StringUtils::stringBelongsToList(anemDesc.windFormat(), AnemDesc::allWindFormatStringList()))
                    {
                        return QVariant(anemDesc.windFormat());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.windFormat());
                }
            case NORTHALIGNMENT:
                if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_0()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_2()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_3()
                    || anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_4())
                {
                    if (anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_2())
                    {
                        return QVariant(anemDesc.northAlignment());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else if (anemDesc.manufacturer() == AnemDesc::getANEM_MANUFACTURER_STRING_1())
                {
                    if (anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_0()
                        || anemDesc.northAlignment() == AnemDesc::getANEM_NORTH_ALIGN_STRING_1())
                    {
                        return QVariant(anemDesc.northAlignment());
                    }
                    else
                    {
                        return nullStrValue;
                    }
                }
                else
                {
                    return QVariant(anemDesc.northAlignment());
                }
            case NORTHOFFSET:
                return QVariant(anemDesc.northOffset());
            case NSEPARATION:
                if (column == 0)
                {
                    return QVariant(QStringLiteral("Reference"));
                }
                else
                {
                    return QVariant(anemDesc.nSeparation());
                }
            case ESEPARATION:
                if (column == 0)
                {
                    return QVariant(QStringLiteral("Reference"));
                }
                else
                {
                    return QVariant(anemDesc.eSeparation());
                }
            case VSEPARATION:
                if (column == 0)
                {
                    return QVariant(QStringLiteral("Reference"));
                }
                else
                {
                    return QVariant(anemDesc.vSeparation());
                }
            case VPATHLENGTH:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(anemDesc.vPathLength());
                }
            case HPATHLENGTH:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(anemDesc.hPathLength());
                }
            case TAU:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(anemDesc.tau());
                }
            default:
                return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch (row)
        {
            case MANUFACTURER:
            case MODEL:
            case SWVERSION:
            case ID:
            case HEIGHT:
            case WINDFORMAT:
            case NORTHALIGNMENT:
            case NORTHOFFSET:
            case NSEPARATION:
            case ESEPARATION:
            case VSEPARATION:
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
            default:
                return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        switch (row)
        {
            case NSEPARATION:
            case ESEPARATION:
            case VSEPARATION:
                if (column == 0)
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
                if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            default:
                return QVariant(QColor(Qt::white));
        }
    }
    // tooltips texts
    else if (role == Qt::ToolTipRole)
    {
        switch (row)
        {
            case MANUFACTURER:
                return QVariant(tr("Manufacturer value"));
            case MODEL:
                return QVariant(tr("Model value"));
            case SWVERSION:
                return QVariant(tr("Sw Version"));
            case ID:
                return QVariant(tr("ID value"));
            case HEIGHT:
                return QVariant(tr("Height value"));
            case WINDFORMAT:
                return QVariant(tr("Wind data format value"));
            case NORTHALIGNMENT:
                if (anemDesc.northAlignment() == QLatin1String("Axis"))
                {
                    return QVariant(tr("Axis alignment..."));
                }
                else if (anemDesc.northAlignment() == QLatin1String("Spar"))
                {
                    return QVariant(tr("Spar alignment..."));
                }
                else
                {
                    return QVariant(tr("North alignment possible value: Axis..., Spar..., N/A..."));
                }
            case NORTHOFFSET:
                return QVariant(tr("North off-set value"));
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
            default:
                return QVariant();
        }
    }

    return QVariant();
}

// Set data at index
bool AnemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (column >= list_->count()) return false;

    // grab existing anem desc
    AnemDesc anemDesc = list_->value(column);

    switch (row)
    {
        case MANUFACTURER:
            // update only if new value is not equal to the old value
            if (value == anemDesc.manufacturer())
            {
                return false;
            }
            anemDesc.setManufacturer(value.toString());
            break;
        case MODEL:
            if (value == anemDesc.model())
            {
                return false;
            }
            anemDesc.setModel(value.toString());
            break;
        case SWVERSION:
            if (value == anemDesc.swVersion())
            {
                return false;
            }
            anemDesc.setSwVersion(value.toString());
            break;
        case ID:
            if (value == anemDesc.id())
            {
                return false;
            }
            anemDesc.setId(value.toString());
            break;
        case HEIGHT:
            if (value == anemDesc.height())
            {
                return false;
            }
            anemDesc.setHeight(value.toReal());
            break;
        case WINDFORMAT:
            if (value == anemDesc.windFormat())
            {
                return false;
            }
            anemDesc.setWindFormat(value.toString());
            break;
        case NORTHALIGNMENT:
            if (value == anemDesc.northAlignment())
            {
                return false;
            }
            anemDesc.setNorthAlignment(value.toString());
            break;
        case NORTHOFFSET:
            if (value == anemDesc.northOffset())
            {
                return false;
            }
            anemDesc.setNorthOffset(value.toReal());
            break;
        case NSEPARATION:
            if (value == anemDesc.nSeparation())
            {
                return false;
            }
            anemDesc.setNSeparation(value.toReal());
            break;
        case ESEPARATION:
            if (value == anemDesc.eSeparation())
            {
                return false;
            }
            anemDesc.setESeparation(value.toReal());
            break;
        case VSEPARATION:
            if (value == anemDesc.vSeparation())
            {
                return false;
            }
            anemDesc.setVSeparation(value.toReal());
            break;
        case VPATHLENGTH:
            if (value == anemDesc.vPathLength())
            {
                return false;
            }
            anemDesc.setVPathLength(value.toReal());
            break;
        case HPATHLENGTH:
            if (value == anemDesc.hPathLength())
            {
                return false;
            }
            anemDesc.setHPathLength(value.toReal());
            break;
        case TAU:
            if (value == anemDesc.tau())
            {
                return false;
            }
            anemDesc.setTau(value.toReal());
            break;
        default:
            return false;
    }

    list_->replace(column, anemDesc);
    emit modified();

    // whole column may have changed
    emit dataChanged(index.sibling(MANUFACTURER, column),
                     index.sibling(TAU, column));
    return true;
}

// Insert columns into table
bool AnemModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)
    if (count != 1) return false; // insert only one column at a time
    if ((column < 0) || (column >= list_->count()))
        column = list_->count();

    AnemDesc anemDesc = AnemDesc();

    beginInsertColumns(QModelIndex(), column, column);
    list_->insert(column, anemDesc);
    endInsertColumns();
    emit modified();
    return true;
}

// Remove columns from table
bool AnemModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)
    if (count != 1) return false; // only remove one column at a time
    if ((column < 0) || (column >= list_->count())) return false;

    if (!WidgetUtils::okToRemoveColumn(qApp->activeWindow()))
    {
        return false;
    }

    beginRemoveColumns(QModelIndex(), column, column);
    list_->removeAt(column);
    endRemoveColumns();

    emit modified();
    return true;
}

// Return header information
QVariant AnemModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case MANUFACTURER:
            case MODEL:
            case SWVERSION:
            case ID:
            case HEIGHT:
            case WINDFORMAT:
            case NORTHALIGNMENT:
            case NORTHOFFSET:
            case NSEPARATION:
            case ESEPARATION:
            case VSEPARATION:
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
                return QVariant(QString());
            default:
                return QVariant();
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return QVariant(section + 1);
    }

    if (orientation == Qt::Horizontal && role == Qt::BackgroundRole)
    {
        return QVariant(QBrush(QColor(Qt::darkGray)));
    }

    return QVariant();
}

// Return flags at index
Qt::ItemFlags AnemModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    Qt::ItemFlags currentFlags = QAbstractTableModel::flags(index);
    currentFlags |= Qt::ItemIsEnabled;
    currentFlags |= Qt::ItemIsEditable;
    currentFlags |= Qt::ItemIsSelectable;

    int row = index.row();
    int column = index.column();

    AnemDesc anemDesc = list_->value(column);

    switch (row)
    {
        case NSEPARATION:
        case ESEPARATION:
        case VSEPARATION:
            if (column == 0)
            {
                currentFlags &= !Qt::ItemIsEnabled;
                currentFlags &= !Qt::ItemIsEditable;
                currentFlags &= !Qt::ItemIsSelectable;
                return currentFlags;
            }
            else
            {
                return currentFlags;
            }
        case VPATHLENGTH:
        case HPATHLENGTH:
        case TAU:
            if (anemDesc.model() != AnemDesc::getANEM_MODEL_STRING_12())
            {
                currentFlags &= !Qt::ItemIsEnabled;
                currentFlags &= !Qt::ItemIsEditable;
                currentFlags &= !Qt::ItemIsSelectable;
                return currentFlags;            }
            else
            {
                return currentFlags;
            }
        default:
            return currentFlags;
    }
}

// Return number of rows of data
int AnemModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return ANEMNUMCOLS;
}

// Return number of columns of data
int AnemModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return list_->count();
}
