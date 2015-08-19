/***************************************************************************
  irga_desc.cpp
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

#include "irga_desc.h"

#include <QDebug>
#include <QStringList>

#include "defs.h"

const QString IrgaDesc::getIRGA_MANUFACTURER_STRING_0()
{
    static const QString s(QStringLiteral("LI-COR"));
    return s;
}

const QString IrgaDesc::getIRGA_MANUFACTURER_STRING_1()
{
    static const QString s(tr("Other"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_0()
{
    static const QString s(QStringLiteral("LI-6262"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_1()
{
    static const QString s(QStringLiteral("LI-7000"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_2()
{
    static const QString s(QStringLiteral("LI-7500"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_3()
{
    static const QString s(QStringLiteral("LI-7500A"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_4()
{
    static const QString s(QStringLiteral("LI-7200"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_5()
{
    static const QString s(QStringLiteral("LI-7700"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_6()
{
    static const QString s(tr("Generic Open Path"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_7()
{
    static const QString s(tr("Generic Closed Path"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_8()
{
    static const QString s(QObject::tr("OP Krypton Hygrometer"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_9()
{
    static const QString s(QStringLiteral("OP Lyman-")
                           + Defs::ALPHA
                           + QObject::tr(" Hygrometer"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_10()
{
    static const QString s(QObject::tr("CP Krypton Hygrometer"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_11()
{
    static const QString s(QStringLiteral("CP Lyman-")
                           + Defs::ALPHA
                           + QObject::tr(" Hygrometer"));
    return s;
}

const QString IrgaDesc::getIRGA_SW_VERSION_0()
{
    static const QString s(QStringLiteral("Unknown"));
    return s;
}

const QString IrgaDesc::getIRGA_SW_VERSION_1()
{
    static const QString s(QStringLiteral("5.0.3 and below"));
    return s;
}

const QString IrgaDesc::getIRGA_SW_VERSION_2()
{
    static const QString s(QStringLiteral("6.0.0"));
    return s;
}

const QString IrgaDesc::getIRGA_SW_VERSION_3()
{
    static const QString s(QStringLiteral("6.5.0 and above"));
    return s;
}

IrgaDesc::IrgaDesc() :
    manufacturer_(QString(getIRGA_MANUFACTURER_STRING_0())),
    model_(QString()),
    swVersion_(QString()),
    sn_(QString()),
    id_(QString()),
//    height_(0.1),
    tubeLength_(0.0),
    tubeDiameter_(0.0),
    tubeFlowRate_(0.0),
    nSeparation_(0.0),
    eSeparation_(0.0),
    vSeparation_(0.0),
    vPathLength_(1.0),
    hPathLength_(1.0),
    tau_(0.1),
    kWater_(0.15),
    kOxygen_(0.0085)
{ ; }

IrgaDesc::IrgaDesc(const QString& manufacture,
                   const QString& model,
                   const QString& swVersion,
                   const QString& id,
//                   qreal height,
                   qreal tubeLength,
                   qreal tubeDiameter,
                   qreal tubeFlowRate,
                   qreal nSeparation,
                   qreal eSeparation,
                   qreal vSeparation,
                   qreal vPathLength,
                   qreal hPathLength,
                   qreal tau,
                   qreal kWater,
                   qreal kOxygen) :
    manufacturer_(manufacture),
    model_(model),
    swVersion_(swVersion),
    id_(id),
//    height_(height),
    tubeLength_(tubeLength),
    tubeDiameter_(tubeDiameter),
    tubeFlowRate_(tubeFlowRate),
    nSeparation_(nSeparation),
    eSeparation_(eSeparation),
    vSeparation_(vSeparation),
    vPathLength_(vPathLength),
    hPathLength_(hPathLength),
    tau_(tau),
    kWater_(kWater),
    kOxygen_(kOxygen)
{ ; }

IrgaDesc::~IrgaDesc() { ; }

IrgaDesc::IrgaDesc(const IrgaDesc& irga) :
    manufacturer_(irga.manufacturer_),
    model_(irga.model_),
    swVersion_(irga.swVersion_),
    sn_(irga.sn_),
    id_(irga.id_),
//    height_(irga.height_),
    tubeLength_(irga.tubeLength_),
    tubeDiameter_(irga.tubeDiameter_),
    tubeFlowRate_(irga.tubeFlowRate_),
    nSeparation_(irga.nSeparation_),
    eSeparation_(irga.eSeparation_),
    vSeparation_(irga.vSeparation_),
    vPathLength_(irga.vPathLength_),
    hPathLength_(irga.hPathLength_),
    tau_(irga.tau_),
    kWater_(irga.kWater_),
    kOxygen_(irga.kOxygen_)
{ ; }

IrgaDesc& IrgaDesc::operator=(const IrgaDesc& irga)
{
    if (this != &irga)
    {
        manufacturer_ = irga.manufacturer_;
        model_ = irga.model_;
        swVersion_ = irga.swVersion_;
        sn_ = irga.sn_;
        id_ = irga.id_;
//        height_ = irga.height_;
        tubeLength_ = irga.tubeLength_;
        tubeDiameter_ = irga.tubeDiameter_;
        tubeFlowRate_ = irga.tubeFlowRate_;
        nSeparation_ = irga.nSeparation_;
        eSeparation_ = irga.eSeparation_;
        vSeparation_ = irga.vSeparation_;
        vPathLength_ = irga.vPathLength_;
        hPathLength_ = irga.hPathLength_;
        tau_ = irga.tau_;
        kWater_ = irga.kWater_;
        kOxygen_ = irga.kOxygen_;
    }
    return *this;
}

bool IrgaDesc::operator==(const IrgaDesc& irga) const
{
    return (manufacturer_ == irga.manufacturer_)
            && (model_ == irga.model_)
            && (swVersion_ == irga.swVersion_)
            && (sn_ == irga.sn_)
            && (id_ == irga.id_)
//            && (height_ == irga.height_)
            && (tubeLength_ == irga.tubeLength_)
            && (tubeDiameter_ == irga.tubeDiameter_)
            && (tubeFlowRate_ == irga.tubeFlowRate_)
            && (nSeparation_ == irga.nSeparation_)
            && (eSeparation_ == irga.eSeparation_)
            && (vSeparation_ == irga.vSeparation_)
            && (vPathLength_ == irga.vPathLength_)
            && (hPathLength_ == irga.hPathLength_)
            && (tau_ == irga.tau_)
            && (kWater_ == irga.kWater_)
            && (kOxygen_ == irga.kOxygen_);
}

// Return string list of anem measures types
const QStringList IrgaDesc::manufacturerStringList()
{
    return (QStringList()
            << getIRGA_MANUFACTURER_STRING_0()
            << getIRGA_MANUFACTURER_STRING_1());
}

// Return string list of usage types
const QStringList IrgaDesc::allModelStringList()
{
    return (QStringList()
            << getIRGA_MODEL_STRING_0()
            << getIRGA_MODEL_STRING_1()
            << getIRGA_MODEL_STRING_2()
            << getIRGA_MODEL_STRING_3()
            << getIRGA_MODEL_STRING_4()
            << getIRGA_MODEL_STRING_5()
            << getIRGA_MODEL_STRING_6()
            << getIRGA_MODEL_STRING_7()
            << getIRGA_MODEL_STRING_8()
            << getIRGA_MODEL_STRING_9()
            << getIRGA_MODEL_STRING_10()
            << getIRGA_MODEL_STRING_11());
}

// Return string list of usage types
const QStringList IrgaDesc::licorModelStringList()
{
    return (QStringList()
            << getIRGA_MODEL_STRING_0()
            << getIRGA_MODEL_STRING_1()
            << getIRGA_MODEL_STRING_2()
            << getIRGA_MODEL_STRING_3()
            << getIRGA_MODEL_STRING_4()
            << getIRGA_MODEL_STRING_5());
}

// Return string list of usage types
const QStringList IrgaDesc::otherModelStringList()
{
    return (QStringList()
            << getIRGA_MODEL_STRING_6()
            << getIRGA_MODEL_STRING_7()
            << getIRGA_MODEL_STRING_8()
            << getIRGA_MODEL_STRING_9()
            << getIRGA_MODEL_STRING_10()
            << getIRGA_MODEL_STRING_11());
}

// Return string list of sw versions
const QStringList IrgaDesc::allSwVersionStringList()
{
    return (QStringList()
            << getIRGA_SW_VERSION_0()
            << getIRGA_SW_VERSION_1()
            << getIRGA_SW_VERSION_2()
            << getIRGA_SW_VERSION_3());
}

bool IrgaDesc::isGoodIrga(IrgaDesc irga)
{
    const QString manufacturer = irga.manufacturer();
    const QString model = irga.model();
//    qreal height = irga.height();
    qreal tubeLength = irga.tubeLength();
    qreal tubeDiameter = irga.tubeDiameter();
    qreal tubeFlowRate = irga.tubeFlowRate();
    qreal NSeparation = irga.tubeNSeparation();
    qreal ESeparation = irga.tubeESeparation();
    qreal VSeparation = irga.tubeVSeparation();
    qreal hPathLength = irga.hPathLength();
    qreal vPathLength = irga.vPathLength();
    qreal tau = irga.tau();
    qreal kWater = irga.kWater();
    qreal kOxygen = irga.kOxygen();

    // 1
    bool isGoodManufacturer = !manufacturer.isEmpty();

    // 2
    bool isLicorModel = ((model == getIRGA_MODEL_STRING_0())
                         || (model == getIRGA_MODEL_STRING_1())
                         || (model == getIRGA_MODEL_STRING_2())
                         || (model == getIRGA_MODEL_STRING_3())
                         || (model == getIRGA_MODEL_STRING_4())
                         || (model == getIRGA_MODEL_STRING_5()));

    bool isGoodModel = false;
    if (!model.isEmpty())
    {
        if (manufacturer == getIRGA_MANUFACTURER_STRING_0())
        {
            isGoodModel = isLicorModel;
        }
        else
        {
            isGoodModel = ((model == getIRGA_MODEL_STRING_6())
                || (model == getIRGA_MODEL_STRING_7())
                || (model == getIRGA_MODEL_STRING_8())
                || (model == getIRGA_MODEL_STRING_9())
                || (model == getIRGA_MODEL_STRING_10())
                || (model == getIRGA_MODEL_STRING_11()));
        }
    }

    // 3
    // always true by min value
//    bool isGoodHeight = height > 0;

    // 4
    bool isGoodTubeLength = tubeLength > 0;

    bool isGoodTubeDiameter = tubeDiameter > 0;

    bool isGoodTubeFlowRate = tubeFlowRate > 0;

    bool isClosedPath = (model == getIRGA_MODEL_STRING_0())
                        || (model == getIRGA_MODEL_STRING_1())
                        || (model == getIRGA_MODEL_STRING_4())
                        || (model == getIRGA_MODEL_STRING_7())
                        || (model == getIRGA_MODEL_STRING_10())
                        || (model == getIRGA_MODEL_STRING_11());

    bool isGoodClosedPath = false;
    if (isClosedPath)
    {
        isGoodClosedPath = isGoodTubeLength
                            && isGoodTubeDiameter
                            && isGoodTubeFlowRate;
    }
    else
    {
        isGoodClosedPath = true;
    }

    // 5
    bool isGoodSeparation = false;
    isGoodSeparation = (NSeparation != 0)
                        || (ESeparation != 0)
                        || (VSeparation != 0);

    // 6
    bool isGoodPathLength = false;
    if (isLicorModel)
    {
        isGoodPathLength = true;
    }
    else
    {
        isGoodPathLength = (hPathLength > 0)
                            && (vPathLength > 0)
                            && (tau > 0);
    }

    // 7
    bool isGoodKorLAnalyzer = false;
    if ((model == getIRGA_MODEL_STRING_8())
        || (model == getIRGA_MODEL_STRING_9())
        || (model == getIRGA_MODEL_STRING_10())
        || (model == getIRGA_MODEL_STRING_11()))
    {
        isGoodKorLAnalyzer = (kWater > 0) && (kOxygen > 0);
    }
    else
    {
        isGoodKorLAnalyzer = true;
    }

    qDebug() << ">> isGoodManufacturer" << isGoodManufacturer;
    qDebug() << ">> isGoodModel" << isGoodModel;
//    qDebug() << ">> isGoodHeight" << isGoodHeight;
    qDebug() << ">> isGoodClosedPath" << isGoodClosedPath;
    qDebug() << ">> isGoodSeparation" << isGoodSeparation;
    qDebug() << ">> isGoodPathLength" << isGoodPathLength;
    qDebug() << ">> isGoodKorLAnalyzer" << isGoodKorLAnalyzer;

    // all
    return (isGoodManufacturer
            && isGoodModel
//            && isGoodHeight
            && isGoodClosedPath
            && isGoodSeparation
            && isGoodPathLength
            && isGoodKorLAnalyzer);
}

bool IrgaDesc::isOpenPathModel(const QString& model)
{
    return (model == IrgaDesc::getIRGA_MODEL_STRING_2()
             || model == IrgaDesc::getIRGA_MODEL_STRING_3()
             || model == IrgaDesc::getIRGA_MODEL_STRING_5()
             || model == IrgaDesc::getIRGA_MODEL_STRING_6()
             || model == IrgaDesc::getIRGA_MODEL_STRING_8()
             || model == IrgaDesc::getIRGA_MODEL_STRING_9());
}
