/***************************************************************************
  calibrationinfo.h
  -------------------
  -------------------
  Copyright (C) 2016, LI-COR Biosciences
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

#include "calibrationinfo.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "dbghelper.h"
#include "stringutils.h"

CalibrationInfo::CalibrationInfo() :
    response_code_(404),
    cal_date_(0),
    cal_link_(QString()),
    cal_recal_(false)

{ ; }

CalibrationInfo::CalibrationInfo(double responseCode,
                                 double calDate,
                                 const QString& calLink,
                                 bool calRecal) :
    response_code_(responseCode),
    cal_date_(calDate),
    cal_link_(calLink),
    cal_recal_(calRecal)

{ ; }

CalibrationInfo::CalibrationInfo(const QByteArray& calibrationAsJson) :
    response_code_(404),
    cal_date_(0),
    cal_link_(QString()),
    cal_recal_(false)

{
    DEBUG_FUNC_NAME

    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(calibrationAsJson, &jerror);

    if (jerror.error == QJsonParseError::NoError)
    {
        QJsonObject obj = jdoc.object();

        auto map = obj.toVariantMap();
        for (auto key : map)
        {
            qDebug() << key.toString();
        }

        response_code_ = map.value(QStringLiteral("response_code")).toDouble();
        cal_date_ = map.value(QStringLiteral("cal_date")).toDouble();
        cal_link_ = map.value(QStringLiteral("text")).toString();
        cal_recal_ = map.value(QStringLiteral("should_recal")).toBool();
    }
    else
    {
        qWarning() << "Calibration API: json parsing failure.";
    }
}

CalibrationInfo &CalibrationInfo::operator=(const CalibrationInfo &calibrationInfo)
{
    if (this != &calibrationInfo)
    {
        response_code_ = calibrationInfo.response_code_;
        cal_date_ = calibrationInfo.cal_date_;
        cal_link_ = calibrationInfo.cal_link_;
        cal_recal_ = calibrationInfo.cal_recal_;
    }
    return *this;
}

double CalibrationInfo::responseCode() const
{
    return response_code_;
}

// NOTE: never used
QString CalibrationInfo::responseCodeAsStr() const
{
    return QString::number(response_code_, 'f', 0);
}

double CalibrationInfo::calDate() const
{
    return cal_date_;
}

// NOTE: never used
QString CalibrationInfo::calDateAsStr() const
{
    return StringUtils::fromUnixTimeToISOString(cal_date_);
}

QString CalibrationInfo::calLink() const
{
    return cal_link_;
}

bool CalibrationInfo::calRecal() const
{
    return cal_recal_;
}

// NOTE: never used
QString CalibrationInfo::calRecalAsStr() const
{
    return StringUtils::fromBool2YesNoString(cal_recal_);
}
