/***************************************************************************
  irga_model.cpp
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

#include "irga_model.h"

#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QDebug>

#include "dbghelper.h"
#include "defs.h"
#include "stringutils.h"
#include "widget_utils.h"

IrgaModel::IrgaModel(QObject *parent, IrgaDescList *list) :
    QAbstractTableModel(parent),
    list_(list)
{;}

IrgaModel::~IrgaModel(){
    DEBUG_FUNC_NAME
}

// Reset the model
void IrgaModel::flush()
{
    DEBUG_FUNC_NAME
    beginResetModel();
    endResetModel();
}

// Return data at index
QVariant IrgaModel::data(const QModelIndex& index, int role) const
{
    // row is the irga field
    int row = index.row();

    // column is the entry in the list
    int column = index.column();

    if (!index.isValid()) return QVariant();
    if (column >= list_->count()) return QVariant();

    const IrgaDesc& irgaDesc = list_->at(column);

    QVariant nullStrValue = QString();

    if (role == Qt::DisplayRole)
    {
        switch (row)
        {
            case MANUFACTURER:
                return QVariant(irgaDesc.manufacturer());
            case MODEL:
                if (irgaDesc.manufacturer() == IrgaDesc::getIRGA_MANUFACTURER_STRING_0())
                {
                    if (StringUtils::stringBelongsToList(irgaDesc.model(), IrgaDesc::licorModelStringList()))
                    {
                        return QVariant(irgaDesc.model());
                    }
                    else
                    {
                        const_cast<IrgaModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
                else
                {
                    if (StringUtils::stringBelongsToList(irgaDesc.model(), IrgaDesc::otherModelStringList()))
                    {
                        return QVariant(irgaDesc.model());
                    }
                    else
                    {
                        const_cast<IrgaModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                }
            case SWVERSION:
                if (irgaDesc.manufacturer() == IrgaDesc::getIRGA_MANUFACTURER_STRING_0()
                    && (irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_3()
                        || irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_4()))
                {
                    return QVariant(irgaDesc.swVersion());
                }
                else
                {
                    return nullStrValue;
                }
            case ID:
                return QVariant(irgaDesc.id());
            case TUBELENGTH:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.tubeLength(), 'f', 1) + QStringLiteral(" [cm]"));
                }
            case TUBEDIAMETER:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.tubeDiameter(), 'f', 1) + QStringLiteral(" [mm]"));
                }
            case TUBEFLOWRATE:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.tubeFlowRate(), 'f', 1) + QStringLiteral(" [l/m]"));
                }
            case TUBENSEPARATION:
                return QVariant(QString::number(irgaDesc.tubeNSeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case TUBEESEPARATION:
                return QVariant(QString::number(irgaDesc.tubeESeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case TUBEVSEPARATION:
                return QVariant(QString::number(irgaDesc.tubeVSeparation(), 'f', 2) + QStringLiteral(" [cm]"));
            case VPATHLENGTH:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.vPathLength(), 'f', 4) + QStringLiteral(" [cm]"));
                }
            case HPATHLENGTH:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.hPathLength(), 'f', 4) + QStringLiteral(" [cm]"));
                }
            case TAU:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.tau(), 'f', 4) + QStringLiteral(" [s]"));
                }
            case KWATER:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.kWater(), 'f', 6) + QStringLiteral(" [") + Defs::M3_G_CM_STRING + QStringLiteral("]"));
                }
            case KOXYGEN:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(irgaDesc.kOxygen(), 'f', 6) + QStringLiteral(" [") + Defs::M3_G_CM_STRING + QStringLiteral("]"));
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
                return QVariant(irgaDesc.manufacturer());
            case MODEL:
                return QVariant(irgaDesc.model());
            case SWVERSION:
                return QVariant(irgaDesc.swVersion());
            case ID:
                return QVariant(irgaDesc.id());
            case TUBELENGTH:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.tubeLength());
                }
            case TUBEDIAMETER:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.tubeDiameter());
                }
            case TUBEFLOWRATE:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.tubeFlowRate());
                }
            case TUBENSEPARATION:
                return QVariant(irgaDesc.tubeNSeparation());
            case TUBEESEPARATION:
                return QVariant(irgaDesc.tubeESeparation());
            case TUBEVSEPARATION:
                return QVariant(irgaDesc.tubeVSeparation());
            case VPATHLENGTH:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.vPathLength());
                }
            case HPATHLENGTH:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.hPathLength());
                }
            case TAU:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.tau());
                }
            case KWATER:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.kWater());
                }
            case KOXYGEN:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(irgaDesc.kOxygen());
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
            case TUBELENGTH:
            case TUBEDIAMETER:
            case TUBEFLOWRATE:
            case TUBENSEPARATION:
            case TUBEESEPARATION:
            case TUBEVSEPARATION:
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
            case KWATER:
            case KOXYGEN:
            default:
                return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        switch (row)
        {
            case SWVERSION:
                if (irgaDesc.manufacturer() == IrgaDesc::getIRGA_MANUFACTURER_STRING_0()
                    && (irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_3()
                        || irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_4()))
                {
                    return QVariant(QColor(Qt::white));
                }
                else
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
            case TUBELENGTH:
            case TUBEDIAMETER:
            case TUBEFLOWRATE:
                if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
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
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            case KWATER:
            case KOXYGEN:
                if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                    && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
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
    else
    {
        return QVariant();
    }
}

// Set data at index
bool IrgaModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (column >= list_->count()) return false;

    // grab existing irga desc
    IrgaDesc irgaDesc = list_->value(column);

    switch (row)
    {
        case MANUFACTURER:
            // update only if new value is not equal to the old value
            if (value == irgaDesc.manufacturer())
            {
                return false;
            }
            irgaDesc.setManufacturer(value.toString());
            break;
        case MODEL:
            if (value == irgaDesc.model())
            {
                return false;
            }
            irgaDesc.setModel(value.toString());
            break;
        case SWVERSION:
            if (value == irgaDesc.swVersion())
            {
                return false;
            }
            irgaDesc.setSwVersion(value.toString());
            break;
        case ID:
            if (value == irgaDesc.id())
            {
                return false;
            }
            irgaDesc.setId(value.toString());
            break;
        case TUBELENGTH:
            if (value == irgaDesc.tubeLength())
            {
                return false;
            }
            irgaDesc.setTubeLength(value.toReal());
            break;
        case TUBEDIAMETER:
            if (value == irgaDesc.tubeDiameter())
            {
                return false;
            }
            irgaDesc.setTubeDiameter(value.toReal());
            break;
        case TUBEFLOWRATE:
            if (value == irgaDesc.tubeFlowRate())
            {
                return false;
            }
            irgaDesc.setTubeFlowRate(value.toReal());
            break;
        case TUBENSEPARATION:
            if (value == irgaDesc.tubeNSeparation())
            {
                return false;
            }
            irgaDesc.setTubeNSeparation(value.toReal());
            break;
        case TUBEESEPARATION:
            if (value == irgaDesc.tubeESeparation())
            {
                return false;
            }
            irgaDesc.setTubeESeparation(value.toReal());
            break;
        case TUBEVSEPARATION:
            if (value == irgaDesc.tubeVSeparation())
            {
                return false;
            }
            irgaDesc.setTubeVSeparation(value.toReal());
            break;
        case VPATHLENGTH:
            if (value == irgaDesc.vPathLength())
            {
                return false;
            }
            irgaDesc.setVPathLength(value.toReal());
            break;
        case HPATHLENGTH:
            if (value == irgaDesc.hPathLength())
            {
                return false;
            }
            irgaDesc.setHPathLength(value.toReal());
            break;
        case TAU:
            if (value == irgaDesc.tau())
            {
                return false;
            }
            irgaDesc.setTau(value.toReal());
            break;
        case KWATER:
            if (value == irgaDesc.kWater())
            {
                return false;
            }
            irgaDesc.setKWater(value.toReal());
            break;
        case KOXYGEN:
            if (value == irgaDesc.kOxygen())
            {
                return false;
            }
            irgaDesc.setKOxygen(value.toReal());
            break;
        default:
            return false;
    }

    list_->replace(column, irgaDesc);
    emit modified();

    // whole column may have changed
//    emit dataChanged(index.sibling(column, MANUFACTURER),
//                     index.sibling(column, KOXYGEN));
    emit dataChanged(index.sibling(MANUFACTURER, column),
                     index.sibling(KOXYGEN, column));
    return true;
}

// Insert columns into table
bool IrgaModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)
    if (count != 1) return false; // insert only one column at a time
    if ((column < 0) || (column >= list_->count())) column = list_->count();

    IrgaDesc irgaDesc = IrgaDesc();

    beginInsertColumns(QModelIndex(), column, column);
    list_->insert(column, irgaDesc);
    endInsertColumns();
    emit modified();
    return true;
}

// Remove columns from table
bool IrgaModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    DEBUG_FUNC_NAME
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
QVariant IrgaModel::headerData(int section, Qt::Orientation orientation,
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
            case TUBELENGTH:
            case TUBEDIAMETER:
            case TUBEFLOWRATE:
            case TUBENSEPARATION:
            case TUBEESEPARATION:
            case TUBEVSEPARATION:
            case VPATHLENGTH:
            case HPATHLENGTH:
            case TAU:
            case KWATER:
            case KOXYGEN:
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
Qt::ItemFlags IrgaModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    Qt::ItemFlags currentFlags = QAbstractTableModel::flags(index);
    currentFlags |= Qt::ItemIsEnabled;
    currentFlags |= Qt::ItemIsEditable;
    currentFlags |= Qt::ItemIsSelectable;

    int row = index.row();
    int column = index.column();

    const IrgaDesc& irgaDesc = list_->at(column);

    switch (row)
    {
        case SWVERSION:
            if (irgaDesc.manufacturer() == IrgaDesc::getIRGA_MANUFACTURER_STRING_0()
                && (irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_3()
                    || irgaDesc.model() == IrgaDesc::getIRGA_MODEL_STRING_4()))
            {
                return currentFlags;
            }
            else
            {
                currentFlags &= !Qt::ItemIsEnabled;
                currentFlags &= !Qt::ItemIsEditable;
                currentFlags &= !Qt::ItemIsSelectable;
                return currentFlags;
            }
        case TUBELENGTH:
        case TUBEDIAMETER:
        case TUBEFLOWRATE:
            if (IrgaDesc::isOpenPathModel(irgaDesc.model()))
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
            if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_6()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_7()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
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
        case KWATER:
        case KOXYGEN:
            if (irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_8()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_9()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_10()
                && irgaDesc.model() != IrgaDesc::getIRGA_MODEL_STRING_11())
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
        default:
            return currentFlags;
    }
}

// Return number of rows of data
int IrgaModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return IRGANUMCOLS;
}

// Return number of columns of data
int IrgaModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return list_->count();
}
