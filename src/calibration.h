/***************************************************************************
  calibration.h
  -------------------
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QString>

struct Calibration
{
    QString serial_number = QString();
    QString calib_date = QString();

    double co2_0_dir = 0.0;
    double co2_1_dir = 0.0;
    double co2_2_dir = 0.0;
    double co2_3_dir = 0.0;
    double co2_4_dir = 0.0;
    double co2_5_dir = 0.0;
    double co2_6_dir = 0.0;
    double co2_0_inv = 0.0;
    double co2_1_inv = 0.0;
    double co2_2_inv = 0.0;
    double co2_3_inv = 0.0;
    double co2_4_inv = 0.0;
    double co2_5_inv = 0.0;
    double co2_6_inv = 0.0;
    double co2_XS = 0.0;
    double co2_Z = 0.0;
    double co2_Zero = 0.0;
    QString co2_Zero_date = QString();
    double co2_Span = 0.0;
    QString co2_Span_date = QString();
    double co2_Span_2 = 0.0;
    QString co2_Span_2_date = QString();
    double co2_CX = 0.0;
    QString co2_CX_date = QString();

    double h2o_0_dir = 0.0;
    double h2o_1_dir = 0.0;
    double h2o_2_dir = 0.0;
    double h2o_3_dir = 0.0;
    double h2o_4_dir = 0.0;
    double h2o_5_dir = 0.0;
    double h2o_6_dir = 0.0;
    double h2o_0_inv = 0.0;
    double h2o_1_inv = 0.0;
    double h2o_2_inv = 0.0;
    double h2o_3_inv = 0.0;
    double h2o_4_inv = 0.0;
    double h2o_5_inv = 0.0;
    double h2o_6_inv = 0.0;
    double h2o_XS = 0.0;
    double h2o_Z = 0.0;
    double h2o_Zero = 0.0;
    QString h2o_Zero_date = QString();
    double h2o_Span = 0.0;
    QString h2o_Span_date = QString();
    double h2o_Span_2 = 0.0;
    QString h2o_Span_2_date = QString();
    double h2o_WX = 0.0;
    QString h2o_WX_date = QString();
};

#endif  // CALIBRATION_H
