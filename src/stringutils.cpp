/***************************************************************************
  stringutils.cpp
  -------------------
  Copyright (C) 2013-2018, LI-COR Biosciences
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

#include "stringutils.h"

#include <QByteArray>
#include <QDate>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QString>
#include <QStringList>

#include "dbghelper.h"

const QString StringUtils::fromBool2YesNoString(bool b)
{
    return ((b) ? QObject::tr("yes") : QObject::tr("no"));
}

const QString StringUtils::insertIndex(const QString& srcStr, int pos, const QString& str)
{
    QString strData = srcStr;
    return strData.insert(pos, str);
}

// NOTE: never used
const char* StringUtils::qString2Char(const QString& str)
{
    QByteArray ba = str.toLatin1();
    return ba.constData();
}

// NOTE: never used
int StringUtils::daysToFromString(const QString& d_1, const QString& d_2)
{
    QDate dStart(QDate::fromString(d_1, Qt::ISODate));
    QDate dEnd(QDate::fromString(d_2, Qt::ISODate));
    return static_cast<int>(dStart.daysTo(dEnd));
}

bool StringUtils::isISODateTimeString(const QString& s)
{
    QRegularExpression dateExp;
    dateExp.setPattern(QStringLiteral("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})"));

    QRegularExpression timeExp;
    timeExp.setPattern(QStringLiteral("([0-9]{,2})([0-9]{,2})([0-9]{,2})"));

    QRegularExpression dateTimeExp;
    dateTimeExp.setPattern(dateExp.pattern() + QLatin1Char('T') + timeExp.pattern());

    QRegularExpressionValidator validator(dateTimeExp);

    const auto dateTimeLenght = 17;
    auto str = QString(s);
    auto pos = 0;

    return (s.length() == dateTimeLenght
            && validator.validate(str, pos) == QValidator::Acceptable
            && pos != s.length());
}

bool StringUtils::stringBelongsToList(const QString& str, const QStringList& list)
{
    bool ok = false;
    for (const auto &s : list)
    {
        if (str == s)
        {
            ok = true;
            break;
        }
    }
    return ok;
}

// assume the input is in classic "MM.NN.PP" form (MM = major, NN = minor, PP = patch)
// and return 0xMMNNPP
// if patch number is missing, it will be zeroed
int StringUtils::getVersionFromString(const QString& versionStr)
{
    auto major = versionStr.section(QLatin1Char('.'), 0, 0).toInt();
    auto minor = versionStr.section(QLatin1Char('.'), 1, 1).toInt();
    auto patchStr = versionStr.section(QLatin1Char('.'), 2, 2);

    // in case of 2 digits version only
    if (patchStr.isEmpty())
    {
        patchStr = QStringLiteral("00");
    }
    auto patch = patchStr.toInt();

    return QT_VERSION_CHECK(major, minor, patch);
}

bool StringUtils::isNewVersion(const QString& remoteVersion, const QString& localVersion)
{
    return getVersionFromString(remoteVersion) > getVersionFromString((localVersion));
}

const QStringList StringUtils::subStringList(const QStringList& list, int begin, int end)
{
    QStringList sublist;

    for (int i = begin; i < end; ++i)
    {
        sublist << list.value(i);
    }
    return sublist;
}

QString StringUtils::fromUnixTimeToISOString(double msec)
{
    QDateTime timestamp(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(msec)));
    return timestamp.toString(Qt::ISODate);
}
