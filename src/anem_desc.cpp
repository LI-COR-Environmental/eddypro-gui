/***************************************************************************
  anem_desc.cpp
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

#include "anem_desc.h"

#include <QDebug>
#include <QStringList>

const QString AnemDesc::getANEM_MANUFACTURER_STRING_0()
{
    static const auto s(QStringLiteral("Campbell Scientific"));
    return s;
}

const QString AnemDesc::getANEM_MANUFACTURER_STRING_1()
{
    static const auto s(QStringLiteral("Gill"));
    return s;
}

const QString AnemDesc::getANEM_MANUFACTURER_STRING_2()
{
    static const QString s(QStringLiteral("Metek"));
    return s;
}

const QString AnemDesc::getANEM_MANUFACTURER_STRING_3()
{
    static const QString s(QStringLiteral("Young"));
    return s;
}

const QString AnemDesc::getANEM_MANUFACTURER_STRING_4()
{
    static const QString s(QStringLiteral("Other"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_0()
{
    static const QString s(QStringLiteral("CSAT-3"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_1()
{
    static const QString s(QStringLiteral("HS-50"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_2()
{
    static const QString s(QStringLiteral("HS-100"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_3()
{
    static const QString s(QStringLiteral("R2"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_4()
{
    static const QString s(QStringLiteral("R3-50"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_5()
{
    static const QString s(QStringLiteral("R3-100"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_6()
{
    static const QString s(QStringLiteral("R3A-100"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_7()
{
    static const QString s(QStringLiteral("Windmaster"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_8()
{
    static const QString s(QStringLiteral("Windmaster Pro"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_9()
{
    static const QString s(QStringLiteral("USA-1 Standard"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_10()
{
    static const QString s(QStringLiteral("USA-1 Fast"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_11()
{
    static const QString s(QStringLiteral("81000"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_12()
{
    static const QString s(tr("Generic Anemometer"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_13()
{
    static const QString s(QStringLiteral("CSAT-3B"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_14()
{
    static const QString s(QStringLiteral("uSonic-3 Class A MP"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_15()
{
    static const QString s(QStringLiteral("uSonic-3 Cage MP"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_16()
{
    static const QString s(QStringLiteral("81000V"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_17()
{
    static const QString s(QStringLiteral("81000RE"));
    return s;
}

const QString AnemDesc::getANEM_MODEL_STRING_18()
{
    static const QString s(QStringLiteral("81000VRE"));
    return s;
}

const QString AnemDesc::getANEM_WIND_FORMAT_STRING_0()
{
    static const QString s(QStringLiteral("U, V & W"));
    return s;
}

const QString AnemDesc::getANEM_WIND_FORMAT_STRING_1()
{
    static const QString s(QStringLiteral("Polar & W"));
    return s;
}

const QString AnemDesc::getANEM_WIND_FORMAT_STRING_2()
{
    static const QString s(QStringLiteral("Axis velocities"));
    return s;
}

const QString AnemDesc::getANEM_NORTH_ALIGN_STRING_0()
{
    static const QString s(QStringLiteral("Axis"));
    return s;
}

const QString AnemDesc::getANEM_NORTH_ALIGN_STRING_1()
{
    static const QString s(QStringLiteral("Spar"));
    return s;
}

const QString AnemDesc::getANEM_NORTH_ALIGN_STRING_2()
{
    static const QString s(QStringLiteral("N/A"));
    return s;
}

AnemDesc::AnemDesc() :
    manufacturer_(QString()),
    model_(QString()),
    swVersion_(QString()),
    id_(QString()),
    height_(0.1),
    windFormat_(getANEM_WIND_FORMAT_STRING_0()),
    northAlignment_(QString()),
    northOffset_(0.0),
    nSeparation_(0.0),
    eSeparation_(0.0),
    vSeparation_(0.0),
    vPathLength_(1.0),
    hPathLength_(1.0),
    tau_(0.1),
    hasGoodWindComponents_(false),
    hasGoodTemp_(false)
{ ; }

AnemDesc::AnemDesc (QString manufacturer,
                    QString model,
                    QString swVersion,
                    QString id,
                    qreal height,
                    QString windFormat,
                    QString northAlignment,
                    qreal northOffset,
                    qreal nSeparation,
                    qreal eSeparation,
                    qreal vSeparation,
                    qreal vPathLength,
                    qreal hPathLength,
                    qreal tau,
                    bool hasGoodWindComponents,
                    bool hasGoodTemp) :
    manufacturer_(std::move(manufacturer)),
    model_(std::move(model)),
    swVersion_(std::move(swVersion)),
    id_(std::move(id)),
    height_(height),
    windFormat_(std::move(windFormat)),
    northAlignment_(std::move(northAlignment)),
    northOffset_(northOffset),
    nSeparation_(nSeparation),
    eSeparation_(eSeparation),
    vSeparation_(vSeparation),
    vPathLength_(vPathLength),
    hPathLength_(hPathLength),
    tau_(tau),
    hasGoodWindComponents_(hasGoodWindComponents),
    hasGoodTemp_(hasGoodTemp)
{ ; }

AnemDesc::AnemDesc(const AnemDesc& anem) :
    manufacturer_(anem.manufacturer_),
    model_(anem.model_),
    swVersion_(anem.swVersion_),
    id_(anem.id_),
    height_(anem.height_),
    windFormat_(anem.windFormat_),
    northAlignment_(anem.northAlignment_),
    northOffset_(anem.northOffset_),
    nSeparation_(anem.nSeparation_),
    eSeparation_(anem.eSeparation_),
    vSeparation_(anem.vSeparation_),
    vPathLength_(anem.vPathLength_),
    hPathLength_(anem.hPathLength_),
    tau_(anem.tau_),
    hasGoodWindComponents_(anem.hasGoodWindComponents_),
    hasGoodTemp_(anem.hasGoodTemp_)
{ ; }

AnemDesc& AnemDesc::operator=(const AnemDesc& anem)
{
    if (this != &anem)
    {
        manufacturer_ = anem.manufacturer_;
        model_ = anem.model_;
        swVersion_ = anem.swVersion_;
        id_ = anem.id_;
        height_ = anem.height_;
        windFormat_ = anem.windFormat_;
        northAlignment_ = anem.northAlignment_;
        northOffset_ = anem.northOffset_;
        nSeparation_ = anem.nSeparation_;
        eSeparation_ = anem.eSeparation_;
        vSeparation_ = anem.vSeparation_;
        vPathLength_ = anem.vPathLength_;
        hPathLength_ = anem.hPathLength_;
        tau_ = anem.tau_;
        hasGoodWindComponents_ = anem.hasGoodWindComponents_;
        hasGoodTemp_ = anem.hasGoodTemp_;
    }
    return *this;
}

// Equivalence operator
bool AnemDesc::operator==(const AnemDesc& anem) const
{
    return ((manufacturer_ == anem.manufacturer_)
             && (model_ == anem.model_)
             && (swVersion_ == anem.swVersion_)
             && (id_ == anem.id_)
             && qFuzzyCompare(height_, anem.height_)
             && (windFormat_ == anem.windFormat_)
             && (northAlignment_ == anem.northAlignment_)
             && qFuzzyCompare(northOffset_, anem.northOffset_)
             && qFuzzyCompare(nSeparation_, anem.nSeparation_)
             && qFuzzyCompare(eSeparation_, anem.eSeparation_)
             && qFuzzyCompare(vSeparation_, anem.vSeparation_)
             && qFuzzyCompare(vPathLength_, anem.vPathLength_)
             && qFuzzyCompare(hPathLength_, anem.hPathLength_)
             && qFuzzyCompare(tau_, anem.tau_)
             && (hasGoodWindComponents_ == anem.hasGoodWindComponents_)
             && (hasGoodTemp_ == anem.hasGoodTemp_));
}

// Return string list of anem types
const QStringList AnemDesc::manufacturerStringList()
{
    return (QStringList()
            << getANEM_MANUFACTURER_STRING_0()
            << getANEM_MANUFACTURER_STRING_1()
            << getANEM_MANUFACTURER_STRING_2()
            << getANEM_MANUFACTURER_STRING_3()
            << getANEM_MANUFACTURER_STRING_4());
}

// Return string list of usage types
const QStringList AnemDesc::allModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_0()
            << getANEM_MODEL_STRING_1()
            << getANEM_MODEL_STRING_2()
            << getANEM_MODEL_STRING_3()
            << getANEM_MODEL_STRING_4()
            << getANEM_MODEL_STRING_5()
            << getANEM_MODEL_STRING_6()
            << getANEM_MODEL_STRING_7()
            << getANEM_MODEL_STRING_8()
            << getANEM_MODEL_STRING_9()
            << getANEM_MODEL_STRING_10()
            << getANEM_MODEL_STRING_11()
            << getANEM_MODEL_STRING_12()
            << getANEM_MODEL_STRING_13()
//            << getANEM_MODEL_STRING_14()
//            << getANEM_MODEL_STRING_15()
            << getANEM_MODEL_STRING_16()
            << getANEM_MODEL_STRING_17()
            << getANEM_MODEL_STRING_18()
            );
}

// Return string list of usage types
const QStringList AnemDesc::campbellModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_0()
            << getANEM_MODEL_STRING_13()
            << getANEM_MODEL_STRING_12());
}

// Return string list of usage types
const QStringList AnemDesc::gillModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_1()
            << getANEM_MODEL_STRING_2()
            << getANEM_MODEL_STRING_3()
            << getANEM_MODEL_STRING_4()
            << getANEM_MODEL_STRING_5()
            << getANEM_MODEL_STRING_6()
            << getANEM_MODEL_STRING_7()
            << getANEM_MODEL_STRING_8()
            << getANEM_MODEL_STRING_12());
}

// Return string list of usage types
const QStringList AnemDesc::metekModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_9()
            << getANEM_MODEL_STRING_10()
//            << getANEM_MODEL_STRING_14()
//            << getANEM_MODEL_STRING_15()
            << getANEM_MODEL_STRING_12()
            );
}

// Return string list of usage types
const QStringList AnemDesc::youngModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_11()
            << getANEM_MODEL_STRING_16()
            << getANEM_MODEL_STRING_17()
            << getANEM_MODEL_STRING_18()
            << getANEM_MODEL_STRING_12()
            );
}

// Return string list of usage types
const QStringList AnemDesc::otherModelStringList()
{
    return (QStringList()
            << getANEM_MODEL_STRING_12());
}

// Return string list of anem types
const QStringList AnemDesc::allWindFormatStringList()
{
    return (QStringList()
            << getANEM_WIND_FORMAT_STRING_0()
            << getANEM_WIND_FORMAT_STRING_1()
            << getANEM_WIND_FORMAT_STRING_2());
}

const QStringList AnemDesc::commonWindFormatStringList()
{
    return (QStringList()
            << getANEM_WIND_FORMAT_STRING_0()
            << getANEM_WIND_FORMAT_STRING_1());
}

const QStringList AnemDesc::simplestWindFormatStringList()
{
    return (QStringList()
            << getANEM_WIND_FORMAT_STRING_0());
}

// Return string list of anem types
const QStringList AnemDesc::allNorthAlignmentStringList()
{
    return (QStringList()
            << getANEM_NORTH_ALIGN_STRING_0()
            << getANEM_NORTH_ALIGN_STRING_1()
            << getANEM_NORTH_ALIGN_STRING_2());
}

const QStringList AnemDesc::gillNorthAlignmentStringList()
{
    return (QStringList()
            << getANEM_NORTH_ALIGN_STRING_0()
            << getANEM_NORTH_ALIGN_STRING_1());
}

const QStringList AnemDesc::naNorthAlignmentStringList()
{
    return (QStringList()
            << getANEM_NORTH_ALIGN_STRING_2());
}

// Return string list of anem types
const QStringList AnemDesc::yesNoStringList()
{
    return (QStringList()
            << tr("yes")
            << tr("no"));
}

bool AnemDesc::isGoodAnemometer(const AnemDesc& anem)
{
    const auto& manufacturer = anem.manufacturer();
    const auto& model = anem.model();
    qreal height = anem.height();
    const auto& windFormat = anem.windFormat();
    const auto& northAlignment = anem.northAlignment();

    // 1
    bool isGoodManufacturer = !manufacturer.isEmpty();

    // 2
    bool isGoodModel = false;
    if (!model.isEmpty())
    {
        if (manufacturer == getANEM_MANUFACTURER_STRING_0())
        {
            isGoodModel = (model == getANEM_MODEL_STRING_0())
                           || (model == getANEM_MODEL_STRING_12())
                           || (model == getANEM_MODEL_STRING_13());
        }
        else if (manufacturer == getANEM_MANUFACTURER_STRING_1())
        {
            isGoodModel = (model == getANEM_MODEL_STRING_1())
                || (model == getANEM_MODEL_STRING_2())
                || (model == getANEM_MODEL_STRING_3())
                || (model == getANEM_MODEL_STRING_4())
                || (model == getANEM_MODEL_STRING_5())
                || (model == getANEM_MODEL_STRING_6())
                || (model == getANEM_MODEL_STRING_7())
                || (model == getANEM_MODEL_STRING_8())
                || (model == getANEM_MODEL_STRING_12());
        }
        else if (manufacturer == getANEM_MANUFACTURER_STRING_2())
        {
            isGoodModel = (model == getANEM_MODEL_STRING_9())
                           || (model == getANEM_MODEL_STRING_10())
                           || (model == getANEM_MODEL_STRING_14())
                           || (model == getANEM_MODEL_STRING_15())
                           || (model == getANEM_MODEL_STRING_12());
        }
        else if (manufacturer == getANEM_MANUFACTURER_STRING_3())
        {
            isGoodModel = (model == getANEM_MODEL_STRING_11())
                           || (model == getANEM_MODEL_STRING_16())
                           || (model == getANEM_MODEL_STRING_17())
                           || (model == getANEM_MODEL_STRING_18())
                           || (model == getANEM_MODEL_STRING_12());
        }
        else if (manufacturer == getANEM_MANUFACTURER_STRING_4())
        {
            isGoodModel = (model == getANEM_MODEL_STRING_12());
        }
    }

    // 3
    // always true by min value
    bool isGoodHeight = height > 0.0;

    // 4
    bool isGoodWindFormat = false;
    if (manufacturer != getANEM_MANUFACTURER_STRING_1())
    {
        isGoodWindFormat = (windFormat == getANEM_WIND_FORMAT_STRING_0())
                            || (windFormat == getANEM_WIND_FORMAT_STRING_1());
    }
    else
    {
        isGoodWindFormat = true;
    }

    // 5
    bool isGoodNorthAlignment = false;
    if (manufacturer == getANEM_MANUFACTURER_STRING_1())
    {
        isGoodNorthAlignment = (northAlignment == getANEM_NORTH_ALIGN_STRING_0())
                                || (northAlignment == getANEM_NORTH_ALIGN_STRING_1());
    }
    else
    {
        isGoodNorthAlignment = northAlignment.isEmpty()
                                || (northAlignment == getANEM_NORTH_ALIGN_STRING_2());
    }

    // 6
    // always true by min value
    bool isGoodGeneric = false;
    if ((model == getANEM_MODEL_STRING_12()))
    {
        isGoodGeneric = (anem.hPathLength() > 0) && (anem.vPathLength() > 0) && (anem.tau() > 0);
    }
    else
    {
        isGoodGeneric = true;
    }

    // all
    return (isGoodManufacturer
            && isGoodModel
            && isGoodHeight
            && isGoodWindFormat
            && isGoodNorthAlignment
            && isGoodGeneric);
}
