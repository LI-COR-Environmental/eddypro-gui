/***************************************************************************
  globalsettings.cpp
  ------------------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
