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

QString CalibrationInfo::responseCodeAsStr() const
{
    return QString::number(response_code_, 'f', 0);
}

double CalibrationInfo::calDate() const
{
    return cal_date_;
}

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

QString CalibrationInfo::calRecalAsStr() const
{
    return StringUtils::fromBool2YesNoString(cal_recal_);
}
