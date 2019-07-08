/***************************************************************************
  irga_desc.cpp
  -------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

const QString IrgaDesc::getIRGA_MODEL_STRING_12()
{
    static const QString s(QStringLiteral("LI-7500RS"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_13()
{
    static const QString s(QStringLiteral("LI-7200RS"));
    return s;
}

const QString IrgaDesc::getIRGA_MODEL_STRING_14()
{
    static const QString s(QStringLiteral("LI-7500DS"));
    return s;
}

IrgaDesc::IrgaDesc() :
    manufacturer_(QString(getIRGA_MANUFACTURER_STRING_0())),
    model_(QString()),
    swVersion_(QString()),
    id_(QString()),
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

IrgaDesc::IrgaDesc(QString manufacture,
                   QString model,
                   QString swVersion,
                   QString id,
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
    manufacturer_(std::move(manufacture)),
    model_(std::move(model)),
    swVersion_(std::move(swVersion)),
    id_(std::move(id)),
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

IrgaDesc::IrgaDesc(const IrgaDesc& irga) :
    manufacturer_(irga.manufacturer_),
    model_(irga.model_),
    swVersion_(irga.swVersion_),
    id_(irga.id_),
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
        id_ = irga.id_;
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
            && (id_ == irga.id_)
            && qFuzzyCompare(tubeLength_, irga.tubeLength_)
            && qFuzzyCompare(tubeDiameter_, irga.tubeDiameter_)
            && qFuzzyCompare(tubeFlowRate_, irga.tubeFlowRate_)
            && qFuzzyCompare(nSeparation_, irga.nSeparation_)
            && qFuzzyCompare(eSeparation_, irga.eSeparation_)
            && qFuzzyCompare(vSeparation_, irga.vSeparation_)
            && qFuzzyCompare(vPathLength_, irga.vPathLength_)
            && qFuzzyCompare(hPathLength_, irga.hPathLength_)
            && qFuzzyCompare(tau_, irga.tau_)
            && qFuzzyCompare(kWater_, irga.kWater_)
            && qFuzzyCompare(kOxygen_, irga.kOxygen_);
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
            << getIRGA_MODEL_STRING_12()
            << getIRGA_MODEL_STRING_4()
            << getIRGA_MODEL_STRING_13()
            << getIRGA_MODEL_STRING_5()
            << getIRGA_MODEL_STRING_6()
            << getIRGA_MODEL_STRING_7()
            << getIRGA_MODEL_STRING_8()
            << getIRGA_MODEL_STRING_9()
            << getIRGA_MODEL_STRING_10()
            << getIRGA_MODEL_STRING_11()
            << getIRGA_MODEL_STRING_14());
}

// Return string list of usage types
const QStringList IrgaDesc::licorModelStringList()
{
    return (QStringList()
            << getIRGA_MODEL_STRING_0()
            << getIRGA_MODEL_STRING_1()
            << getIRGA_MODEL_STRING_2()
            << getIRGA_MODEL_STRING_3()
            << getIRGA_MODEL_STRING_12()
            << getIRGA_MODEL_STRING_4()
            << getIRGA_MODEL_STRING_13()
            << getIRGA_MODEL_STRING_5()
            << getIRGA_MODEL_STRING_14());
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

bool IrgaDesc::isALicorModel(const QString& model)
{
    return ((model == getIRGA_MODEL_STRING_0())
             || (model == getIRGA_MODEL_STRING_1())
             || (model == getIRGA_MODEL_STRING_2())
             || (model == getIRGA_MODEL_STRING_3())
             || (model == getIRGA_MODEL_STRING_4())
             || (model == getIRGA_MODEL_STRING_5())
             || (model == getIRGA_MODEL_STRING_12())
             || (model == getIRGA_MODEL_STRING_13())
             || (model == getIRGA_MODEL_STRING_14()));
}

bool IrgaDesc::isWellNamed(const IrgaDesc& irga)
{
    const auto& model = irga.model();
    const QString& manufacturer = irga.manufacturer();

    auto isGoodManufacturer = !manufacturer.isEmpty();
    auto isLicorModel = isALicorModel(model);

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

    return (isGoodManufacturer && isGoodModel);
}

// return true if at least one separation is different from 0.0
bool IrgaDesc::hasGoodSeparations(const IrgaDesc& irga)
{
    return (!qFuzzyCompare(irga.tubeNSeparation() + 1, 0.0 + 1)
            || !qFuzzyCompare(irga.tubeESeparation() + 1, 0.0 + 1)
            || !qFuzzyCompare(irga.tubeVSeparation() + 1, 0.0 + 1));
}

bool IrgaDesc::isAGoodClosedPath(const IrgaDesc& irga)
{
    const auto& model = irga.model();

    auto isGoodTubeLength = irga.tubeLength() > 0.0;
    auto isGoodTubeDiameter = irga.tubeDiameter() > 0.0;

    auto isClosedPath = (model == getIRGA_MODEL_STRING_0())
                        || (model == getIRGA_MODEL_STRING_1())
                        || (model == getIRGA_MODEL_STRING_4())
                        || (model == getIRGA_MODEL_STRING_7())
                        || (model == getIRGA_MODEL_STRING_10())
                        || (model == getIRGA_MODEL_STRING_11())
                        || (model == getIRGA_MODEL_STRING_13());

    auto isGoodClosedPath = false;
    if (isClosedPath)
    {
        isGoodClosedPath = isGoodTubeLength && isGoodTubeDiameter;
    }
    else
    {
        isGoodClosedPath = true;
    }
    return isGoodClosedPath;
}

bool IrgaDesc::hasGoodFlowRate(const IrgaDesc& irga)
{
    const auto& model = irga.model();

    auto isGoodTubeFlowRate = irga.tubeFlowRate() > 0.0;

    auto isClosedPath = (model == getIRGA_MODEL_STRING_0())
                        || (model == getIRGA_MODEL_STRING_1())
                        || (model == getIRGA_MODEL_STRING_4())
                        || (model == getIRGA_MODEL_STRING_7())
                        || (model == getIRGA_MODEL_STRING_10())
                        || (model == getIRGA_MODEL_STRING_11())
                        || (model == getIRGA_MODEL_STRING_13());

    auto hasGoodFlowRate = false;
    if (isClosedPath)
    {
        hasGoodFlowRate = isGoodTubeFlowRate;
    }
    else
    {
        hasGoodFlowRate = true;
    }
    return hasGoodFlowRate;
}

bool IrgaDesc::hasGoodPathLength(const IrgaDesc& irga)
{
    auto isGoodPathLength = false;
    if (isALicorModel(irga.model()))
    {
        isGoodPathLength = true;
    }
    else
    {
        isGoodPathLength = (irga.hPathLength() > 0)
                            && (irga.vPathLength() > 0)
                            && (irga.tau() > 0);
    }
    return isGoodPathLength;
}

bool IrgaDesc::isGoodIrga(const IrgaDesc &irga)
{
    const QString& model = irga.model();

    // 1
    auto isGoodManufacturer = !irga.manufacturer().isEmpty();

    // 2
    auto isGoodModel = isWellNamed(irga);

    // 4
    auto isGoodClosedPath = isAGoodClosedPath(irga);

    // 5
    auto isGoodSeparation = hasGoodSeparations(irga);

    // 6
    auto isGoodPathLength = hasGoodPathLength(irga);

    // 7
    auto isGoodKorLAnalyzer = false;
    qreal kWater = irga.kWater();
    qreal kOxygen = irga.kOxygen();

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

    // all
    return (isGoodManufacturer
            && isGoodModel
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
             || model == IrgaDesc::getIRGA_MODEL_STRING_9()
             || model == IrgaDesc::getIRGA_MODEL_STRING_12()
             || model == IrgaDesc::getIRGA_MODEL_STRING_14());
}
