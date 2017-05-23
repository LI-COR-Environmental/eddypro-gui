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

#ifndef CALIBRATIONINFO_H
#define CALIBRATIONINFO_H

#include <QByteArray>
#include <QString>

class CalibrationInfo
{
public:
    explicit CalibrationInfo();
    explicit CalibrationInfo(double responseCode, double calDate, const QString &calLink, bool calRecal);
    explicit CalibrationInfo(const QByteArray &calibrationAsJson);
    explicit CalibrationInfo(const CalibrationInfo& calibrationInfo);
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
    double cal_date_;  // unix time in msecs
    QString cal_link_;
    bool cal_recal_;
};

#endif  // CALIBRATIONINFO_H
