/***************************************************************************
  dlprojectstate.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#ifndef DLPROJECTSTATE_H
#define DLPROJECTSTATE_H

#include "anem_desc.h"       // for AnemDescList typedef
#include "irga_desc.h"       // for IrgaDescList typedef
#include "variable_desc.h"   // for VariableDescList typedef
#include "defs.h"

struct GeneralState {
    GeneralState() :
        sw_version(Defs::APP_VERSION_STR),
        ini_version(Defs::METADATA_VERSION_STR),
        creation_date(QString()),
        last_change_date(QString()),
        start_date(QString()),
        end_date(QString()),
        file_name(QString()),
        title(QString()),
        id(QString())
    { ; }
    QString sw_version;
    QString ini_version;
    QString creation_date;
    QString last_change_date;
    QString start_date;
    QString end_date;
    QString file_name;
    QString title;
    QString id;
};

struct FilesState {
    FilesState() :
        data_path(QString()),
        saved_native(false),
        timestamp(true),
        isoformat(false),
        end_of_line(QStringLiteral("lf")),
        enable_processing(true),
        tstamp_end(0)
    { ; }
    QString data_path;
    bool saved_native;
    bool timestamp;
    bool isoformat;
    QString end_of_line;
    bool enable_processing;
    int tstamp_end;             // 0 = start
};

struct SiteState {
    SiteState() :
        site_name(QString()),
        site_id(QString()),
        altitude(0.0),
        latitude(0.0),
        longitude(0.0),
        canopy_height(0.0),
        displacement_height(0.0),
        roughness_length(0.0)
    { ; }
    QString site_name;
    QString site_id;
    double altitude;
    double latitude;
    double longitude;
    double canopy_height;
    double displacement_height;
    double roughness_length;
};

struct StationState {
    StationState() :
        name(QString()),
        id(QString())
    { ; }
    QString name;
    QString id;
};

struct TimingState {
    TimingState() :
        acquisition_frequency(10.0),
        file_duration(30),
        pc_time_settings(QStringLiteral("utc"))
    { ; }
    double acquisition_frequency;
    int file_duration;
    QString pc_time_settings;
};

struct VarDescState {
    VarDescState() :
        separator(QString()),
        header_rows(-1),
        data_label(QT_TR_NOOP(QStringLiteral("Not set")))
    { ; }
    QString separator;
    int header_rows;
    QString data_label;
};

struct ProjectState
{
    ProjectState() :
        general(),
        files(),
        site(),
        station(),
        timing(),
        anemometerList(),
        irgaList(),
        varDesc(),
        variableList()
    { ; }
    GeneralState general;
    FilesState files;
    SiteState site;
    StationState station;
    TimingState timing;
    AnemDescList anemometerList;
    IrgaDescList irgaList;
    VarDescState varDesc;
    VariableDescList variableList;
};

#endif // DLPROJECTSTATE_H
