/***************************************************************************
  globalsettings.cpp
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

#include "globalsettings.h"

#include <QSettings>

#include "defs.h"

QVariant GlobalSettings::getAppPersistentSettings(const QString& group,
                                  const QString& key,
                                  const QVariant& defaultValue)
{
    if (group.isEmpty() || key.isEmpty()) return QVariant();

    QSettings settings;

    settings.beginGroup(group);
    QVariant value = settings.value(key, defaultValue);
    settings.endGroup();

    return value;
}

void GlobalSettings::setAppPersistentSettings(const QString& group,
                              const QString& key,
                              const QVariant& value)
{
    if (group.isEmpty() || key.isEmpty() || !value.isValid()) { return; }

    QSettings settings;

    settings.beginGroup(group);
        settings.setValue(key, value);
    settings.endGroup();

    settings.sync();
}

QVariant GlobalSettings::getFirstAppPersistentSettings(const QString &group, const QString &key, const QVariant &defaultValue)
{
    auto value = GlobalSettings::getAppPersistentSettings(group, key);

    // if the key is not present, create it
    if (value == QVariant())
    {
        value = defaultValue;
        setAppPersistentSettings(group, key, value);
    }

    return value;
}

void GlobalSettings::updateLastDatapath(const QString& dir)
{
    setAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                             Defs::CONF_WIN_LAST_DATAPATH,
                             dir);
}

void GlobalSettings::getCustomVariableList(QStringList* varList)
{
    QString stringValue =
        getAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                 Defs::CONF_PROJ_CUSTOM_VARS,
                                 QString()).toString();

    *varList = stringValue.split(QLatin1Char(','), QString::SkipEmptyParts);
}

void GlobalSettings::setCustomVariableList(const QStringList& varList)
{
    setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                             Defs::CONF_PROJ_CUSTOM_VARS,
                             varList.join(QLatin1Char(',')));
}

//  NOTE: add error management using QSettings::status()
