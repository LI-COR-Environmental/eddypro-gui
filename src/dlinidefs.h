/***************************************************************************
  dlinidefs.h
  -------------------
  Metadata file INI strings
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2017, LI-COR Biosciences
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
***************************************************************************/

#ifndef DLINIDEFS_H
#define DLINIDEFS_H

#include <QString>

namespace DlIni
{
    // file ini strings
    const auto INIGROUP_PROJECT             = QStringLiteral("Project");
    const auto INI_GENE_TITLE               = QStringLiteral("title");
    const auto INI_GENE_ID                  = QStringLiteral("id");
    const auto INI_GENE_CREATION_DATE       = QStringLiteral("creation_date");
    const auto INI_GENE_LAST_CHANGE_DATE    = QStringLiteral("last_change_date");
    const auto INI_GENE_START_DATE          = QStringLiteral("start_date");
    const auto INI_GENE_END_DATE            = QStringLiteral("end_date");
    const auto INI_GENE_FILE_NAME           = QStringLiteral("file_name");
    const auto INI_GENE_SW_VERSION          = QStringLiteral("sw_version");
    const auto INI_GENE_INI_VERSION         = QStringLiteral("ini_version");

    const auto INIGROUP_FILES               = QStringLiteral("Files");
    const auto INI_FILE_DATA_PATH           = QStringLiteral("data_path");
    const auto INI_FILE_SAVED_NATIVE        = QStringLiteral("saved_native");
    const auto INI_FILE_HEADER_ROWS         = QStringLiteral("header_rows");
    const auto INI_FILE_TIMESTAMP           = QStringLiteral("timestamp");
    const auto INI_FILE_ISOFORMAT           = QStringLiteral("iso_format");
    const auto INI_FILE_END_OF_LINE         = QStringLiteral("end_of_line");
    const auto INI_FILE_ENABLE_PROC         = QStringLiteral("enable_processing");
    const auto INI_FILE_FORMAT              = QStringLiteral("native_format");
    const auto INI_FILE_TIMESTAMP_END       = QStringLiteral("tstamp_end");

    const auto INIGROUP_SITE                = QStringLiteral("Site");
    const auto INI_SITE_SITE_NAME           = QStringLiteral("site_name");
    const auto INI_SITE_SITE_ID             = QStringLiteral("site_id");
    const auto INI_SITE_ALTITUDE            = QStringLiteral("altitude");
    const auto INI_SITE_LATITUDE            = QStringLiteral("latitude");
    const auto INI_SITE_LONGITUDE           = QStringLiteral("longitude");
    const auto INI_SITE_CANOPY_HEIGHT       = QStringLiteral("canopy_height");
    const auto INI_SITE_DISPLACEMENT_HEIGHT = QStringLiteral("displacement_height");
    const auto INI_SITE_ROUGHNESS_LENGTH    = QStringLiteral("roughness_length");

    const auto INIGROUP_STATION             = QStringLiteral("Station");
    const auto INI_STATION_NAME             = QStringLiteral("station_name");
    const auto INI_STATION_ID               = QStringLiteral("station_id");

    const auto INIGROUP_TIMING              = QStringLiteral("Timing");
    const auto INI_TIME_ACQUISITION_FREQ    = QStringLiteral("acquisition_frequency");
    const auto INI_TIME_FILE_DURATION       = QStringLiteral("file_duration");
    const auto INI_TIME_PC_TIME_SET         = QStringLiteral("pc_time_settings");

    const auto INIGROUP_INSTRUMENTS = QStringLiteral("Instruments");
    const auto INI_INSTR_PREFIX     = QStringLiteral("instr_");
    const auto INI_ANEM_1           = QStringLiteral("manufacturer");
    const auto INI_ANEM_2           = QStringLiteral("model");
    const auto INI_ANEM_4           = QStringLiteral("id");
    const auto INI_ANEM_5           = QStringLiteral("height");
    const auto INI_ANEM_6           = QStringLiteral("wformat");
    const auto INI_ANEM_7           = QStringLiteral("wref");
    const auto INI_ANEM_8           = QStringLiteral("north_offset");
    const auto INI_ANEM_10          = QStringLiteral("northward_separation");
    const auto INI_ANEM_11          = QStringLiteral("eastward_separation");
    const auto INI_ANEM_12          = QStringLiteral("vertical_separation");
    const auto INI_ANEM_13          = QStringLiteral("hpath_length");
    const auto INI_ANEM_14          = QStringLiteral("vpath_length");
    const auto INI_ANEM_15          = QStringLiteral("tau");
    const auto INI_ANEM_16          = QStringLiteral("sw_version");
    const auto INI_IRGA_0           = QStringLiteral("manufacturer");
    const auto INI_IRGA_1           = QStringLiteral("model");
    const auto INI_IRGA_3           = QStringLiteral("id");
    const auto INI_IRGA_5           = QStringLiteral("tube_length");
    const auto INI_IRGA_6           = QStringLiteral("tube_diameter");
    const auto INI_IRGA_7           = QStringLiteral("tube_flowrate");
    const auto INI_IRGA_8           = QStringLiteral("northward_separation");
    const auto INI_IRGA_9           = QStringLiteral("eastward_separation");
    const auto INI_IRGA_10          = QStringLiteral("vertical_separation");
    const auto INI_IRGA_11          = QStringLiteral("hpath_length");
    const auto INI_IRGA_12          = QStringLiteral("vpath_length");
    const auto INI_IRGA_13          = QStringLiteral("tau");
    const auto INI_IRGA_14          = QStringLiteral("kw");
    const auto INI_IRGA_15          = QStringLiteral("ko");
    const auto INI_IRGA_16          = QStringLiteral("sw_version");

    const auto INIGROUP_VARDESC         = QStringLiteral("FileDescription");
    const auto INI_VARDESC_PREFIX       = QStringLiteral("col_");
    const auto INI_VARDESC_FIELDSEP     = QStringLiteral("separator");
    const auto INI_VARDESC_HEADER_ROWS  = QStringLiteral("header_rows");
    const auto INI_VARDESC_DATA_LABEL   = QStringLiteral("data_label");
    const auto INI_VARDESC_VAR          = QStringLiteral("variable");
    const auto INI_VARDESC_INSTRUMENT   = QStringLiteral("instrument");
    const auto INI_VARDESC_MEASURE_TYPE = QStringLiteral("measure_type");
    const auto INI_VARDESC_UNIT_IN      = QStringLiteral("unit_in");
    const auto INI_VARDESC_MIN_VALUE    = QStringLiteral("min_value");
    const auto INI_VARDESC_MAX_VALUE    = QStringLiteral("max_value");
    const auto INI_VARDESC_CONVERSION   = QStringLiteral("conversion");
    const auto INI_VARDESC_UNIT_OUT     = QStringLiteral("unit_out");
    const auto INI_VARDESC_A_VALUE      = QStringLiteral("a_value");
    const auto INI_VARDESC_B_VALUE      = QStringLiteral("b_value");
    const auto INI_VARDESC_NOM_TIMELAG  = QStringLiteral("nom_timelag");
    const auto INI_VARDESC_MIN_TIMELAG  = QStringLiteral("min_timelag");
    const auto INI_VARDESC_MAX_TIMELAG  = QStringLiteral("max_timelag");
} // namespace DlIni

#endif // DLINIDEFS_H
