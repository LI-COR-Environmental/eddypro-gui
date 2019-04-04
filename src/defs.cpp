/***************************************************************************
  defs.cpp
  --------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#include "defs.h"

#include <QMetaType>

// debug helper to show Defs::RawFileType enum values
QDebug Defs::operator<<(QDebug dbg, const RawFileType& fileType)
{
    switch (fileType) {
    case RawFileType::GHG:
        dbg << "Defs::RawFileType::GHG";
        break;
    case RawFileType::ASCII:
        dbg << "Defs::RawFileType::ASCII";
        break;
    case RawFileType::BIN:
        dbg << "Defs::RawFileType::BIN";
        break;
    case RawFileType::TOB1:
        dbg << "Defs::RawFileType::TOB1";
        break;
    case RawFileType::SLT1:
        dbg << "Defs::RawFileType::SLT1";
        break;
    case RawFileType::SLT2:
        dbg << "Defs::RawFileType::SLT2";
        break;
//    default:
//        dbg << "Defs::RawFileType(" << static_cast<int>(fileType) << ')';
//        break;
    }
    return dbg;
}

// NOTE: necessary if streaming directly Defs::RawFileType in QSettings
// QDataStream& Defs::operator<<(QDataStream& out, const RawFileType& fileType)
// {
//     out << static_cast<int>(fileType);
//     return out;
// }

// NOTE: necessary if streaming directly Defs::RawFileType in QSettings
// QDataStream& Defs::operator>>(QDataStream& in, RawFileType& fileType)
// {
//     int ft;
//     in >> ft;
//     fileType = static_cast<Defs::RawFileType>(ft);
//     return in;
// }

void Defs::qt_registerCustomTypes()
{
    qRegisterMetaType<Defs::RawFileType>("Defs::RawFileType");

    // NOTE: necessary if streaming directly Defs::RawFileType in QSettings
//    qRegisterMetaTypeStreamOperators<Defs::RawFileType>("Defs::RawFileType");
}
