/***************************************************************************
  dlprojectstate.h
  ----------------
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
 ***************************************************************************/

#ifndef DLPROJECTSTATE_H
#define DLPROJECTSTATE_H

#include "anem_desc.h"       // NOTE: for AnemDescList
#include "defs.h"
#include "irga_desc.h"       // NOTE: for IrgaDescList
#include "variable_desc.h"   // NOTE: for VariableDescList

struct GeneralState {
    QString sw_version = Defs::APP_VERSION_STR;
    QString ini_version = Defs::METADATA_FILE_VERSION_STR;
    QString creation_date = QString();
    QString last_change_date = QString();
    QString start_date = QString();
    QString end_date = QString();
    QString file_name = QString();
    QString title = QString();
    QString id = QString();
};

struct FilesState {
    bool saved_native = false;
    bool timestamp = true;
    bool isoformat = false;
    bool enable_processing = true;
    int tstamp_end = 0;             // 0 = start
    QString data_path = QString();
    QString end_of_line = QStringLiteral("lf");
};

struct SiteState {
    double altitude = 0.0;
    double latitude = 0.0;
    double longitude = 0.0;
    double canopy_height = 0.0;
    double displacement_height = 0.0;
    double roughness_length = 0.0;
    QString site_name = QString();
    QString site_id = QString();
};

struct StationState {
    QString name = QString();
    QString id = QString();
};

struct TimingState {
    int file_duration = 30;
    double acquisition_frequency = 10.0;
    QString pc_time_settings = QStringLiteral("utc");
};

struct VarDescState {
    int header_rows = -1;
    QString separator = QString();
    QString data_label = QObject::tr("Not set");
};

struct ProjectState
{
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
