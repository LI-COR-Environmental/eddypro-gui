/***************************************************************************
  variable_desc.cpp
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

#include "variable_desc.h"

#include <QDebug>
#include <QStringList>

#include "anem_desc.h"
#include "dbghelper.h"
#include "defs.h"
#include "irga_desc.h"

const QString VariableDesc::getVARIABLE_VAR_STRING_0()
{
    static const QString s(QStringLiteral("u"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_1()
{
    static const QString s(QStringLiteral("v"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_2()
{
    static const QString s(QStringLiteral("w"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_3()
{
    static const QString s(tr("Sonic Temperature"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_4()
{
    static const QString s(tr("Speed of Sound"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_5()
{
    static const QString s(QStringLiteral("CO") + Defs::SUBTWO);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_6()
{
    static const QString s(QLatin1Char('H') + Defs::SUBTWO + QLatin1Char('O'));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_7()
{
    static const QString s(QStringLiteral("CH") + Defs::SUBFOUR);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_8()
{
    static const QString s(QLatin1Char('N') + Defs::SUBTWO + QLatin1Char('O'));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_9()
{
    static const QString s(tr("Cell Temperature In"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_10()
{
    static const QString s(tr("Cell Temperature Out"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_11()
{
    static const QString s(tr("Cell Pressure"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_12()
{
    static const QString s(tr("Ambient Temperature"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_13()
{
    static const QString s(tr("Ambient Pressure"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_14()
{
    static const QString s(tr("Ignore"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_15()
{
    static const QString s(tr("Average Cell Temperature"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_16()
{
    static const QString s(Defs::RHO + tr(" (rho)"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_17()
{
    static const QString s(Defs::THETA + tr(" (theta)"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_18()
{
    static const QString s(tr("Not Numeric"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_19()
{
    static const QString s(QStringLiteral("CO"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_20()
{
    static const QString s(QStringLiteral("SO") + Defs::SUBTWO);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_21()
{
    static const QString s(QLatin1Char('O') + Defs::SUBTHREE);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_22()
{
    static const QString s(QStringLiteral("NH") + Defs::SUBTHREE);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_23()
{
    static const QString s(QStringLiteral("NO"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_24()
{
    static const QString s(QStringLiteral("NO") + Defs::SUBTWO);
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_25()
{
    static const QString s(tr("LI-7500/A/RS/DS Diagnostics"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_26()
{
    static const QString s(tr("LI-7200/RS Diagnostics"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_27()
{
    static const QString s(tr("LI-7700 Diagnostics"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_28()
{
    static const QString s(tr("Fast Ambient Temperature"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_29()
{
    static const QString s(tr("Sampling Line Flow Rate"));
    return s;
}

const QString VariableDesc::getVARIABLE_VAR_STRING_30()
{
    static const QString s(tr("Anemometer Diagnostics"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0()
{
    static const QString s(tr("Molar/Mass density"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1()
{
    static const QString s(tr("Mole fraction"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2()
{
    static const QString s(tr("Mixing ratio"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_3()
{
    static const QString s(tr("Other"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_0()
{
    static const QString s(QStringLiteral("mV"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_1()
{
    static const QString s(QStringLiteral("V"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_2()
{
    static const QString s(QStringLiteral("mm/s"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_3()
{
    static const QString s(QStringLiteral("cm/s"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_4()
{
    static const QString s(QStringLiteral("m/s"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_5()
{
    static const QString s(Defs::DEGREE_K);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_6()
{
    static const QString s(Defs::CDEGREE_K_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_7()
{
    static const QString s(Defs::DEGREE_C);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_8()
{
    static const QString s(Defs::CDEGREE_C_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_9()
{
    static const QString s(Defs::MMOL_MOL_STRING + QStringLiteral(" (ppt)"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_10()
{
    static const QString s(Defs::UMOL_MOL_STRING + QStringLiteral(" (ppm)"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_11()
{
    static const QString s(Defs::NMOL_MOL_STRING + QStringLiteral(" (ppb)"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_12()
{
    static const QString s(Defs::MMOL_M3_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_13()
{
    static const QString s(Defs::UMOL_M3_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_14()
{
    static const QString s(QStringLiteral("Pa"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_15()
{
    static const QString s(QStringLiteral("hPa"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_16()
{
    static const QString s(QStringLiteral("kPa"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17()
{
    static const QString s(tr("Dimensionless"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_18()
{
    static const QString s(tr("Other"));
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_19()
{
    static const QString s(Defs::G_M3_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_20()
{
    static const QString s(Defs::MG_M3_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_21()
{
    static const QString s(Defs::UG_M3_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_22()
{
    static const QString s(Defs::L_MIN_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_23()
{
    static const QString s(Defs::M3_S_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_24()
{
    static const QString s(Defs::CM3_S_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_25()
{
    static const QString s(Defs::FT3_S_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_26()
{
    static const QString s(Defs::IN3_S_STRING);
    return s;
}

const QString VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_27()
{
    static const QString s(tr("Degrees"));
    return s;
}

const QString VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_0()
{
    static const QString s(tr("Zero (A) - Full scale (B)"));
    return s;
}

const QString VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1()
{
    static const QString s(tr("Gain - Offset"));
    return s;
}

const QString VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_2()
{
    static const QString s(tr("Doesn't need scaling"));
    return s;
}

VariableDesc::VariableDesc() :
    ignore_(QStringLiteral("no")),
    numeric_(QStringLiteral("yes")),
    variable_(QString()),
    instrument_(QString()),
    measureType_(QString()),
    inputUnit_(QString()),
    minValue_(0.0),
    maxValue_(0.0),
//    conversionType_(getVARIABLE_CONVERSION_TYPE_STRING_2()),
    conversionType_(QString()),
    outputUnit_(QString()),
    aValue_(1.0),
    bValue_(0.0),
    nomTimelag_(0.0),
    minTimelag_(0.0),
    maxTimelag_(0.0)
{ ; }

VariableDesc::VariableDesc(QString ignore,
                           QString notNumeric,
                           QString variable,
                           QString instrument,
                           QString measureType,
                           QString inputUnit,
                           qreal minValue,
                           qreal maxValue,
                           QString conversionType,
                           QString outputUnit,
                           qreal aValue,
                           qreal bValue,
                           qreal nomTimelag,
                           qreal minTimelag,
                           qreal maxTimelag
                           ) :
    ignore_(std::move(ignore)),
    numeric_(std::move(notNumeric)),
    variable_(std::move(variable)),
    instrument_(std::move(instrument)),
    measureType_(std::move(measureType)),
    inputUnit_(std::move(inputUnit)),
    minValue_(minValue),
    maxValue_(maxValue),
    conversionType_(std::move(conversionType)),
    outputUnit_(std::move(outputUnit)),
    aValue_(aValue),
    bValue_(bValue),
    nomTimelag_(nomTimelag),
    minTimelag_(minTimelag),
    maxTimelag_(maxTimelag)
{ ; }

VariableDesc::VariableDesc(const VariableDesc& fileDesc) :
    ignore_(fileDesc.ignore_),
    numeric_(fileDesc.numeric_),
    variable_(fileDesc.variable_),
    instrument_(fileDesc.instrument_),
    measureType_(fileDesc.measureType_),
    inputUnit_(fileDesc.inputUnit_),
    minValue_(fileDesc.minValue_),
    maxValue_(fileDesc.maxValue_),
    conversionType_(fileDesc.conversionType_),
    outputUnit_(fileDesc.outputUnit_),
    aValue_(fileDesc.aValue_),
    bValue_(fileDesc.bValue_),
    nomTimelag_(fileDesc.nomTimelag_),
    minTimelag_(fileDesc.minTimelag_),
    maxTimelag_(fileDesc.maxTimelag_)
{ ; }

VariableDesc& VariableDesc::operator=(const VariableDesc& fileDesc)
{
    if (this != &fileDesc)
    {
        ignore_ = fileDesc.ignore_;
        numeric_ = fileDesc.numeric_;
        variable_ = fileDesc.variable_;
        instrument_ = fileDesc.instrument_;
        measureType_ = fileDesc.measureType_;
        inputUnit_ = fileDesc.inputUnit_;
        minValue_ = fileDesc.minValue_;
        maxValue_ = fileDesc.maxValue_;
        conversionType_ = fileDesc.conversionType_;
        outputUnit_ = fileDesc.outputUnit_;
        aValue_ = fileDesc.aValue_;
        bValue_ = fileDesc.bValue_;
        nomTimelag_ = fileDesc.nomTimelag_;
        minTimelag_ = fileDesc.minTimelag_;
        maxTimelag_ = fileDesc.maxTimelag_;
    }
    return *this;
}

bool VariableDesc::operator==(const VariableDesc& fileDesc) const
{
    return (ignore_ == fileDesc.ignore_)
            && (numeric_ == fileDesc.numeric_)
            && (variable_ == fileDesc.variable_)
            && (instrument_ == fileDesc.instrument_)
            && (measureType_ == fileDesc.measureType_)
            && (inputUnit_ == fileDesc.inputUnit_)
            && qFuzzyCompare(minValue_, fileDesc.minValue_)
            && qFuzzyCompare(maxValue_, fileDesc.maxValue_)
            && (conversionType_ == fileDesc.conversionType_)
            && (outputUnit_ == fileDesc.outputUnit_)
            && qFuzzyCompare(aValue_, fileDesc.aValue_)
            && qFuzzyCompare(bValue_, fileDesc.bValue_)
            && qFuzzyCompare(nomTimelag_, fileDesc.nomTimelag_)
            && qFuzzyCompare(minTimelag_, fileDesc.minTimelag_)
            && qFuzzyCompare(maxTimelag_, fileDesc.maxTimelag_);
}

// Return string list of anem types
const QStringList VariableDesc::variableStringList()
{
    return (QStringList()
            << getVARIABLE_VAR_STRING_0()
            << getVARIABLE_VAR_STRING_1()
            << getVARIABLE_VAR_STRING_2()
            << getVARIABLE_VAR_STRING_16()
            << getVARIABLE_VAR_STRING_17()
            << getVARIABLE_VAR_STRING_3()
            << getVARIABLE_VAR_STRING_28()
            << getVARIABLE_VAR_STRING_4()
            << getVARIABLE_VAR_STRING_5()
            << getVARIABLE_VAR_STRING_6()
            << getVARIABLE_VAR_STRING_7()
            << getVARIABLE_VAR_STRING_8()
            << getVARIABLE_VAR_STRING_19()
            << getVARIABLE_VAR_STRING_20()
            << getVARIABLE_VAR_STRING_21()
            << getVARIABLE_VAR_STRING_22()
            << getVARIABLE_VAR_STRING_23()
            << getVARIABLE_VAR_STRING_24()
            << getVARIABLE_VAR_STRING_29()
            << getVARIABLE_VAR_STRING_15()
            << getVARIABLE_VAR_STRING_9()
            << getVARIABLE_VAR_STRING_10()
            << getVARIABLE_VAR_STRING_11()
            << getVARIABLE_VAR_STRING_12()
            << getVARIABLE_VAR_STRING_13()
            << getVARIABLE_VAR_STRING_25()
            << getVARIABLE_VAR_STRING_26()
            << getVARIABLE_VAR_STRING_27()
            << getVARIABLE_VAR_STRING_30()
            );
}

const QStringList VariableDesc::measureTypeStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_TYPE_STRING_0()
            << getVARIABLE_MEASURE_TYPE_STRING_1()
            << getVARIABLE_MEASURE_TYPE_STRING_2()
            << getVARIABLE_MEASURE_TYPE_STRING_3()
            );
}

const QStringList VariableDesc::inputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << getVARIABLE_MEASURE_UNIT_STRING_2()
            << getVARIABLE_MEASURE_UNIT_STRING_3()
            << getVARIABLE_MEASURE_UNIT_STRING_4()
            << getVARIABLE_MEASURE_UNIT_STRING_5()
            << getVARIABLE_MEASURE_UNIT_STRING_6()
            << getVARIABLE_MEASURE_UNIT_STRING_7()
            << getVARIABLE_MEASURE_UNIT_STRING_8()
            << getVARIABLE_MEASURE_UNIT_STRING_9()
            << getVARIABLE_MEASURE_UNIT_STRING_10()
            << getVARIABLE_MEASURE_UNIT_STRING_11()
            << getVARIABLE_MEASURE_UNIT_STRING_12()
            << getVARIABLE_MEASURE_UNIT_STRING_13()
            << getVARIABLE_MEASURE_UNIT_STRING_19()
            << getVARIABLE_MEASURE_UNIT_STRING_20()
            << getVARIABLE_MEASURE_UNIT_STRING_21()
            << getVARIABLE_MEASURE_UNIT_STRING_14()
            << getVARIABLE_MEASURE_UNIT_STRING_15()
            << getVARIABLE_MEASURE_UNIT_STRING_16()
            << getVARIABLE_MEASURE_UNIT_STRING_22()
            << getVARIABLE_MEASURE_UNIT_STRING_23()
            << getVARIABLE_MEASURE_UNIT_STRING_24()
            << getVARIABLE_MEASURE_UNIT_STRING_25()
            << getVARIABLE_MEASURE_UNIT_STRING_26()
            << getVARIABLE_MEASURE_UNIT_STRING_27()
            << getVARIABLE_MEASURE_UNIT_STRING_18()
            << getVARIABLE_MEASURE_UNIT_STRING_17()
            );
}

const QStringList VariableDesc::outputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_2()
            << getVARIABLE_MEASURE_UNIT_STRING_3()
            << getVARIABLE_MEASURE_UNIT_STRING_4()
            << getVARIABLE_MEASURE_UNIT_STRING_5()
            << getVARIABLE_MEASURE_UNIT_STRING_6()
            << getVARIABLE_MEASURE_UNIT_STRING_7()
            << getVARIABLE_MEASURE_UNIT_STRING_8()
            << getVARIABLE_MEASURE_UNIT_STRING_9()
            << getVARIABLE_MEASURE_UNIT_STRING_10()
            << getVARIABLE_MEASURE_UNIT_STRING_11()
            << getVARIABLE_MEASURE_UNIT_STRING_12()
            << getVARIABLE_MEASURE_UNIT_STRING_13()
            << getVARIABLE_MEASURE_UNIT_STRING_19()
            << getVARIABLE_MEASURE_UNIT_STRING_20()
            << getVARIABLE_MEASURE_UNIT_STRING_21()
            << getVARIABLE_MEASURE_UNIT_STRING_14()
            << getVARIABLE_MEASURE_UNIT_STRING_15()
            << getVARIABLE_MEASURE_UNIT_STRING_16()
            << getVARIABLE_MEASURE_UNIT_STRING_22()
            << getVARIABLE_MEASURE_UNIT_STRING_23()
            << getVARIABLE_MEASURE_UNIT_STRING_24()
            << getVARIABLE_MEASURE_UNIT_STRING_25()
            << getVARIABLE_MEASURE_UNIT_STRING_26()
            << getVARIABLE_MEASURE_UNIT_STRING_27()
            << getVARIABLE_MEASURE_UNIT_STRING_17()
            );
}

const QStringList VariableDesc::conversionTypeStringList()
{
    return (QStringList()
            << getVARIABLE_CONVERSION_TYPE_STRING_1()
//            << getVARIABLE_CONVERSION_TYPE_STRING_2()
            );
}

// Return string list of anem types
const QStringList VariableDesc::yesNoStringList()
{
    return (QStringList()
            << tr("yes")
            << tr("no"));
}

bool VariableDesc::isGoodWindComponent(const VariableDesc& var)
{
    const QString& name = var.variable();
    const QString& instrument = var.instrument();
    const QString& inputUnit = var.inputUnit();
    const QString& conversionType = var.conversionType();
    const QString& outputUnit = var.outputUnit();

    // 1
    bool isGoodName = false;
    if (!name.isEmpty())
    {
        isGoodName = (name == getVARIABLE_VAR_STRING_0())
                      || (name == getVARIABLE_VAR_STRING_1())
                      || (name == getVARIABLE_VAR_STRING_2())
                      || (name == getVARIABLE_VAR_STRING_16())
                      || (name == getVARIABLE_VAR_STRING_17());
    }

    // 2
    bool isGoodInstrument = false;
    if (!instrument.isEmpty())
    {
        isGoodInstrument = instrument.contains(tr("Sonic"));
    }

    // 3
    bool isGoodInputUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        if (!inputUnit.isEmpty())
        {
            isGoodInputUnit = (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_2())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_3())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_4())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
        }
    }
    else
    {
        isGoodInputUnit = true;
    }

    bool isGoodOutputUnit = false;
    if (!outputUnit.isEmpty() && outputUnit != getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        isGoodOutputUnit = (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_2())
                        || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_3())
                        || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_4());

    }

    bool isGoodUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodUnit = isGoodInputUnit;
    }
    else
    {
        isGoodUnit = isGoodOutputUnit;
    }

    // 5
    bool isGoodABValue = goodGainOffsetTest(var);

    // all
    return (isGoodName
            && isGoodInstrument
            && isGoodUnit
            && isGoodABValue);
}

namespace {

// private helper
bool isGoodGasUnit(const QString& unit, const QString& type)
{
    bool isGoodUnit = false;
    if (!unit.isEmpty())
    {
        if (type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0())
        {
            isGoodUnit = (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_12())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_13())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_19())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_20())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_21())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_18());
        }
        else if ((type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1())
                 || (type == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2()))
        {
            isGoodUnit = (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_9())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_10())
                               || (unit == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_11());
        }
    }
    return isGoodUnit;
}

} // namespace

bool VariableDesc::isGoodGas(const VariableDesc& var, bool isCustom)
{
    const QString& name = var.variable();
    const QString& instrument = var.instrument();
    const QString& measureType = var.measureType();
    const QString& inputUnit = var.inputUnit();
    const QString& conversionType = var.conversionType();
    const QString& outputUnit = var.outputUnit();

    // 1
    bool isGoodName = false;
    if (!name.isEmpty())
    {
        isGoodName = (name == getVARIABLE_VAR_STRING_5())
                      || (name == getVARIABLE_VAR_STRING_6())
                      || (name == getVARIABLE_VAR_STRING_7())
                      || (name == getVARIABLE_VAR_STRING_8())
                      || (name == getVARIABLE_VAR_STRING_19())
                      || (name == getVARIABLE_VAR_STRING_20())
                      || (name == getVARIABLE_VAR_STRING_21())
                      || (name == getVARIABLE_VAR_STRING_22())
                      || (name == getVARIABLE_VAR_STRING_23())
                      || (name == getVARIABLE_VAR_STRING_24())
                      || isCustom;
    }

    // 2
    bool isGoodInstrument = false;
    if (!instrument.isEmpty())
    {
        isGoodInstrument = instrument.contains(tr("Irga"));
    }

    // 3
    bool isGoodMeasureType = false;
    if (!measureType.isEmpty())
    {
        isGoodMeasureType = measureType != getVARIABLE_MEASURE_TYPE_STRING_3();
    }

    // 4
    bool isGoodInputUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodInputUnit = isGoodGasUnit(inputUnit, measureType);
    }
    else
    {
        isGoodInputUnit = true;
    }

    bool isGoodOutputUnit = false;
    if (!outputUnit.isEmpty() && outputUnit != getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        isGoodOutputUnit = isGoodGasUnit(outputUnit, measureType);
    }

    bool isGoodUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodUnit = isGoodInputUnit;
    }
    else
    {
        isGoodUnit = isGoodOutputUnit;
    }

    // 6
    bool isGoodABValue = goodGainOffsetTest(var);

    // all
    return (isGoodName
            && isGoodInstrument
            && isGoodMeasureType
            && isGoodUnit
            && isGoodABValue);
}

bool VariableDesc::isGoodSonicTempOrSpeed(const VariableDesc& var)
{
    const QString& name = var.variable();
    const QString& instrument = var.instrument();
    const QString& inputUnit = var.inputUnit();
    const QString& conversionType = var.conversionType();
    const QString& outputUnit = var.outputUnit();

    // 1
    bool isGoodName = false;
    if (!name.isEmpty())
    {
        isGoodName = (name == getVARIABLE_VAR_STRING_3())
                      || (name == getVARIABLE_VAR_STRING_4());
    }

    // 2
    bool isGoodInstrument = false;
    if (!instrument.isEmpty())
    {
        if (name == getVARIABLE_VAR_STRING_3()
            || name == getVARIABLE_VAR_STRING_4())
        {
            isGoodInstrument = (instrument.contains(AnemDesc::getANEM_MODEL_STRING_0()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_1()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_2()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_3()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_4()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_5()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_6()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_7()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_8()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_9()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_10()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_11()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_12()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_13()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_14()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_15()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_16()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_17()))
                                || (instrument.contains(AnemDesc::getANEM_MODEL_STRING_18()));
        }
    }

    // 3
    bool isGoodInputUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        if (!inputUnit.isEmpty())
        {
            // Ts
            if (name == getVARIABLE_VAR_STRING_3())
            {
                isGoodInputUnit = (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_5())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_6())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_7())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_8())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
            }
            // SoS
            else
            {
                isGoodInputUnit = (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_2())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_3())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_4())
                                   || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
            }
        }
    }
    else
    {
        isGoodInputUnit = true;
    }

    bool isGoodOutputUnit = false;
    if (!outputUnit.isEmpty() && outputUnit != getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        if (name == getVARIABLE_VAR_STRING_3())
        {
            isGoodOutputUnit = (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_5())
                                || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_6())
                                || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_7())
                                || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_8());
        }
        else
        {
            isGoodOutputUnit = (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_2())
                               || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_3())
                               || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_4());
        }
    }

    bool isGoodUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodUnit = isGoodInputUnit;
    }
    else
    {
        isGoodUnit = isGoodOutputUnit;
    }

    // 5
    bool isGoodABValue = goodGainOffsetTest(var);

    // all
    return (isGoodName
            && isGoodInstrument
            && isGoodUnit
            && isGoodABValue);
}

bool VariableDesc::isGoodTemperature(const VariableDesc& var, AnalogType type)
{
    const QString& name = var.variable();
    const QString& instrument = var.instrument();
    const QString& inputUnit = var.inputUnit();
    const QString& conversionType = var.conversionType();
    const QString& outputUnit = var.outputUnit();

    // 1
    bool isGoodName = false;
    if (!name.isEmpty())
    {
        if (type == AnalogType::SLOW)
        {
            isGoodName = (name == getVARIABLE_VAR_STRING_9())
                          || (name == getVARIABLE_VAR_STRING_10())
                          || (name == getVARIABLE_VAR_STRING_12())
                          || (name == getVARIABLE_VAR_STRING_15());
        }
        else
        {
            isGoodName = (name == getVARIABLE_VAR_STRING_28());
        }
    }

    // 2
    bool isGoodInstrument = false;
    if (!instrument.isEmpty())
    {
        if (type == AnalogType::SLOW)
        {
            if (name == getVARIABLE_VAR_STRING_9()
                || name == getVARIABLE_VAR_STRING_10()
                || name == getVARIABLE_VAR_STRING_15())
            {
                isGoodInstrument = (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_0()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_1()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_4()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_7()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_10()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_11()))
                                    || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_13()));
            }
            else
            {
                isGoodInstrument = true;
            }
        }
        else
        {
            isGoodInstrument = !((instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_0()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_1()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_2()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_3()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_4()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_5()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_12()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_13()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_14())));
        }
    }

    // 3
    bool isGoodInputUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        if (!inputUnit.isEmpty())
        {
            isGoodInputUnit = (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_5())
                            || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_6())
                            || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_7())
                            || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_8())
                            || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
        }
    }
    else
    {
        isGoodInputUnit = true;
    }

    bool isGoodOutputUnit = false;
    if (!outputUnit.isEmpty() && outputUnit != getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        isGoodOutputUnit = (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_5())
                            || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_6())
                            || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_7())
                            || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_8());
    }

    bool isGoodUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodUnit = isGoodInputUnit;
    }
    else
    {
        isGoodUnit = isGoodOutputUnit;
    }

    // 5
    bool isGoodABValue = goodGainOffsetTest(var);

    // all
    bool ret = isGoodName
               && isGoodInstrument
               && isGoodUnit
               && isGoodABValue;

    return ret;
}

bool VariableDesc::isGoodPressure(const VariableDesc& var)
{
    const QString& name = var.variable();
    const QString& instrument = var.instrument();
    const QString& inputUnit = var.inputUnit();
    const QString& conversionType = var.conversionType();
    const QString& outputUnit = var.outputUnit();

    // 1
    bool isGoodName = false;
    if (!name.isEmpty())
    {
        isGoodName = (name == getVARIABLE_VAR_STRING_11())
                      || (name == getVARIABLE_VAR_STRING_13());
    }

    // 2
    bool isGoodInstrument = false;
    if (!instrument.isEmpty())
    {
        if (name == getVARIABLE_VAR_STRING_11())
        {
            isGoodInstrument = (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_0()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_1()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_4()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_7()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_10()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_11()))
                                || (instrument.contains(IrgaDesc::getIRGA_MODEL_STRING_13()));
        }
        else
        {
            isGoodInstrument = true;
        }
    }

    // 3
    bool isGoodInputUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        if (!inputUnit.isEmpty())
        {
            isGoodInputUnit = (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_14())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_15())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_16())
                           || (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
        }
    }
    else
    {
        isGoodInputUnit = true;
    }

    bool isGoodOutputUnit = false;
    if (!outputUnit.isEmpty() && outputUnit != getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        isGoodOutputUnit = (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_14())
                            || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_15())
                            || (outputUnit == getVARIABLE_MEASURE_UNIT_STRING_16());
    }

    bool isGoodUnit = false;
    if (conversionType.isEmpty()
        || conversionType == getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        isGoodUnit = isGoodInputUnit;
    }
    else
    {
        isGoodUnit = isGoodOutputUnit;
    }

    // 5
    bool isGoodABValue = goodGainOffsetTest(var);

    // all
    return (isGoodName
            && isGoodInstrument
            && isGoodUnit
            && isGoodABValue);
}

bool VariableDesc::isGasVariable(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_5()
            || var == getVARIABLE_VAR_STRING_6()
            || var == getVARIABLE_VAR_STRING_7()
            || var == getVARIABLE_VAR_STRING_8()
            || var == getVARIABLE_VAR_STRING_19()
            || var == getVARIABLE_VAR_STRING_20()
            || var == getVARIABLE_VAR_STRING_21()
            || var == getVARIABLE_VAR_STRING_22()
            || var == getVARIABLE_VAR_STRING_23()
            || var == getVARIABLE_VAR_STRING_24());
}

bool VariableDesc::isCustomVariable(const QString& var)
{
    return (var != getVARIABLE_VAR_STRING_0()
            && var != getVARIABLE_VAR_STRING_1()
            && var != getVARIABLE_VAR_STRING_2()
            && var != getVARIABLE_VAR_STRING_3()
            && var != getVARIABLE_VAR_STRING_4()
            && var != getVARIABLE_VAR_STRING_5()
            && var != getVARIABLE_VAR_STRING_6()
            && var != getVARIABLE_VAR_STRING_7()
            && var != getVARIABLE_VAR_STRING_8()
            && var != getVARIABLE_VAR_STRING_9()
            && var != getVARIABLE_VAR_STRING_10()
            && var != getVARIABLE_VAR_STRING_11()
            && var != getVARIABLE_VAR_STRING_12()
            && var != getVARIABLE_VAR_STRING_13()
            && var != getVARIABLE_VAR_STRING_14()
            && var != getVARIABLE_VAR_STRING_15()
            && var != getVARIABLE_VAR_STRING_16()
            && var != getVARIABLE_VAR_STRING_17()
            && var != getVARIABLE_VAR_STRING_18()
            && var != getVARIABLE_VAR_STRING_19()
            && var != getVARIABLE_VAR_STRING_20()
            && var != getVARIABLE_VAR_STRING_21()
            && var != getVARIABLE_VAR_STRING_22()
            && var != getVARIABLE_VAR_STRING_23()
            && var != getVARIABLE_VAR_STRING_24()
            && var != getVARIABLE_VAR_STRING_25()
            && var != getVARIABLE_VAR_STRING_26()
            && var != getVARIABLE_VAR_STRING_27()
            && var != getVARIABLE_VAR_STRING_28()
            && var != getVARIABLE_VAR_STRING_29()
            && var != getVARIABLE_VAR_STRING_30());
}

bool VariableDesc::isScalableVariable(const QString& inputUnit)
{
    return (inputUnit == getVARIABLE_MEASURE_UNIT_STRING_0()
            || inputUnit == getVARIABLE_MEASURE_UNIT_STRING_1()
            || inputUnit == getVARIABLE_MEASURE_UNIT_STRING_17()
            || inputUnit == getVARIABLE_MEASURE_UNIT_STRING_18());
}

bool VariableDesc::isVelocityVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_0()
            || var == getVARIABLE_VAR_STRING_1()
            || var == getVARIABLE_VAR_STRING_2()
            || var == getVARIABLE_VAR_STRING_4()
            || var == getVARIABLE_VAR_STRING_16());
}

bool VariableDesc::isAngleVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_17());
}

bool VariableDesc::isTemperatureVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_3()
            || var == getVARIABLE_VAR_STRING_9()
            || var == getVARIABLE_VAR_STRING_10()
            || var == getVARIABLE_VAR_STRING_12()
            || var == getVARIABLE_VAR_STRING_15()
            || var == getVARIABLE_VAR_STRING_28());
}

bool VariableDesc::isPressureVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_11()
            || var == getVARIABLE_VAR_STRING_13());
}

bool VariableDesc::isFlowRateVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_29());
}

bool VariableDesc::isDiagnosticVar(const QString& var)
{
    return (var == getVARIABLE_VAR_STRING_25()
            || var == getVARIABLE_VAR_STRING_26()
            || var == getVARIABLE_VAR_STRING_27()
            || var == getVARIABLE_VAR_STRING_30());
}

const QStringList VariableDesc::velocityInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << velocityOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::velocityOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_2()
            << getVARIABLE_MEASURE_UNIT_STRING_3()
            << getVARIABLE_MEASURE_UNIT_STRING_4());
}

const QStringList VariableDesc::angleInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << angleOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::angleOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_27());
}

const QStringList VariableDesc::temperatureInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << temperatureOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::temperatureOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_5()
            << getVARIABLE_MEASURE_UNIT_STRING_6()
            << getVARIABLE_MEASURE_UNIT_STRING_7()
            << getVARIABLE_MEASURE_UNIT_STRING_8());
}

const QStringList VariableDesc::pressureInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << pressureOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::pressureOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_14()
            << getVARIABLE_MEASURE_UNIT_STRING_15()
            << getVARIABLE_MEASURE_UNIT_STRING_16());
}

const QStringList VariableDesc::gasInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << gasOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::gasOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_9()
            << getVARIABLE_MEASURE_UNIT_STRING_10()
            << getVARIABLE_MEASURE_UNIT_STRING_11()
            << getVARIABLE_MEASURE_UNIT_STRING_12()
            << getVARIABLE_MEASURE_UNIT_STRING_13()
            << getVARIABLE_MEASURE_UNIT_STRING_19()
            << getVARIABLE_MEASURE_UNIT_STRING_20()
            << getVARIABLE_MEASURE_UNIT_STRING_21());
}

const QStringList VariableDesc::flowRateInputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_0()
            << getVARIABLE_MEASURE_UNIT_STRING_1()
            << flowRateOutputUnitStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_18());
}

const QStringList VariableDesc::flowRateOutputUnitStringList()
{
    return (QStringList()
            << getVARIABLE_MEASURE_UNIT_STRING_22()
            << getVARIABLE_MEASURE_UNIT_STRING_23()
            << getVARIABLE_MEASURE_UNIT_STRING_24()
            << getVARIABLE_MEASURE_UNIT_STRING_25()
            << getVARIABLE_MEASURE_UNIT_STRING_26());
}

bool VariableDesc::goodGainOffsetTest(const VariableDesc& var)
{
    const QString& conversionType = var.conversionType();
    qreal aValue = var.aValue();
    qreal bValue = var.bValue();

    // zero-full scale
    if (conversionType == getVARIABLE_CONVERSION_TYPE_STRING_0())
    {
        return !qFuzzyCompare(aValue, bValue);
    }
    // gain-offset
    if (conversionType == getVARIABLE_CONVERSION_TYPE_STRING_1())
    {
        return (aValue != 0.0);
    }
    return true;
}
