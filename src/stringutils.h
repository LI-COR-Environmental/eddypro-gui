/***************************************************************************
  stringutils.h
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

#include <QCoreApplication>  // for Q_DECLARE_TR_FUNCTIONS

class QString;
class QStringList;

class StringUtils
{
    Q_DECLARE_TR_FUNCTIONS(StringUtils)

public:
    static const QString fromBool2YesNoString(bool b);

    static const QString insertIndex(const QString& srcStr, int pos, const QString& str);

    static const char* qString2Char(const QString& str);

    static int daysToFromString(const QString& d_1, const QString& d_2);

    static bool isISODateTimeString(const QString &s);

    static bool stringBelongsToList(const QString &string, const QStringList &list);

    static bool isNewVersion(const QString &remoteVersion, const QString &localVersion);
};

#endif // STRINGUTILS_H
