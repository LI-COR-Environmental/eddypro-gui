/***************************************************************************
  variable_model.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#include "variable_model.h"

#include <QApplication>
#include <QDebug>

#include "stringutils.h"
#include "widget_utils.h"

VariableModel::VariableModel(QObject *parent, VariableDescList *list) :
    QAbstractTableModel(parent),
    list_(list)
{;}

void VariableModel::flush()
{
    beginResetModel();
    endResetModel();
}

// Return data at index
QVariant VariableModel::data(const QModelIndex& index, int role) const
{
    // row is the var field
    int row = index.row();

    // column is the entry in the list
    int column = index.column();

    if (!index.isValid()) return QVariant();
    if (column >= list_->count()) return QVariant();

    const VariableDesc& variableDesc = list_->at(column);

    const QString& var = variableDesc.variable();

    QVariant nullStrValue = QString();

    if (role == Qt::DisplayRole)
    {
        switch (row)
        {
            case IGNORE:
                if (variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QStringLiteral("yes"));
                }
                return QVariant(variableDesc.ignore());
            case NUMERIC:
                return QVariant(variableDesc.numeric());
            case VARIABLE:
                return QVariant(var);
            case INSTRUMENT:
                // filter no more available instruments
                if (instrModelList_.contains(variableDesc.instrument()))
                {
                    return QVariant(variableDesc.instrument());
                }
                else
                {
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
            case MEASURETYPE:
                if (!VariableDesc::isGasVariable(var)
                    && !VariableDesc::isCustomVariable(var))
                {
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.measureType());
                }
            case INPUTUNIT:
                if (VariableDesc::isVelocityVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::velocityInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    // NOTE: hack to call a non const member from within a const member
                    // to use just for string variables and in DisplayRole
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isAngleVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::angleInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isTemperatureVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::temperatureInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isPressureVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::pressureInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isGasVariable(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::gasInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isFlowRateVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                  VariableDesc::flowRateInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else if (VariableDesc::isDiagnosticVar(var))
                {
                    if (variableDesc.inputUnit() == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17())
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.inputUnit());
                }
            case CONVERSIONTYPE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                            && VariableDesc::isDiagnosticVar(var)))
                {
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                const_cast<VariableModel *>(this)->setData(index, QVariant(VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1()));
                return QVariant(VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1());
            case OUTPUTUNIT:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                    return nullStrValue;
                }
                else
                {
                    if (VariableDesc::isVelocityVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::velocityOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        // NOTE: hack to call a non const member from within a const member
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isAngleVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::angleOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isTemperatureVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::temperatureOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isPressureVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::pressureOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isGasVariable(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::gasOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isFlowRateVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                      VariableDesc::flowRateOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    if (VariableDesc::isDiagnosticVar(var))
                    {
                        if (variableDesc.outputUnit() == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17())
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        const_cast<VariableModel *>(this)->setData(index, nullStrValue);
                        return nullStrValue;
                    }
                    return QVariant(variableDesc.outputUnit());
                }
            case AVALUE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    // reset the gain value to 1
                    const_cast<VariableModel *>(this)->setData(index, 1);
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(variableDesc.aValue(), 'f', 6));
                }
            case BVALUE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    // reset the offset value to 0
                    const_cast<VariableModel *>(this)->setData(index, 0);
                    return nullStrValue;
                }
                else
                {
                    return QVariant(QString::number(variableDesc.bValue(), 'f', 6));
                }
            case NOMTIMELAG:
                return QVariant(QString::number(variableDesc.nomTimelag(), 'f', 2) + QStringLiteral(" [s]"));
            case MINTIMELAG:
                return QVariant(QString::number(variableDesc.minTimelag(), 'f', 2) + QStringLiteral(" [s]"));
            case MAXTIMELAG:
                return QVariant(QString::number(variableDesc.maxTimelag(), 'f', 2) + QStringLiteral(" [s]"));
            default:
                return QVariant();
        }
    }
    else if (role == Qt::EditRole)
    {
        switch (row)
        {
            case IGNORE:
                if (variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QStringLiteral("yes"));
                }
                return QVariant(variableDesc.ignore());
            case NUMERIC:
                return QVariant(variableDesc.numeric());
            case VARIABLE:
                if (!var.isEmpty())
                {
                    return QVariant(var);
                }
                // to avoid editing parent items ('standard/custom variables')
                // pick the first available var
                else
                {
                    return QVariant(QStringLiteral("u"));
                }
            case INSTRUMENT:
                // filter no more available instruments
                if (instrModelList_.contains(variableDesc.instrument()))
                {
                    return QVariant(variableDesc.instrument());
                }
                else
                {
                    return nullStrValue;
                }
            case MEASURETYPE:
                if (!VariableDesc::isGasVariable(var)
                    && !VariableDesc::isCustomVariable(var))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.measureType());
                }
            case INPUTUNIT:
                if (VariableDesc::isVelocityVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::velocityInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isAngleVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::angleInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isTemperatureVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::temperatureInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isPressureVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::pressureInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isGasVariable(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::gasInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isFlowRateVar(var))
                {
                    if (StringUtils::stringBelongsToList(variableDesc.inputUnit(),
                                                         VariableDesc::flowRateInputUnitStringList()))
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else if (VariableDesc::isDiagnosticVar(var))
                {
                    if (variableDesc.inputUnit() == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17())
                    {
                        return QVariant(variableDesc.inputUnit());
                    }
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.inputUnit());
                }
            case CONVERSIONTYPE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1());
                }
            case OUTPUTUNIT:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    return nullStrValue;
                }
                else
                {
                    if (VariableDesc::isVelocityVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::velocityOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isAngleVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::angleOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isTemperatureVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::temperatureOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isPressureVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::pressureOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isGasVariable(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::gasOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isFlowRateVar(var))
                    {
                        if (StringUtils::stringBelongsToList(variableDesc.outputUnit(),
                                                             VariableDesc::flowRateOutputUnitStringList()))
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    if (VariableDesc::isDiagnosticVar(var))
                    {
                        if (variableDesc.outputUnit() == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17())
                        {
                            return QVariant(variableDesc.outputUnit());
                        }
                        return nullStrValue;
                    }
                    return QVariant(variableDesc.outputUnit());
                }
            case AVALUE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.aValue());
                }
            case BVALUE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(var)))
                {
                    return nullStrValue;
                }
                else
                {
                    return QVariant(variableDesc.bValue());
                }
            case NOMTIMELAG:
                return QVariant(variableDesc.nomTimelag());
            case MINTIMELAG:
                return QVariant(variableDesc.minTimelag());
            case MAXTIMELAG:
                return QVariant(variableDesc.maxTimelag());
            default:
                return QVariant();
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        switch (row)
        {
            case IGNORE:
            case NUMERIC:
            case VARIABLE:
            case INSTRUMENT:
            case MEASURETYPE:
            case INPUTUNIT:
            case CONVERSIONTYPE:
            case OUTPUTUNIT:
            case AVALUE:
            case BVALUE:
            case NOMTIMELAG:
            case MINTIMELAG:
            case MAXTIMELAG:
            default:
                return QVariant(Qt::AlignVCenter | Qt::AlignRight);
        }
    }
    else if (role == Qt::TextColorRole)
    {
        switch (row)
        {
            case IGNORE:
                if (variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#D69696"))));
                }
                else if (variableDesc.ignore() == QLatin1String("yes")
                         && variableDesc.numeric() == QLatin1String("yes"))
                {
                    return QVariant(QBrush(QColor(Qt::red)));
                }
                else
                {
                    return QVariant(QColor(Qt::black));
                }
            case NUMERIC:
                if (variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(Qt::red)));
                }
                else if (variableDesc.numeric() == QLatin1String("yes")
                         && variableDesc.ignore() == QLatin1String("yes"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#D69696"))));
                }
                else
                {
                    return QVariant(QColor(Qt::black));
                }
            default:
                if (variableDesc.ignore() == QLatin1String("yes")
                    || variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#D69696"))));
                }
                else
                {
                    return QVariant(QColor(Qt::black));
                }
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        switch (row)
        {
            case IGNORE:
                if (variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            case NUMERIC:
                if (variableDesc.numeric() == QLatin1String("yes")
                    && variableDesc.ignore() == QLatin1String("yes"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            case MEASURETYPE:
                if ((!VariableDesc::isGasVariable(var)
                    && !VariableDesc::isCustomVariable(var))
                    || variableDesc.ignore() == QLatin1String("yes")
                    || variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            case CONVERSIONTYPE:
            case OUTPUTUNIT:
            case AVALUE:
            case BVALUE:
                if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    || variableDesc.ignore() == QLatin1String("yes")
                    || variableDesc.numeric() == QLatin1String("no")
                    || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                        && VariableDesc::isDiagnosticVar(variableDesc.variable())))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
            default:
                if (variableDesc.ignore() == QLatin1String("yes")
                    || variableDesc.numeric() == QLatin1String("no"))
                {
                    return QVariant(QBrush(QColor(QStringLiteral("#eff0f1"))));
                }
                else
                {
                    return QVariant(QColor(Qt::white));
                }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        switch (row)
        {
            default:
                return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

// Set data at index
bool VariableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int row = index.row();
    int column = index.column();

    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (column >= list_->count()) return false;

    // grab existing var desc for the column
    VariableDesc variableDesc = list_->value(column);

    switch (row)
    {
        case IGNORE:
            if (value == variableDesc.ignore())
            {
                return false;
            }

            if (variableDesc.numeric() == QLatin1String("no"))
            {
                variableDesc.setIgnore(QStringLiteral("yes"));
            }
            else
            {
                variableDesc.setIgnore(value.toString());
            }
            break;
        case NUMERIC:
            if (value == variableDesc.numeric())
            {
                return false;
            }
            variableDesc.setNumeric(value.toString());
            break;
        case VARIABLE:
            // update only if new value is not equal to the old value
            if (value == variableDesc.variable())
            {
                return false;
            }
            // skip parent items
            if (value != QStringLiteral("Standard Variables")
                and value != QStringLiteral("Standard Variables"))
            {
                variableDesc.setVariable(value.toString());
            }
            else
            {
                return false;
            }
            break;
        case INSTRUMENT:
            if (value == variableDesc.instrument())
            {
                return false;
            }

            // filter no more available instruments
            if (instrModelList_.contains(value.toString()))
            {
                variableDesc.setInstrument(value.toString());
            }
            else
            {
                variableDesc.setInstrument(QString());
            }
            break;
        case MEASURETYPE:
            if (value == variableDesc.measureType())
            {
                return false;
            }
            variableDesc.setMeasureType(value.toString());
            break;
        case INPUTUNIT:
            if (value == variableDesc.inputUnit())
            {
                return false;
            }
            variableDesc.setInputUnit(value.toString());
            break;
        case CONVERSIONTYPE:
            if (value == variableDesc.conversionType())
            {
                return false;
            }
            variableDesc.setConversionType(value.toString());
            break;
        case OUTPUTUNIT:
            if (value == variableDesc.outputUnit())
            {
                return false;
            }
            variableDesc.setOutputUnit(value.toString());
            break;
        case AVALUE:
            if (value == variableDesc.aValue())
            {
                return false;
            }
            variableDesc.setAValue(value.toReal());
            break;
        case BVALUE:
            if (value == variableDesc.bValue())
            {
                return false;
            }
            variableDesc.setBValue(value.toReal());
            break;
        case NOMTIMELAG:
            if (value == variableDesc.nomTimelag())
            {
                return false;
            }
            variableDesc.setNomTimelag(value.toReal());
            break;
        case MINTIMELAG:
            if (value == variableDesc.minTimelag())
            {
                return false;
            }
            variableDesc.setMinTimelag(value.toReal());
            break;
        case MAXTIMELAG:
            if (value == variableDesc.maxTimelag())
            {
                return false;
            }
            variableDesc.setMaxTimelag(value.toReal());
            break;
        default:
            return false;
    }

    list_->replace(column, variableDesc);
    emit modified();

    // whole column may have changed
    emit dataChanged(index.sibling(IGNORE, column),
                     index.sibling(MAXTIMELAG, column));
    return true;
}

// Insert columns into table
bool VariableModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)

    if (count != 1) return false; // insert only one column at a time
    if ((column < 0) || (column >= list_->count())) column = list_->count();

    VariableDesc variableDesc = VariableDesc();

    beginInsertColumns(QModelIndex(), column, column);
    list_->insert(column, variableDesc);
    endInsertColumns();
    emit modified();
    return true;
}

// Remove columns from table
bool VariableModel::removeColumns(int column, int count, const QModelIndex& parent)
{
    Q_UNUSED(parent)

    if (count != 1) return false; // only remove one column at a time
    if ((column < 0) || (column >= list_->count())) return false;

    if (!WidgetUtils::okToRemoveColumn(QApplication::activeWindow()))
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
QVariant VariableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Vertical && role == Qt::DisplayRole)
    {
        switch (section)
        {
            case IGNORE:
            case NUMERIC:
            case VARIABLE:
            case INSTRUMENT:
            case MEASURETYPE:
            case INPUTUNIT:
            case CONVERSIONTYPE:
            case OUTPUTUNIT:
            case AVALUE:
            case BVALUE:
            case NOMTIMELAG:
            case MINTIMELAG:
            case MAXTIMELAG:
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
Qt::ItemFlags VariableModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::ItemIsEnabled;

    Qt::ItemFlags currentFlags = QAbstractTableModel::flags(index);

    Qt::ItemFlags normalFlags = currentFlags;
    normalFlags |= Qt::ItemIsEnabled;
    normalFlags |= Qt::ItemIsEditable;
    normalFlags |= Qt::ItemIsSelectable;

    Qt::ItemFlags disabledFlags = currentFlags;
    disabledFlags &= !Qt::ItemIsEditable;
    disabledFlags &= !Qt::ItemIsSelectable;

    int row = index.row();
    int column = index.column();

    const VariableDesc& variableDesc = list_->at(column);

    switch (row)
    {
        case IGNORE:
            if (variableDesc.numeric() == QLatin1String("no"))
            {
                return disabledFlags;
            }
            return normalFlags;
        case NUMERIC:
            if (variableDesc.ignore() == QLatin1String("yes")
                && variableDesc.numeric() == QLatin1String("yes"))
            {
                return disabledFlags;
            }
            return normalFlags;
        case VARIABLE:
            if (variableDesc.ignore() == QLatin1String("yes"))
            {
                return disabledFlags;
            }
            if (variableDesc.variable() == QLatin1String("Standard Variables")
                || variableDesc.variable() == QLatin1String("Custom Variables"))
            {
                return disabledFlags;
            }
            return normalFlags;
        case INSTRUMENT:
        case INPUTUNIT:
        case NOMTIMELAG:
        case MINTIMELAG:
        case MAXTIMELAG:
            if (variableDesc.ignore() == QLatin1String("yes"))
            {
                return disabledFlags;
            }
            return normalFlags;
        case MEASURETYPE:
            if (variableDesc.ignore() == QLatin1String("yes"))
            {
                return disabledFlags;
            }
            if (variableDesc.numeric() == QLatin1String("no")
                || (!VariableDesc::isGasVariable(variableDesc.variable())
                && !VariableDesc::isCustomVariable(variableDesc.variable())))
            {
                return disabledFlags;
            }
            return normalFlags;
        case CONVERSIONTYPE:
        case OUTPUTUNIT:
        case AVALUE:
        case BVALUE:
            if (variableDesc.ignore() == QLatin1String("yes"))
            {
                return disabledFlags;
            }
            if (!VariableDesc::isScalableVariable(variableDesc.inputUnit())
                || variableDesc.numeric() == QLatin1String("no")
                || (VariableDesc::isScalableVariable(variableDesc.inputUnit())
                    && VariableDesc::isDiagnosticVar(variableDesc.variable())))
            {
                return disabledFlags;
            }
            return normalFlags;
        default:
            return normalFlags;
    }
}

// Return number of rows of data
int VariableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return VARNUMCOLS;
}

// Return number of columns of data
int VariableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return list_->count();
}

const QStringList VariableModel::instrModels() const
{
    return instrModelList_;
}

void VariableModel::setInstrModels(const QStringList& list)
{
    instrModelList_ = list;

    for (int i = 0; i < list_->count(); ++i)
    {
        QString value = list_->at(i).instrument();
        VariableDesc variableDesc = list_->value(i);
        if (instrModelList_.contains(value))
        {
            variableDesc.setInstrument(value);
        }
        else
        {
            variableDesc.setInstrument(QString());

        }
        list_->replace(i, variableDesc);
    }

    emit layoutChanged();
}

// NOTE: never used
void VariableModel::triggerSetData() const
{
    // NOTE: hack for emitting a signal from within a const member function to a const slots
    // easier with custom signal
    emit const_cast<VariableModel *>(this)->layoutChanged();
}
