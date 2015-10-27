#ifndef CALIBRATIONINFO_H
#define CALIBRATIONINFO_H

#include <QByteArray>
#include <QString>

class CalibrationInfo
{
public:
    CalibrationInfo();
    CalibrationInfo(double responseCode, double calDate, const QString &calLink, bool calRecal);
    CalibrationInfo(const QByteArray &calibrationAsJson);
    CalibrationInfo& operator=(const CalibrationInfo& calibrationInfo);

    double responseCode() const;
    QString responseCodeAsStr() const;

    double calDate() const;
    QString calDateAsStr() const;

    QString calLink() const;

    bool calRecal() const;
    QString calRecalAsStr() const;

private:
    double response_code_;
    double cal_date_; // unix time in msecs
    QString cal_link_;
    bool cal_recal_;
};

#endif // CALIBRATIONINFO_H
