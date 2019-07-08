/***************************************************************************
  defs.cpp
  -------------------
  Copyright (C) 2014-2018, LI-COR Biosciences
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
