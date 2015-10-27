/***************************************************************************
  stringutils.h
  -------------------
  Copyright (C) 2013-2015, LI-COR Biosciences
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

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

class QString;
class QStringList;

namespace StringUtils
{
    const QString fromBool2YesNoString(bool b);

    const QString insertIndex(const QString& srcStr, int pos, const QString& str);

    const char* qString2Char(const QString& str);

    int daysToFromString(const QString& d_1, const QString& d_2);

    QString fromUnixTimeToISOString(double msec);

    bool isISODateTimeString(const QString &s);

    bool stringBelongsToList(const QString &str, const QStringList &list);

    int getVersionFromString(const QString& versionStr);

    bool isNewVersion(const QString &remoteVersion, const QString &localVersion);

    const QStringList subStringList(const QStringList &list, int begin, int end);

} // StringUtils

#endif // STRINGUTILS_H
