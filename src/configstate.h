/***************************************************************************
  configstate.h
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
