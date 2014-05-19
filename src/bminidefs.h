/***************************************************************************
  bminidefs.h
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

#ifndef BMINIDEFS_H
#define BMINIDEFS_H

class QString;

namespace BmIni
{
    // processing file ini keys
    const QString INI_PREFIX = QStringLiteral("biomet_");

    const QString INIGROUP_FILE = QStringLiteral("BiometFileDescription");
    const QString INI_FILE_0    = QStringLiteral("separator");
    const QString INI_FILE_1    = QStringLiteral("header_rows");
    const QString INI_FILE_2    = QStringLiteral("file_duration");
    const QString INI_FILE_3    = QStringLiteral("data_rate");
    const QString INI_FILE_4    = QStringLiteral("data_label");

    const QString INIGROUP_VARS_OLD = QStringLiteral("biomet_variables");
    const QString INIGROUP_VARS     = QStringLiteral("BiometVariables");
    const QString INI_VARS_0        = QStringLiteral("variable");
    const QString INI_VARS_1        = QStringLiteral("id");
    const QString INI_VARS_2        = QStringLiteral("instrument");
    const QString INI_VARS_3        = QStringLiteral("unit_in");
    const QString INI_VARS_4        = QStringLiteral("gain");
    const QString INI_VARS_5        = QStringLiteral("offset");
    const QString INI_VARS_6        = QStringLiteral("unit_out");
}

#endif // BMINIDEFS_H
