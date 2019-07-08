/***************************************************************************
  stringutils.cpp
  ---------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#include "stringutils.h"

#include <QByteArray>
#include <QDate>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QString>
#include <QStringList>

const QString StringUtils::fromBool2YesNoString(bool b)
{
    return ((b) ? QObject::tr("yes") : QObject::tr("no"));
}

// simple wrapper around QString::insert()
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
int StringUtils::getHexVersionFromString(const QString& versionStr)
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
    return getHexVersionFromString(remoteVersion) > getHexVersionFromString((localVersion));
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
