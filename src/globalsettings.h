/***************************************************************************
  globalsettings.h
  -------------------
  Copyright (C) 2014-2015, LI-COR Biosciences
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

#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QString>
#include <QStringList>
#include <QVariant>

namespace GlobalSettings
{
QVariant getAppPersistentSettings(const QString& group,
                                  const QString& key,
                                  const QVariant& defaultValue = QVariant());

    void setAppPersistentSettings(const QString& group,
                                  const QString& key,
                                  const QVariant& value);

QVariant getFirstAppPersistentSettings(const QString& group,
                                       const QString& key,
                                       const QVariant& defaultValue);

    void updateLastDatapath(const QString& dir);

    void getCustomVariableList(QStringList* varList);
    void setCustomVariableList(const QStringList& varList);

}  // namespace GlobalSettings

#endif  // GLOBALSETTINGS_H
