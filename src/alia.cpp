/***************************************************************************
  alia.cpp
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
****************************************************************************/

#include "alia.h"

#include <QApplication>
#include <QComboBox>
#include <QDebug>
#include <QDesktopServices>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QNetworkConfigurationManager>
#include <QNetworkProxyFactory>
#include <QProcess>
#include <QPushButton>
#include <QSettings>
#include <QStyle>
#include <QTcpSocket>
#include <QToolButton>
#include <QUrl>

#include "defs.h"
#include "dbghelper.h"
#include "docchooser.h"
#include "infomessage.h"
#include "variable_desc.h"

bool Alia::isOnline_2()
{
    QNetworkConfigurationManager ifConfigManager;
    return ifConfigManager.isOnline();
}
