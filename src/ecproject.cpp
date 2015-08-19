/***************************************************************************
  ecproject.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include "ecproject.h"

#include <QDebug>
#include <QSettings>

#include "dbghelper.h"
#include "ecinidefs.h"
#include "fileutils.h"
#include "stringutils.h"
#include "widget_utils.h"

EcProject::EcProject(QObject *parent, const ProjConfigState& project_config) :
    QObject(parent),
    modified_(false),
    ec_project_state_(EcProjectState()),
    project_config_state_(project_config)
{
    Defs::qt_registerCustomTypes();
}

EcProject::EcProject(const EcProject& project) :
    QObject(0),
    modified_(project.modified_),
    ec_project_state_(project.ec_project_state_),
    project_config_state_(project.project_config_state_)
{
}

EcProject& EcProject::operator=(const EcProject &project)
{
    if (this != &project)
    {
        modified_ = project.modified_;
        ec_project_state_ = project.ec_project_state_;
        project_config_state_ = project.project_config_state_;
    }
    return *this;
}

// destructor
EcProject::~EcProject()
{ ; }

bool EcProject::previousSettingsCompare(bool current, bool previous)
{
    if (previous)
    {
        return true;
    }
    else
    {
        if (current)
            return false;
        else
            return true;
    }
}

bool EcProject::previousFourthGasCompare(int currentGas, double currGasMw, double currGasDiff,
                                         int previousGas, double previousGasMw, double previousGasDiff)
{
    DEBUG_FUNC_NAME
    qDebug() << "currentGas" << currentGas;
    qDebug() << "previousGasMw" << previousGasMw;
    qDebug() << "currGasMw" << currGasMw;
    qDebug() << "currGasDiff" << currGasDiff;
    qDebug() << "previousGasMw" << previousGasMw;
    qDebug() << "previousGasDiff" << previousGasDiff;

    if (currentGas >= 0 && currentGas == previousGas)
    {
        if (currGasMw == previousGasMw && currGasDiff == previousGasDiff)
        {
            qDebug() << "return true";
            return true;
        }
        else
        {
            qDebug() << "return false 1";
            return false;
        }
    }
    qDebug() << "return false 2";
    return false;
}

bool EcProject::previousFileNameCompare(const QString& currentPath, const QString& previousPath)
{
    QFileInfo currentFilename(currentPath);
    QFileInfo previousFilename(previousPath);
    return (currentFilename.fileName() == previousFilename.fileName());
}

// true if current range is a subrange of the previous one
bool EcProject::compareDates(const QString& currStartDate, const QString& prevStartDate,
                             const QString& currStartTime, const QString& prevStartTime,
                             const QString& currEndDate, const QString& prevEndDate,
                             const QString& currEndTime, const QString& prevEndTime)
{
    QDate cStartDate = QDate::fromString(currStartDate, Qt::ISODate);
    QDate pStartDate = QDate::fromString(prevStartDate, Qt::ISODate);
    QTime cStartTime = QTime::fromString(currStartTime, QStringLiteral("hh:mm"));
    QTime pStartTime = QTime::fromString(prevStartTime, QStringLiteral("hh:mm"));
    QDate cEndDate = QDate::fromString(currEndDate, Qt::ISODate);
    QDate pEndDate = QDate::fromString(prevEndDate, Qt::ISODate);
    QTime cEndTime = QTime::fromString(currEndTime, QStringLiteral("hh:mm"));
    QTime pEndTime = QTime::fromString(prevEndTime, QStringLiteral("hh:mm"));

    qDebug() << "currStartDate" << currStartDate;
    qDebug() << "prevStartDate" << prevStartDate;
    qDebug() << "cStartDate" << cStartDate;
    qDebug() << "pStartDate" << pStartDate;
    qDebug() << "currEndDate" << currEndDate;
    qDebug() << "prevEndDate" << prevEndDate;
    qDebug() << "cEndDate" << cEndDate;
    qDebug() << "pEndDate" << pEndDate;

    qDebug() << "currStartTime" << currStartTime;
    qDebug() << "prevStartTime" << prevStartTime;
    qDebug() << "cStartTime" << cStartTime;
    qDebug() << "pStartTime" << pStartTime;
    qDebug() << "currEndTime" << currEndTime;
    qDebug() << "prevEndTime" << prevEndTime;
    qDebug() << "cEndTime" << cEndTime;
    qDebug() << "pEndTime" << pEndTime;

    bool test_1 = (cStartDate > pStartDate);
    bool test_2 = (cStartDate == pStartDate);
    bool test_3 = true;
    if (test_2)
        test_3 = (cStartTime >= pStartTime);
    qDebug() << "test_1" << test_1;
    qDebug() << "test_2" << test_2;
    qDebug() << "test_3" << test_3;
    bool startTest = (test_1 || test_2) && test_3;

    bool test_4 = (cEndDate < pEndDate);
    bool test_5 = (cEndDate == pEndDate);
    bool test_6 = true;
    if (test_5)
        test_6 = (cEndTime <= pEndTime);
    qDebug() << "test_4" << test_4;
    qDebug() << "test_5" << test_5;
    qDebug() << "test_6" << test_6;
    bool endTest = (test_4 || test_5) && test_6;

    qDebug() << "startTest && endTest" << (startTest && endTest) << startTest << endTest;
    return (startTest && endTest);
}

bool EcProject::fuzzyCompare(const EcProject& previousProject)
{
    DEBUG_FUNC_NAME

    bool dataSetTest = (ec_project_state_.projectGeneral.file_type == previousProject.ec_project_state_.projectGeneral.file_type)
        && (ec_project_state_.projectGeneral.file_prototype == previousProject.ec_project_state_.projectGeneral.file_prototype)
        && (ec_project_state_.projectGeneral.col_ts == previousProject.ec_project_state_.projectGeneral.col_ts)
        && (ec_project_state_.projectGeneral.col_co2 == previousProject.ec_project_state_.projectGeneral.col_co2)
        && (ec_project_state_.projectGeneral.col_h2o == previousProject.ec_project_state_.projectGeneral.col_h2o)
        && (ec_project_state_.projectGeneral.col_ch4 == previousProject.ec_project_state_.projectGeneral.col_ch4)
        && previousFourthGasCompare(ec_project_state_.projectGeneral.col_n2o,
                                    ec_project_state_.projectGeneral.gas_mw,
                                    ec_project_state_.projectGeneral.gas_diff,
                                    previousProject.ec_project_state_.projectGeneral.col_n2o,
                                    previousProject.ec_project_state_.projectGeneral.gas_mw,
                                    previousProject.ec_project_state_.projectGeneral.gas_diff)
        && (ec_project_state_.projectGeneral.col_int_t_1 == previousProject.ec_project_state_.projectGeneral.col_int_t_1)
        && (ec_project_state_.projectGeneral.col_int_t_2 == previousProject.ec_project_state_.projectGeneral.col_int_t_2)
        && (ec_project_state_.projectGeneral.col_int_p == previousProject.ec_project_state_.projectGeneral.col_int_p)
        && (ec_project_state_.projectGeneral.col_air_t == previousProject.ec_project_state_.projectGeneral.col_air_t)
        && (ec_project_state_.projectGeneral.col_air_p == previousProject.ec_project_state_.projectGeneral.col_air_p)
        && (ec_project_state_.projectGeneral.col_int_t_c == previousProject.ec_project_state_.projectGeneral.col_int_t_c)
        && (ec_project_state_.projectGeneral.col_diag_72 == previousProject.ec_project_state_.projectGeneral.col_diag_72)
        && (ec_project_state_.projectGeneral.col_diag_75 == previousProject.ec_project_state_.projectGeneral.col_diag_75)
        && (ec_project_state_.projectGeneral.col_diag_77 == previousProject.ec_project_state_.projectGeneral.col_diag_77);
    qDebug() << "dataSetTest 1" << dataSetTest;

    if (ec_project_state_.projectGeneral.subset)
    {
        qDebug() << "subset" << ec_project_state_.projectGeneral.subset;

        dataSetTest = dataSetTest
            && compareDates(ec_project_state_.projectGeneral.start_date, previousProject.ec_project_state_.projectGeneral.start_date,
                ec_project_state_.projectGeneral.start_time, previousProject.ec_project_state_.projectGeneral.start_time,
                ec_project_state_.projectGeneral.end_date, previousProject.ec_project_state_.projectGeneral.end_date,
                ec_project_state_.projectGeneral.end_time, previousProject.ec_project_state_.projectGeneral.end_time);
    }
    else
    {
        qDebug() << "subTest false";
        dataSetTest = dataSetTest
            && (ec_project_state_.projectGeneral.files_found == previousProject.ec_project_state_.projectGeneral.files_found);
    }
    qDebug() << "dataSetTest 1.1" << dataSetTest;

    dataSetTest = dataSetTest
        && previousSettingsCompare(ec_project_state_.screenGeneral.recurse, previousProject.ec_project_state_.screenGeneral.recurse);
    qDebug() << "dataSetTest 2.1" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag1_col == previousProject.ec_project_state_.screenGeneral.flag1_col);
    qDebug() << "dataSetTest 2.2" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag1_threshold == previousProject.ec_project_state_.screenGeneral.flag1_threshold);
    qDebug() << "dataSetTest 2.3" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag1_policy == previousProject.ec_project_state_.screenGeneral.flag1_policy);
    qDebug() << "dataSetTest 2.4" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag2_col == previousProject.ec_project_state_.screenGeneral.flag2_col);
    qDebug() << "dataSetTest 2.5" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag2_threshold == previousProject.ec_project_state_.screenGeneral.flag2_threshold);
    qDebug() << "dataSetTest 2.6" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag2_policy == previousProject.ec_project_state_.screenGeneral.flag2_policy);
    qDebug() << "dataSetTest 2.7" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag3_col == previousProject.ec_project_state_.screenGeneral.flag3_col);
    qDebug() << "dataSetTest 2.8" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag3_threshold == previousProject.ec_project_state_.screenGeneral.flag3_threshold);
    qDebug() << "dataSetTest 2.9" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag3_policy == previousProject.ec_project_state_.screenGeneral.flag3_policy);
    qDebug() << "dataSetTest 2.10" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag4_col == previousProject.ec_project_state_.screenGeneral.flag4_col);
    qDebug() << "dataSetTest 2.11" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag4_threshold == previousProject.ec_project_state_.screenGeneral.flag4_threshold);
    qDebug() << "dataSetTest 2.12" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag4_policy == previousProject.ec_project_state_.screenGeneral.flag4_policy);
    qDebug() << "dataSetTest 2.13" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag5_col == previousProject.ec_project_state_.screenGeneral.flag5_col);
    qDebug() << "dataSetTest 2.14" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag5_threshold == previousProject.ec_project_state_.screenGeneral.flag5_threshold);
    qDebug() << "dataSetTest 2.15" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag5_policy == previousProject.ec_project_state_.screenGeneral.flag5_policy);
    qDebug() << "dataSetTest 2.16" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag6_col == previousProject.ec_project_state_.screenGeneral.flag6_col);
    qDebug() << "dataSetTest 2.17" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag6_threshold == previousProject.ec_project_state_.screenGeneral.flag6_threshold);
    qDebug() << "dataSetTest 2.18" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag6_policy == previousProject.ec_project_state_.screenGeneral.flag6_policy);
    qDebug() << "dataSetTest 2.19" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag7_col == previousProject.ec_project_state_.screenGeneral.flag7_col);
    qDebug() << "dataSetTest 2.20" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag7_threshold == previousProject.ec_project_state_.screenGeneral.flag7_threshold);
    qDebug() << "dataSetTest 2.21" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag7_policy == previousProject.ec_project_state_.screenGeneral.flag7_policy);
    qDebug() << "dataSetTest 2.22" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag8_col == previousProject.ec_project_state_.screenGeneral.flag8_col);
    qDebug() << "dataSetTest 2.23" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag8_threshold == previousProject.ec_project_state_.screenGeneral.flag8_threshold);
    qDebug() << "dataSetTest 2.24" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag8_policy == previousProject.ec_project_state_.screenGeneral.flag8_policy);
    qDebug() << "dataSetTest 2.25" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag9_col == previousProject.ec_project_state_.screenGeneral.flag9_col);
    qDebug() << "dataSetTest 2.26" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag9_threshold == previousProject.ec_project_state_.screenGeneral.flag9_threshold);
    qDebug() << "dataSetTest 2.27" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag9_policy == previousProject.ec_project_state_.screenGeneral.flag9_policy);
    qDebug() << "dataSetTest 2.28" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag10_col == previousProject.ec_project_state_.screenGeneral.flag10_col);
    qDebug() << "dataSetTest 2.29" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag10_threshold == previousProject.ec_project_state_.screenGeneral.flag10_threshold);
    qDebug() << "dataSetTest 2.30" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenGeneral.flag10_policy == previousProject.ec_project_state_.screenGeneral.flag10_policy);
    qDebug() << "dataSetTest 2.31" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.nfiles == previousProject.ec_project_state_.screenSetting.nfiles);
    qDebug() << "dataSetTest 2.32" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.max_lack == previousProject.ec_project_state_.screenSetting.max_lack);
    qDebug() << "dataSetTest 2.33" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.u_offset == previousProject.ec_project_state_.screenSetting.u_offset);
    qDebug() << "dataSetTest 2.34" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.v_offset == previousProject.ec_project_state_.screenSetting.v_offset);
    qDebug() << "dataSetTest 2.35" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.w_offset == previousProject.ec_project_state_.screenSetting.w_offset);
    qDebug() << "dataSetTest 2.36" << dataSetTest;
    dataSetTest = dataSetTest && (ec_project_state_.screenSetting.avrg_len == previousProject.ec_project_state_.screenSetting.avrg_len);
    qDebug() << "dataSetTest 2.37" << dataSetTest;

    bool advSettingsTest = true;
    bool subTest = (ec_project_state_.screenSetting.detrend_meth == previousProject.ec_project_state_.screenSetting.detrend_meth);
    qDebug() << "advSettingsTest 1" << advSettingsTest;
    if (subTest && ec_project_state_.screenSetting.detrend_meth)
    {
        qDebug() << "advSettingsTest 2" << advSettingsTest;
        advSettingsTest = advSettingsTest && ec_project_state_.screenSetting.timeconst == previousProject.ec_project_state_.screenSetting.timeconst;
    }
    qDebug() << "advSettingsTest 3" << advSettingsTest;

    if ((ec_project_state_.projectGeneral.hf_meth > 1
         && ec_project_state_.projectGeneral.hf_meth < 5
         && ec_project_state_.spectraSettings.sa_mode)
        || ec_project_state_.screenSetting.out_bin_sp == 1
        || ec_project_state_.screenSetting.out_bin_og == 1
        || ec_project_state_.screenSetting.out_full_sp_u == 1
        || ec_project_state_.screenSetting.out_full_sp_v == 1
        || ec_project_state_.screenSetting.out_full_sp_w == 1
        || ec_project_state_.screenSetting.out_full_sp_ts == 1
        || ec_project_state_.screenSetting.out_full_sp_ch4 == 1
        || ec_project_state_.screenSetting.out_full_sp_co2 == 1
        || ec_project_state_.screenSetting.out_full_sp_h2o == 1
        || ec_project_state_.screenSetting.out_full_sp_n2o == 1
        || ec_project_state_.screenSetting.out_full_cosp_ch4 == 1
        || ec_project_state_.screenSetting.out_full_cosp_co2 == 1
        || ec_project_state_.screenSetting.out_full_cosp_h2o == 1
        || ec_project_state_.screenSetting.out_full_cosp_n2o == 1
        || ec_project_state_.screenSetting.out_full_cosp_ts == 1
        || ec_project_state_.screenSetting.out_full_cosp_u == 1
        || ec_project_state_.screenSetting.out_full_cosp_v == 1
        || ec_project_state_.projectGeneral.out_mean_cosp == 1)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenSetting.tap_win == previousProject.ec_project_state_.screenSetting.tap_win)
               && (ec_project_state_.screenSetting.nbins == previousProject.ec_project_state_.screenSetting.nbins);
    }
    qDebug() << "advSettingsTest 2" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && (ec_project_state_.screenSetting.timeconst== previousProject.ec_project_state_.screenSetting.timeconst);
    qDebug() << "advSettingsTest 21" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && (ec_project_state_.screenSetting.tlag_meth == previousProject.ec_project_state_.screenSetting.tlag_meth);
    qDebug() << "advSettingsTest 22" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_bin_sp, previousProject.ec_project_state_.screenSetting.out_bin_sp);
    qDebug() << "advSettingsTest 23" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_bin_og, previousProject.ec_project_state_.screenSetting.out_bin_og);
    qDebug() << "advSettingsTest 24" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.projectGeneral.out_mean_cosp, previousProject.ec_project_state_.projectGeneral.out_mean_cosp);
    qDebug() << "advSettingsTest 25" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_u, previousProject.ec_project_state_.screenSetting.out_full_sp_u);
    qDebug() << "advSettingsTest 26" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_v, previousProject.ec_project_state_.screenSetting.out_full_sp_v);
    qDebug() << "advSettingsTest 27" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_w, previousProject.ec_project_state_.screenSetting.out_full_sp_w);
    qDebug() << "advSettingsTest 271" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_ts, previousProject.ec_project_state_.screenSetting.out_full_sp_ts);
    qDebug() << "advSettingsTest 272" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_co2, previousProject.ec_project_state_.screenSetting.out_full_sp_co2);
    qDebug() << "advSettingsTest 273" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_h2o, previousProject.ec_project_state_.screenSetting.out_full_sp_h2o);
    qDebug() << "advSettingsTest 274" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_ch4, previousProject.ec_project_state_.screenSetting.out_full_sp_ch4);
    qDebug() << "advSettingsTest 275" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_sp_n2o, previousProject.ec_project_state_.screenSetting.out_full_sp_n2o);
    qDebug() << "advSettingsTest 276" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_u, previousProject.ec_project_state_.screenSetting.out_full_cosp_u);
    qDebug() << "advSettingsTest 277" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_v, previousProject.ec_project_state_.screenSetting.out_full_cosp_v);
    qDebug() << "advSettingsTest 278" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_ts, previousProject.ec_project_state_.screenSetting.out_full_cosp_ts);
    qDebug() << "advSettingsTest 279" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_co2, previousProject.ec_project_state_.screenSetting.out_full_cosp_co2);
    qDebug() << "advSettingsTest 270" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_h2o, previousProject.ec_project_state_.screenSetting.out_full_cosp_h2o);
    qDebug() << "advSettingsTest 271" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_ch4, previousProject.ec_project_state_.screenSetting.out_full_cosp_ch4);
    qDebug() << "advSettingsTest 272" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_full_cosp_n2o, previousProject.ec_project_state_.screenSetting.out_full_cosp_n2o);
    qDebug() << "advSettingsTest 28" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_1, previousProject.ec_project_state_.screenSetting.out_st_1);
    qDebug() << "advSettingsTest 29" << advSettingsTest;
    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_2, previousProject.ec_project_state_.screenSetting.out_st_2)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_3, previousProject.ec_project_state_.screenSetting.out_st_3)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_4, previousProject.ec_project_state_.screenSetting.out_st_4)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_5, previousProject.ec_project_state_.screenSetting.out_st_5)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_6, previousProject.ec_project_state_.screenSetting.out_st_6)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_st_7, previousProject.ec_project_state_.screenSetting.out_st_7)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_details, previousProject.ec_project_state_.screenSetting.out_details)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_1, previousProject.ec_project_state_.screenSetting.out_raw_1)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_2, previousProject.ec_project_state_.screenSetting.out_raw_2)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_3, previousProject.ec_project_state_.screenSetting.out_raw_3)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_4, previousProject.ec_project_state_.screenSetting.out_raw_4)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_5, previousProject.ec_project_state_.screenSetting.out_raw_5)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_6, previousProject.ec_project_state_.screenSetting.out_raw_6)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_7, previousProject.ec_project_state_.screenSetting.out_raw_7)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_u, previousProject.ec_project_state_.screenSetting.out_raw_u)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_v, previousProject.ec_project_state_.screenSetting.out_raw_v)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_w, previousProject.ec_project_state_.screenSetting.out_raw_w)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_ts, previousProject.ec_project_state_.screenSetting.out_raw_ts)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_co2, previousProject.ec_project_state_.screenSetting.out_raw_co2)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_h2o, previousProject.ec_project_state_.screenSetting.out_raw_h2o)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_ch4, previousProject.ec_project_state_.screenSetting.out_raw_ch4)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_gas4, previousProject.ec_project_state_.screenSetting.out_raw_gas4)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_tair, previousProject.ec_project_state_.screenSetting.out_raw_tair)
        && previousSettingsCompare(ec_project_state_.screenSetting.out_raw_pair, previousProject.ec_project_state_.screenSetting.out_raw_pair);
    qDebug() << "advSettingsTest 30" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenTest.test_sr, previousProject.ec_project_state_.screenTest.test_sr);
    qDebug() << "advSettingsTest 31" << advSettingsTest;

    advSettingsTest = advSettingsTest
        && previousSettingsCompare(ec_project_state_.screenTest.test_ar, previousProject.ec_project_state_.screenTest.test_ar)
        && previousSettingsCompare(ec_project_state_.screenTest.test_do, previousProject.ec_project_state_.screenTest.test_do)
        && previousSettingsCompare(ec_project_state_.screenTest.test_al, previousProject.ec_project_state_.screenTest.test_al)
        && previousSettingsCompare(ec_project_state_.screenTest.test_sk, previousProject.ec_project_state_.screenTest.test_sk)
        && previousSettingsCompare(ec_project_state_.screenTest.test_ds, previousProject.ec_project_state_.screenTest.test_ds)
        && previousSettingsCompare(ec_project_state_.screenTest.test_tl, previousProject.ec_project_state_.screenTest.test_tl)
        && previousSettingsCompare(ec_project_state_.screenTest.test_aa, previousProject.ec_project_state_.screenTest.test_aa)
        && previousSettingsCompare(ec_project_state_.screenTest.test_ns, previousProject.ec_project_state_.screenTest.test_ns);
    qDebug() << "advSettingsTest 3" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_sr && previousProject.ec_project_state_.screenTest.test_sr);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.sr_num_spk == previousProject.ec_project_state_.screenParam.sr_num_spk)
               && (ec_project_state_.screenParam.sr_lim_hf == previousProject.ec_project_state_.screenParam.sr_lim_hf)
               && (ec_project_state_.screenSetting.filter_sr == previousProject.ec_project_state_.screenSetting.filter_sr)
               && (ec_project_state_.screenParam.sr_lim_u == previousProject.ec_project_state_.screenParam.sr_lim_u)
               && (ec_project_state_.screenParam.sr_lim_w == previousProject.ec_project_state_.screenParam.sr_lim_w)
               && (ec_project_state_.screenParam.sr_lim_co2 == previousProject.ec_project_state_.screenParam.sr_lim_co2)
               && (ec_project_state_.screenParam.sr_lim_h2o == previousProject.ec_project_state_.screenParam.sr_lim_h2o)
               && (ec_project_state_.screenParam.sr_lim_ch4 == previousProject.ec_project_state_.screenParam.sr_lim_ch4)
               && (ec_project_state_.screenParam.sr_lim_n2o == previousProject.ec_project_state_.screenParam.sr_lim_n2o);
    }
    qDebug() << "advSettingsTest 4" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_ar && previousProject.ec_project_state_.screenTest.test_ar);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.ar_lim == previousProject.ec_project_state_.screenParam.ar_lim)
               && (ec_project_state_.screenParam.ar_bins == previousProject.ec_project_state_.screenParam.ar_bins)
               && (ec_project_state_.screenParam.ar_hf_lim == previousProject.ec_project_state_.screenParam.ar_hf_lim);
    }
    qDebug() << "advSettingsTest 5" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_do && previousProject.ec_project_state_.screenTest.test_do);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.do_extlim_dw == previousProject.ec_project_state_.screenParam.do_extlim_dw)
               && (ec_project_state_.screenParam.do_hf1_lim == previousProject.ec_project_state_.screenParam.do_hf1_lim)
               && (ec_project_state_.screenParam.do_hf2_lim == previousProject.ec_project_state_.screenParam.do_hf2_lim);
    }
    qDebug() << "advSettingsTest 6" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_al && previousProject.ec_project_state_.screenTest.test_al);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.al_u_max == previousProject.ec_project_state_.screenParam.al_u_max)
               && (ec_project_state_.screenParam.al_w_max == previousProject.ec_project_state_.screenParam.al_w_max)
               && (ec_project_state_.screenParam.al_tson_min == previousProject.ec_project_state_.screenParam.al_tson_min)
               && (ec_project_state_.screenParam.al_tson_max == previousProject.ec_project_state_.screenParam.al_tson_max)
               && (ec_project_state_.screenParam.al_co2_min == previousProject.ec_project_state_.screenParam.al_co2_min)
               && (ec_project_state_.screenParam.al_co2_max == previousProject.ec_project_state_.screenParam.al_co2_max)
               && (ec_project_state_.screenParam.al_h2o_min == previousProject.ec_project_state_.screenParam.al_h2o_min)
               && (ec_project_state_.screenParam.al_h2o_max == previousProject.ec_project_state_.screenParam.al_h2o_max)
               && (ec_project_state_.screenParam.al_ch4_min == previousProject.ec_project_state_.screenParam.al_ch4_min)
               && (ec_project_state_.screenParam.al_ch4_max == previousProject.ec_project_state_.screenParam.al_ch4_max)
               && (ec_project_state_.screenParam.al_n2o_min == previousProject.ec_project_state_.screenParam.al_n2o_min)
               && (ec_project_state_.screenParam.al_n2o_max == previousProject.ec_project_state_.screenParam.al_n2o_max)
               && (ec_project_state_.screenSetting.filter_al == previousProject.ec_project_state_.screenSetting.filter_al);
    }
    qDebug() << "advSettingsTest 7" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_sk && previousProject.ec_project_state_.screenTest.test_sk);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.sk_hf_kumax == previousProject.ec_project_state_.screenParam.sk_hf_kumax)
               && (ec_project_state_.screenParam.sk_hf_kumin == previousProject.ec_project_state_.screenParam.sk_hf_kumin)
               && (ec_project_state_.screenParam.sk_hf_skmax == previousProject.ec_project_state_.screenParam.sk_hf_skmax)
               && (ec_project_state_.screenParam.sk_hf_skmin == previousProject.ec_project_state_.screenParam.sk_hf_skmin)
               && (ec_project_state_.screenParam.sk_sf_kumax == previousProject.ec_project_state_.screenParam.sk_sf_kumax)
               && (ec_project_state_.screenParam.sk_sf_kumin == previousProject.ec_project_state_.screenParam.sk_sf_kumin)
               && (ec_project_state_.screenParam.sk_sf_skmax == previousProject.ec_project_state_.screenParam.sk_sf_skmax)
               && (ec_project_state_.screenParam.sk_sf_skmin == previousProject.ec_project_state_.screenParam.sk_sf_skmin);
    }
    qDebug() << "advSettingsTest 8" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_ds && previousProject.ec_project_state_.screenTest.test_ds);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.ds_hf_ch4 == previousProject.ec_project_state_.screenParam.ds_hf_ch4)
               && (ec_project_state_.screenParam.ds_hf_co2 == previousProject.ec_project_state_.screenParam.ds_hf_co2)
               && (ec_project_state_.screenParam.ds_hf_h2o == previousProject.ec_project_state_.screenParam.ds_hf_h2o)
               && (ec_project_state_.screenParam.ds_hf_n2o == previousProject.ec_project_state_.screenParam.ds_hf_n2o)
               && (ec_project_state_.screenParam.ds_hf_t == previousProject.ec_project_state_.screenParam.ds_hf_t)
               && (ec_project_state_.screenParam.ds_hf_uv == previousProject.ec_project_state_.screenParam.ds_hf_uv)
               && (ec_project_state_.screenParam.ds_hf_var == previousProject.ec_project_state_.screenParam.ds_hf_var)
               && (ec_project_state_.screenParam.ds_hf_w == previousProject.ec_project_state_.screenParam.ds_hf_w)
               && (ec_project_state_.screenParam.ds_sf_ch4 == previousProject.ec_project_state_.screenParam.ds_sf_ch4)
               && (ec_project_state_.screenParam.ds_sf_co2 == previousProject.ec_project_state_.screenParam.ds_sf_co2)
               && (ec_project_state_.screenParam.ds_sf_h2o == previousProject.ec_project_state_.screenParam.ds_sf_h2o)
               && (ec_project_state_.screenParam.ds_sf_n2o == previousProject.ec_project_state_.screenParam.ds_sf_n2o)
               && (ec_project_state_.screenParam.ds_sf_t == previousProject.ec_project_state_.screenParam.ds_sf_t)
               && (ec_project_state_.screenParam.ds_sf_uv == previousProject.ec_project_state_.screenParam.ds_sf_uv)
               && (ec_project_state_.screenParam.ds_sf_var == previousProject.ec_project_state_.screenParam.ds_sf_var)
               && (ec_project_state_.screenParam.ds_sf_w == previousProject.ec_project_state_.screenParam.ds_sf_w);
    }
    qDebug() << "advSettingsTest 9" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_tl && previousProject.ec_project_state_.screenTest.test_tl);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.tl_def_ch4 == previousProject.ec_project_state_.screenParam.tl_def_ch4)
               && (ec_project_state_.screenParam.tl_def_co2 == previousProject.ec_project_state_.screenParam.tl_def_co2)
               && (ec_project_state_.screenParam.tl_def_h2o == previousProject.ec_project_state_.screenParam.tl_def_h2o)
               && (ec_project_state_.screenParam.tl_def_n2o == previousProject.ec_project_state_.screenParam.tl_def_n2o)
               && (ec_project_state_.screenParam.tl_hf_lim == previousProject.ec_project_state_.screenParam.tl_hf_lim)
               && (ec_project_state_.screenParam.tl_sf_lim == previousProject.ec_project_state_.screenParam.tl_sf_lim);
    }
    qDebug() << "advSettingsTest 10" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_aa && previousProject.ec_project_state_.screenTest.test_aa);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.aa_lim == previousProject.ec_project_state_.screenParam.aa_lim)
               && (ec_project_state_.screenParam.aa_max == previousProject.ec_project_state_.screenParam.aa_max)
               && (ec_project_state_.screenParam.aa_min == previousProject.ec_project_state_.screenParam.aa_min);
    }
    qDebug() << "advSettingsTest 11" << advSettingsTest;

    subTest = (ec_project_state_.screenTest.test_ns && previousProject.ec_project_state_.screenTest.test_ns);
    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.screenParam.ns_hf_lim == previousProject.ec_project_state_.screenParam.ns_hf_lim);
    }
    qDebug() << "advSettingsTest 12" << advSettingsTest;

    subTest = (ec_project_state_.projectGeneral.use_biomet == previousProject.ec_project_state_.projectGeneral.use_biomet);
    dataSetTest = dataSetTest && subTest;
    qDebug() << "dataSetTest 3" << dataSetTest;

    if (subTest)
    {
        advSettingsTest = advSettingsTest
               && (ec_project_state_.biomParam.col_lwin == previousProject.ec_project_state_.biomParam.col_lwin)
               && (ec_project_state_.biomParam.col_pa == previousProject.ec_project_state_.biomParam.col_pa)
               && (ec_project_state_.biomParam.col_ppfd == previousProject.ec_project_state_.biomParam.col_ppfd)
               && (ec_project_state_.biomParam.col_rg == previousProject.ec_project_state_.biomParam.col_rg)
               && (ec_project_state_.biomParam.col_rh == previousProject.ec_project_state_.biomParam.col_rh)
               && (ec_project_state_.biomParam.col_ta == previousProject.ec_project_state_.biomParam.col_ta);
    }
    qDebug() << "advSettingsTest 13" << advSettingsTest;

    subTest = (ec_project_state_.projectGeneral.use_alt_md_file == previousProject.ec_project_state_.projectGeneral.use_alt_md_file);
    dataSetTest = dataSetTest && subTest;

    if (subTest && ec_project_state_.projectGeneral.use_alt_md_file)
        subTest = subTest && previousFileNameCompare(ec_project_state_.projectGeneral.md_file, previousProject.ec_project_state_.projectGeneral.md_file);
    dataSetTest = dataSetTest && subTest;
    qDebug() << "dataSetTest 4" << dataSetTest;

    subTest = (ec_project_state_.projectGeneral.use_tlfile == previousProject.ec_project_state_.projectGeneral.use_tlfile);
    dataSetTest = dataSetTest && subTest;

    if (subTest && ec_project_state_.projectGeneral.use_tlfile)
        subTest = subTest && previousFileNameCompare(ec_project_state_.projectGeneral.timeline_file, previousProject.ec_project_state_.projectGeneral.timeline_file);
    dataSetTest = dataSetTest && subTest;
    qDebug() << "dataSetTest 5" << dataSetTest;

    subTest = (ec_project_state_.projectGeneral.wpl_meth == previousProject.ec_project_state_.projectGeneral.wpl_meth);
    advSettingsTest = advSettingsTest && subTest;
    qDebug() << "advSettingsTest 14" << advSettingsTest;

    if (subTest && ec_project_state_.projectGeneral.wpl_meth)
    {
        subTest = (ec_project_state_.screenSetting.to_mixratio == previousProject.ec_project_state_.screenSetting.to_mixratio);
        advSettingsTest = advSettingsTest && subTest;

        if (subTest && !ec_project_state_.screenSetting.to_mixratio)
        {
            advSettingsTest = advSettingsTest && (ec_project_state_.screenSetting.bu_corr == previousProject.ec_project_state_.screenSetting.bu_corr)
                && (ec_project_state_.screenSetting.bu_multi == previousProject.ec_project_state_.screenSetting.bu_multi)
                && (ec_project_state_.screenSetting.l_day_bot_gain == previousProject.ec_project_state_.screenSetting.l_day_bot_gain)
                && (ec_project_state_.screenSetting.l_day_bot_offset == previousProject.ec_project_state_.screenSetting.l_day_bot_offset)
                && (ec_project_state_.screenSetting.l_day_spar_gain == previousProject.ec_project_state_.screenSetting.l_day_spar_gain)
                && (ec_project_state_.screenSetting.l_day_spar_offset == previousProject.ec_project_state_.screenSetting.l_day_spar_offset)
                && (ec_project_state_.screenSetting.l_day_top_gain == previousProject.ec_project_state_.screenSetting.l_day_top_gain)
                && (ec_project_state_.screenSetting.l_day_top_offset == previousProject.ec_project_state_.screenSetting.l_day_top_offset)
                && (ec_project_state_.screenSetting.l_night_bot_gain == previousProject.ec_project_state_.screenSetting.l_night_bot_gain)
                && (ec_project_state_.screenSetting.l_night_bot_offset == previousProject.ec_project_state_.screenSetting.l_night_bot_offset)
                && (ec_project_state_.screenSetting.l_night_spar_gain == previousProject.ec_project_state_.screenSetting.l_night_spar_gain)
                && (ec_project_state_.screenSetting.l_night_spar_offset == previousProject.ec_project_state_.screenSetting.l_night_spar_offset)
                && (ec_project_state_.screenSetting.l_night_top_gain == previousProject.ec_project_state_.screenSetting.l_night_top_gain)
                && (ec_project_state_.screenSetting.l_night_top_offset == previousProject.ec_project_state_.screenSetting.l_night_top_offset)
                && (ec_project_state_.screenSetting.m_day_bot1 == previousProject.ec_project_state_.screenSetting.m_day_bot1)
                && (ec_project_state_.screenSetting.m_day_bot2 == previousProject.ec_project_state_.screenSetting.m_day_bot2)
                && (ec_project_state_.screenSetting.m_day_bot3 == previousProject.ec_project_state_.screenSetting.m_day_bot3)
                && (ec_project_state_.screenSetting.m_day_bot4 == previousProject.ec_project_state_.screenSetting.m_day_bot4)
                && (ec_project_state_.screenSetting.m_day_spar1 == previousProject.ec_project_state_.screenSetting.m_day_spar1)
                && (ec_project_state_.screenSetting.m_day_spar2 == previousProject.ec_project_state_.screenSetting.m_day_spar2)
                && (ec_project_state_.screenSetting.m_day_spar3 == previousProject.ec_project_state_.screenSetting.m_day_spar3)
                && (ec_project_state_.screenSetting.m_day_spar4 == previousProject.ec_project_state_.screenSetting.m_day_spar4)
                && (ec_project_state_.screenSetting.m_day_top1 == previousProject.ec_project_state_.screenSetting.m_day_top1)
                && (ec_project_state_.screenSetting.m_day_top2 == previousProject.ec_project_state_.screenSetting.m_day_top2)
                && (ec_project_state_.screenSetting.m_day_top3 == previousProject.ec_project_state_.screenSetting.m_day_top3)
                && (ec_project_state_.screenSetting.m_day_top4 == previousProject.ec_project_state_.screenSetting.m_day_top4)
                && (ec_project_state_.screenSetting.m_night_bot1 == previousProject.ec_project_state_.screenSetting.m_night_bot1)
                && (ec_project_state_.screenSetting.m_night_bot2 == previousProject.ec_project_state_.screenSetting.m_night_bot2)
                && (ec_project_state_.screenSetting.m_night_bot3 == previousProject.ec_project_state_.screenSetting.m_night_bot3)
                && (ec_project_state_.screenSetting.m_night_bot4 == previousProject.ec_project_state_.screenSetting.m_night_bot4)
                && (ec_project_state_.screenSetting.m_night_spar1 == previousProject.ec_project_state_.screenSetting.m_night_spar1)
                && (ec_project_state_.screenSetting.m_night_spar2 == previousProject.ec_project_state_.screenSetting.m_night_spar2)
                && (ec_project_state_.screenSetting.m_night_spar3 == previousProject.ec_project_state_.screenSetting.m_night_spar3)
                && (ec_project_state_.screenSetting.m_night_spar4 == previousProject.ec_project_state_.screenSetting.m_night_spar4)
                && (ec_project_state_.screenSetting.m_night_top1 == previousProject.ec_project_state_.screenSetting.m_night_top1)
                && (ec_project_state_.screenSetting.m_night_top2 == previousProject.ec_project_state_.screenSetting.m_night_top2)
                && (ec_project_state_.screenSetting.m_night_top3 == previousProject.ec_project_state_.screenSetting.m_night_top3)
                && (ec_project_state_.screenSetting.m_night_top4 == previousProject.ec_project_state_.screenSetting.m_night_top4);
        }
    }
    qDebug() << "advSettingsTest 15" << advSettingsTest;

    subTest = (ec_project_state_.projectGeneral.master_sonic == previousProject.ec_project_state_.projectGeneral.master_sonic);
    dataSetTest = dataSetTest && subTest;
    if (subTest && !ec_project_state_.projectGeneral.master_sonic.contains(QStringLiteral("csat")))
    {
        dataSetTest = dataSetTest && (ec_project_state_.screenSetting.cross_wind == previousProject.ec_project_state_.screenSetting.cross_wind);
    }
    qDebug() << "dataSetTest 6" << dataSetTest;
    if (subTest && (ec_project_state_.projectGeneral.master_sonic.contains(QStringLiteral("hs"))
                    || ec_project_state_.projectGeneral.master_sonic.contains(QStringLiteral("wm"))
                    || ec_project_state_.projectGeneral.master_sonic.contains(QStringLiteral("r2"))
                    || ec_project_state_.projectGeneral.master_sonic.contains(QStringLiteral("r3"))))
    {
        dataSetTest = dataSetTest && (ec_project_state_.screenSetting.flow_distortion == previousProject.ec_project_state_.screenSetting.flow_distortion);
    }
    qDebug() << "dataSetTest 7" << dataSetTest;

    subTest = (ec_project_state_.screenSetting.rot_meth == previousProject.ec_project_state_.screenSetting.rot_meth);
    advSettingsTest = advSettingsTest && subTest;
    qDebug() << "advSettingsTest 16" << advSettingsTest;

    if (subTest && (ec_project_state_.screenSetting.rot_meth == 3
                    || ec_project_state_.screenSetting.rot_meth == 4))
    {
        subTest = (ec_project_state_.screenTilt.mode == previousProject.ec_project_state_.screenTilt.mode);
        advSettingsTest = advSettingsTest && subTest;

        if (subTest && ec_project_state_.screenTilt.mode == 1)
        {
            advSettingsTest = advSettingsTest
                && (ec_project_state_.screenTilt.north_offset == previousProject.ec_project_state_.screenTilt.north_offset)
                && (ec_project_state_.screenTilt.min_num_per_sec == previousProject.ec_project_state_.screenTilt.min_num_per_sec)
                && (ec_project_state_.screenTilt.w_max == previousProject.ec_project_state_.screenTilt.w_max)
                && (ec_project_state_.screenTilt.u_min == previousProject.ec_project_state_.screenTilt.u_min)
                && (ec_project_state_.screenTilt.fix_policy == previousProject.ec_project_state_.screenTilt.fix_policy);

            subTest = (ec_project_state_.screenTilt.subset == previousProject.ec_project_state_.screenTilt.subset);
            if (subTest)
            {
                if (ec_project_state_.screenTilt.subset)
                {
                    advSettingsTest = advSettingsTest
                                      && (ec_project_state_.screenTilt.start_date == previousProject.ec_project_state_.screenTilt.start_date)
                                      && (ec_project_state_.screenTilt.end_date == previousProject.ec_project_state_.screenTilt.end_date);
                }
                else
                {
                    advSettingsTest = advSettingsTest
                                      && (ec_project_state_.projectGeneral.files_found == previousProject.ec_project_state_.projectGeneral.files_found);
                }
            }
            else
            {
                advSettingsTest = false;
            }
        }
        else if (subTest && ec_project_state_.screenTilt.mode == 0)
        {
            advSettingsTest = advSettingsTest
                              && previousFileNameCompare(ec_project_state_.screenTilt.file, previousProject.ec_project_state_.screenTilt.file);
        }
        else
        {
            advSettingsTest = false;
        }
    }
    qDebug() << "advSettingsTest 17" << advSettingsTest;

    subTest = (ec_project_state_.screenSetting.tlag_meth == previousProject.ec_project_state_.screenSetting.tlag_meth);
    advSettingsTest = advSettingsTest && subTest;
    qDebug() << "advSettingsTest 18" << advSettingsTest;

    if (subTest && (ec_project_state_.screenSetting.tlag_meth == 4))
    {
        subTest = (ec_project_state_.timelagOpt.mode == previousProject.ec_project_state_.timelagOpt.mode);
        advSettingsTest = advSettingsTest && subTest;

        if (subTest && ec_project_state_.timelagOpt.mode == 1)
        {
            advSettingsTest = advSettingsTest
                && (ec_project_state_.timelagOpt.ch4_max_lag == previousProject.ec_project_state_.timelagOpt.ch4_max_lag)
                && (ec_project_state_.timelagOpt.ch4_min_flux == previousProject.ec_project_state_.timelagOpt.ch4_min_flux)
                && (ec_project_state_.timelagOpt.ch4_min_lag == previousProject.ec_project_state_.timelagOpt.ch4_min_lag)
                && (ec_project_state_.timelagOpt.co2_max_lag == previousProject.ec_project_state_.timelagOpt.co2_max_lag)
                && (ec_project_state_.timelagOpt.co2_min_flux == previousProject.ec_project_state_.timelagOpt.co2_min_flux)
                && (ec_project_state_.timelagOpt.co2_min_lag == previousProject.ec_project_state_.timelagOpt.co2_min_lag)
                && (ec_project_state_.timelagOpt.gas4_max_lag == previousProject.ec_project_state_.timelagOpt.gas4_max_lag)
                && (ec_project_state_.timelagOpt.gas4_min_flux == previousProject.ec_project_state_.timelagOpt.gas4_min_flux)
                && (ec_project_state_.timelagOpt.gas4_min_lag == previousProject.ec_project_state_.timelagOpt.gas4_min_lag)
                && (ec_project_state_.timelagOpt.h2o_max_lag == previousProject.ec_project_state_.timelagOpt.h2o_max_lag)
                && (ec_project_state_.timelagOpt.h2o_min_lag == previousProject.ec_project_state_.timelagOpt.h2o_min_lag)
                && (ec_project_state_.timelagOpt.le_min_flux == previousProject.ec_project_state_.timelagOpt.le_min_flux)
                && (ec_project_state_.timelagOpt.pg_range == previousProject.ec_project_state_.timelagOpt.pg_range)
                && (ec_project_state_.timelagOpt.to_h2o_nclass == previousProject.ec_project_state_.timelagOpt.to_h2o_nclass);

            subTest = (ec_project_state_.timelagOpt.subset == previousProject.ec_project_state_.timelagOpt.subset);
            if (subTest)
            {
                if (ec_project_state_.timelagOpt.subset)
                {
                    advSettingsTest = advSettingsTest
                                      && (ec_project_state_.timelagOpt.start_date == previousProject.ec_project_state_.timelagOpt.start_date)
                                      && (ec_project_state_.timelagOpt.end_date == previousProject.ec_project_state_.timelagOpt.end_date);
                }
                else
                {
                    advSettingsTest = advSettingsTest
                                      && (ec_project_state_.projectGeneral.files_found == previousProject.ec_project_state_.projectGeneral.files_found);
                }
            }
            else
            {
                advSettingsTest = false;
            }
        }
        else if (subTest && ec_project_state_.timelagOpt.mode == 0)
        {
            advSettingsTest = advSettingsTest
                              && previousFileNameCompare(ec_project_state_.timelagOpt.file, previousProject.ec_project_state_.timelagOpt.file);
        }
        else
        {
            advSettingsTest = false;
        }
    }
    qDebug() << "advSettingsTest 19" << advSettingsTest;

    subTest = ec_project_state_.randomError.method == previousProject.ec_project_state_.randomError.method;
    advSettingsTest = advSettingsTest && subTest;
    qDebug() << "advSettingsTest 20" << advSettingsTest;

    if (subTest && (ec_project_state_.randomError.method == 1 || ec_project_state_.randomError.method == 2))
    {
        advSettingsTest = advSettingsTest
            && ec_project_state_.randomError.its_method == previousProject.ec_project_state_.randomError.its_method
            && ec_project_state_.randomError.its_tlag_max == previousProject.ec_project_state_.randomError.its_tlag_max;
    }
    qDebug() << "advSettingsTest 21" << advSettingsTest;
    qDebug() << "dataSetTest 8" << dataSetTest;

    qDebug() << "run_mode" << static_cast<int>(ec_project_state_.projectGeneral.run_mode);
    switch (ec_project_state_.projectGeneral.run_mode)
    {
        case Defs::CurrRunMode::Express:
            return dataSetTest;
        case Defs::CurrRunMode::Advanced:
            return (dataSetTest && advSettingsTest);
        case Defs::CurrRunMode::Retriever:
            return false;
        default:
            return false;
    }
}

// New project
void EcProject::newEcProject(const ProjConfigState& project_config)
{
    DEBUG_FUNC_NAME

    // data e ora in formato ISO
    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString(Qt::ISODate);

    // update project configuration
    project_config_state_ = project_config;

    ec_project_state_.projectGeneral.sw_version = Defs::APP_VERSION_STR;
    ec_project_state_.projectGeneral.ini_version = Defs::PROJECT_FILE_VERSION_STR;
    ec_project_state_.projectGeneral.creation_date = now_str;
    ec_project_state_.projectGeneral.last_change_date.clear();
    ec_project_state_.projectGeneral.run_mode = Defs::CurrRunMode::Advanced;
    ec_project_state_.projectGeneral.run_fcc = false;
    ec_project_state_.projectGeneral.file_name.clear();
    ec_project_state_.projectGeneral.project_title.clear();
    ec_project_state_.projectGeneral.project_id.clear();
    ec_project_state_.projectGeneral.file_type = Defs::RawFileType::GHG;
    ec_project_state_.projectGeneral.file_prototype.clear();
    ec_project_state_.projectGeneral.use_alt_md_file = false;
    ec_project_state_.projectGeneral.md_file.clear();
    ec_project_state_.projectGeneral.use_tlfile = false;
    ec_project_state_.projectGeneral.timeline_file.clear();
    ec_project_state_.projectGeneral.binary_hnlines = -1;
    ec_project_state_.projectGeneral.binary_eol = -1;
    ec_project_state_.projectGeneral.binary_nbytes = -1;
    ec_project_state_.projectGeneral.binary_little_end = -1;
    ec_project_state_.projectGeneral.master_sonic.clear();
    ec_project_state_.projectGeneral.col_co2 = -1;
    ec_project_state_.projectGeneral.col_h2o = -1;
    ec_project_state_.projectGeneral.col_ch4 = -1;
    ec_project_state_.projectGeneral.col_n2o = -1;
    ec_project_state_.projectGeneral.col_int_t_c = -1;
    ec_project_state_.projectGeneral.col_int_t_1 = -1;
    ec_project_state_.projectGeneral.col_int_t_2 = -1;
    ec_project_state_.projectGeneral.col_int_p = -1;
    ec_project_state_.projectGeneral.col_air_t = -1;
    ec_project_state_.projectGeneral.col_air_p = -1;
    ec_project_state_.projectGeneral.col_diag_75 = -1;
    ec_project_state_.projectGeneral.col_diag_72 = -1;
    ec_project_state_.projectGeneral.col_diag_77 = -1;
    ec_project_state_.projectGeneral.col_ts = -1;
    ec_project_state_.projectGeneral.gas_mw = -1.0;
    ec_project_state_.projectGeneral.gas_diff = -1.0;
    ec_project_state_.projectGeneral.out_ghg_eu = 0;
    ec_project_state_.projectGeneral.out_amflux = 0;
    ec_project_state_.projectGeneral.out_rich = 1;
    ec_project_state_.projectGeneral.out_md = 1;
    ec_project_state_.projectGeneral.out_biomet = 0;
    ec_project_state_.projectGeneral.make_dataset = false;
    ec_project_state_.projectGeneral.subset = 0;
    ec_project_state_.projectGeneral.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.projectGeneral.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.projectGeneral.start_time = QTime(0, 0).toString(QStringLiteral("hh:mm"));
    ec_project_state_.projectGeneral.end_time = QTime(23, 59).toString(QStringLiteral("hh:mm"));
    ec_project_state_.projectGeneral.hf_meth = 1;
    ec_project_state_.projectGeneral.lf_meth = 1;
    ec_project_state_.projectGeneral.wpl_meth = 1;
    ec_project_state_.projectGeneral.foot_meth = 1;
    ec_project_state_.projectGeneral.tob1_format = 0;
    ec_project_state_.projectGeneral.out_path.clear();
    ec_project_state_.projectGeneral.fix_out_format = 0;
    ec_project_state_.projectGeneral.err_label = QStringLiteral("-9999.0");
    ec_project_state_.projectGeneral.qcflag_meth = 1;
    ec_project_state_.projectGeneral.use_biomet = 0;
    ec_project_state_.projectGeneral.biom_file.clear();
    ec_project_state_.projectGeneral.biom_dir.clear();
    ec_project_state_.projectGeneral.biom_recurse = 0;
    ec_project_state_.projectGeneral.biom_ext = QStringLiteral("txt");
    ec_project_state_.projectGeneral.out_mean_cosp = 0;
    ec_project_state_.projectGeneral.bin_sp_avail = 0;
    ec_project_state_.projectGeneral.full_sp_avail = 0;
    ec_project_state_.projectGeneral.files_found = 0;

    // preproc general section
    ec_project_state_.screenGeneral.start_run.clear();
    ec_project_state_.screenGeneral.end_run.clear();
    ec_project_state_.screenGeneral.data_path.clear();
    ec_project_state_.screenGeneral.use_geo_north = false;
    ec_project_state_.screenGeneral.mag_dec = 0.0;
    ec_project_state_.screenGeneral.dec_date = ec_project_state_.projectGeneral.end_date;
    ec_project_state_.screenGeneral.recurse = 1;
    ec_project_state_.screenGeneral.flag1_col = -1;
    ec_project_state_.screenGeneral.flag1_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag1_policy = 1;
    ec_project_state_.screenGeneral.flag2_col = -1;
    ec_project_state_.screenGeneral.flag2_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag2_policy = 1;
    ec_project_state_.screenGeneral.flag3_col = -1;
    ec_project_state_.screenGeneral.flag3_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag3_policy = 1;
    ec_project_state_.screenGeneral.flag4_col = -1;
    ec_project_state_.screenGeneral.flag4_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag4_policy = 1;
    ec_project_state_.screenGeneral.flag5_col = -1;
    ec_project_state_.screenGeneral.flag5_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag5_policy = 1;
    ec_project_state_.screenGeneral.flag6_col = -1;
    ec_project_state_.screenGeneral.flag6_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag6_policy = 1;
    ec_project_state_.screenGeneral.flag7_col = -1;
    ec_project_state_.screenGeneral.flag7_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag7_policy = 1;
    ec_project_state_.screenGeneral.flag8_col = -1;
    ec_project_state_.screenGeneral.flag8_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag8_policy = 1;
    ec_project_state_.screenGeneral.flag9_col = -1;
    ec_project_state_.screenGeneral.flag9_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag9_policy = 1;
    ec_project_state_.screenGeneral.flag10_col = -1;
    ec_project_state_.screenGeneral.flag10_threshold = -9999.0;
    ec_project_state_.screenGeneral.flag10_policy = 1;

    // preproc setting section
    ec_project_state_.screenSetting.nfiles = 1;
    ec_project_state_.screenSetting.avrg_len = 30;
    ec_project_state_.screenSetting.max_lack = 10;
    ec_project_state_.screenSetting.u_offset = 0.0;
    ec_project_state_.screenSetting.v_offset = 0.0;
    ec_project_state_.screenSetting.w_offset = 0.0;
    ec_project_state_.screenSetting.cross_wind = 0;
    ec_project_state_.screenSetting.flow_distortion = 0;
    ec_project_state_.screenSetting.rot_meth = 1;
    ec_project_state_.screenSetting.detrend_meth = 0;
    ec_project_state_.screenSetting.timeconst = 250.0;
    ec_project_state_.screenSetting.tlag_meth = 2;
    ec_project_state_.screenSetting.tap_win = 3;
    ec_project_state_.screenSetting.nbins = 50;
    ec_project_state_.screenSetting.out_bin_sp = 1;
    ec_project_state_.screenSetting.out_bin_og = 0;
    ec_project_state_.screenSetting.out_full_sp_u = 0;
    ec_project_state_.screenSetting.out_full_sp_v = 0;
    ec_project_state_.screenSetting.out_full_sp_w = 0;
    ec_project_state_.screenSetting.out_full_sp_ts = 0;
    ec_project_state_.screenSetting.out_full_sp_co2 = 0;
    ec_project_state_.screenSetting.out_full_sp_h2o = 0;
    ec_project_state_.screenSetting.out_full_sp_ch4 = 0;
    ec_project_state_.screenSetting.out_full_sp_n2o = 0;
    ec_project_state_.screenSetting.out_st_1 = 1;
    ec_project_state_.screenSetting.out_st_2 = 0;
    ec_project_state_.screenSetting.out_st_3 = 0;
    ec_project_state_.screenSetting.out_st_4 = 0;
    ec_project_state_.screenSetting.out_st_5 = 0;
    ec_project_state_.screenSetting.out_st_6 = 0;
    ec_project_state_.screenSetting.out_st_7 = 0;
    ec_project_state_.screenSetting.out_raw_1 = 0;
    ec_project_state_.screenSetting.out_raw_2 = 0;
    ec_project_state_.screenSetting.out_raw_3 = 0;
    ec_project_state_.screenSetting.out_raw_4 = 0;
    ec_project_state_.screenSetting.out_raw_5 = 0;
    ec_project_state_.screenSetting.out_raw_6 = 0;
    ec_project_state_.screenSetting.out_raw_7 = 0;
    ec_project_state_.screenSetting.out_raw_u = 0;
    ec_project_state_.screenSetting.out_raw_v = 0;
    ec_project_state_.screenSetting.out_raw_w = 0;
    ec_project_state_.screenSetting.out_raw_ts = 0;
    ec_project_state_.screenSetting.out_raw_co2 = 0;
    ec_project_state_.screenSetting.out_raw_h2o = 0;
    ec_project_state_.screenSetting.out_raw_ch4 = 0;
    ec_project_state_.screenSetting.out_raw_gas4 = 0;
    ec_project_state_.screenSetting.out_raw_tair = 0;
    ec_project_state_.screenSetting.out_raw_pair = 0;
    ec_project_state_.screenSetting.out_full_cosp_u = 0;
    ec_project_state_.screenSetting.out_full_cosp_v = 0;
    ec_project_state_.screenSetting.out_full_cosp_ts = 1;
    ec_project_state_.screenSetting.out_full_cosp_co2 = 0;
    ec_project_state_.screenSetting.out_full_cosp_h2o = 0;
    ec_project_state_.screenSetting.out_full_cosp_ch4 = 0;
    ec_project_state_.screenSetting.out_full_cosp_n2o = 0;
    ec_project_state_.screenSetting.to_mixratio = 1;
    ec_project_state_.screenSetting.filter_sr = 1;
    ec_project_state_.screenSetting.filter_al = 1;
    ec_project_state_.screenSetting.bu_corr = 0;
    ec_project_state_.screenSetting.bu_multi = 0;
    ec_project_state_.screenSetting.l_day_bot_gain = 0.944;
    ec_project_state_.screenSetting.l_day_bot_offset = 2.57;
    ec_project_state_.screenSetting.l_day_top_gain = 1.005;
    ec_project_state_.screenSetting.l_day_top_offset = 0.24;
    ec_project_state_.screenSetting.l_day_spar_gain = 1.010;
    ec_project_state_.screenSetting.l_day_spar_offset = 0.36;
    ec_project_state_.screenSetting.l_night_bot_gain = 0.883;
    ec_project_state_.screenSetting.l_night_bot_offset = 2.17;
    ec_project_state_.screenSetting.l_night_top_gain = 1.008;
    ec_project_state_.screenSetting.l_night_top_offset = -0.41;
    ec_project_state_.screenSetting.l_night_spar_gain = 1.010;
    ec_project_state_.screenSetting.l_night_spar_offset = -0.17;
    ec_project_state_.screenSetting.m_day_bot1 = 2.8;
    ec_project_state_.screenSetting.m_day_bot2 = -0.0681;
    ec_project_state_.screenSetting.m_day_bot3 = 0.0021;
    ec_project_state_.screenSetting.m_day_bot4 = -0.334;
    ec_project_state_.screenSetting.m_day_top1 = -0.1;
    ec_project_state_.screenSetting.m_day_top2 = -0.0044;
    ec_project_state_.screenSetting.m_day_top3 = 0.0011;
    ec_project_state_.screenSetting.m_day_top4 = -0.022;
    ec_project_state_.screenSetting.m_day_spar1 = 0.3;
    ec_project_state_.screenSetting.m_day_spar2 = -0.0007;
    ec_project_state_.screenSetting.m_day_spar3 = 0.0006;
    ec_project_state_.screenSetting.m_day_spar4 = -0.044;
    ec_project_state_.screenSetting.m_night_bot1 = 0.5;
    ec_project_state_.screenSetting.m_night_bot2 = -0.1160;
    ec_project_state_.screenSetting.m_night_bot3 = 0.0087;
    ec_project_state_.screenSetting.m_night_bot4 = -0.206;
    ec_project_state_.screenSetting.m_night_top1 = -1.7;
    ec_project_state_.screenSetting.m_night_top2 = -0.1160;
    ec_project_state_.screenSetting.m_night_top3 = 0.0051;
    ec_project_state_.screenSetting.m_night_top4 = -0.029;
    ec_project_state_.screenSetting.m_night_spar1 = -2.1;
    ec_project_state_.screenSetting.m_night_spar2 = -0.0200;
    ec_project_state_.screenSetting.m_night_spar3 = 0.0070;
    ec_project_state_.screenSetting.m_night_spar4 = -0.026;
    ec_project_state_.screenSetting.out_details = 0;
    ec_project_state_.screenSetting.power_of_two = 1;

    // preproc test section
    ec_project_state_.screenTest.test_sr = 1;
    ec_project_state_.screenTest.test_ar = 1;
    ec_project_state_.screenTest.test_do = 1;
    ec_project_state_.screenTest.test_al = 1;
    ec_project_state_.screenTest.test_sk = 1;
    ec_project_state_.screenTest.test_ds = 0;
    ec_project_state_.screenTest.test_tl = 0;
    ec_project_state_.screenTest.test_aa = 0;
    ec_project_state_.screenTest.test_ns = 0;

    // preproc parameters section
    ec_project_state_.screenParam.aa_lim = 10.0;
    ec_project_state_.screenParam.aa_max = 30.0;
    ec_project_state_.screenParam.aa_min = -30.0;
    ec_project_state_.screenParam.al_co2_min = 200.0;
    ec_project_state_.screenParam.al_co2_max = 900.0;
    ec_project_state_.screenParam.al_h2o_min = 0.0;
    ec_project_state_.screenParam.al_h2o_max = 40.0;
    ec_project_state_.screenParam.al_ch4_min = 0.17;
    ec_project_state_.screenParam.al_ch4_max = 1000.0;
    ec_project_state_.screenParam.al_n2o_min = 0.032;
    ec_project_state_.screenParam.al_n2o_max = 1000.0;
    ec_project_state_.screenParam.al_tson_min = -40.0;
    ec_project_state_.screenParam.al_tson_max = 50.0;
    ec_project_state_.screenParam.al_u_max = 30.0;
    ec_project_state_.screenParam.al_w_max = 5.0;
    ec_project_state_.screenParam.ar_bins = 100;
    ec_project_state_.screenParam.ar_hf_lim = 70;
    ec_project_state_.screenParam.ar_lim = 7.0;
    ec_project_state_.screenParam.ds_hf_uv = 4.0;
    ec_project_state_.screenParam.ds_hf_w = 2.0;
    ec_project_state_.screenParam.ds_hf_t = 4.0;
    ec_project_state_.screenParam.ds_hf_co2 = 40.0;
    ec_project_state_.screenParam.ds_hf_h2o = 3.26;
    ec_project_state_.screenParam.ds_hf_ch4 = 40.0;
    ec_project_state_.screenParam.ds_hf_n2o = 40.0;
    ec_project_state_.screenParam.ds_hf_var = 3.0;
    ec_project_state_.screenParam.ds_sf_uv = 2.7;
    ec_project_state_.screenParam.ds_sf_w = 1.3;
    ec_project_state_.screenParam.ds_sf_t = 2.7;
    ec_project_state_.screenParam.ds_sf_co2 = 27.0;
    ec_project_state_.screenParam.ds_sf_h2o = 2.2;
    ec_project_state_.screenParam.ds_sf_ch4 = 30.0;
    ec_project_state_.screenParam.ds_sf_n2o = 30.0;
    ec_project_state_.screenParam.ds_sf_var = 2.0;
    ec_project_state_.screenParam.do_extlim_dw = 10;
    ec_project_state_.screenParam.do_hf1_lim = 10.0;
    ec_project_state_.screenParam.do_hf2_lim = 6.0;
    ec_project_state_.screenParam.ns_hf_lim = 0.5;
    ec_project_state_.screenParam.sk_hf_kumax = 8.0;
    ec_project_state_.screenParam.sk_hf_kumin = 1.0;
    ec_project_state_.screenParam.sk_hf_skmax = 2.0;
    ec_project_state_.screenParam.sk_hf_skmin = -2.0;
    ec_project_state_.screenParam.sk_sf_kumax = 5.0;
    ec_project_state_.screenParam.sk_sf_kumin = 2.0;
    ec_project_state_.screenParam.sk_sf_skmax = 1.0;
    ec_project_state_.screenParam.sk_sf_skmin = -1.0;
    ec_project_state_.screenParam.sr_num_spk = 3;
    ec_project_state_.screenParam.sr_lim_u = 3.5;
    ec_project_state_.screenParam.sr_lim_w = 5.0;
    ec_project_state_.screenParam.sr_lim_co2 = 3.5;
    ec_project_state_.screenParam.sr_lim_h2o = 3.5;
    ec_project_state_.screenParam.sr_lim_ch4 = 8.0;
    ec_project_state_.screenParam.sr_lim_n2o = 8.0;
    ec_project_state_.screenParam.sr_lim_hf = 1.0;
    ec_project_state_.screenParam.tl_hf_lim = 20.0;
    ec_project_state_.screenParam.tl_def_co2 = 3.5;
    ec_project_state_.screenParam.tl_def_h2o = 2.5;
    ec_project_state_.screenParam.tl_def_ch4 = 3.5;
    ec_project_state_.screenParam.tl_def_n2o = 2.5;
    ec_project_state_.screenParam.tl_sf_lim = 10.0;

    ec_project_state_.spectraSettings.start_sa_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.spectraSettings.end_sa_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.spectraSettings.sa_mode = 1;
    ec_project_state_.spectraSettings.sa_file.clear();
    ec_project_state_.spectraSettings.horst_lens = 2;
    ec_project_state_.spectraSettings.sa_min_smpl = 10;
    ec_project_state_.spectraSettings.sa_fmin_co2 = 0.0006;
    ec_project_state_.spectraSettings.sa_fmin_h2o = 0.0006;
    ec_project_state_.spectraSettings.sa_fmin_ch4 = 0.0006;
    ec_project_state_.spectraSettings.sa_fmin_gas4 = 0.0006;
    ec_project_state_.spectraSettings.sa_fmax_co2 = 5.0;
    ec_project_state_.spectraSettings.sa_fmax_h2o = 5.0;
    ec_project_state_.spectraSettings.sa_fmax_ch4 = 5.0;
    ec_project_state_.spectraSettings.sa_fmax_gas4 = 5.0;
    ec_project_state_.spectraSettings.sa_hfn_co2_fmin = 5.0;
    ec_project_state_.spectraSettings.sa_hfn_h2o_fmin = 5.0;
    ec_project_state_.spectraSettings.sa_hfn_ch4_fmin = 5.0;
    ec_project_state_.spectraSettings.sa_hfn_gas4_fmin = 5.0;
    ec_project_state_.spectraSettings.add_sonic_lptf = 1;
    ec_project_state_.spectraSettings.sa_min_co2 = 2.0;
    ec_project_state_.spectraSettings.sa_min_ch4 = 0.2;
    ec_project_state_.spectraSettings.sa_min_gas4 = 0.02;
    ec_project_state_.spectraSettings.sa_min_le = 20.0;
    ec_project_state_.spectraSettings.sa_min_h = 20.0;
    ec_project_state_.spectraSettings.f10_co2_trshld = 2.0;
    ec_project_state_.spectraSettings.f10_ch4_trshld = 0.2;
    ec_project_state_.spectraSettings.f10_gas4_trshld = 0.02;
    ec_project_state_.spectraSettings.f10_le_trshld = 10.0;
    ec_project_state_.spectraSettings.f10_h_trshld = 10.0;
    ec_project_state_.spectraSettings.ex_file.clear();
    ec_project_state_.spectraSettings.sa_bin_spectra.clear();
    ec_project_state_.spectraSettings.sa_full_spectra.clear();
    ec_project_state_.spectraSettings.ex_dir.clear();
    ec_project_state_.spectraSettings.subset = 1;

    ec_project_state_.screenTilt.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.screenTilt.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.screenTilt.mode = 1;
    ec_project_state_.screenTilt.north_offset = 0.0;
    ec_project_state_.screenTilt.min_num_per_sec = 0;
    ec_project_state_.screenTilt.w_max = 0.099;
    ec_project_state_.screenTilt.u_min = -0.001;
    ec_project_state_.screenTilt.file.clear();
    ec_project_state_.screenTilt.fix_policy = 0;
    ec_project_state_.screenTilt.angles.clear();
    ec_project_state_.screenTilt.subset = 0;

    ec_project_state_.timelagOpt.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.timelagOpt.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.timelagOpt.mode = 1;
    ec_project_state_.timelagOpt.file.clear();
    ec_project_state_.timelagOpt.pg_range = 1.5;
    ec_project_state_.timelagOpt.le_min_flux = 20.0;
    ec_project_state_.timelagOpt.to_h2o_nclass = 10;
    ec_project_state_.timelagOpt.co2_min_flux = 2.0;
    ec_project_state_.timelagOpt.ch4_min_flux = 0.2;
    ec_project_state_.timelagOpt.gas4_min_flux = 0.02;
    ec_project_state_.timelagOpt.co2_min_lag = -1000.1;
    ec_project_state_.timelagOpt.co2_max_lag = -1000.1;
    ec_project_state_.timelagOpt.h2o_min_lag = -1000.1;
    ec_project_state_.timelagOpt.h2o_max_lag = -1000.1;
    ec_project_state_.timelagOpt.ch4_min_lag = -1000.1;
    ec_project_state_.timelagOpt.ch4_max_lag = -1000.1;
    ec_project_state_.timelagOpt.gas4_min_lag = -1000.1;
    ec_project_state_.timelagOpt.gas4_max_lag = -1000.1;
    ec_project_state_.timelagOpt.subset  = 0;

    ec_project_state_.randomError.method = 0;
    ec_project_state_.randomError.its_method = 1;
    ec_project_state_.randomError.its_tlag_max = 10.0;
    ec_project_state_.randomError.its_sec_factor = 20.0;

    ec_project_state_.biomParam.native_header = 1;
    ec_project_state_.biomParam.hlines = 2;
    ec_project_state_.biomParam.separator = QStringLiteral("comma");
    ec_project_state_.biomParam.tstamp_ref = QStringLiteral("end");
    ec_project_state_.biomParam.col_ta = 999;
    ec_project_state_.biomParam.col_pa = 999;
    ec_project_state_.biomParam.col_rh = -1;
    ec_project_state_.biomParam.col_ppfd = -1;
    ec_project_state_.biomParam.col_rg = -1;
    ec_project_state_.biomParam.col_lwin = -1;

    setModified(false); // new documents are not in a modified state
    emit ecProjectNew();
}

// Save an ec project
bool EcProject::saveEcProject(const QString &filename)
{
    DEBUG_FUNC_NAME

    // try to open file
    QFile datafile(filename);
    if (!datafile.open(QFile::WriteOnly | QFile::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open file" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Write Project Error"),
                             tr("Cannot write file %1:\n%2")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString(Qt::ISODate);

    QFileInfo fileinfo = QFileInfo(filename);

    QSettings project_ini(filename, QSettings::IniFormat);

    // general section
    project_ini.beginGroup(EcIni::INIGROUP_PROJECT);
        project_ini.setValue(EcIni::INI_PROJECT_0, ec_project_state_.projectGeneral.creation_date);
        project_ini.setValue(EcIni::INI_PROJECT_1, now_str);
        project_ini.setValue(EcIni::INI_PROJECT_2, fileinfo.absoluteFilePath());
        project_ini.setValue(EcIni::INI_PROJECT_33, QVariant::fromValue(ec_project_state_.projectGeneral.run_mode).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_40, QVariant(ec_project_state_.projectGeneral.run_fcc).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_3, ec_project_state_.projectGeneral.project_title);

        // update sw version if empty or old
        if (ec_project_state_.projectGeneral.sw_version.isEmpty()
            || ec_project_state_.projectGeneral.sw_version != Defs::APP_VERSION_STR)
        {
            project_ini.setValue(EcIni::INI_PROJECT_4, Defs::APP_VERSION_STR);
        }
        else
        {
            project_ini.setValue(EcIni::INI_PROJECT_4, ec_project_state_.projectGeneral.sw_version);
        }

        // update ini version if empty or old
        if (ec_project_state_.projectGeneral.ini_version.isEmpty()
            || ec_project_state_.projectGeneral.ini_version != Defs::PROJECT_FILE_VERSION_STR)
        {
            project_ini.setValue(EcIni::INI_PROJECT_5, Defs::PROJECT_FILE_VERSION_STR);
        }
        else
        {
            project_ini.setValue(EcIni::INI_PROJECT_5, ec_project_state_.projectGeneral.ini_version);
        }
        project_ini.setValue(EcIni::INI_PROJECT_6, ec_project_state_.projectGeneral.project_id);
        project_ini.setValue(EcIni::INI_PROJECT_7, QVariant::fromValue(ec_project_state_.projectGeneral.file_type).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_8, ec_project_state_.projectGeneral.file_prototype);
        project_ini.setValue(EcIni::INI_PROJECT_9, QVariant(ec_project_state_.projectGeneral.use_alt_md_file).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_10, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.md_file));
        project_ini.setValue(EcIni::INI_PROJECT_11, QVariant(ec_project_state_.projectGeneral.use_tlfile).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_12, ec_project_state_.projectGeneral.timeline_file);
        project_ini.setValue(EcIni::INI_PROJECT_13, ec_project_state_.projectGeneral.binary_hnlines);
        project_ini.setValue(EcIni::INI_PROJECT_14, ec_project_state_.projectGeneral.binary_eol);
        project_ini.setValue(EcIni::INI_PROJECT_15, ec_project_state_.projectGeneral.binary_nbytes);
        project_ini.setValue(EcIni::INI_PROJECT_16, ec_project_state_.projectGeneral.binary_little_end);
        project_ini.setValue(EcIni::INI_PROJECT_17, ec_project_state_.projectGeneral.master_sonic);
        project_ini.setValue(EcIni::INI_PROJECT_18, ec_project_state_.projectGeneral.col_co2);
        project_ini.setValue(EcIni::INI_PROJECT_19, ec_project_state_.projectGeneral.col_h2o);
        project_ini.setValue(EcIni::INI_PROJECT_20, ec_project_state_.projectGeneral.col_ch4);
        project_ini.setValue(EcIni::INI_PROJECT_21, ec_project_state_.projectGeneral.col_n2o);
        project_ini.setValue(EcIni::INI_PROJECT_22, ec_project_state_.projectGeneral.col_int_t_1);
        project_ini.setValue(EcIni::INI_PROJECT_23, ec_project_state_.projectGeneral.col_int_t_2);
        project_ini.setValue(EcIni::INI_PROJECT_24, ec_project_state_.projectGeneral.col_int_p);
        project_ini.setValue(EcIni::INI_PROJECT_25, ec_project_state_.projectGeneral.col_air_t);
        project_ini.setValue(EcIni::INI_PROJECT_26, ec_project_state_.projectGeneral.col_air_p);
        project_ini.setValue(EcIni::INI_PROJECT_27, ec_project_state_.projectGeneral.col_int_t_c);
        project_ini.setValue(EcIni::INI_PROJECT_28, ec_project_state_.projectGeneral.col_diag_75);
        project_ini.setValue(EcIni::INI_PROJECT_29, ec_project_state_.projectGeneral.col_diag_72);
        project_ini.setValue(EcIni::INI_PROJECT_30, ec_project_state_.projectGeneral.col_diag_77);
        project_ini.setValue(EcIni::INI_PROJECT_31, QString::number(ec_project_state_.projectGeneral.gas_mw, 'f', 4));
        project_ini.setValue(EcIni::INI_PROJECT_32, QString::number(ec_project_state_.projectGeneral.gas_diff, 'f', 5));
        project_ini.setValue(EcIni::INI_PROJECT_36, ec_project_state_.projectGeneral.col_ts);
        project_ini.setValue(EcIni::INI_PROJECT_37, ec_project_state_.projectGeneral.out_ghg_eu);
        project_ini.setValue(EcIni::INI_PROJECT_38, ec_project_state_.projectGeneral.out_amflux);
        project_ini.setValue(EcIni::INI_PROJECT_39, ec_project_state_.projectGeneral.out_rich);
        project_ini.setValue(EcIni::INI_PROJECT_56, ec_project_state_.projectGeneral.out_md);
        project_ini.setValue(EcIni::INI_PROJECT_61, ec_project_state_.projectGeneral.out_biomet);
        project_ini.setValue(EcIni::INI_PROJECT_41, QVariant(ec_project_state_.projectGeneral.make_dataset).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_54, ec_project_state_.projectGeneral.subset);
        project_ini.setValue(EcIni::INI_PROJECT_42, ec_project_state_.projectGeneral.start_date);
        project_ini.setValue(EcIni::INI_PROJECT_44, ec_project_state_.projectGeneral.start_time);
        project_ini.setValue(EcIni::INI_PROJECT_43, ec_project_state_.projectGeneral.end_date);
        project_ini.setValue(EcIni::INI_PROJECT_45, ec_project_state_.projectGeneral.end_time);
        project_ini.setValue(EcIni::INI_PROJECT_46, ec_project_state_.projectGeneral.hf_meth);
        project_ini.setValue(EcIni::INI_PROJECT_47, ec_project_state_.projectGeneral.lf_meth);
        project_ini.setValue(EcIni::INI_PROJECT_48, ec_project_state_.projectGeneral.wpl_meth);
        project_ini.setValue(EcIni::INI_PROJECT_49, ec_project_state_.projectGeneral.foot_meth);
        project_ini.setValue(EcIni::INI_PROJECT_50, ec_project_state_.projectGeneral.tob1_format);
        project_ini.setValue(EcIni::INI_PROJECT_51, ec_project_state_.projectGeneral.out_path);
        project_ini.setValue(EcIni::INI_PROJECT_52, ec_project_state_.projectGeneral.fix_out_format);
        project_ini.setValue(EcIni::INI_PROJECT_53, ec_project_state_.projectGeneral.err_label);
        project_ini.setValue(EcIni::INI_PROJECT_55, ec_project_state_.projectGeneral.qcflag_meth);
        project_ini.setValue(EcIni::INI_PROJECT_34, QVariant(ec_project_state_.projectGeneral.use_biomet).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_35, ec_project_state_.projectGeneral.biom_file);
        project_ini.setValue(EcIni::INI_PROJECT_57, ec_project_state_.projectGeneral.biom_dir);
        project_ini.setValue(EcIni::INI_PROJECT_58, ec_project_state_.projectGeneral.biom_recurse);
        project_ini.setValue(EcIni::INI_PROJECT_59, QVariant(QString(QLatin1Char('.')) + ec_project_state_.projectGeneral.biom_ext));
        project_ini.setValue(EcIni::INI_PROJECT_60, ec_project_state_.projectGeneral.out_mean_cosp);
        project_ini.setValue(EcIni::INI_PROJECT_62, ec_project_state_.projectGeneral.bin_sp_avail);
        project_ini.setValue(EcIni::INI_PROJECT_63, ec_project_state_.projectGeneral.full_sp_avail);
        project_ini.setValue(EcIni::INI_PROJECT_64, ec_project_state_.projectGeneral.files_found);
    project_ini.endGroup();

    // spec settings section
    project_ini.beginGroup(EcIni::INIGROUP_SPEC_SETTINGS);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_0, ec_project_state_.spectraSettings.start_sa_date);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_1, ec_project_state_.spectraSettings.end_sa_date);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_2, ec_project_state_.spectraSettings.sa_mode);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_3, ec_project_state_.spectraSettings.sa_file);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_4, ec_project_state_.spectraSettings.sa_min_smpl);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_5, ec_project_state_.spectraSettings.sa_fmin_co2);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_6, ec_project_state_.spectraSettings.sa_fmin_h2o);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_7, ec_project_state_.spectraSettings.sa_fmin_ch4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_8, ec_project_state_.spectraSettings.sa_fmin_gas4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_9, ec_project_state_.spectraSettings.sa_fmax_co2);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_10, ec_project_state_.spectraSettings.sa_fmax_h2o);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_11, ec_project_state_.spectraSettings.sa_fmax_ch4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_12, ec_project_state_.spectraSettings.sa_fmax_gas4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_13, ec_project_state_.spectraSettings.sa_hfn_co2_fmin);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_14, ec_project_state_.spectraSettings.sa_hfn_h2o_fmin);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_15, ec_project_state_.spectraSettings.sa_hfn_ch4_fmin);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_16, ec_project_state_.spectraSettings.sa_hfn_gas4_fmin);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_17, ec_project_state_.spectraSettings.sa_min_co2);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_18, ec_project_state_.spectraSettings.sa_min_ch4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_19, ec_project_state_.spectraSettings.sa_min_gas4);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_20, ec_project_state_.spectraSettings.sa_min_le);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_21, ec_project_state_.spectraSettings.sa_min_h);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_22, ec_project_state_.spectraSettings.add_sonic_lptf);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_23, ec_project_state_.spectraSettings.f10_co2_trshld);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_24, ec_project_state_.spectraSettings.f10_ch4_trshld);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_25, ec_project_state_.spectraSettings.f10_gas4_trshld);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_26, ec_project_state_.spectraSettings.f10_le_trshld);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_27, ec_project_state_.spectraSettings.f10_h_trshld);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_28, ec_project_state_.spectraSettings.horst_lens);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_29, ec_project_state_.spectraSettings.ex_file);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_30, ec_project_state_.spectraSettings.sa_bin_spectra);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_31, ec_project_state_.spectraSettings.sa_full_spectra);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_32, ec_project_state_.spectraSettings.ex_dir);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_33, ec_project_state_.spectraSettings.subset);

        // NOTE: temporary placeholders for SA Groups
        project_ini.setValue(QString(QStringLiteral("sa_co2_g1_start")), 1);
        project_ini.setValue(QString(QStringLiteral("sa_co2_g1_stop")), 12);
        project_ini.setValue(QString(QStringLiteral("sa_ch4_g1_start")), 1);
        project_ini.setValue(QString(QStringLiteral("sa_ch4_g1_stop")), 12);
        project_ini.setValue(QString(QStringLiteral("sa_gas4_g1_start")), 1);
        project_ini.setValue(QString(QStringLiteral("sa_gas4_g1_stop")), 12);

    project_ini.endGroup();

    // screen general section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_GENERAL);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_4, ec_project_state_.screenGeneral.data_path);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_6, ec_project_state_.screenGeneral.recurse);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_40, QVariant(ec_project_state_.screenGeneral.use_geo_north).toInt());
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_38, ec_project_state_.screenGeneral.mag_dec);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_39, ec_project_state_.screenGeneral.dec_date);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_8, ec_project_state_.screenGeneral.flag1_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_9, QString::number(ec_project_state_.screenGeneral.flag1_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_10, ec_project_state_.screenGeneral.flag1_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_11, ec_project_state_.screenGeneral.flag2_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_12, QString::number(ec_project_state_.screenGeneral.flag2_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_13, ec_project_state_.screenGeneral.flag2_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_14, ec_project_state_.screenGeneral.flag3_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_15, QString::number(ec_project_state_.screenGeneral.flag3_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_16, ec_project_state_.screenGeneral.flag3_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_17, ec_project_state_.screenGeneral.flag4_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_18, QString::number(ec_project_state_.screenGeneral.flag4_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_19, ec_project_state_.screenGeneral.flag4_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_20, ec_project_state_.screenGeneral.flag5_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_21, QString::number(ec_project_state_.screenGeneral.flag5_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_22, ec_project_state_.screenGeneral.flag5_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_23, ec_project_state_.screenGeneral.flag6_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_24, QString::number(ec_project_state_.screenGeneral.flag6_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_25, ec_project_state_.screenGeneral.flag6_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_26, ec_project_state_.screenGeneral.flag7_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_27, QString::number(ec_project_state_.screenGeneral.flag7_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_28, ec_project_state_.screenGeneral.flag7_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_29, ec_project_state_.screenGeneral.flag8_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_30, QString::number(ec_project_state_.screenGeneral.flag8_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_31, ec_project_state_.screenGeneral.flag8_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_32, ec_project_state_.screenGeneral.flag9_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_33, QString::number(ec_project_state_.screenGeneral.flag9_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_34, ec_project_state_.screenGeneral.flag9_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_35, ec_project_state_.screenGeneral.flag10_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_36, QString::number(ec_project_state_.screenGeneral.flag10_threshold, 'f', 4));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_37, ec_project_state_.screenGeneral.flag10_policy);
        project_ini.endGroup();

    // screen settings section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_SETTINGS);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_0, ec_project_state_.screenSetting.nfiles);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_1, ec_project_state_.screenSetting.max_lack);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_12, ec_project_state_.screenSetting.u_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_13, ec_project_state_.screenSetting.v_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_14, ec_project_state_.screenSetting.w_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_2, ec_project_state_.screenSetting.cross_wind);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_3, ec_project_state_.screenSetting.flow_distortion);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_4, ec_project_state_.screenSetting.rot_meth);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_5, ec_project_state_.screenSetting.detrend_meth);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_6, QString::number(ec_project_state_.screenSetting.timeconst, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_7, ec_project_state_.screenSetting.tlag_meth);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_8, ec_project_state_.screenSetting.tap_win);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_9, ec_project_state_.screenSetting.nbins);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_10, ec_project_state_.screenSetting.avrg_len);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_15, ec_project_state_.screenSetting.out_bin_sp);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_38, ec_project_state_.screenSetting.out_bin_og);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_16, ec_project_state_.screenSetting.out_full_sp_u);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_17, ec_project_state_.screenSetting.out_full_sp_v);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_18, ec_project_state_.screenSetting.out_full_sp_w);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_19, ec_project_state_.screenSetting.out_full_sp_ts);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_20, ec_project_state_.screenSetting.out_full_sp_co2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_21, ec_project_state_.screenSetting.out_full_sp_h2o);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_22, ec_project_state_.screenSetting.out_full_sp_ch4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_23, ec_project_state_.screenSetting.out_full_sp_n2o);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_24, ec_project_state_.screenSetting.out_st_1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_25, ec_project_state_.screenSetting.out_st_2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_26, ec_project_state_.screenSetting.out_st_3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_27, ec_project_state_.screenSetting.out_st_4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_28, ec_project_state_.screenSetting.out_st_5);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_29, ec_project_state_.screenSetting.out_st_6);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_30, ec_project_state_.screenSetting.out_st_7);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_82, ec_project_state_.screenSetting.out_raw_1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_83, ec_project_state_.screenSetting.out_raw_2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_84, ec_project_state_.screenSetting.out_raw_3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_85, ec_project_state_.screenSetting.out_raw_4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_86, ec_project_state_.screenSetting.out_raw_5);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_87, ec_project_state_.screenSetting.out_raw_6);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_88, ec_project_state_.screenSetting.out_raw_7);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_89, ec_project_state_.screenSetting.out_raw_u);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_90, ec_project_state_.screenSetting.out_raw_v);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_91, ec_project_state_.screenSetting.out_raw_w);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_92, ec_project_state_.screenSetting.out_raw_ts);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_93, ec_project_state_.screenSetting.out_raw_co2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_94, ec_project_state_.screenSetting.out_raw_h2o);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_95, ec_project_state_.screenSetting.out_raw_ch4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_96, ec_project_state_.screenSetting.out_raw_gas4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_97, ec_project_state_.screenSetting.out_raw_tair);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_98, ec_project_state_.screenSetting.out_raw_pair);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_31, ec_project_state_.screenSetting.out_full_cosp_u);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_32, ec_project_state_.screenSetting.out_full_cosp_v);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_33, ec_project_state_.screenSetting.out_full_cosp_ts);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_34, ec_project_state_.screenSetting.out_full_cosp_co2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_35, ec_project_state_.screenSetting.out_full_cosp_h2o);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_36, ec_project_state_.screenSetting.out_full_cosp_ch4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_37, ec_project_state_.screenSetting.out_full_cosp_n2o);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_39, ec_project_state_.screenSetting.to_mixratio);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_40, ec_project_state_.screenSetting.filter_sr);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_41, ec_project_state_.screenSetting.filter_al);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_44, ec_project_state_.screenSetting.bu_corr);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_45, ec_project_state_.screenSetting.bu_multi);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_46, ec_project_state_.screenSetting.l_day_bot_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_47, ec_project_state_.screenSetting.l_day_bot_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_48, ec_project_state_.screenSetting.l_day_top_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_49, ec_project_state_.screenSetting.l_day_top_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_50, ec_project_state_.screenSetting.l_day_spar_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_51, ec_project_state_.screenSetting.l_day_spar_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_52, ec_project_state_.screenSetting.l_night_bot_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_53, ec_project_state_.screenSetting.l_night_bot_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_54, ec_project_state_.screenSetting.l_night_top_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_55, ec_project_state_.screenSetting.l_night_top_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_56, ec_project_state_.screenSetting.l_night_spar_gain);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_57, ec_project_state_.screenSetting.l_night_spar_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_58, ec_project_state_.screenSetting.m_day_bot1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_59, ec_project_state_.screenSetting.m_day_bot2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_60, ec_project_state_.screenSetting.m_day_bot3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_61, ec_project_state_.screenSetting.m_day_bot4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_62, ec_project_state_.screenSetting.m_day_top1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_63, ec_project_state_.screenSetting.m_day_top2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_64, ec_project_state_.screenSetting.m_day_top3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_65, ec_project_state_.screenSetting.m_day_top4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_66, ec_project_state_.screenSetting.m_day_spar1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_67, ec_project_state_.screenSetting.m_day_spar2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_68, ec_project_state_.screenSetting.m_day_spar3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_69, ec_project_state_.screenSetting.m_day_spar4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_70, ec_project_state_.screenSetting.m_night_bot1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_71, ec_project_state_.screenSetting.m_night_bot2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_72, ec_project_state_.screenSetting.m_night_bot3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_73, ec_project_state_.screenSetting.m_night_bot4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_74, ec_project_state_.screenSetting.m_night_top1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_75, ec_project_state_.screenSetting.m_night_top2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_76, ec_project_state_.screenSetting.m_night_top3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_77, ec_project_state_.screenSetting.m_night_top4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_78, ec_project_state_.screenSetting.m_night_spar1);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_79, ec_project_state_.screenSetting.m_night_spar2);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_80, ec_project_state_.screenSetting.m_night_spar3);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_81, ec_project_state_.screenSetting.m_night_spar4);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_99, ec_project_state_.screenSetting.out_details);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_101, ec_project_state_.screenSetting.power_of_two);
    project_ini.endGroup();

    // screen test section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TESTS);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_0, ec_project_state_.screenTest.test_sr);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_1, ec_project_state_.screenTest.test_ar);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_2, ec_project_state_.screenTest.test_do);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_3, ec_project_state_.screenTest.test_al);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_4, ec_project_state_.screenTest.test_sk);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_5, ec_project_state_.screenTest.test_ds);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_6, ec_project_state_.screenTest.test_tl);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_7, ec_project_state_.screenTest.test_aa);
        project_ini.setValue(EcIni::INI_SCREEN_TESTS_8, ec_project_state_.screenTest.test_ns);
    project_ini.endGroup();

    // screen param section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_PARAM);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_0 , ec_project_state_.screenParam.sr_num_spk);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_1 , QString::number(ec_project_state_.screenParam.sr_lim_u, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_45 , QString::number(ec_project_state_.screenParam.sr_lim_w, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_46 , QString::number(ec_project_state_.screenParam.sr_lim_co2, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_47 , QString::number(ec_project_state_.screenParam.sr_lim_h2o, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_48 , QString::number(ec_project_state_.screenParam.sr_lim_ch4, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_49 , QString::number(ec_project_state_.screenParam.sr_lim_n2o, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_2 , QString::number(ec_project_state_.screenParam.sr_lim_hf, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_3 , QString::number(ec_project_state_.screenParam.ar_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_4 , ec_project_state_.screenParam.ar_bins);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_5 , ec_project_state_.screenParam.ar_hf_lim);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_6 , ec_project_state_.screenParam.do_extlim_dw);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_7 , QString::number(ec_project_state_.screenParam.do_hf1_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_8 , QString::number(ec_project_state_.screenParam.do_hf2_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_9 , QString::number(ec_project_state_.screenParam.al_u_max, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_10, QString::number(ec_project_state_.screenParam.al_w_max, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_11, QString::number(ec_project_state_.screenParam.al_tson_min, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_12, QString::number(ec_project_state_.screenParam.al_tson_max, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_13, QString::number(ec_project_state_.screenParam.al_co2_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_14, QString::number(ec_project_state_.screenParam.al_co2_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_15, QString::number(ec_project_state_.screenParam.al_h2o_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_16, QString::number(ec_project_state_.screenParam.al_h2o_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_54, QString::number(ec_project_state_.screenParam.al_ch4_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_55, QString::number(ec_project_state_.screenParam.al_ch4_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_56, QString::number(ec_project_state_.screenParam.al_n2o_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_57, QString::number(ec_project_state_.screenParam.al_n2o_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_17, QString::number(ec_project_state_.screenParam.sk_hf_skmin, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_18, QString::number(ec_project_state_.screenParam.sk_hf_skmax, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_19, QString::number(ec_project_state_.screenParam.sk_sf_skmin, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_20, QString::number(ec_project_state_.screenParam.sk_sf_skmax, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_21, QString::number(ec_project_state_.screenParam.sk_hf_kumin, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_22, QString::number(ec_project_state_.screenParam.sk_hf_kumax, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_23, QString::number(ec_project_state_.screenParam.sk_sf_kumin, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_24, QString::number(ec_project_state_.screenParam.sk_sf_kumax, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_25, ec_project_state_.screenParam.ds_hf_uv);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_26, ec_project_state_.screenParam.ds_hf_w);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_27, ec_project_state_.screenParam.ds_hf_t);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_28, ec_project_state_.screenParam.ds_hf_co2);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_29, ec_project_state_.screenParam.ds_hf_h2o);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_50, ec_project_state_.screenParam.ds_hf_ch4);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_51, ec_project_state_.screenParam.ds_hf_n2o);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_30, ec_project_state_.screenParam.ds_hf_var);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_31, ec_project_state_.screenParam.ds_sf_uv);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_32, ec_project_state_.screenParam.ds_sf_w);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_33, ec_project_state_.screenParam.ds_sf_t);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_34, ec_project_state_.screenParam.ds_sf_co2);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_35, ec_project_state_.screenParam.ds_sf_h2o);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_52, ec_project_state_.screenParam.ds_sf_ch4);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_53, ec_project_state_.screenParam.ds_sf_n2o);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_36, ec_project_state_.screenParam.ds_sf_var);
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_37, QString::number(ec_project_state_.screenParam.tl_hf_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_38, QString::number(ec_project_state_.screenParam.tl_sf_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_39, QString::number(ec_project_state_.screenParam.tl_def_co2, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_40, QString::number(ec_project_state_.screenParam.tl_def_h2o, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_58, QString::number(ec_project_state_.screenParam.tl_def_ch4, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_59, QString::number(ec_project_state_.screenParam.tl_def_n2o, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_41, QString::number(ec_project_state_.screenParam.aa_min, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_42, QString::number(ec_project_state_.screenParam.aa_max, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_43, QString::number(ec_project_state_.screenParam.aa_lim, 'f', 1));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_44, QString::number(ec_project_state_.screenParam.ns_hf_lim, 'f', 1));
    project_ini.endGroup();

    // planar fit section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TILT);
        // firstly remove all previous keys because they are variable in number
        project_ini.remove(QString());

        project_ini.setValue(EcIni::INI_SCREEN_TILT_0, ec_project_state_.screenTilt.start_date);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_1, ec_project_state_.screenTilt.end_date);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_2, ec_project_state_.screenTilt.mode);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_3, ec_project_state_.screenTilt.north_offset);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_4, ec_project_state_.screenTilt.min_num_per_sec);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_5, QString::number(ec_project_state_.screenTilt.w_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_TILT_6, QString::number(ec_project_state_.screenTilt.u_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_TILT_7, ec_project_state_.screenTilt.file);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_8, ec_project_state_.screenTilt.fix_policy);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_11, ec_project_state_.screenTilt.subset);

        // iterate through angle list
        int k = 0;
        foreach (const AngleItem& angle, ec_project_state_.screenTilt.angles)
        {
            QString index = QStringLiteral("_") + QString::number(k + 1);
            QString prefix = StringUtils::insertIndex(EcIni::INI_SCREEN_TILT_PREFIX, 7, index);

            int excluded = angle.included_ ? 0 : 1;

            project_ini.setValue(prefix + EcIni::INI_SCREEN_TILT_9, angle.angle_);
            project_ini.setValue(prefix + EcIni::INI_SCREEN_TILT_10, excluded);

            ++k;
        }
    project_ini.endGroup();

    // timelag opt section
    project_ini.beginGroup(EcIni::INIGROUP_TIMELAG_OPT);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_0, ec_project_state_.timelagOpt.start_date);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_1, ec_project_state_.timelagOpt.end_date);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_2, ec_project_state_.timelagOpt.mode);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_3, ec_project_state_.timelagOpt.file);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_17, ec_project_state_.timelagOpt.to_h2o_nclass);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_4, QString::number(ec_project_state_.timelagOpt.co2_min_flux, 'f', 3));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_5, QString::number(ec_project_state_.timelagOpt.ch4_min_flux, 'f', 3));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_6, QString::number(ec_project_state_.timelagOpt.gas4_min_flux, 'f', 3));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_7, QString::number(ec_project_state_.timelagOpt.le_min_flux, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_8, QString::number(ec_project_state_.timelagOpt.pg_range, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_9, QString::number(ec_project_state_.timelagOpt.co2_min_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_10, QString::number(ec_project_state_.timelagOpt.co2_max_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_11, QString::number(ec_project_state_.timelagOpt.h2o_min_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_12, QString::number(ec_project_state_.timelagOpt.h2o_max_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_13, QString::number(ec_project_state_.timelagOpt.ch4_min_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_14, QString::number(ec_project_state_.timelagOpt.ch4_max_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_15, QString::number(ec_project_state_.timelagOpt.gas4_min_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_16, QString::number(ec_project_state_.timelagOpt.gas4_max_lag, 'f', 1));
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_18, ec_project_state_.timelagOpt.subset);
    project_ini.endGroup();

    // random error section
    project_ini.beginGroup(EcIni::INIGROUP_RAND_ERROR);
        project_ini.setValue(EcIni::INI_RAND_ERROR_0, ec_project_state_.randomError.method);
        project_ini.setValue(EcIni::INI_RAND_ERROR_1, ec_project_state_.randomError.its_method);
        project_ini.setValue(EcIni::INI_RAND_ERROR_2, QString::number(ec_project_state_.randomError.its_tlag_max, 'f', 1));

        // NOTE: temporarly disabled
        //project_ini.setValue(EcIni::INI_RAND_ERROR_3, QString::number(ec_project_state_.randomError.its_sec_factor, 'f', 1));
    project_ini.endGroup();

    // biomet section
    project_ini.beginGroup(EcIni::INIGROUP_BIOMET);
        project_ini.setValue(EcIni::INI_BIOMET_0, ec_project_state_.biomParam.native_header);
        project_ini.setValue(EcIni::INI_BIOMET_1, ec_project_state_.biomParam.hlines);
        project_ini.setValue(EcIni::INI_BIOMET_2, ec_project_state_.biomParam.separator);
        project_ini.setValue(EcIni::INI_BIOMET_3, ec_project_state_.biomParam.tstamp_ref);
        project_ini.setValue(EcIni::INI_BIOMET_4, ec_project_state_.biomParam.col_ta - 1000);
        project_ini.setValue(EcIni::INI_BIOMET_5, ec_project_state_.biomParam.col_pa - 1000);
        project_ini.setValue(EcIni::INI_BIOMET_6, ec_project_state_.biomParam.col_rh);
        project_ini.setValue(EcIni::INI_BIOMET_7, ec_project_state_.biomParam.col_rg);
        project_ini.setValue(EcIni::INI_BIOMET_8, ec_project_state_.biomParam.col_lwin);
        project_ini.setValue(EcIni::INI_BIOMET_9, ec_project_state_.biomParam.col_ppfd);
    project_ini.endGroup();

    project_ini.sync();

    bool result = tagProject(filename);
    if (!result)
    {
        WidgetUtils::warning(nullptr,
                             tr("Write Project Error"),
                             tr("Unable to tag project file!"));
    }

    // project is saved, so set flags accordingly
    setModified(false);
//    QApplication::restoreOverrideCursor();
    return true;
}

// Load a project. Assumes file has been checked with nativeFormat()
bool EcProject::loadEcProject(const QString &filename, bool checkVersion, bool *modified)
{
    DEBUG_FUNC_NAME

    bool isVersionCompatible = true;
    QVariant v = QVariant(); // container for conversions

    // open file
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open [loadEcProject()]" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file<br /><p>%1:</p>\n<b>%2</b>")
                             .arg(filename).arg(datafile.errorString()));
        return false;
    }

//    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QSettings project_ini(filename, QSettings::IniFormat);

    // in case of old non existing file name, use the current existing
    QString projectFilename = project_ini.value(EcIni::INI_PROJECT_2, QString()).toString();
    if (!FileUtils::existsPath(projectFilename))
    {
        projectFilename = filename;
    }
    qDebug() << "projectFilename" << projectFilename;

    // general section
    project_ini.beginGroup(EcIni::INIGROUP_PROJECT);
    ec_project_state_.projectGeneral.project_title = project_ini.value(EcIni::INI_PROJECT_3, QString()).toString();
        ec_project_state_.projectGeneral.project_id = project_ini.value(EcIni::INI_PROJECT_6, QString()).toString();
        ec_project_state_.projectGeneral.creation_date = project_ini.value(EcIni::INI_PROJECT_0, QString()).toString();
        ec_project_state_.projectGeneral.last_change_date = project_ini.value(EcIni::INI_PROJECT_1, QString()).toString();

        v = project_ini.value(EcIni::INI_PROJECT_7,
                              QVariant::fromValue(Defs::RawFileType::GHG).toInt());
        ec_project_state_.projectGeneral.file_type
                = static_cast<Defs::RawFileType>(v.toInt());

        ec_project_state_.projectGeneral.use_alt_md_file = project_ini.value(EcIni::INI_PROJECT_9, false).toBool();

        // NOTE: backward compatibility, is_express key
        if (project_ini.contains(EcIni::INI_PROJECT_33_OLD))
        {
            if (checkVersion)
            {
//                QApplication::restoreOverrideCursor();
                if(!queryProjectImport(filename))
                {
                    return false;
                }

                v = project_ini.value(EcIni::INI_PROJECT_33_OLD,
                         QVariant::fromValue(Defs::CurrRunMode::Advanced).toInt());
                ec_project_state_.projectGeneral.run_mode
                    = static_cast<Defs::CurrRunMode>(v.toInt());

                project_ini.remove(EcIni::INI_PROJECT_33_OLD);
                isVersionCompatible = false;
            }
            else
            {
                // abort the file loading to skip it during previous data comparison
                return false;
            }
        }
        else
        {
            v = project_ini.value(EcIni::INI_PROJECT_33,
                            QVariant::fromValue(Defs::CurrRunMode::Advanced));

            if (v.canConvert<Defs::CurrRunMode>())
            {
                ec_project_state_.projectGeneral.run_mode
                    = v.value<Defs::CurrRunMode>();
            }

//            v = project_ini.value(EcIni::INI_PROJECT_40,
//                            QVariant::fromValue(Defs::CurrRunMode::Advanced));
//            ec_project_state_.projectGeneral.run_fcc
//                    = v.value<Defs::CurrRunMode>();

            ec_project_state_.projectGeneral.run_fcc
                    = project_ini.value(EcIni::INI_PROJECT_40, false).toBool();
        }

        ec_project_state_.projectGeneral.file_name = projectFilename;
        ec_project_state_.projectGeneral.sw_version = project_ini.value(EcIni::INI_PROJECT_4, Defs::APP_VERSION_STR).toString();
        ec_project_state_.projectGeneral.ini_version = project_ini.value(EcIni::INI_PROJECT_5, Defs::PROJECT_FILE_VERSION_STR).toString();
        ec_project_state_.projectGeneral.file_prototype = project_ini.value(EcIni::INI_PROJECT_8, QString()).toString();
        ec_project_state_.projectGeneral.md_file = project_ini.value(EcIni::INI_PROJECT_10, QString()).toString();
        ec_project_state_.projectGeneral.use_tlfile = project_ini.value(EcIni::INI_PROJECT_11, false).toBool();
        ec_project_state_.projectGeneral.timeline_file = project_ini.value(EcIni::INI_PROJECT_12, QString()).toString();
        ec_project_state_.projectGeneral.binary_hnlines = project_ini.value(EcIni::INI_PROJECT_13, -1).toInt();
        ec_project_state_.projectGeneral.binary_eol = project_ini.value(EcIni::INI_PROJECT_14, -1).toInt();
        ec_project_state_.projectGeneral.binary_nbytes = project_ini.value(EcIni::INI_PROJECT_15, -1).toInt();
        ec_project_state_.projectGeneral.binary_little_end = project_ini.value(EcIni::INI_PROJECT_16, -1).toInt();
        ec_project_state_.projectGeneral.master_sonic = project_ini.value(EcIni::INI_PROJECT_17, QString()).toString();
        ec_project_state_.projectGeneral.col_co2 = project_ini.value(EcIni::INI_PROJECT_18, -1).toInt();
        ec_project_state_.projectGeneral.col_h2o = project_ini.value(EcIni::INI_PROJECT_19, -1).toInt();
        ec_project_state_.projectGeneral.col_ch4 = project_ini.value(EcIni::INI_PROJECT_20, -1).toInt();
        ec_project_state_.projectGeneral.col_n2o = project_ini.value(EcIni::INI_PROJECT_21, -1).toInt();
        ec_project_state_.projectGeneral.col_int_t_1 = project_ini.value(EcIni::INI_PROJECT_22, -1).toInt();
        ec_project_state_.projectGeneral.col_int_t_2 = project_ini.value(EcIni::INI_PROJECT_23, -1).toInt();
        ec_project_state_.projectGeneral.col_int_p = project_ini.value(EcIni::INI_PROJECT_24, -1).toInt();
        ec_project_state_.projectGeneral.col_air_t = project_ini.value(EcIni::INI_PROJECT_25, -1).toInt();
        ec_project_state_.projectGeneral.col_air_p = project_ini.value(EcIni::INI_PROJECT_26, -1).toInt();
        ec_project_state_.projectGeneral.col_int_t_c = project_ini.value(EcIni::INI_PROJECT_27, -1).toInt();
        ec_project_state_.projectGeneral.col_diag_75 = project_ini.value(EcIni::INI_PROJECT_28, -1).toInt();
        ec_project_state_.projectGeneral.col_diag_72 = project_ini.value(EcIni::INI_PROJECT_29, -1).toInt();
        ec_project_state_.projectGeneral.col_diag_77 = project_ini.value(EcIni::INI_PROJECT_30, -1).toInt();
        ec_project_state_.projectGeneral.col_ts = project_ini.value(EcIni::INI_PROJECT_36, -1).toInt();
        ec_project_state_.projectGeneral.gas_mw = project_ini.value(EcIni::INI_PROJECT_31, -1.0).toReal();
        ec_project_state_.projectGeneral.gas_diff = project_ini.value(EcIni::INI_PROJECT_32, -1.0).toReal();
        ec_project_state_.projectGeneral.out_ghg_eu = project_ini.value(EcIni::INI_PROJECT_37, 0).toInt();
        ec_project_state_.projectGeneral.out_amflux = project_ini.value(EcIni::INI_PROJECT_38, 0).toInt();

        // NOTE: backward compatibility change, out_rich key
        if (project_ini.value(EcIni::INI_PROJECT_5, Defs::PROJECT_FILE_VERSION_STR).toString() == QLatin1String("1.0"))
        {
            if (checkVersion)
            {
//                QApplication::restoreOverrideCursor();
                if(!queryProjectImport(filename))
                {
                    return false;
                }
                ec_project_state_.projectGeneral.out_rich = 1;
                isVersionCompatible = false;
            }
            else
            {
                // abort the file loading to skip it during previous data comparison
                return false;
            }
        }
        else
        {
            ec_project_state_.projectGeneral.out_rich = project_ini.value(EcIni::INI_PROJECT_39, 1).toInt();
        }

        ec_project_state_.projectGeneral.out_md = project_ini.value(EcIni::INI_PROJECT_56, 1).toInt();
        ec_project_state_.projectGeneral.out_biomet = project_ini.value(EcIni::INI_PROJECT_61, 0).toInt();

        ec_project_state_.projectGeneral.make_dataset = project_ini.value(EcIni::INI_PROJECT_41, false).toBool();
        ec_project_state_.projectGeneral.subset = project_ini.value(EcIni::INI_PROJECT_54, 0).toInt();
        ec_project_state_.projectGeneral.start_date = project_ini.value(EcIni::INI_PROJECT_42, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.projectGeneral.start_time = project_ini.value(EcIni::INI_PROJECT_44, QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();

//        if (ec_project_state_.projectGeneral.subset)
//        {
//            ec_project_state_.projectGeneral.start_date = project_ini.value(EcIni::INI_PROJECT_42, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
//            ec_project_state_.projectGeneral.start_time = project_ini.value(EcIni::INI_PROJECT_44, QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
//            ec_project_state_.projectGeneral.end_date = project_ini.value(EcIni::INI_PROJECT_43, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
//            ec_project_state_.projectGeneral.end_time = project_ini.value(EcIni::INI_PROJECT_45, QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
//        }
//        else
//        {
            ec_project_state_.projectGeneral.start_date = project_ini.value(EcIni::INI_PROJECT_42, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
            ec_project_state_.projectGeneral.start_time = project_ini.value(EcIni::INI_PROJECT_44, QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
            ec_project_state_.projectGeneral.end_date = project_ini.value(EcIni::INI_PROJECT_43, QDate::currentDate().toString(Qt::ISODate)).toString();
            ec_project_state_.projectGeneral.end_time = project_ini.value(EcIni::INI_PROJECT_45, QTime(23, 59).toString(QStringLiteral("hh:mm"))).toString();
//        }
        ec_project_state_.projectGeneral.hf_meth = project_ini.value(EcIni::INI_PROJECT_46, 1).toInt();
        ec_project_state_.projectGeneral.lf_meth = project_ini.value(EcIni::INI_PROJECT_47, 1).toInt();
        ec_project_state_.projectGeneral.wpl_meth = project_ini.value(EcIni::INI_PROJECT_48, 1).toInt();
        ec_project_state_.projectGeneral.foot_meth = project_ini.value(EcIni::INI_PROJECT_49, 1).toInt();
        ec_project_state_.projectGeneral.tob1_format = project_ini.value(EcIni::INI_PROJECT_50, 0).toInt();
        ec_project_state_.projectGeneral.out_path = project_ini.value(EcIni::INI_PROJECT_51, QString()).toString();
        ec_project_state_.projectGeneral.fix_out_format = project_ini.value(EcIni::INI_PROJECT_52, 0).toInt();
        ec_project_state_.projectGeneral.err_label = project_ini.value(EcIni::INI_PROJECT_53, QString(QStringLiteral("-9999.0"))).toString();
        ec_project_state_.projectGeneral.qcflag_meth = project_ini.value(EcIni::INI_PROJECT_55, 1).toInt();
        ec_project_state_.projectGeneral.use_biomet = project_ini.value(EcIni::INI_PROJECT_34, 0).toInt();
        ec_project_state_.projectGeneral.biom_file = project_ini.value(EcIni::INI_PROJECT_35, QString()).toString();
        ec_project_state_.projectGeneral.biom_dir = project_ini.value(EcIni::INI_PROJECT_57, QString()).toString();
        ec_project_state_.projectGeneral.biom_recurse = project_ini.value(EcIni::INI_PROJECT_58, 0).toInt();
        ec_project_state_.projectGeneral.biom_ext = project_ini.value(EcIni::INI_PROJECT_59, QString(QStringLiteral("txt"))).toString().remove(QLatin1Char('.'));
        ec_project_state_.projectGeneral.out_mean_cosp = project_ini.value(EcIni::INI_PROJECT_60, 0).toInt();
        ec_project_state_.projectGeneral.bin_sp_avail = project_ini.value(EcIni::INI_PROJECT_62, 0).toInt();
        ec_project_state_.projectGeneral.full_sp_avail = project_ini.value(EcIni::INI_PROJECT_63, 0).toInt();
        ec_project_state_.projectGeneral.files_found = project_ini.value(EcIni::INI_PROJECT_64, 0).toInt();
    project_ini.endGroup();

    // spec settings section
    project_ini.beginGroup(EcIni::INIGROUP_SPEC_SETTINGS);
        ec_project_state_.spectraSettings.start_sa_date = project_ini.value(EcIni::INI_SPEC_SETTINGS_0, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.spectraSettings.end_sa_date = project_ini.value(EcIni::INI_SPEC_SETTINGS_1, QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.spectraSettings.sa_mode = project_ini.value(EcIni::INI_SPEC_SETTINGS_2, 1).toInt();
        ec_project_state_.spectraSettings.sa_file = project_ini.value(EcIni::INI_SPEC_SETTINGS_3, QString()).toString();
        ec_project_state_.spectraSettings.sa_min_smpl = project_ini.value(EcIni::INI_SPEC_SETTINGS_4, 10).toInt();
        ec_project_state_.spectraSettings.sa_fmin_co2 = project_ini.value(EcIni::INI_SPEC_SETTINGS_5, 0.004).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_h2o = project_ini.value(EcIni::INI_SPEC_SETTINGS_6, 0.004).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_ch4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_7, 0.004).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_gas4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_8, 0.004).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_co2 = project_ini.value(EcIni::INI_SPEC_SETTINGS_9, 1.0).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_h2o = project_ini.value(EcIni::INI_SPEC_SETTINGS_10, 1.0).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_ch4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_11, 1.0).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_gas4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_12, 1.0).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_co2_fmin = project_ini.value(EcIni::INI_SPEC_SETTINGS_13, 2.0).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_h2o_fmin = project_ini.value(EcIni::INI_SPEC_SETTINGS_14, 2.0).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_ch4_fmin = project_ini.value(EcIni::INI_SPEC_SETTINGS_15, 2.0).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_gas4_fmin = project_ini.value(EcIni::INI_SPEC_SETTINGS_16, 2.0).toDouble();
        ec_project_state_.spectraSettings.sa_min_co2 = project_ini.value(EcIni::INI_SPEC_SETTINGS_17, 2.0).toDouble();
        ec_project_state_.spectraSettings.sa_min_ch4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_18, 0.2).toDouble();
        ec_project_state_.spectraSettings.sa_min_gas4 = project_ini.value(EcIni::INI_SPEC_SETTINGS_19, 0.02).toDouble();
        ec_project_state_.spectraSettings.sa_min_le = project_ini.value(EcIni::INI_SPEC_SETTINGS_20, 20.0).toDouble();
        ec_project_state_.spectraSettings.sa_min_h = project_ini.value(EcIni::INI_SPEC_SETTINGS_21, 20.0).toDouble();
        ec_project_state_.spectraSettings.add_sonic_lptf = project_ini.value(EcIni::INI_SPEC_SETTINGS_22, 1).toInt();
        ec_project_state_.spectraSettings.f10_co2_trshld = project_ini.value(EcIni::INI_SPEC_SETTINGS_23, 2.0).toDouble();
        ec_project_state_.spectraSettings.f10_ch4_trshld = project_ini.value(EcIni::INI_SPEC_SETTINGS_24, 0.2).toDouble();
        ec_project_state_.spectraSettings.f10_gas4_trshld = project_ini.value(EcIni::INI_SPEC_SETTINGS_25, 0.02).toDouble();
        ec_project_state_.spectraSettings.f10_le_trshld = project_ini.value(EcIni::INI_SPEC_SETTINGS_26, 10.0).toDouble();
        ec_project_state_.spectraSettings.f10_h_trshld = project_ini.value(EcIni::INI_SPEC_SETTINGS_27, 10.0).toDouble();
        ec_project_state_.spectraSettings.horst_lens = project_ini.value(EcIni::INI_SPEC_SETTINGS_28, 2).toInt();
        ec_project_state_.spectraSettings.ex_file = project_ini.value(EcIni::INI_SPEC_SETTINGS_29, QString()).toString();
        ec_project_state_.spectraSettings.sa_bin_spectra = project_ini.value(EcIni::INI_SPEC_SETTINGS_30, QString()).toString();
        ec_project_state_.spectraSettings.sa_full_spectra = project_ini.value(EcIni::INI_SPEC_SETTINGS_31, QString()).toString();
        ec_project_state_.spectraSettings.ex_dir = project_ini.value(EcIni::INI_SPEC_SETTINGS_32, QString()).toString();
        ec_project_state_.spectraSettings.subset = project_ini.value(EcIni::INI_SPEC_SETTINGS_33, 0).toInt();
    project_ini.endGroup();

    // preproc general section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_GENERAL);
        ec_project_state_.screenGeneral.data_path = project_ini.value(EcIni::INI_SCREEN_GENERAL_4, QString()).toString();
        ec_project_state_.screenGeneral.recurse = project_ini.value(EcIni::INI_SCREEN_GENERAL_6, 1).toInt();
        ec_project_state_.screenGeneral.use_geo_north = project_ini.value(EcIni::INI_SCREEN_GENERAL_40, false).toBool();
        ec_project_state_.screenGeneral.mag_dec = project_ini.value(EcIni::INI_SCREEN_GENERAL_38, 0.0).toReal();
        ec_project_state_.screenGeneral.dec_date = project_ini.value(EcIni::INI_SCREEN_GENERAL_39, ec_project_state_.projectGeneral.end_date).toString();
        ec_project_state_.screenGeneral.flag1_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_8, -1).toInt();
        ec_project_state_.screenGeneral.flag1_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_9, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag1_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_10, 1).toInt();
        ec_project_state_.screenGeneral.flag2_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_11, -1).toInt();
        ec_project_state_.screenGeneral.flag2_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_12, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag2_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_13, 1).toInt();
        ec_project_state_.screenGeneral.flag3_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_14, -1).toInt();
        ec_project_state_.screenGeneral.flag3_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_15, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag3_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_16, 1).toInt();
        ec_project_state_.screenGeneral.flag4_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_17, -1).toInt();
        ec_project_state_.screenGeneral.flag4_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_18, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag4_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_19, 1).toInt();
        ec_project_state_.screenGeneral.flag5_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_20, -1).toInt();
        ec_project_state_.screenGeneral.flag5_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_21, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag5_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_22, 1).toInt();
        ec_project_state_.screenGeneral.flag6_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_23, -1).toInt();
        ec_project_state_.screenGeneral.flag6_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_24, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag6_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_25, 1).toInt();
        ec_project_state_.screenGeneral.flag7_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_26, -1).toInt();
        ec_project_state_.screenGeneral.flag7_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_27, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag7_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_28, 1).toInt();
        ec_project_state_.screenGeneral.flag8_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_29, -1).toInt();
        ec_project_state_.screenGeneral.flag8_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_30, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag8_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_31, 1).toInt();
        ec_project_state_.screenGeneral.flag9_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_32, -1).toInt();
        ec_project_state_.screenGeneral.flag9_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_33, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag9_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_34, 1).toInt();
        ec_project_state_.screenGeneral.flag10_col = project_ini.value(EcIni::INI_SCREEN_GENERAL_35, -1).toInt();
        ec_project_state_.screenGeneral.flag10_threshold = project_ini.value(EcIni::INI_SCREEN_GENERAL_36, -9999.0).toReal();
        ec_project_state_.screenGeneral.flag10_policy = project_ini.value(EcIni::INI_SCREEN_GENERAL_37, 1).toInt();
    project_ini.endGroup();

    // preproc setup section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_SETTINGS);
        ec_project_state_.screenSetting.nfiles = project_ini.value(EcIni::INI_SCREEN_SETTINGS_0, 1).toInt();
        ec_project_state_.screenSetting.max_lack = project_ini.value(EcIni::INI_SCREEN_SETTINGS_1, 10).toInt();
        ec_project_state_.screenSetting.u_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_12, 0.0).toDouble();
        ec_project_state_.screenSetting.v_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_13, 0.0).toDouble();
        ec_project_state_.screenSetting.w_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_14, 0.0).toDouble();
        ec_project_state_.screenSetting.cross_wind = project_ini.value(EcIni::INI_SCREEN_SETTINGS_2, 0).toInt();
        ec_project_state_.screenSetting.flow_distortion = project_ini.value(EcIni::INI_SCREEN_SETTINGS_3, 0).toInt();
        ec_project_state_.screenSetting.rot_meth = project_ini.value(EcIni::INI_SCREEN_SETTINGS_4, 1).toInt();
        ec_project_state_.screenSetting.detrend_meth = project_ini.value(EcIni::INI_SCREEN_SETTINGS_5, 0).toInt();
        ec_project_state_.screenSetting.timeconst = project_ini.value(EcIni::INI_SCREEN_SETTINGS_6, 250.0).toDouble();
        ec_project_state_.screenSetting.tlag_meth = project_ini.value(EcIni::INI_SCREEN_SETTINGS_7, 2.0).toInt();
        ec_project_state_.screenSetting.tap_win = project_ini.value(EcIni::INI_SCREEN_SETTINGS_8, 3).toInt();
        ec_project_state_.screenSetting.nbins = project_ini.value(EcIni::INI_SCREEN_SETTINGS_9, 90).toInt();
        ec_project_state_.screenSetting.avrg_len = project_ini.value(EcIni::INI_SCREEN_SETTINGS_10, 30).toInt();
        ec_project_state_.screenSetting.out_bin_sp = project_ini.value(EcIni::INI_SCREEN_SETTINGS_15, 1).toInt();
        ec_project_state_.screenSetting.out_bin_og = project_ini.value(EcIni::INI_SCREEN_SETTINGS_38, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_u = project_ini.value(EcIni::INI_SCREEN_SETTINGS_16, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_v = project_ini.value(EcIni::INI_SCREEN_SETTINGS_17, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_w = project_ini.value(EcIni::INI_SCREEN_SETTINGS_18, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_ts = project_ini.value(EcIni::INI_SCREEN_SETTINGS_19, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_co2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_20, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_h2o = project_ini.value(EcIni::INI_SCREEN_SETTINGS_21, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_ch4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_22, 0).toInt();
        ec_project_state_.screenSetting.out_full_sp_n2o = project_ini.value(EcIni::INI_SCREEN_SETTINGS_23, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_u = project_ini.value(EcIni::INI_SCREEN_SETTINGS_31, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_v = project_ini.value(EcIni::INI_SCREEN_SETTINGS_32, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_ts = project_ini.value(EcIni::INI_SCREEN_SETTINGS_33, 1).toInt();
        ec_project_state_.screenSetting.out_full_cosp_co2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_34, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_h2o = project_ini.value(EcIni::INI_SCREEN_SETTINGS_35, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_ch4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_36, 0).toInt();
        ec_project_state_.screenSetting.out_full_cosp_n2o = project_ini.value(EcIni::INI_SCREEN_SETTINGS_37, 0).toInt();
        ec_project_state_.screenSetting.out_st_1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_24, 1).toInt();
        ec_project_state_.screenSetting.out_st_2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_25, 0).toInt();
        ec_project_state_.screenSetting.out_st_3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_26, 0).toInt();
        ec_project_state_.screenSetting.out_st_4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_27, 0).toInt();
        ec_project_state_.screenSetting.out_st_5 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_28, 0).toInt();
        ec_project_state_.screenSetting.out_st_6 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_29, 0).toInt();
        ec_project_state_.screenSetting.out_st_7 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_30, 0).toInt();
        ec_project_state_.screenSetting.out_raw_1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_82, 0).toInt();
        ec_project_state_.screenSetting.out_raw_2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_83, 0).toInt();
        ec_project_state_.screenSetting.out_raw_3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_84, 0).toInt();
        ec_project_state_.screenSetting.out_raw_4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_85, 0).toInt();
        ec_project_state_.screenSetting.out_raw_5 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_86, 0).toInt();
        ec_project_state_.screenSetting.out_raw_6 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_87, 0).toInt();
        ec_project_state_.screenSetting.out_raw_7 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_88, 0).toInt();
        ec_project_state_.screenSetting.out_raw_u = project_ini.value(EcIni::INI_SCREEN_SETTINGS_89, 0).toInt();
        ec_project_state_.screenSetting.out_raw_v = project_ini.value(EcIni::INI_SCREEN_SETTINGS_90, 0).toInt();
        ec_project_state_.screenSetting.out_raw_w = project_ini.value(EcIni::INI_SCREEN_SETTINGS_91, 0).toInt();
        ec_project_state_.screenSetting.out_raw_ts = project_ini.value(EcIni::INI_SCREEN_SETTINGS_92, 0).toInt();
        ec_project_state_.screenSetting.out_raw_co2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_93, 0).toInt();
        ec_project_state_.screenSetting.out_raw_h2o = project_ini.value(EcIni::INI_SCREEN_SETTINGS_94, 0).toInt();
        ec_project_state_.screenSetting.out_raw_ch4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_95, 0).toInt();
        ec_project_state_.screenSetting.out_raw_gas4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_96, 0).toInt();
        ec_project_state_.screenSetting.out_raw_tair = project_ini.value(EcIni::INI_SCREEN_SETTINGS_97, 0).toInt();
        ec_project_state_.screenSetting.out_raw_pair = project_ini.value(EcIni::INI_SCREEN_SETTINGS_98, 0).toInt();
        ec_project_state_.screenSetting.to_mixratio = project_ini.value(EcIni::INI_SCREEN_SETTINGS_39, 1).toInt();
        ec_project_state_.screenSetting.filter_sr = project_ini.value(EcIni::INI_SCREEN_SETTINGS_40, 1).toInt();
        ec_project_state_.screenSetting.filter_al = project_ini.value(EcIni::INI_SCREEN_SETTINGS_41, 1).toInt();
        ec_project_state_.screenSetting.bu_corr = project_ini.value(EcIni::INI_SCREEN_SETTINGS_44, 0).toInt();
        ec_project_state_.screenSetting.bu_multi = project_ini.value(EcIni::INI_SCREEN_SETTINGS_45, 0).toInt();
        ec_project_state_.screenSetting.l_day_bot_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_46, 0.944).toDouble();
        ec_project_state_.screenSetting.l_day_bot_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_47, 2.57).toDouble();
        ec_project_state_.screenSetting.l_day_top_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_48).toDouble();
        ec_project_state_.screenSetting.l_day_top_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_49, 1.005).toDouble();
        ec_project_state_.screenSetting.l_day_spar_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_50, 1.010).toDouble();
        ec_project_state_.screenSetting.l_day_spar_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_51, 0.36).toDouble();
        ec_project_state_.screenSetting.l_night_bot_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_52, 0.883).toDouble();
        ec_project_state_.screenSetting.l_night_bot_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_53, 2.17).toDouble();
        ec_project_state_.screenSetting.l_night_top_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_54, 1.008).toDouble();
        ec_project_state_.screenSetting.l_night_top_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_55, -0.41).toDouble();
        ec_project_state_.screenSetting.l_night_spar_gain = project_ini.value(EcIni::INI_SCREEN_SETTINGS_56, 1.010).toDouble();
        ec_project_state_.screenSetting.l_night_spar_offset = project_ini.value(EcIni::INI_SCREEN_SETTINGS_57, -0.17).toDouble();
        ec_project_state_.screenSetting.m_day_bot1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_58, 2.8).toDouble();
        ec_project_state_.screenSetting.m_day_bot2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_59, -0.0681).toDouble();
        ec_project_state_.screenSetting.m_day_bot3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_60, 0.0021).toDouble();
        ec_project_state_.screenSetting.m_day_bot4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_61, -0.334).toDouble();
        ec_project_state_.screenSetting.m_day_top1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_62, -0.1).toDouble();
        ec_project_state_.screenSetting.m_day_top2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_63, -0.0044).toDouble();
        ec_project_state_.screenSetting.m_day_top3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_64, 0.0011).toDouble();
        ec_project_state_.screenSetting.m_day_top4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_65, -0.022).toDouble();
        ec_project_state_.screenSetting.m_day_spar1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_66, 0.3).toDouble();
        ec_project_state_.screenSetting.m_day_spar2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_67, -0.0007).toDouble();
        ec_project_state_.screenSetting.m_day_spar3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_68, 0.0006).toDouble();
        ec_project_state_.screenSetting.m_day_spar4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_69, -0.044).toDouble();
        ec_project_state_.screenSetting.m_night_bot1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_70, 0.5).toDouble();
        ec_project_state_.screenSetting.m_night_bot2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_71, -0.1160).toDouble();
        ec_project_state_.screenSetting.m_night_bot3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_72, 0.0087).toDouble();
        ec_project_state_.screenSetting.m_night_bot4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_73, -0.206).toDouble();
        ec_project_state_.screenSetting.m_night_top1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_74, -1.7).toDouble();
        ec_project_state_.screenSetting.m_night_top2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_75, -0.1160).toDouble();
        ec_project_state_.screenSetting.m_night_top3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_76, 0.0051).toDouble();
        ec_project_state_.screenSetting.m_night_top4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_77, -0.029).toDouble();
        ec_project_state_.screenSetting.m_night_spar1 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_78, -2.1).toDouble();
        ec_project_state_.screenSetting.m_night_spar2 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_79, -0.0200).toDouble();
        ec_project_state_.screenSetting.m_night_spar3 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_80, 0.0070).toDouble();
        ec_project_state_.screenSetting.m_night_spar4 = project_ini.value(EcIni::INI_SCREEN_SETTINGS_81, -0.026).toDouble();
        ec_project_state_.screenSetting.out_details = project_ini.value(EcIni::INI_SCREEN_SETTINGS_99, 0).toInt();
        ec_project_state_.screenSetting.power_of_two = project_ini.value(EcIni::INI_SCREEN_SETTINGS_101, 1).toInt();
    project_ini.endGroup();

    // preproc test section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TESTS);
        ec_project_state_.screenTest.test_sr = project_ini.value(EcIni::INI_SCREEN_TESTS_0, 1).toInt();
        ec_project_state_.screenTest.test_ar = project_ini.value(EcIni::INI_SCREEN_TESTS_1, 1).toInt();
        ec_project_state_.screenTest.test_do = project_ini.value(EcIni::INI_SCREEN_TESTS_2, 1).toInt();
        ec_project_state_.screenTest.test_al = project_ini.value(EcIni::INI_SCREEN_TESTS_3, 1).toInt();
        ec_project_state_.screenTest.test_sk = project_ini.value(EcIni::INI_SCREEN_TESTS_4, 1).toInt();
        ec_project_state_.screenTest.test_ds = project_ini.value(EcIni::INI_SCREEN_TESTS_5, 0).toInt();
        ec_project_state_.screenTest.test_tl = project_ini.value(EcIni::INI_SCREEN_TESTS_6, 0).toInt();
        ec_project_state_.screenTest.test_aa = project_ini.value(EcIni::INI_SCREEN_TESTS_7, 0).toInt();
        ec_project_state_.screenTest.test_ns = project_ini.value(EcIni::INI_SCREEN_TESTS_8, 0).toInt();
    project_ini.endGroup();

    // preproc test section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_PARAM);
        ec_project_state_.screenParam.sr_num_spk = project_ini.value(EcIni::INI_SCREEN_PARAM_0, 3).toInt();
        ec_project_state_.screenParam.sr_lim_u = project_ini.value(EcIni::INI_SCREEN_PARAM_1, 3.5).toDouble();
        ec_project_state_.screenParam.sr_lim_w = project_ini.value(EcIni::INI_SCREEN_PARAM_45, 5.0).toDouble();
        ec_project_state_.screenParam.sr_lim_co2 = project_ini.value(EcIni::INI_SCREEN_PARAM_46, 3.5).toDouble();
        ec_project_state_.screenParam.sr_lim_h2o = project_ini.value(EcIni::INI_SCREEN_PARAM_47, 3.5).toDouble();
        ec_project_state_.screenParam.sr_lim_ch4 = project_ini.value(EcIni::INI_SCREEN_PARAM_48, 8.0).toDouble();
        ec_project_state_.screenParam.sr_lim_n2o = project_ini.value(EcIni::INI_SCREEN_PARAM_49, 8.0).toDouble();
        ec_project_state_.screenParam.sr_lim_hf = project_ini.value(EcIni::INI_SCREEN_PARAM_2, 1.0).toDouble();
        ec_project_state_.screenParam.ar_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_3, 7.0).toDouble();
        ec_project_state_.screenParam.ar_bins = project_ini.value(EcIni::INI_SCREEN_PARAM_4, 100).toInt();
        ec_project_state_.screenParam.ar_hf_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_5, 70).toInt();
        ec_project_state_.screenParam.do_extlim_dw = project_ini.value(EcIni::INI_SCREEN_PARAM_6, 10).toInt();
        ec_project_state_.screenParam.do_hf1_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_7, 10.0).toDouble();
        ec_project_state_.screenParam.do_hf2_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_8, 6.0).toDouble();
        ec_project_state_.screenParam.al_u_max = project_ini.value(EcIni::INI_SCREEN_PARAM_9, 30.0).toDouble();
        ec_project_state_.screenParam.al_w_max = project_ini.value(EcIni::INI_SCREEN_PARAM_10, 5.0).toDouble();
        ec_project_state_.screenParam.al_tson_min = project_ini.value(EcIni::INI_SCREEN_PARAM_11, -40.0).toDouble();
        ec_project_state_.screenParam.al_tson_max = project_ini.value(EcIni::INI_SCREEN_PARAM_12, 50.0).toDouble();
        ec_project_state_.screenParam.al_co2_min = project_ini.value(EcIni::INI_SCREEN_PARAM_13, 200.0).toDouble();
        ec_project_state_.screenParam.al_co2_max = project_ini.value(EcIni::INI_SCREEN_PARAM_14, 900.0).toDouble();
        ec_project_state_.screenParam.al_h2o_min = project_ini.value(EcIni::INI_SCREEN_PARAM_15, 0.0).toDouble();
        ec_project_state_.screenParam.al_h2o_max = project_ini.value(EcIni::INI_SCREEN_PARAM_16, 40.0).toDouble();
        ec_project_state_.screenParam.al_ch4_min = project_ini.value(EcIni::INI_SCREEN_PARAM_54, 0.17).toDouble();
        ec_project_state_.screenParam.al_ch4_max = project_ini.value(EcIni::INI_SCREEN_PARAM_55, 1000.0).toDouble();
        ec_project_state_.screenParam.al_n2o_min = project_ini.value(EcIni::INI_SCREEN_PARAM_56, 0.032).toDouble();
        ec_project_state_.screenParam.al_n2o_max = project_ini.value(EcIni::INI_SCREEN_PARAM_57, 1000.0).toDouble();
        ec_project_state_.screenParam.sk_hf_skmin = project_ini.value(EcIni::INI_SCREEN_PARAM_17, -2.0).toDouble();
        ec_project_state_.screenParam.sk_hf_skmax = project_ini.value(EcIni::INI_SCREEN_PARAM_18, 2.0).toDouble();
        ec_project_state_.screenParam.sk_sf_skmin = project_ini.value(EcIni::INI_SCREEN_PARAM_19, -1.0).toDouble();
        ec_project_state_.screenParam.sk_sf_skmax = project_ini.value(EcIni::INI_SCREEN_PARAM_20, 1.0).toDouble();
        ec_project_state_.screenParam.sk_hf_kumin = project_ini.value(EcIni::INI_SCREEN_PARAM_21, 1.0).toDouble();
        ec_project_state_.screenParam.sk_hf_kumax = project_ini.value(EcIni::INI_SCREEN_PARAM_22, 8.0).toDouble();
        ec_project_state_.screenParam.sk_sf_kumin = project_ini.value(EcIni::INI_SCREEN_PARAM_23, 2.0).toDouble();
        ec_project_state_.screenParam.sk_sf_kumax = project_ini.value(EcIni::INI_SCREEN_PARAM_24, 5.0).toDouble();
        ec_project_state_.screenParam.ds_hf_uv = project_ini.value(EcIni::INI_SCREEN_PARAM_25, 4.0).toDouble();
        ec_project_state_.screenParam.ds_hf_w = project_ini.value(EcIni::INI_SCREEN_PARAM_26, 2.0).toDouble();
        ec_project_state_.screenParam.ds_hf_t = project_ini.value(EcIni::INI_SCREEN_PARAM_27, 4.0).toDouble();
        ec_project_state_.screenParam.ds_hf_co2 = project_ini.value(EcIni::INI_SCREEN_PARAM_28, 40.0).toDouble();
        ec_project_state_.screenParam.ds_hf_h2o = project_ini.value(EcIni::INI_SCREEN_PARAM_29, 3.26).toDouble();
        ec_project_state_.screenParam.ds_hf_ch4 = project_ini.value(EcIni::INI_SCREEN_PARAM_50, 40.0).toDouble();
        ec_project_state_.screenParam.ds_hf_n2o = project_ini.value(EcIni::INI_SCREEN_PARAM_51, 40.0).toDouble();
        ec_project_state_.screenParam.ds_hf_var = project_ini.value(EcIni::INI_SCREEN_PARAM_30, 3.0).toDouble();
        ec_project_state_.screenParam.ds_sf_uv = project_ini.value(EcIni::INI_SCREEN_PARAM_31, 2.7).toDouble();
        ec_project_state_.screenParam.ds_sf_w = project_ini.value(EcIni::INI_SCREEN_PARAM_32, 1.3).toDouble();
        ec_project_state_.screenParam.ds_sf_t = project_ini.value(EcIni::INI_SCREEN_PARAM_33, 2.7).toDouble();
        ec_project_state_.screenParam.ds_sf_co2 = project_ini.value(EcIni::INI_SCREEN_PARAM_34, 27.0).toDouble();
        ec_project_state_.screenParam.ds_sf_h2o = project_ini.value(EcIni::INI_SCREEN_PARAM_35, 2.2).toDouble();
        ec_project_state_.screenParam.ds_sf_ch4 = project_ini.value(EcIni::INI_SCREEN_PARAM_52, 30.0).toDouble();
        ec_project_state_.screenParam.ds_sf_n2o = project_ini.value(EcIni::INI_SCREEN_PARAM_53, 30.0).toDouble();
        ec_project_state_.screenParam.ds_sf_var = project_ini.value(EcIni::INI_SCREEN_PARAM_36, 2.0).toDouble();
        ec_project_state_.screenParam.tl_hf_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_37, 20.0).toDouble();
        ec_project_state_.screenParam.tl_sf_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_38, 10.0).toDouble();
        ec_project_state_.screenParam.tl_def_co2 = project_ini.value(EcIni::INI_SCREEN_PARAM_39, 3.5).toDouble();
        ec_project_state_.screenParam.tl_def_h2o = project_ini.value(EcIni::INI_SCREEN_PARAM_40, 2.5).toDouble();
        ec_project_state_.screenParam.tl_def_ch4 = project_ini.value(EcIni::INI_SCREEN_PARAM_58, 3.5).toDouble();
        ec_project_state_.screenParam.tl_def_n2o = project_ini.value(EcIni::INI_SCREEN_PARAM_59, 2.5).toDouble();
        ec_project_state_.screenParam.aa_min = project_ini.value(EcIni::INI_SCREEN_PARAM_41, -30.0).toDouble();
        ec_project_state_.screenParam.aa_max = project_ini.value(EcIni::INI_SCREEN_PARAM_42, 30.0).toDouble();
        ec_project_state_.screenParam.aa_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_43, 10.0).toDouble();
        ec_project_state_.screenParam.ns_hf_lim = project_ini.value(EcIni::INI_SCREEN_PARAM_44, 0.5).toDouble();
    project_ini.endGroup();

    // planar fit section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TILT);
        ec_project_state_.screenTilt.start_date = project_ini.value(EcIni::INI_SCREEN_TILT_0, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.screenTilt.end_date = project_ini.value(EcIni::INI_SCREEN_TILT_1, QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.screenTilt.mode = project_ini.value(EcIni::INI_SCREEN_TILT_2, 1).toInt();
        ec_project_state_.screenTilt.north_offset = project_ini.value(EcIni::INI_SCREEN_TILT_3, 0.0).toDouble();
        ec_project_state_.screenTilt.min_num_per_sec = project_ini.value(EcIni::INI_SCREEN_TILT_4).toInt();
        ec_project_state_.screenTilt.w_max = project_ini.value(EcIni::INI_SCREEN_TILT_5, 0.099).toDouble();
        ec_project_state_.screenTilt.u_min = project_ini.value(EcIni::INI_SCREEN_TILT_6, -0.001).toDouble();
        ec_project_state_.screenTilt.file = project_ini.value(EcIni::INI_SCREEN_TILT_7, QString()).toString();
        ec_project_state_.screenTilt.fix_policy = project_ini.value(EcIni::INI_SCREEN_TILT_8, 0).toInt();
        ec_project_state_.screenTilt.subset = project_ini.value(EcIni::INI_SCREEN_TILT_11, 0).toInt();

        ec_project_state_.screenTilt.angles.clear();
        int numAngles = countPlanarFitAngles(project_ini.allKeys());
        // iterate through angle list
        for (int k = 0; k < numAngles; ++k)
        {
            QString prefix = EcIni::INI_SCREEN_TILT_PREFIX + QString::number(k + 1) + QStringLiteral("_");

            int exclude = project_ini.value(prefix + EcIni::INI_SCREEN_TILT_10).toInt();
            int include = exclude ? 0 : (exclude + 2);
            Qt::CheckState included = static_cast<Qt::CheckState>(include);

            AngleItem item;
            item.angle_ = project_ini.value(prefix + EcIni::INI_SCREEN_TILT_9).toDouble();
            item.included_ = included;
            item.color_ = WidgetUtils::getColor(k);
            addPlanarFitAngle(item);
        }

    project_ini.endGroup();

    // time lag opt section
    project_ini.beginGroup(EcIni::INIGROUP_TIMELAG_OPT);
        ec_project_state_.timelagOpt.start_date = project_ini.value(EcIni::INI_TIMELAG_OPT_0, QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.timelagOpt.end_date = project_ini.value(EcIni::INI_TIMELAG_OPT_1, QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.timelagOpt.mode = project_ini.value(EcIni::INI_TIMELAG_OPT_2, 1).toInt();
        ec_project_state_.timelagOpt.file = project_ini.value(EcIni::INI_TIMELAG_OPT_3, QString()).toString();
        ec_project_state_.timelagOpt.to_h2o_nclass = project_ini.value(EcIni::INI_TIMELAG_OPT_17, 10).toInt();
        ec_project_state_.timelagOpt.co2_min_flux = project_ini.value(EcIni::INI_TIMELAG_OPT_4, 2.0).toDouble();
        ec_project_state_.timelagOpt.ch4_min_flux = project_ini.value(EcIni::INI_TIMELAG_OPT_5, 0.2).toDouble();
        ec_project_state_.timelagOpt.gas4_min_flux = project_ini.value(EcIni::INI_TIMELAG_OPT_6, 0.02).toDouble();
        ec_project_state_.timelagOpt.le_min_flux = project_ini.value(EcIni::INI_TIMELAG_OPT_7, 20.0).toDouble();
        ec_project_state_.timelagOpt.pg_range = project_ini.value(EcIni::INI_TIMELAG_OPT_8, 1.5).toDouble();
        ec_project_state_.timelagOpt.co2_min_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_9, -1000.1).toDouble();
        ec_project_state_.timelagOpt.co2_max_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_10, -1000.1).toDouble();
        ec_project_state_.timelagOpt.h2o_min_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_11, -1000.1).toDouble();
        ec_project_state_.timelagOpt.h2o_max_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_12, -1000.1).toDouble();
        ec_project_state_.timelagOpt.ch4_min_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_13, -1000.1).toDouble();
        ec_project_state_.timelagOpt.ch4_max_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_14, -1000.1).toDouble();
        ec_project_state_.timelagOpt.gas4_min_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_15, -1000.1).toDouble();
        ec_project_state_.timelagOpt.gas4_max_lag = project_ini.value(EcIni::INI_TIMELAG_OPT_16, -1000.1).toDouble();
        ec_project_state_.timelagOpt.subset = project_ini.value(EcIni::INI_TIMELAG_OPT_18, 0).toInt();
    project_ini.endGroup();

    // random error section
    project_ini.beginGroup(EcIni::INIGROUP_RAND_ERROR);
        ec_project_state_.randomError.method = project_ini.value(EcIni::INI_RAND_ERROR_0, 0).toInt();
        ec_project_state_.randomError.its_method = project_ini.value(EcIni::INI_RAND_ERROR_1, 1).toInt();
        ec_project_state_.randomError.its_tlag_max = project_ini.value(EcIni::INI_RAND_ERROR_2, 10.0).toDouble();

        // NOTE: temporarly disabled
//        ec_project_state_.randomError.its_sec_factor = project_ini.value(EcIni::INI_RAND_ERROR_3, 20.0).toDouble();
    project_ini.endGroup();

    // biomet section
    project_ini.beginGroup(EcIni::INIGROUP_BIOMET);
        ec_project_state_.biomParam.col_ta = project_ini.value(EcIni::INI_BIOMET_4, -1).toInt() + 1000;
        ec_project_state_.biomParam.col_pa = project_ini.value(EcIni::INI_BIOMET_5, -1).toInt() + 1000;
        ec_project_state_.biomParam.col_rh = project_ini.value(EcIni::INI_BIOMET_6, -1).toInt();
        ec_project_state_.biomParam.col_rg = project_ini.value(EcIni::INI_BIOMET_7, -1).toInt();
        ec_project_state_.biomParam.col_lwin = project_ini.value(EcIni::INI_BIOMET_8, -1).toInt();
        ec_project_state_.biomParam.col_ppfd = project_ini.value(EcIni::INI_BIOMET_9, -1).toInt();
    project_ini.endGroup();

    datafile.close();

    // just loaded projects are not modified
    setModified(false);
    emit ecProjectChanged();

    if (!isVersionCompatible)
        *modified = true;

//    QApplication::restoreOverrideCursor();
    return true;
}

bool EcProject::nativeFormat(const QString &filename)
{
    DEBUG_FUNC_NAME

    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open file: doesn't exists (check the path) "
                   << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename).arg(datafile.errorString()));
        return false;
    }

    // test if the first row of the file start with the correct tag
    // case sensitive for default
    QTextStream in(&datafile);
    QString firstLine;
    in >> firstLine;
    datafile.close();
    // filter metadata files
    if (firstLine.startsWith(Defs::APP_MD_INI_TAG)
        || firstLine.startsWith(QLatin1String(";ECO2S_METADATA"))
        || firstLine.startsWith(QLatin1String(";ECO2S_DATALOGGING"))
        || firstLine.startsWith(QLatin1String(";ECO2catch"))
        || firstLine.startsWith(QLatin1String(";ECCOCatch")))
    {
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file <p>%1:</p>\n"
                                "<b>not in %2 native format.</b>").arg(filename).arg(Defs::APP_NAME));
        return false;
    }
    // filter generic files
    if (!firstLine.startsWith(Defs::APP_PD_INI_TAG)
        && !firstLine.startsWith(QLatin1String(";ECO2S_PROCESSING"))
        && !firstLine.startsWith(QLatin1String(";ECO2S_DATAPROCESSING")))
    {
        WidgetUtils::warning(nullptr,
                             tr("Load Error"),
                             tr("Cannot read file <p>%1:</p>\n"
                                "<b>not in %2 native format.</b>")
                             .arg(filename).arg(Defs::APP_NAME));
        return false;
    }

    return true;
}

bool EcProject::tagProject(const QString &filename)
{
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot tag file" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Write Error"),
                             tr("Cannot write file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QString app_tag(Defs::APP_PD_INI_TAG);
    app_tag += QLatin1String("\n");

    QTextStream out(&datafile);
    QString textfile = out.readAll();
    textfile.prepend(app_tag);
    datafile.resize(0);
    out << textfile;
    datafile.close();

    return true;
}

void EcProject::setModified(bool mod)
{
    modified_ = mod;
    if (mod)
    {
//        DEBUG_FUNC_NAME
//        DEBUG_FUNC_MSG(tr("project modified"));
        emit ecProjectModified();
    }
}

bool EcProject::modified() const
{
    return modified_;
}

void EcProject::setGeneralSubset(int n)
{
    ec_project_state_.projectGeneral.subset = n;
    setModified(true);
}

void EcProject::setSpectraMinSmpl(int n)
{
    ec_project_state_.spectraSettings.sa_min_smpl = n;
    setModified(true);
}

void EcProject::setSpectraFminCo2(double d)
{
    ec_project_state_.spectraSettings.sa_fmin_co2 = d;
    setModified(true);
}

void EcProject::setSpectraFminH2o(double d)
{
    ec_project_state_.spectraSettings.sa_fmin_h2o = d;
    setModified(true);
}

void EcProject::setSpectraFminCh4(double d)
{
    ec_project_state_.spectraSettings.sa_fmin_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraFminGas4(double d)
{
    ec_project_state_.spectraSettings.sa_fmin_gas4 = d;
    setModified(true);
}

void EcProject::setSpectraFmaxCo2(double d)
{
    ec_project_state_.spectraSettings.sa_fmax_co2 = d;
    setModified(true);
}

void EcProject::setSpectraFmaxH2o(double d)
{
    ec_project_state_.spectraSettings.sa_fmax_h2o = d;
    setModified(true);
}

void EcProject::setSpectraFmaxCh4(double d)
{
    ec_project_state_.spectraSettings.sa_fmax_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraFmaxGas4(double d)
{
    ec_project_state_.spectraSettings.sa_fmax_gas4 = d;
    setModified(true);
}

void EcProject::setSpectraHfnCo2(double d)
{
    ec_project_state_.spectraSettings.sa_hfn_co2_fmin = d;
    setModified(true);
}

void EcProject::setSpectraHfnH2o(double d)
{
    ec_project_state_.spectraSettings.sa_hfn_h2o_fmin = d;
    setModified(true);
}

void EcProject::setSpectraHfnCh4(double d)
{
    ec_project_state_.spectraSettings.sa_hfn_ch4_fmin= d;
    setModified(true);
}

void EcProject::setSpectraHfnGas4(double d)
{
    ec_project_state_.spectraSettings.sa_hfn_gas4_fmin = d;
    setModified(true);
}

void EcProject::setSpectraMinCo2(double d)
{
    ec_project_state_.spectraSettings.sa_min_co2 = d;
    setModified(true);
}

void EcProject::setSpectraMinCh4(double d)
{
    ec_project_state_.spectraSettings.sa_min_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraMinGas4(double d)
{
    ec_project_state_.spectraSettings.sa_min_gas4 = d;
    setModified(true);
}

void EcProject::setSpectraMinLe(double d)
{
    ec_project_state_.spectraSettings.sa_min_le = d;
    setModified(true);
}

void EcProject::setSpectraMinH(double d)
{
    ec_project_state_.spectraSettings.sa_min_h = d;
    setModified(true);
}

void EcProject::setSpectraHorst(int n)
{
    ec_project_state_.spectraSettings.horst_lens = n;
    setModified(true);
}

void EcProject::setSpectraExFile(const QString &p)
{
    ec_project_state_.spectraSettings.ex_file = p;
}

void EcProject::setSpectraBinSpectra(const QString &p)
{
    ec_project_state_.spectraSettings.sa_bin_spectra = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setSpectraFullSpectra(const QString &p)
{
    ec_project_state_.spectraSettings.sa_full_spectra = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setSpectraExDir(const QString &p)
{
    ec_project_state_.spectraSettings.ex_dir = p;
    setModified(true);
}

void EcProject::setSpectraSubset(int n)
{
    ec_project_state_.spectraSettings.subset = n;
    setModified(true);
}

void EcProject::setSpectraAddSonic(int n)
{
    ec_project_state_.spectraSettings.add_sonic_lptf = n;
    setModified(true);
}

void EcProject::setSpectraF10Co2(double d)
{
    ec_project_state_.spectraSettings.f10_co2_trshld = d;
    setModified(true);
}

void EcProject::setSpectraF10Ch4(double d)
{
    ec_project_state_.spectraSettings.f10_ch4_trshld = d;
    setModified(true);
}

void EcProject::setSpectraF10Gas4(double d)
{
    ec_project_state_.spectraSettings.f10_gas4_trshld = d;
    setModified(true);
}

void EcProject::setSpectraF10Le(double d)
{
    ec_project_state_.spectraSettings.f10_le_trshld = d;
    setModified(true);
}

void EcProject::setSpectraF10H(double d)
{
    ec_project_state_.spectraSettings.f10_h_trshld = d;
    setModified(true);
}

void EcProject::setScreenDataPath(const QString &p)
{
    ec_project_state_.screenGeneral.data_path = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setScreenRecurse(int n)
{
    ec_project_state_.screenGeneral.recurse = n;
    setModified(true);
}

void EcProject::setScreenUseGeoNorth(bool b)
{
    ec_project_state_.screenGeneral.use_geo_north = b;
    setModified(true);
    emit updateInfo();
}

void EcProject::setScreenMagDec(double n)
{
    ec_project_state_.screenGeneral.mag_dec = n;
    setModified(true);
}

void EcProject::setScreenDecDate(const QString &d)
{
    ec_project_state_.screenGeneral.dec_date = d;
    setModified(true);
}

void EcProject::setScreenFlag1Col(int n)
{
    ec_project_state_.screenGeneral.flag1_col = n;
    setModified(true);
}

void EcProject::setScreenFlag1Threshold(double n)
{
    ec_project_state_.screenGeneral.flag1_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag1Policy(int n)
{
    ec_project_state_.screenGeneral.flag1_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag2Col(int n)
{
    ec_project_state_.screenGeneral.flag2_col = n;
    setModified(true);
}

void EcProject::setScreenFlag2Threshold(double n)
{
    ec_project_state_.screenGeneral.flag2_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag2Policy(int n)
{
    ec_project_state_.screenGeneral.flag2_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag3Col(int n)
{
    ec_project_state_.screenGeneral.flag3_col = n;
    setModified(true);
}

void EcProject::setScreenFlag3Threshold(double n)
{
    ec_project_state_.screenGeneral.flag3_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag3Policy(int n)
{
    ec_project_state_.screenGeneral.flag3_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag4Col(int n)
{
    ec_project_state_.screenGeneral.flag4_col = n;
    setModified(true);
}

void EcProject::setScreenFlag4Threshold(double n)
{
    ec_project_state_.screenGeneral.flag4_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag4Policy(int n)
{
    ec_project_state_.screenGeneral.flag4_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag5Col(int n)
{
    ec_project_state_.screenGeneral.flag5_col = n;
    setModified(true);
}

void EcProject::setScreenFlag5Threshold(double n)
{
    ec_project_state_.screenGeneral.flag5_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag5Policy(int n)
{
    ec_project_state_.screenGeneral.flag5_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag6Col(int n)
{
    ec_project_state_.screenGeneral.flag6_col = n;
    setModified(true);
}

void EcProject::setScreenFlag6Threshold(double n)
{
    ec_project_state_.screenGeneral.flag6_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag6Policy(int n)
{
    ec_project_state_.screenGeneral.flag6_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag7Col(int n)
{
    ec_project_state_.screenGeneral.flag7_col = n;
    setModified(true);
}

void EcProject::setScreenFlag7Threshold(double n)
{
    ec_project_state_.screenGeneral.flag7_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag7Policy(int n)
{
    ec_project_state_.screenGeneral.flag7_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag8Col(int n)
{
    ec_project_state_.screenGeneral.flag8_col = n;
    setModified(true);
}

void EcProject::setScreenFlag8Threshold(double n)
{
    ec_project_state_.screenGeneral.flag8_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag8Policy(int n)
{
    ec_project_state_.screenGeneral.flag8_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag9Col(int n)
{
    ec_project_state_.screenGeneral.flag9_col = n;
    setModified(true);
}

void EcProject::setScreenFlag9Threshold(double n)
{
    ec_project_state_.screenGeneral.flag9_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag9Policy(int n)
{
    ec_project_state_.screenGeneral.flag9_policy= n;
    setModified(true);
}

void EcProject::setScreenFlag10Col(int n)
{
    ec_project_state_.screenGeneral.flag10_col = n;
    setModified(true);
}

void EcProject::setScreenFlag10Threshold(double n)
{
    ec_project_state_.screenGeneral.flag10_threshold = n;
    setModified(true);
}

void EcProject::setScreenFlag10Policy(int n)
{
    ec_project_state_.screenGeneral.flag10_policy= n;
    setModified(true);
}

void EcProject::setGeneralOutPath(const QString &p)
{
    ec_project_state_.projectGeneral.out_path = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralFixedOutFormat(int n)
{
    ec_project_state_.projectGeneral.fix_out_format = n;
    setModified(true);
}

void EcProject::setGeneralErrorLabel(const QString &p)
{
    ec_project_state_.projectGeneral.err_label = p;
    setModified(true);
}

void EcProject::setGeneralOutMeanCosp(int n)
{
    ec_project_state_.projectGeneral.out_mean_cosp = n;
    setModified(true);
}

void EcProject::setGeneralBinSpectraAvail(int n)
{
    ec_project_state_.projectGeneral.bin_sp_avail = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralFullSpectraAvail(int n)
{
    ec_project_state_.projectGeneral.full_sp_avail = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralFilesFound(int n)
{
    ec_project_state_.projectGeneral.files_found = n;

    // NOTE: create side effects when loading a project and then a refresh.
    // in fact, the corresponding recursion checkbox is enough to inform
    // about a possible interactive change
//    setModified(true);
}

void EcProject::setScreenNFiles(int n)
{
    ec_project_state_.screenSetting.nfiles = n;
    setModified(true);
}

void EcProject::setScreenMaxLack(int n)
{
    ec_project_state_.screenSetting.max_lack = n;
    setModified(true);
}

void EcProject::setScreenUOffset(double d)
{
    ec_project_state_.screenSetting.u_offset = d;
    setModified(true);
}

void EcProject::setScreenVOffset(double d)
{
    ec_project_state_.screenSetting.v_offset = d;
    setModified(true);
}

void EcProject::setScreenWOffset(double d)
{
    ec_project_state_.screenSetting.w_offset = d;
    setModified(true);
}

void EcProject::setScreenFlowDistortion(int n)
{
    ec_project_state_.screenSetting.flow_distortion = n;
    setModified(true);
}

void EcProject::setScreenToMixRatio(int n)
{
    ec_project_state_.screenSetting.to_mixratio = n;
    setModified(true);
}

void EcProject::setScreenFilterSr(int n)
{
    ec_project_state_.screenSetting.filter_sr = n;
    setModified(true);
}

void EcProject::setScreenFilterAl(int n)
{
    ec_project_state_.screenSetting.filter_al = n;
    setModified(true);
}

void EcProject::setScreenCrossWind(int n)
{
    ec_project_state_.screenSetting.cross_wind = n;
    setModified(true);
}

void EcProject::setScreenRotMethod(int n)
{
    ec_project_state_.screenSetting.rot_meth = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setScreenDetrendMeth(int n)
{
    ec_project_state_.screenSetting.detrend_meth = n;
    setModified(true);
}

void EcProject::setScreenTimeConst(double n)
{
    ec_project_state_.screenSetting.timeconst = n;
    setModified(true);
}

void EcProject::setScreenTlagMeth(int n)
{
    ec_project_state_.screenSetting.tlag_meth = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setScreenTapWin(int n)
{
    ec_project_state_.screenSetting.tap_win = n;
    setModified(true);
}

void EcProject::setScreenNBins(int n)
{
    ec_project_state_.screenSetting.nbins = n;
    setModified(true);
}

void EcProject::setScreenOutBinSpectra(int n)
{
    ec_project_state_.screenSetting.out_bin_sp = n;
    setModified(true);
}

void EcProject::setScreenOutBinOgives(int n)
{
    ec_project_state_.screenSetting.out_bin_og = n;
    setModified(true);
}
void EcProject::setScreenOutFullSpectraU(int n)
{
    ec_project_state_.screenSetting.out_full_sp_u = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraV(int n)
{
    ec_project_state_.screenSetting.out_full_sp_v = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraW(int n)
{
    ec_project_state_.screenSetting.out_full_sp_w = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraTs(int n)
{
    ec_project_state_.screenSetting.out_full_sp_ts = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraCo2(int n)
{
    ec_project_state_.screenSetting.out_full_sp_co2 = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraH2o(int n)
{
    ec_project_state_.screenSetting.out_full_sp_h2o = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraCh4(int n)
{
    ec_project_state_.screenSetting.out_full_sp_ch4 = n;
    setModified(true);
}

void EcProject::setScreenOutFullSpectraN2o(int n)
{
    ec_project_state_.screenSetting.out_full_sp_n2o = n;
    setModified(true);
}

void EcProject::setScreenOutSt1(int n)
{
    ec_project_state_.screenSetting.out_st_1 = n;
    setModified(true);
}

void EcProject::setScreenOutSt2(int n)
{
    ec_project_state_.screenSetting.out_st_2 = n;
    setModified(true);
}

void EcProject::setScreenOutSt3(int n)
{
    ec_project_state_.screenSetting.out_st_3 = n;
    setModified(true);
}

void EcProject::setScreenOutSt4(int n)
{
    ec_project_state_.screenSetting.out_st_4 = n;
    setModified(true);
}

void EcProject::setScreenOutSt5(int n)
{
    ec_project_state_.screenSetting.out_st_5 = n;
    setModified(true);
}

void EcProject::setScreenOutSt6(int n)
{
    ec_project_state_.screenSetting.out_st_6 = n;
    setModified(true);
}

void EcProject::setScreenOutSt7(int n)
{
    ec_project_state_.screenSetting.out_st_7 = n;
    setModified(true);
}
void EcProject::setScreenOutRaw1(int n)
{
    ec_project_state_.screenSetting.out_raw_1 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw2(int n)
{
    ec_project_state_.screenSetting.out_raw_2 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw3(int n)
{
    ec_project_state_.screenSetting.out_raw_3 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw4(int n)
{
    ec_project_state_.screenSetting.out_raw_4 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw5(int n)
{
    ec_project_state_.screenSetting.out_raw_5 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw6(int n)
{
    ec_project_state_.screenSetting.out_raw_6 = n;
    setModified(true);
}

void EcProject::setScreenOutRaw7(int n)
{
    ec_project_state_.screenSetting.out_raw_7 = n;
    setModified(true);
}
void EcProject::setScreenOutRawU(int n)
{
    ec_project_state_.screenSetting.out_raw_u = n;
    setModified(true);
}

void EcProject::setScreenOutRawV(int n)
{
    ec_project_state_.screenSetting.out_raw_v = n;
    setModified(true);
}

void EcProject::setScreenOutRawW(int n)
{
    ec_project_state_.screenSetting.out_raw_w = n;
    setModified(true);
}

void EcProject::setScreenOutRawTs(int n)
{
    ec_project_state_.screenSetting.out_raw_ts = n;
    setModified(true);
}

void EcProject::setScreenOutRawCo2(int n)
{
    ec_project_state_.screenSetting.out_raw_co2 = n;
    setModified(true);
}

void EcProject::setScreenOutRawH2o(int n)
{
    ec_project_state_.screenSetting.out_raw_h2o = n;
    setModified(true);
}

void EcProject::setScreenOutRawCh4(int n)
{
    ec_project_state_.screenSetting.out_raw_ch4 = n;
    setModified(true);
}

void EcProject::setScreenOutRawGas4(int n)
{
    ec_project_state_.screenSetting.out_raw_gas4 = n;
    setModified(true);
}

void EcProject::setScreenOutRawTair(int n)
{
    ec_project_state_.screenSetting.out_raw_tair = n;
    setModified(true);
}

void EcProject::setScreenOutRawPair(int n)
{
    ec_project_state_.screenSetting.out_raw_pair = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraU(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_u = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraV(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_v = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraTs(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_ts = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraCo2(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_co2 = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraH2o(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_h2o = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraCh4(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_ch4 = n;
    setModified(true);
}

void EcProject::setScreenOutFullCospectraN2o(int n)
{
    ec_project_state_.screenSetting.out_full_cosp_n2o = n;
    setModified(true);
}

void EcProject::setGeneralFpMeth(int n)
{
    ec_project_state_.projectGeneral.foot_meth = n;
    setModified(true);
}

void EcProject::setGeneralTob1Format(int n)
{
    ec_project_state_.projectGeneral.tob1_format = n;
    setModified(true);
}

void EcProject::setGeneralOutGhgEu(int n)
{
    ec_project_state_.projectGeneral.out_ghg_eu = n;
    setModified(true);
}

void EcProject::setGeneralOutAmFluxOut(int n)
{
    ec_project_state_.projectGeneral.out_amflux = n;
    setModified(true);
}

void EcProject::setGeneralOutRich(int n)
{
    ec_project_state_.projectGeneral.out_rich = n;
    setModified(true);
}

void EcProject::setGeneralOutMd(int n)
{
    ec_project_state_.projectGeneral.out_md = n;
    setModified(true);
}

void EcProject::setGeneralOutBiomet(int n)
{
    ec_project_state_.projectGeneral.out_biomet = n;
    setModified(true);
}

void EcProject::setScreenTestSr(int l)
{
    ec_project_state_.screenTest.test_sr = l;
    setModified(true);
}

void EcProject::setScreenTestAr(int l)
{
    ec_project_state_.screenTest.test_ar = l;
    setModified(true);
}

void EcProject::setScreenTestDo(int l)
{
    ec_project_state_.screenTest.test_do = l;
    setModified(true);
}

void EcProject::setScreenTestAl(int l)
{
    ec_project_state_.screenTest.test_al = l;
    setModified(true);
}

void EcProject::setScreenTestSk(int l)
{
    ec_project_state_.screenTest.test_sk = l;
    setModified(true);
}

void EcProject::setScreenTestDs(int l)
{
    ec_project_state_.screenTest.test_ds = l;
    setModified(true);
}

void EcProject::setScreenTestTl(int l)
{
    ec_project_state_.screenTest.test_tl = l;
    setModified(true);
}

void EcProject::setScreenTestAa(int l)
{
    ec_project_state_.screenTest.test_aa = l;
    setModified(true);
}

void EcProject::setScreenTestNs(int l)
{
    ec_project_state_.screenTest.test_ns = l;
    setModified(true);
}

void EcProject::setScreenParamSrNumSpk(int n)
{
    ec_project_state_.screenParam.sr_num_spk = n;
    setModified(true);
}

void EcProject::setScreenParamSrHfLim(double n)
{
    ec_project_state_.screenParam.sr_lim_hf = n;
    setModified(true);
}

void EcProject::setScreenParamSrULim(double n)
{
    ec_project_state_.screenParam.sr_lim_u = n;
    setModified(true);
}

void EcProject::setScreenParamSrWLim(double n)
{
    ec_project_state_.screenParam.sr_lim_w = n;
    setModified(true);
}

void EcProject::setScreenParamSrCo2Lim(double n)
{
    ec_project_state_.screenParam.sr_lim_co2 = n;
    setModified(true);
}

void EcProject::setScreenParamSrH2oLim(double n)
{
    ec_project_state_.screenParam.sr_lim_h2o = n;
    setModified(true);
}

void EcProject::setScreenParamSrCh4Lim(double n)
{
    ec_project_state_.screenParam.sr_lim_ch4 = n;
    setModified(true);
}

void EcProject::setScreenParamSrN2oLim(double n)
{
    ec_project_state_.screenParam.sr_lim_n2o = n;
    setModified(true);
}

void EcProject::setScreenParamArLim(double n)
{
    ec_project_state_.screenParam.ar_lim = n;
    setModified(true);
}

void EcProject::setScreenParamArBins(int n)
{
    ec_project_state_.screenParam.ar_bins = n;
    setModified(true);
}

void EcProject::setScreenParamArHfLim(int n)
{
    ec_project_state_.screenParam.ar_hf_lim = n;
    setModified(true);
}

void EcProject::setScreenParamDoExtLimDw(int n)
{
    ec_project_state_.screenParam.do_extlim_dw = n;
    setModified(true);
}

void EcProject::setScreenParamDoHf1Lim(double n)
{
    ec_project_state_.screenParam.do_hf1_lim = n;
    setModified(true);
}

void EcProject::setScreenParamDoHf2Lim(double n)
{
    ec_project_state_.screenParam.do_hf2_lim = n;
    setModified(true);
}

void EcProject::setScreenParamAlUMax(double n)
{
    ec_project_state_.screenParam.al_u_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlWMax(double n)
{
    ec_project_state_.screenParam.al_w_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlTsonMin(double n)
{
    ec_project_state_.screenParam.al_tson_min = n;
    setModified(true);
}

void EcProject::setScreenParamAlTsonMax(double n)
{
    ec_project_state_.screenParam.al_tson_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlCo2Min(double n)
{
    ec_project_state_.screenParam.al_co2_min = n;
    setModified(true);
}

void EcProject::setScreenParamAlCo2Max(double n)
{
    ec_project_state_.screenParam.al_co2_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlH2oMin(double n)
{
    ec_project_state_.screenParam.al_h2o_min = n;
    setModified(true);
}

void EcProject::setScreenParamAlH2oMax(double n)
{
    ec_project_state_.screenParam.al_h2o_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlCh4Min(double n)
{
    ec_project_state_.screenParam.al_ch4_min = n;
    setModified(true);
}

void EcProject::setScreenParamAlCh4Max(double n)
{
    ec_project_state_.screenParam.al_ch4_max = n;
    setModified(true);
}

void EcProject::setScreenParamAlN2oMin(double n)
{
    ec_project_state_.screenParam.al_n2o_min = n;
    setModified(true);
}

void EcProject::setScreenParamAlN2oMax(double n)
{
    ec_project_state_.screenParam.al_n2o_max = n;
    setModified(true);
}

void EcProject::setScreenParamSkHfSkmin(double n)
{
    ec_project_state_.screenParam.sk_hf_skmin = n;
    setModified(true);
}

void EcProject::setScreenParamSkHfSkmax(double n)
{
    ec_project_state_.screenParam.sk_hf_skmax = n;
    setModified(true);
}

void EcProject::setScreenParamSkSfSkmin(double n)
{
    ec_project_state_.screenParam.sk_sf_skmin = n;
    setModified(true);
}

void EcProject::setScreenParamSkSfSkmax(double n)
{
    ec_project_state_.screenParam.sk_sf_skmax = n;
    setModified(true);
}

void EcProject::setScreenParamSkHfKumin(double n)
{
    ec_project_state_.screenParam.sk_hf_kumin = n;
    setModified(true);
}

void EcProject::setScreenParamSkHfKumax(double n)
{
    ec_project_state_.screenParam.sk_hf_kumax = n;
    setModified(true);
}

void EcProject::setScreenParamSkSfKumin(double n)
{
    ec_project_state_.screenParam.sk_sf_kumin = n;
    setModified(true);
}

void EcProject::setScreenParamSkSfKumax(double n)
{
    ec_project_state_.screenParam.sk_sf_kumax = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfUV(double n)
{
    ec_project_state_.screenParam.ds_hf_uv = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfW(double n)
{
    ec_project_state_.screenParam.ds_hf_w = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfT(double n)
{
    ec_project_state_.screenParam.ds_hf_t = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfCo2(double n)
{
    ec_project_state_.screenParam.ds_hf_co2 = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfH2o(double n)
{
    ec_project_state_.screenParam.ds_hf_h2o = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfCh4(double n)
{
    ec_project_state_.screenParam.ds_hf_ch4 = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfN2o(double n)
{
    ec_project_state_.screenParam.ds_hf_n2o = n;
    setModified(true);
}

void EcProject::setScreenParamDsHfVar(double n)
{
    ec_project_state_.screenParam.ds_hf_var = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfUV(double n)
{
    ec_project_state_.screenParam.ds_sf_uv = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfW(double n)
{
    ec_project_state_.screenParam.ds_sf_w = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfT(double n)
{
    ec_project_state_.screenParam.ds_sf_t = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfCo2(double n)
{
    ec_project_state_.screenParam.ds_sf_co2 = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfH2o(double n)
{
    ec_project_state_.screenParam.ds_sf_h2o = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfCh4(double n)
{
    ec_project_state_.screenParam.ds_sf_ch4 = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfN2o(double n)
{
    ec_project_state_.screenParam.ds_sf_n2o = n;
    setModified(true);
}

void EcProject::setScreenParamDsSfVar(double n)
{
    ec_project_state_.screenParam.ds_sf_var = n;
    setModified(true);
}

void EcProject::setScreenParamTlHfLim(double n)
{
    ec_project_state_.screenParam.tl_hf_lim = n;
    setModified(true);
}

void EcProject::setScreenParamTlSfLim(double n)
{
    ec_project_state_.screenParam.tl_sf_lim = n;
    setModified(true);
}

void EcProject::setScreenParamTlDefCo2(double n)
{
    ec_project_state_.screenParam.tl_def_co2 = n;
    setModified(true);
}

void EcProject::setScreenParamTlDefH2o(double n)
{
    ec_project_state_.screenParam.tl_def_h2o = n;
    setModified(true);
}

void EcProject::setScreenParamTlDefCh4(double n)
{
    ec_project_state_.screenParam.tl_def_ch4 = n;
    setModified(true);
}

void EcProject::setScreenParamTlDefN2o(double n)
{
    ec_project_state_.screenParam.tl_def_n2o = n;
    setModified(true);
}

void EcProject::setScreenParamAaMin(double n)
{
    ec_project_state_.screenParam.aa_min = n;
    setModified(true);
}

void EcProject::setScreenParamAaMax(double n)
{
    ec_project_state_.screenParam.aa_max = n;
    setModified(true);
}

void EcProject::setScreenParamAaLim(double n)
{
    ec_project_state_.screenParam.aa_lim = n;
    setModified(true);
}

void EcProject::setScreenParamNsHfLim(double n)
{
    ec_project_state_.screenParam.ns_hf_lim = n;
    setModified(true);
}

void EcProject::setGeneralStartDate(const QString &d)
{
    ec_project_state_.projectGeneral.start_date = d;
    setModified(true);
}

void EcProject::setGeneralEndDate(const QString &d)
{
    ec_project_state_.projectGeneral.end_date = d;
    setModified(true);
}

void EcProject::setGeneralStartTime(const QString &t)
{
    ec_project_state_.projectGeneral.start_time = t;
    setModified(true);
}

void EcProject::setGeneralEndTime(const QString &t)
{
    ec_project_state_.projectGeneral.end_time = t;
    setModified(true);
}

void EcProject::setGeneralHfMethod(int n)
{
    ec_project_state_.projectGeneral.hf_meth = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralLfMethod(int n)
{
    ec_project_state_.projectGeneral.lf_meth = n;
    setModified(true);
}

void EcProject::setScreenBuCorr(int n)
{
    ec_project_state_.screenSetting.bu_corr = n;
    setModified(true);
}

void EcProject::setScreenBuMulti(int n)
{
    ec_project_state_.screenSetting.bu_multi = n;
    setModified(true);
}

void EcProject::setScreenLDayBotGain(double d)
{
    ec_project_state_.screenSetting.l_day_bot_gain = d;
    setModified(true);
}

void EcProject::setScreenLDayBotOffset(double d)
{
    ec_project_state_.screenSetting.l_day_bot_offset = d;
    setModified(true);
}

void EcProject::setScreenLDayTopGain(double d)
{
    ec_project_state_.screenSetting.l_day_top_gain = d;
    setModified(true);
}

void EcProject::setScreenLDayTopOffset(double d)
{
    ec_project_state_.screenSetting.l_day_top_offset = d;
    setModified(true);
}

void EcProject::setScreenLDaySparGain(double d)
{
    ec_project_state_.screenSetting.l_day_spar_gain = d;
    setModified(true);
}

void EcProject::setScreenLDaySparOffset(double d)
{
    ec_project_state_.screenSetting.l_day_spar_offset = d;
    setModified(true);
}

void EcProject::setScreenLNightBotGain(double d)
{
    ec_project_state_.screenSetting.l_night_bot_gain = d;
    setModified(true);
}

void EcProject::setScreenLNightBotOffset(double d)
{
    ec_project_state_.screenSetting.l_night_bot_offset = d;
    setModified(true);
}

void EcProject::setScreenLNightTopGain(double d)
{
    ec_project_state_.screenSetting.l_night_top_gain = d;
    setModified(true);
}

void EcProject::setScreenLNightTopOffset(double d)
{
    ec_project_state_.screenSetting.l_night_top_offset = d;
    setModified(true);
}

void EcProject::setScreenLNightSparGain(double d)
{
    ec_project_state_.screenSetting.l_night_spar_gain = d;
    setModified(true);
}

void EcProject::setScreenLNightSparOffset(double d)
{
    ec_project_state_.screenSetting.l_night_spar_offset = d;
    setModified(true);
}

void EcProject::setScreenMDayBot1(double d)
{
    ec_project_state_.screenSetting.m_day_bot1 = d;
    setModified(true);
}

void EcProject::setScreenMDayBot2(double d)
{
    ec_project_state_.screenSetting.m_day_bot2 = d;
    setModified(true);
}

void EcProject::setScreenMDayBot3(double d)
{
    ec_project_state_.screenSetting.m_day_bot3 = d;
    setModified(true);
}

void EcProject::setScreenMDayBot4(double d)
{
    ec_project_state_.screenSetting.m_day_bot4 = d;
    setModified(true);
}

void EcProject::setScreenMDayTop1(double d)
{
    ec_project_state_.screenSetting.m_day_top1 = d;
    setModified(true);
}

void EcProject::setScreenMDayTop2(double d)
{
    ec_project_state_.screenSetting.m_day_top2 = d;
    setModified(true);
}

void EcProject::setScreenMDayTop3(double d)
{
    ec_project_state_.screenSetting.m_day_top3 = d;
    setModified(true);
}

void EcProject::setScreenMDayTop4(double d)
{
    ec_project_state_.screenSetting.m_day_top4 = d;
    setModified(true);
}

void EcProject::setScreenMDaySpar1(double d)
{
    ec_project_state_.screenSetting.m_day_spar1 = d;
    setModified(true);
}

void EcProject::setScreenMDaySpar2(double d)
{
    ec_project_state_.screenSetting.m_day_spar2 = d;
    setModified(true);
}

void EcProject::setScreenMDaySpar3(double d)
{
    ec_project_state_.screenSetting.m_day_spar3 = d;
    setModified(true);
}

void EcProject::setScreenMDaySpar4(double d)
{
    ec_project_state_.screenSetting.m_day_spar4 = d;
    setModified(true);
}

void EcProject::setScreenMNightBot1(double d)
{
    ec_project_state_.screenSetting.m_night_bot1 = d;
    setModified(true);
}

void EcProject::setScreenMNightBot2(double d)
{
    ec_project_state_.screenSetting.m_night_bot2 = d;
    setModified(true);
}

void EcProject::setScreenMNightBot3(double d)
{
    ec_project_state_.screenSetting.m_night_bot3 = d;
    setModified(true);
}

void EcProject::setScreenMNightBot4(double d)
{
    ec_project_state_.screenSetting.m_night_bot4 = d;
    setModified(true);
}

void EcProject::setScreenMNightTop1(double d)
{
    ec_project_state_.screenSetting.m_night_top1 = d;
    setModified(true);
}

void EcProject::setScreenMNightTop2(double d)
{
    ec_project_state_.screenSetting.m_night_top2 = d;
    setModified(true);
}

void EcProject::setScreenMNightTop3(double d)
{
    ec_project_state_.screenSetting.m_night_top3 = d;
    setModified(true);
}

void EcProject::setScreenMNightTop4(double d)
{
    ec_project_state_.screenSetting.m_night_top4 = d;
    setModified(true);
}

void EcProject::setScreenMNightSpar1(double d)
{
    ec_project_state_.screenSetting.m_night_spar1 = d;
    setModified(true);
}

void EcProject::setScreenMNightSpar2(double d)
{
    ec_project_state_.screenSetting.m_night_spar2 = d;
    setModified(true);
}

void EcProject::setScreenMNightSpar3(double d)
{
    ec_project_state_.screenSetting.m_night_spar3 = d;
    setModified(true);
}

void EcProject::setScreenMNightSpar4(double d)
{
    ec_project_state_.screenSetting.m_night_spar4 = d;
    setModified(true);
}

void EcProject::setScreenlOutDetails(int n)
{
    ec_project_state_.screenSetting.out_details = n;
    setModified(true);
}

void EcProject::setScreenlPowerOfTwo(int n)
{
    ec_project_state_.screenSetting.power_of_two = n;
    setModified(true);
}

void EcProject::setGeneralRunMode(Defs::CurrRunMode mode)
{
    ec_project_state_.projectGeneral.run_mode = mode;
    setModified(true);
}

void EcProject::setGeneralRunFcc(bool yes)
{
    ec_project_state_.projectGeneral.run_fcc = yes;
    setModified(true);
}

void EcProject::setGeneralUseAltMdFile(bool b)
{
    ec_project_state_.projectGeneral.use_alt_md_file = b;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralMdFilepath(const QString &p)
{
    ec_project_state_.projectGeneral.md_file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralUseTimelineFile(bool b)
{
    ec_project_state_.projectGeneral.use_tlfile = b;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralTimelineFilepath(const QString &p)
{
    ec_project_state_.projectGeneral.timeline_file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralFileType(Defs::RawFileType type)
{
    ec_project_state_.projectGeneral.file_type = type;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBinaryHNLines(int n)
{
    ec_project_state_.projectGeneral.binary_hnlines = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBinaryEol(int n)
{
    ec_project_state_.projectGeneral.binary_eol = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBinaryNBytes(int n)
{
    ec_project_state_.projectGeneral.binary_nbytes = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBinaryLittleEnd(int n)
{
    ec_project_state_.projectGeneral.binary_little_end = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralTitle(const QString &s)
{
    ec_project_state_.projectGeneral.project_title = s;
    setModified(true);
}

void EcProject::setGeneralFileName(const QString &n)
{
    ec_project_state_.projectGeneral.file_name = n;
    setModified(true);
}

void EcProject::setGeneralId(const QString &id)
{
    ec_project_state_.projectGeneral.project_id = id;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralFilePrototype(const QString &f)
{
    ec_project_state_.projectGeneral.file_prototype = f;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralWplMeth(int n)
{
    ec_project_state_.projectGeneral.wpl_meth = n;
    setModified(true);
}

void EcProject::setGeneralColMasterSonic(const QString &s)
{
    DEBUG_FUNC_NAME
    ec_project_state_.projectGeneral.master_sonic = s;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralColCo2(int n)
{
    ec_project_state_.projectGeneral.col_co2 = n;
    setModified(true);
}

void EcProject::setGeneralColH2o(int n)
{
    ec_project_state_.projectGeneral.col_h2o = n;
    setModified(true);
}

void EcProject::setGeneralColCh4(int n)
{
    ec_project_state_.projectGeneral.col_ch4 = n;
    setModified(true);
}

void EcProject::setGeneralColGas4(int n)
{
    ec_project_state_.projectGeneral.col_n2o = n;
    setModified(true);
}

void EcProject::setGeneralColIntTc(int n)
{
    ec_project_state_.projectGeneral.col_int_t_c = n;
    setModified(true);
}

void EcProject::setGeneralColIntT1(int n)
{
    ec_project_state_.projectGeneral.col_int_t_1 = n;
    setModified(true);
}

void EcProject::setGeneralColIntT2(int n)
{
    ec_project_state_.projectGeneral.col_int_t_2 = n;
    setModified(true);
}

void EcProject::setGeneralColIntP(int n)
{
    ec_project_state_.projectGeneral.col_int_p = n;
    setModified(true);
}

void EcProject::setGeneralColAirT(int n)
{
    ec_project_state_.projectGeneral.col_air_t = n;
    setModified(true);
}

void EcProject::setGeneralColAirP(int n)
{
    ec_project_state_.projectGeneral.col_air_p = n;
    setModified(true);
}

void EcProject::setGeneralColDiag75(int n)
{
    ec_project_state_.projectGeneral.col_diag_75 = n;
    setModified(true);
}

void EcProject::setGeneralColDiag72(int n)
{
    ec_project_state_.projectGeneral.col_diag_72 = n;
    setModified(true);
}

void EcProject::setGeneralColDiag77(int n)
{
    ec_project_state_.projectGeneral.col_diag_77 = n;
    setModified(true);
}

void EcProject::setGeneralColGasMw(double n)
{
    ec_project_state_.projectGeneral.gas_mw = n;
    setModified(true);
}

void EcProject::setGeneralColTs(int n)
{
    ec_project_state_.projectGeneral.col_ts = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralColGasDiff(double n)
{
    ec_project_state_.projectGeneral.gas_diff = n;
    setModified(true);
}

void EcProject::setGeneralQcMeth(int n)
{
    ec_project_state_.projectGeneral.qcflag_meth = n;
    setModified(true);
}

void EcProject::setGeneralUseBiomet(int n)
{
    ec_project_state_.projectGeneral.use_biomet = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBiomFile(const QString &p)
{
    ec_project_state_.projectGeneral.biom_file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBiomDir(const QString &p)
{
    ec_project_state_.projectGeneral.biom_dir = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralBiomRecurse(int n)
{
    ec_project_state_.projectGeneral.biom_recurse = n;
    setModified(true);
}

void EcProject::setGeneralBiomExt(const QString &p)
{
    ec_project_state_.projectGeneral.biom_ext = p;
    setModified(true);
}

void EcProject::setScreenAvrgLen(int n)
{
    ec_project_state_.screenSetting.avrg_len = n;
    setModified(true);
}

void EcProject::setGeneralMakeDataset(bool b)
{
    ec_project_state_.projectGeneral.make_dataset = b;
    setModified(true);
}

void EcProject::setPlanarFitStartDate(const QString& date)
{
    ec_project_state_.screenTilt.start_date = date;
    setModified(true);
}

void EcProject::setPlanarFitEndDate(const QString& date)
{
    ec_project_state_.screenTilt.end_date = date;
    setModified(true);
}

void EcProject::setPlanarFitMode(int i)
{
    ec_project_state_.screenTilt.mode = i;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitNorthOffset(double d)
{
    ec_project_state_.screenTilt.north_offset = d;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitItemPerSector(int i)
{
    ec_project_state_.screenTilt.min_num_per_sec = i;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitWmax(double d)
{
    ec_project_state_.screenTilt.w_max = d;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitUmin(double d)
{
    ec_project_state_.screenTilt.u_min = d;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitFile(const QString &p)
{
    ec_project_state_.screenTilt.file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setPlanarFitPolicy(int i)
{
    ec_project_state_.screenTilt.fix_policy = i;
    setModified(true);
}

void EcProject::setPlanarFitSubset(int n)
{
    ec_project_state_.screenTilt.subset = n;
    setModified(true);
}

void EcProject::setSpectraStartDate(const QString& date)
{
    ec_project_state_.spectraSettings.start_sa_date = date;
    setModified(true);
}

void EcProject::setSpectraEndDate(const QString& date)
{
    ec_project_state_.spectraSettings.end_sa_date = date;
    setModified(true);
}

void EcProject::setSpectraMode(int i)
{
    ec_project_state_.spectraSettings.sa_mode = i;
    setModified(true);
    emit updateInfo();
}

void EcProject::setSpectraFile(const QString &p)
{
    ec_project_state_.spectraSettings.sa_file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setTimelagOptStartDate(const QString& date)
{
    ec_project_state_.timelagOpt.start_date = date;
    setModified(true);
}

void EcProject::setTimelagOptEndDate(const QString& date)
{
    ec_project_state_.timelagOpt.end_date = date;
    setModified(true);
}

void EcProject::setTimelagOptMode(int i)
{
    ec_project_state_.timelagOpt.mode = i;
    setModified(true);
}

void EcProject::setTimelagOptSubset(int i)
{
    ec_project_state_.timelagOpt.subset = i;
    setModified(true);
}

void EcProject::setTimelagOptFile(const QString &p)
{
    ec_project_state_.timelagOpt.file = p;
    setModified(true);
    emit updateInfo();
}

void EcProject::setTimelagOptH2oNClass(int n)
{
    ec_project_state_.timelagOpt.to_h2o_nclass = n;
    setModified(true);
}

void EcProject::setTimelagOptCo2MinFlux(double d)
{
    ec_project_state_.timelagOpt.co2_min_flux = d;
    setModified(true);
}

void EcProject::setTimelagOptCh4MinFlux(double d)
{
    ec_project_state_.timelagOpt.ch4_min_flux = d;
    setModified(true);
}

void EcProject::setTimelagOptGas4MinFlux(double d)
{
    ec_project_state_.timelagOpt.gas4_min_flux = d;
    setModified(true);
}

void EcProject::setTimelagOptLeMinFlux(double d)
{
    ec_project_state_.timelagOpt.le_min_flux = d;
    setModified(true);
}

void EcProject::setTimelagOptPgRange(double d)
{
    ec_project_state_.timelagOpt.pg_range = d;
    setModified(true);
}

void EcProject::setTimelagOptCo2MinLag(double d)
{
    ec_project_state_.timelagOpt.co2_min_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptCo2MaxLag(double d)
{
    ec_project_state_.timelagOpt.co2_max_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptH2oMinLag(double d)
{
    ec_project_state_.timelagOpt.h2o_min_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptH2oMaxLag(double d)
{
    ec_project_state_.timelagOpt.h2o_max_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptCh4MinLag(double d)
{
    ec_project_state_.timelagOpt.ch4_min_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptCh4MaxLag(double d)
{
    ec_project_state_.timelagOpt.ch4_max_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptGas4MinLag(double d)
{
    ec_project_state_.timelagOpt.gas4_min_lag = d;
    setModified(true);
}

void EcProject::setTimelagOptGas4MaxLag(double d)
{
    ec_project_state_.timelagOpt.gas4_max_lag = d;
    setModified(true);
}

void EcProject::setRandomErrorMethod(int n)
{
    ec_project_state_.randomError.method = n;
    setModified(true);
}

void EcProject::setRandomErrorItsMethod(int n)
{
    ec_project_state_.randomError.its_method = n;
    setModified(true);
}

void EcProject::setRandomErrorItsTlagMax(double d)
{
    ec_project_state_.randomError.its_tlag_max = d;
    setModified(true);
}

void EcProject::setRandomErrorItsSecFactor(double d)
{
    ec_project_state_.randomError.its_sec_factor = d;
    setModified(true);
}

bool EcProject::planarFitSectorDefined()
{
    return false;
}

QList<AngleItem>* EcProject::planarFitAngles()
{
    return &ec_project_state_.screenTilt.angles;
}

bool EcProject::hasPlanarFitFullAngle()
{
    QList<AngleItem>* angles = planarFitAngles();

    double angleSum = 0.0;
    for (int n = 0; n < angles->count(); ++n)
    {
        angleSum += angles->at(n).angle_;
    }

    return (angleSum == 360.0);
}

void EcProject::addPlanarFitAngle(const AngleItem& angle)
{
    ec_project_state_.screenTilt.angles.append(angle);
}

int EcProject::countPlanarFitAngles(const QStringList& list)
{
    int i = 0;
    foreach (const QString& s, list)
    {
        if (s.contains(QStringLiteral("width")))
            ++i;
    }
    return i;
}

void EcProject::setBiomParamColAirT(int n)
{
    ec_project_state_.biomParam.col_ta = n;
    setModified(true);
}

void EcProject::setBiomParamColAirP(int n)
{
    ec_project_state_.biomParam.col_pa = n;
    setModified(true);
}

void EcProject::setBiomParamColRh(int n)
{
    ec_project_state_.biomParam.col_rh = n;
    setModified(true);
}

void EcProject::setBiomParamColRg(int n)
{
    ec_project_state_.biomParam.col_rg = n;
    setModified(true);
}

void EcProject::setBiomParamColLwin(int n)
{
    ec_project_state_.biomParam.col_lwin = n;
    setModified(true);
}

void EcProject::setBiomParamColPpfd(int n)
{
    ec_project_state_.biomParam.col_ppfd = n;
    setModified(true);
}

const QString EcProject::getFilenamePrototype() const
{
    DEBUG_FUNC_NAME

    QString format;

    if (generalFileType() == Defs::RawFileType::GHG)
    {
        format = QStringLiteral("yyyy-mm-ddTHHMM_") + this->generalId()
                 + QStringLiteral(".") + Defs::GHG_NATIVE_DATA_FILE_EXT;
    }
    else
    {
        format = generalFilePrototype();
    }

    return format;
}

bool EcProject::isEngineStep2Needed()
{
    bool test = false;

    switch (generalHfMethod())
    {
        // case no HF spectral corrections or Moncrieff or Masssmann
        case 0:
        case 1:
        case 5:
            break;
        // case Horst, Ibrom, Fratini
        case 2:
        case 3:
        case 4:
            test = true;
            break;
        default:
            break;
    }

    // in smartflux mode, this output is always disabled
    if (generalOutMeanCosp())
        test = true;

    return test;
}

bool EcProject::queryProjectImport(const QString& filename)
{
    return WidgetUtils::okToQuestion(nullptr,
        tr("Import Project"),
        tr("<p>Your project file has to be imported "
           "and updated to a new version. "
           "If you proceed, you will "
           "lose your file and the "
           "compatibility with previous versions of EddyPro "
           "but you will have a smooth "
           "transition to the new EddyPro version. "
           "If you are unsure, "
           "select 'No' and create a backup copy of your "
           "project file before proceeding.</p>"),
        tr("<p>Are you sure you want to "
           "import the following file?<br>"
           "<b>\"%1\"</b></p>").arg(filename));
}

bool EcProject::isGoodRawFileNameFormat(const QString& s)
{
    DEBUG_FUNC_NAME
    bool test = !s.isEmpty()
                && s.contains(QStringLiteral("yy"))
                && s.contains(QStringLiteral("dd"))
                && s.contains(QStringLiteral("HH"))
                && s.contains(QStringLiteral("MM"))
                && s.contains(QStringLiteral("."));
    qDebug() << "test" << test;
    return test;
}
