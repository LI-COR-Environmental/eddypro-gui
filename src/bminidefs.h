/***************************************************************************
  bminidefs.h
  -----------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#ifndef BMINIDEFS_H
#define BMINIDEFS_H

#include <QString>

namespace BmIni
{
    // processing file ini keys
    const auto INI_PREFIX = QStringLiteral("biomet_");

    const auto INIGROUP_FILE = QStringLiteral("BiometFileDescription");
    const auto INI_FILE_0    = QStringLiteral("separator");
    const auto INI_FILE_1    = QStringLiteral("header_rows");
    const auto INI_FILE_2    = QStringLiteral("file_duration");
    const auto INI_FILE_3    = QStringLiteral("data_rate");
    const auto INI_FILE_4    = QStringLiteral("data_label");

    const auto INIGROUP_VARS_OLD = QStringLiteral("biomet_variables");
    const auto INIGROUP_VARS     = QStringLiteral("BiometVariables");
    const auto INI_VARS_0        = QStringLiteral("variable");
    const auto INI_VARS_1        = QStringLiteral("id");
    const auto INI_VARS_2        = QStringLiteral("instrument");
    const auto INI_VARS_3        = QStringLiteral("unit_in");
    const auto INI_VARS_4        = QStringLiteral("gain");
    const auto INI_VARS_5        = QStringLiteral("offset");
    const auto INI_VARS_6        = QStringLiteral("unit_out");
} // namespace BmIni

#endif // BMINIDEFS_H
