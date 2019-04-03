/***************************************************************************
  configstate.h
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
 ***************************************************************************/

#ifndef CONFIGSTATE_H
#define CONFIGSTATE_H

#include <QStringList>

////////////////////////////////////////////////////////////////////////////////
/// \file src/configstate.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

/// \struct GenConfigState
/// \brief Container structure for application preferences to be
/// preserved in the operating system
struct GenConfigState
{
    bool loadlastproject = false;
    bool showsplash = true;
    int recentnum = 4;
    QString env = QString();
    QStringList recentfiles = QStringList();
};

/// \struct ProjConfigState
/// \brief Container structure for project parameters to be
/// preserved in the operating system
struct ProjConfigState
{
    bool smartfluxMode = false;
    QString default_data_path = QString();
    QString custom_variables = QString();
};

/// \struct WinConfigState
/// \brief Container structure for application status information to be
/// preserved in the operating system
struct WinConfigState
{
    bool statusBar = true;
    bool fullScreen = false;
    bool logDock = false;
    bool consoleDock = true;
    bool tooltips = true;
    bool infoDock = true;
    bool autoChooseHelp = false;
    bool offlineHelp = false;
    bool showRunMessages = true;
    QByteArray mainwin_state = QByteArray();
    QByteArray mainwin_geometry = QByteArray();
    QString last_data_path = QString();
};

/// \struct ConfigState
/// \brief Container structure for application status, options and
/// parameters to be preserved in the operating system
struct ConfigState
{
    GenConfigState general;
    ProjConfigState project;
    WinConfigState window;
};

#endif // CONFIGSTATE_H
