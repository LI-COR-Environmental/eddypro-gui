/***************************************************************************
  configstate.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

/// \struct WinConfigState
/// \brief Container structure for application status information to be
/// preserved in the operating system
struct WinConfigState
{
   WinConfigState()
       : mainwin_state(QByteArray()),
         mainwin_geometry(QByteArray()),
         statusBar(true),
         fullScreen(false),
         logDock(false),
         consoleDock(true),
         tooltips(true),
         infoDock(true),
         autoChooseHelp(false),
         offlineHelp(false),
         last_data_path(QString()),
         showRunMessages(true)
    { ; }
    QByteArray mainwin_state;
    QByteArray mainwin_geometry;
    bool statusBar;
    bool fullScreen;
    bool logDock;
    bool consoleDock;
    bool tooltips;
    bool infoDock;
    bool autoChooseHelp;
    bool offlineHelp;
    QString last_data_path;
    bool showRunMessages;
};

/// \struct GenConfigState
/// \brief Container structure for application preferences to be
/// preserved in the operating system
struct GenConfigState
{
    GenConfigState() :
        showsplash(true),
        recentnum(4),
        recentfiles(QStringList()),
        loadlastproject(false),
        env(QString())
    { ; }
    bool showsplash;
    int recentnum;
    QStringList recentfiles;
    bool loadlastproject;
    QString env;
};

/// \struct ProjConfigState
/// \brief Container structure for project parameters to be
/// preserved in the operating system
struct ProjConfigState
{
    ProjConfigState() :
        default_data_path(QString()),
        custom_variables(QString()),
        smartfluxMode(false)
    { ; }
    QString default_data_path;
    QString custom_variables;
    bool smartfluxMode;
};

/// \struct ConfigState
/// \brief Container structure for application status, options and
/// parameters to be preserved in the operating system
struct ConfigState
{
    ConfigState() :
        window(WinConfigState()),
        general(GenConfigState()),
        project(ProjConfigState())
    { ; }
    WinConfigState window;
    GenConfigState general;
    ProjConfigState project;
};

#endif // CONFIGSTATE_H
