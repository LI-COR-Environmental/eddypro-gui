/***************************************************************************
  stringutils.cpp
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

#include <QByteArray>
#include <QDate>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "dbghelper.h"
#include "stringutils.h"

const QString StringUtils::fromBool2YesNoString(bool b)
{
    return ((b) ? tr("yes") : tr("no"));
}

const QString StringUtils::insertIndex(const QString& srcStr, int pos, const QString& str)
{
    QString strData = srcStr;
    return strData.insert(pos, str);
}

const char* StringUtils::qString2Char(const QString& str)
{
    QByteArray ba = str.toLatin1();
    return ba.constData();
}

int StringUtils::daysToFromString(const QString& d_1, const QString& d_2)
{
    QDate dStart(QDate::fromString(d_1, Qt::ISODate));
    QDate dEnd(QDate::fromString(d_2, Qt::ISODate));
    return dStart.daysTo(dEnd);
}

bool StringUtils::isISODateTimeString(const QString& s)
{
    QRegExp dateExp;
    dateExp.setPattern(QStringLiteral("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})"));
    QRegExp timeExp;
    timeExp.setPattern(QStringLiteral("([0-9]{,2})([0-9]{,2})([0-9]{,2})"));
    QRegExp dateTimeExp;
    dateTimeExp.setPattern(dateExp.pattern() + QLatin1Char('T') + timeExp.pattern());
    QRegExpValidator validator(dateTimeExp);
    int pos = 0;

    return (s.length() == 17
            && validator.validate(const_cast<QString&>(s), pos) == QValidator::Acceptable);
}

bool StringUtils::stringBelongsToList(const QString& string, const QStringList& list)
{
    bool ok = false;
    foreach (const QString& s, list)
    {
        if (string == s)
        {
            ok = true;
            break;
        }
    }
    return ok;
}

bool StringUtils::isNewVersion(const QString& remoteVersion, const QString& localVersion)
{
    DEBUG_FUNC_NAME

    QStringList remoteNumbers = remoteVersion.split(QLatin1Char('.'));
    QStringList localNumbers = localVersion.split(QLatin1Char('.'));

    qDebug() << remoteNumbers;
    qDebug() << localNumbers;

    if (remoteNumbers.at(0) > localNumbers.at(0))
        return true;
    else if (remoteNumbers.at(0) < localNumbers.at(0))
        return false;

    if (remoteNumbers.at(1) > localNumbers.at(1))
        return true;
    else if (remoteNumbers.at(1) < localNumbers.at(1))
        return false;

    if (remoteNumbers.at(2) > localNumbers.at(2))
        return true;
    else if (remoteNumbers.at(2) < localNumbers.at(2))
        return false;

    return false;
}
