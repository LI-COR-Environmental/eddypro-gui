/***************************************************************************
  irga_desc.h
  -----------
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

#ifndef IRGA_DESC_H
#define IRGA_DESC_H

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS
#include <QList>

class QString;
class QStringList;

class IrgaDesc
{
    Q_DECLARE_TR_FUNCTIONS(IrgaMeasure)

public:
    // irga desc strings
    static const QString getIRGA_MANUFACTURER_STRING_0();
    static const QString getIRGA_MANUFACTURER_STRING_1();

    static const QString getIRGA_MODEL_STRING_0();
    static const QString getIRGA_MODEL_STRING_1();
    static const QString getIRGA_MODEL_STRING_2();
    static const QString getIRGA_MODEL_STRING_3();
    static const QString getIRGA_MODEL_STRING_4();
    static const QString getIRGA_MODEL_STRING_5();
    static const QString getIRGA_MODEL_STRING_6();
    static const QString getIRGA_MODEL_STRING_7();
    static const QString getIRGA_MODEL_STRING_8();
    static const QString getIRGA_MODEL_STRING_9();
    static const QString getIRGA_MODEL_STRING_10();
    static const QString getIRGA_MODEL_STRING_11();
    static const QString getIRGA_MODEL_STRING_12();
    static const QString getIRGA_MODEL_STRING_13();
    static const QString getIRGA_MODEL_STRING_14();

    IrgaDesc();

    IrgaDesc(QString  manufacture,
             QString  model,
             QString  swVersion,
             QString  id,
//             qreal height,
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
             qreal kOxygen);

    IrgaDesc(const IrgaDesc& irga);

    IrgaDesc& operator=(const IrgaDesc& irga);
    bool operator==(const IrgaDesc& irga) const;
    bool operator<(const IrgaDesc& irga) const;

    // get/set manufacturer
    const QString& manufacturer() const;
    void setManufacturer(const QString& m);

    // get/set model
    const QString& model() const;
    void setModel(const QString& m);

    // get/set version
    const QString& swVersion() const;
    void setSwVersion(const QString& v);

    // get/set id
    const QString& id() const;
    void setId(const QString& i);

    // get/set tube length
    qreal tubeLength() const;
    void setTubeLength(qreal l);

    // get/set tube diameter
    qreal tubeDiameter() const;
    void setTubeDiameter(qreal d);

    // get/set tube flow rate
    qreal tubeFlowRate() const;
    void setTubeFlowRate(qreal r);

    // get/set northward sep
    qreal tubeNSeparation() const;
    void setTubeNSeparation(qreal s);

    // get/set eastward sep
    qreal tubeESeparation() const;
    void setTubeESeparation(qreal s);

    // get/set vertical sep
    qreal tubeVSeparation() const;
    void setTubeVSeparation(qreal s);

    qreal vPathLength() const;
    void setVPathLength(qreal s);

    qreal hPathLength() const;
    void setHPathLength(qreal s);

    qreal tau() const;
    void setTau(qreal s);

    qreal kWater() const;
    void setKWater(qreal s);

    qreal kOxygen() const;
    void setKOxygen(qreal s);

    // return a list of manufacturer strings
    static const QStringList manufacturerStringList();

    // return a list of model strings
    static const QStringList allModelStringList();
    static const QStringList licorModelStringList();
    static const QStringList otherModelStringList();

    static bool isWellNamed(const IrgaDesc& irga);
    static bool hasGoodSeparations(const IrgaDesc& irga);
    static bool isAGoodClosedPath(const IrgaDesc& irga);
    static bool hasGoodFlowRate(const IrgaDesc& irga);
    static bool hasGoodPathLength(const IrgaDesc& irga);
    static bool isGoodIrga(const IrgaDesc& irga);

    static bool isOpenPathModel(const QString &model);

private:
    static bool isALicorModel(const QString& model);

    QString manufacturer_;
    QString model_;
    QString swVersion_;
    QString id_;
    qreal tubeLength_;
    qreal tubeDiameter_;
    qreal tubeFlowRate_;
    qreal nSeparation_;
    qreal eSeparation_;
    qreal vSeparation_;
    qreal vPathLength_;
    qreal hPathLength_;
    qreal tau_;
    qreal kWater_;
    qreal kOxygen_;
};

using IrgaDescList = QList<IrgaDesc>;

// Inlined Methods
inline bool IrgaDesc::operator<(const IrgaDesc& irga) const
    { return (manufacturer_ < irga.manufacturer_); }

inline const QString& IrgaDesc::manufacturer() const
    { return manufacturer_; }

inline void IrgaDesc::setManufacturer(const QString& m)
    { manufacturer_ = m; }

inline const QString& IrgaDesc::model() const
    { return model_; }

inline void IrgaDesc::setModel(const QString& m)
    { model_ = m; }

inline const QString& IrgaDesc::swVersion() const
    { return swVersion_; }

inline void IrgaDesc::setSwVersion(const QString& v)
    { swVersion_ = v; }

inline const QString& IrgaDesc::id() const
    { return id_; }

inline void IrgaDesc::setId(const QString& i)
    { id_ = i; }

inline qreal IrgaDesc::tubeLength() const
    { return tubeLength_; }

inline void IrgaDesc::setTubeLength(qreal l)
    { tubeLength_ = l; }

inline qreal IrgaDesc::tubeDiameter() const
    { return tubeDiameter_; }

inline void IrgaDesc::setTubeDiameter(qreal d)
    { tubeDiameter_ = d; }

inline qreal IrgaDesc::tubeFlowRate() const
    { return tubeFlowRate_; }

inline void IrgaDesc::setTubeFlowRate(qreal r)
    { tubeFlowRate_ = r; }

inline qreal IrgaDesc::tubeNSeparation() const
    { return nSeparation_; }

inline void IrgaDesc::setTubeNSeparation(qreal s)
    { nSeparation_ = s; }

inline qreal IrgaDesc::tubeESeparation() const
    { return eSeparation_; }

inline void IrgaDesc::setTubeESeparation(qreal s)
    { eSeparation_ = s; }

inline qreal IrgaDesc::tubeVSeparation() const
    { return vSeparation_; }

inline void IrgaDesc::setTubeVSeparation(qreal s)
    { vSeparation_ = s; }

inline qreal IrgaDesc::vPathLength() const
    { return vPathLength_; }

inline void IrgaDesc::setVPathLength(qreal s)
    { vPathLength_ = s; }

inline qreal IrgaDesc::hPathLength() const
    { return hPathLength_; }

inline void IrgaDesc::setHPathLength(qreal s)
    { hPathLength_ = s; }

inline qreal IrgaDesc::tau() const
    { return tau_; }

inline void IrgaDesc::setTau(qreal s)
    { tau_ = s; }

inline qreal IrgaDesc::kWater() const
    { return kWater_; }

inline void IrgaDesc::setKWater(qreal s)
    { kWater_ = s; }

inline qreal IrgaDesc::kOxygen() const
    { return kOxygen_; }

inline void IrgaDesc::setKOxygen(qreal s)
    { kOxygen_ = s; }

#endif // IRGA_DESC_H
