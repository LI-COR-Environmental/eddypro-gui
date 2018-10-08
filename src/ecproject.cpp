/***************************************************************************
  ecproject.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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
#include "mainwindow.h"
#include "stringutils.h"
#include "widget_utils.h"

EcProject::EcProject(QObject *parent, ProjConfigState project_config) :
    QObject(parent),
    defaultSettings(EcProjectState()),
    modified_(false),
    ec_project_state_(EcProjectState()),
    project_config_state_(std::move(project_config))
{
    Defs::qt_registerCustomTypes();
}

EcProject::EcProject(const EcProject& project) :
    QObject(nullptr),
    defaultSettings(EcProjectState()),
    modified_(project.modified_),
    ec_project_state_(project.ec_project_state_),
    project_config_state_(project.project_config_state_)
{
}

EcProject& EcProject::operator=(const EcProject &project)
{
    if (this != &project)
    {
        defaultSettings = project.defaultSettings;
        modified_ = project.modified_;
        ec_project_state_ = project.ec_project_state_;
        project_config_state_ = project.project_config_state_;
    }
    return *this;
}

// New project
void EcProject::newEcProject(const ProjConfigState& project_config)
{
    // data e ora in formato ISO
    auto now = QDateTime::currentDateTime();
    auto now_str = now.toString(Qt::ISODate);

    // update project configuration
    project_config_state_ = project_config;

    EcProjectState defaultEcProjectState;
    ec_project_state_.projectGeneral.sw_version = defaultEcProjectState.projectGeneral.sw_version;
    ec_project_state_.projectGeneral.ini_version = defaultEcProjectState.projectGeneral.ini_version;
    ec_project_state_.projectGeneral.creation_date = now_str;
    ec_project_state_.projectGeneral.last_change_date.clear();
    ec_project_state_.projectGeneral.run_mode = defaultEcProjectState.projectGeneral.run_mode;
    ec_project_state_.projectGeneral.run_fcc = defaultEcProjectState.projectGeneral.run_fcc;
    ec_project_state_.projectGeneral.file_name.clear();
    ec_project_state_.projectGeneral.project_title.clear();
    ec_project_state_.projectGeneral.project_id.clear();
    ec_project_state_.projectGeneral.file_type = defaultEcProjectState.projectGeneral.file_type;
    ec_project_state_.projectGeneral.file_prototype.clear();
    ec_project_state_.projectGeneral.use_alt_md_file = defaultEcProjectState.projectGeneral.use_alt_md_file;
    ec_project_state_.projectGeneral.md_file.clear();
    ec_project_state_.projectGeneral.use_tlfile = defaultEcProjectState.projectGeneral.use_tlfile;
    ec_project_state_.projectGeneral.timeline_file.clear();
    ec_project_state_.projectGeneral.binary_hnlines = defaultEcProjectState.projectGeneral.binary_hnlines;
    ec_project_state_.projectGeneral.binary_eol = defaultEcProjectState.projectGeneral.binary_eol;
    ec_project_state_.projectGeneral.binary_nbytes = defaultEcProjectState.projectGeneral.binary_nbytes;
    ec_project_state_.projectGeneral.binary_little_end = defaultEcProjectState.projectGeneral.binary_little_end;
    ec_project_state_.projectGeneral.master_sonic.clear();
    ec_project_state_.projectGeneral.col_co2 = defaultEcProjectState.projectGeneral.col_co2;
    ec_project_state_.projectGeneral.col_h2o = defaultEcProjectState.projectGeneral.col_h2o;
    ec_project_state_.projectGeneral.col_ch4 = defaultEcProjectState.projectGeneral.col_ch4;
    ec_project_state_.projectGeneral.col_n2o = defaultEcProjectState.projectGeneral.col_n2o;
    ec_project_state_.projectGeneral.col_int_t_c = defaultEcProjectState.projectGeneral.col_int_t_c;
    ec_project_state_.projectGeneral.col_int_t_1 = defaultEcProjectState.projectGeneral.col_int_t_1;
    ec_project_state_.projectGeneral.col_int_t_2 = defaultEcProjectState.projectGeneral.col_int_t_2;
    ec_project_state_.projectGeneral.col_int_p = defaultEcProjectState.projectGeneral.col_int_p;
    ec_project_state_.projectGeneral.col_air_t = defaultEcProjectState.projectGeneral.col_air_t;
    ec_project_state_.projectGeneral.col_air_p = defaultEcProjectState.projectGeneral.col_air_p;
    ec_project_state_.projectGeneral.col_diag_75 = defaultEcProjectState.projectGeneral.col_diag_75;
    ec_project_state_.projectGeneral.col_diag_72 = defaultEcProjectState.projectGeneral.col_diag_72;
    ec_project_state_.projectGeneral.col_diag_77 = defaultEcProjectState.projectGeneral.col_diag_77;
    ec_project_state_.projectGeneral.col_diag_anem = defaultEcProjectState.projectGeneral.col_diag_anem;
    ec_project_state_.projectGeneral.col_ts = defaultEcProjectState.projectGeneral.col_ts;
    ec_project_state_.projectGeneral.gas_mw = defaultEcProjectState.projectGeneral.gas_mw;
    ec_project_state_.projectGeneral.gas_diff = defaultEcProjectState.projectGeneral.gas_diff;
    ec_project_state_.projectGeneral.out_rich = defaultEcProjectState.projectGeneral.out_rich;
    ec_project_state_.projectGeneral.out_md = defaultEcProjectState.projectGeneral.out_md;
    ec_project_state_.projectGeneral.out_biomet = defaultEcProjectState.projectGeneral.out_biomet;
    ec_project_state_.projectGeneral.make_dataset = defaultEcProjectState.projectGeneral.make_dataset;
    ec_project_state_.projectGeneral.subset = defaultEcProjectState.projectGeneral.subset;
    ec_project_state_.projectGeneral.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.projectGeneral.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.projectGeneral.start_time = QTime(0, 0).toString(QStringLiteral("hh:mm"));
    ec_project_state_.projectGeneral.end_time = QTime(23, 59).toString(QStringLiteral("hh:mm"));
    ec_project_state_.projectGeneral.hf_meth = defaultEcProjectState.projectGeneral.hf_meth;
    ec_project_state_.projectGeneral.lf_meth = defaultEcProjectState.projectGeneral.lf_meth;
    ec_project_state_.projectGeneral.wpl_meth = defaultEcProjectState.projectGeneral.wpl_meth;
    ec_project_state_.projectGeneral.foot_meth = defaultEcProjectState.projectGeneral.foot_meth;
    ec_project_state_.projectGeneral.tob1_format = defaultEcProjectState.projectGeneral.tob1_format;
    ec_project_state_.projectGeneral.out_path.clear();
    ec_project_state_.projectGeneral.fix_out_format = defaultEcProjectState.projectGeneral.fix_out_format;
    ec_project_state_.projectGeneral.err_label = defaultEcProjectState.projectGeneral.err_label;
    ec_project_state_.projectGeneral.qcflag_meth = defaultEcProjectState.projectGeneral.qcflag_meth;
    ec_project_state_.projectGeneral.use_biomet = defaultEcProjectState.projectGeneral.use_biomet;
    ec_project_state_.projectGeneral.biom_file.clear();
    ec_project_state_.projectGeneral.biom_dir.clear();
    ec_project_state_.projectGeneral.biom_recurse = defaultEcProjectState.projectGeneral.biom_recurse;
    ec_project_state_.projectGeneral.biom_ext = defaultEcProjectState.projectGeneral.biom_ext;
    ec_project_state_.projectGeneral.out_mean_spectra = defaultEcProjectState.projectGeneral.out_mean_spectra;
    ec_project_state_.projectGeneral.out_mean_cosp = defaultEcProjectState.projectGeneral.out_mean_cosp;
    ec_project_state_.projectGeneral.bin_sp_avail = defaultEcProjectState.projectGeneral.bin_sp_avail;
    ec_project_state_.projectGeneral.full_sp_avail = defaultEcProjectState.projectGeneral.full_sp_avail;
    ec_project_state_.projectGeneral.files_found = defaultEcProjectState.projectGeneral.files_found;
    ec_project_state_.projectGeneral.hf_correct_ghg_ba = defaultEcProjectState.projectGeneral.hf_correct_ghg_ba;
    ec_project_state_.projectGeneral.hf_correct_ghg_zoh = defaultEcProjectState.projectGeneral.hf_correct_ghg_zoh;
    ec_project_state_.projectGeneral.sonic_output_rate = defaultEcProjectState.projectGeneral.sonic_output_rate;

    // preproc general section
    ec_project_state_.screenGeneral.start_run.clear();
    ec_project_state_.screenGeneral.end_run.clear();
    ec_project_state_.screenGeneral.data_path.clear();
    ec_project_state_.screenGeneral.use_geo_north = defaultEcProjectState.screenGeneral.use_geo_north;
    ec_project_state_.screenGeneral.mag_dec = defaultEcProjectState.screenGeneral.mag_dec;
    ec_project_state_.screenGeneral.dec_date = ec_project_state_.projectGeneral.end_date;
    ec_project_state_.screenGeneral.recurse = defaultEcProjectState.screenGeneral.recurse;
    ec_project_state_.screenGeneral.flag1_col = defaultEcProjectState.screenGeneral.flag1_col;
    ec_project_state_.screenGeneral.flag1_threshold = defaultEcProjectState.screenGeneral.flag1_threshold;
    ec_project_state_.screenGeneral.flag1_policy = defaultEcProjectState.screenGeneral.flag1_policy;
    ec_project_state_.screenGeneral.flag2_col = defaultEcProjectState.screenGeneral.flag2_col;
    ec_project_state_.screenGeneral.flag2_threshold = defaultEcProjectState.screenGeneral.flag2_threshold;
    ec_project_state_.screenGeneral.flag2_policy = defaultEcProjectState.screenGeneral.flag2_policy;
    ec_project_state_.screenGeneral.flag3_col = defaultEcProjectState.screenGeneral.flag3_col;
    ec_project_state_.screenGeneral.flag3_threshold = defaultEcProjectState.screenGeneral.flag3_threshold;
    ec_project_state_.screenGeneral.flag3_policy = defaultEcProjectState.screenGeneral.flag3_policy;
    ec_project_state_.screenGeneral.flag4_col = defaultEcProjectState.screenGeneral.flag4_col;
    ec_project_state_.screenGeneral.flag4_threshold = defaultEcProjectState.screenGeneral.flag4_threshold;
    ec_project_state_.screenGeneral.flag4_policy = defaultEcProjectState.screenGeneral.flag4_policy;
    ec_project_state_.screenGeneral.flag5_col = defaultEcProjectState.screenGeneral.flag5_col;
    ec_project_state_.screenGeneral.flag5_threshold = defaultEcProjectState.screenGeneral.flag5_threshold;
    ec_project_state_.screenGeneral.flag5_policy = defaultEcProjectState.screenGeneral.flag5_policy;
    ec_project_state_.screenGeneral.flag6_col = defaultEcProjectState.screenGeneral.flag6_col;
    ec_project_state_.screenGeneral.flag6_threshold = defaultEcProjectState.screenGeneral.flag6_threshold;
    ec_project_state_.screenGeneral.flag6_policy = defaultEcProjectState.screenGeneral.flag6_policy;
    ec_project_state_.screenGeneral.flag7_col = defaultEcProjectState.screenGeneral.flag7_col;
    ec_project_state_.screenGeneral.flag7_threshold = defaultEcProjectState.screenGeneral.flag7_threshold;
    ec_project_state_.screenGeneral.flag7_policy = defaultEcProjectState.screenGeneral.flag7_policy;
    ec_project_state_.screenGeneral.flag8_col = defaultEcProjectState.screenGeneral.flag8_col;
    ec_project_state_.screenGeneral.flag8_threshold = defaultEcProjectState.screenGeneral.flag8_threshold;
    ec_project_state_.screenGeneral.flag8_policy = defaultEcProjectState.screenGeneral.flag8_policy;
    ec_project_state_.screenGeneral.flag9_col = defaultEcProjectState.screenGeneral.flag9_col;
    ec_project_state_.screenGeneral.flag9_threshold = defaultEcProjectState.screenGeneral.flag9_threshold;
    ec_project_state_.screenGeneral.flag9_policy = defaultEcProjectState.screenGeneral.flag9_policy;
    ec_project_state_.screenGeneral.flag10_col = defaultEcProjectState.screenGeneral.flag10_col;
    ec_project_state_.screenGeneral.flag10_threshold = defaultEcProjectState.screenGeneral.flag10_threshold;
    ec_project_state_.screenGeneral.flag10_policy = defaultEcProjectState.screenGeneral.flag10_policy;

    // preproc setting section
    ec_project_state_.screenSetting.avrg_len = defaultEcProjectState.screenSetting.avrg_len;
    ec_project_state_.screenSetting.max_lack = defaultEcProjectState.screenSetting.max_lack;
    ec_project_state_.screenSetting.u_offset = defaultEcProjectState.screenSetting.u_offset;
    ec_project_state_.screenSetting.v_offset = defaultEcProjectState.screenSetting.v_offset;
    ec_project_state_.screenSetting.w_offset = defaultEcProjectState.screenSetting.w_offset;
    ec_project_state_.screenSetting.cross_wind = defaultEcProjectState.screenSetting.cross_wind;
    ec_project_state_.screenSetting.gill_wm_wboost = defaultEcProjectState.screenSetting.gill_wm_wboost;
    ec_project_state_.screenSetting.flow_distortion = defaultEcProjectState.screenSetting.flow_distortion;
    ec_project_state_.screenSetting.rot_meth = defaultEcProjectState.screenSetting.rot_meth;
    ec_project_state_.screenSetting.detrend_meth = defaultEcProjectState.screenSetting.detrend_meth;
    ec_project_state_.screenSetting.timeconst = defaultEcProjectState.screenSetting.timeconst;
    ec_project_state_.screenSetting.tlag_meth = defaultEcProjectState.screenSetting.tlag_meth;
    ec_project_state_.screenSetting.tap_win = defaultEcProjectState.screenSetting.tap_win;
    ec_project_state_.screenSetting.nbins = defaultEcProjectState.screenSetting.nbins;
    ec_project_state_.screenSetting.out_bin_sp = defaultEcProjectState.screenSetting.out_bin_sp;
    ec_project_state_.screenSetting.out_bin_og = defaultEcProjectState.screenSetting.out_bin_og;
    ec_project_state_.screenSetting.out_full_sp_u = defaultEcProjectState.screenSetting.out_full_sp_u;
    ec_project_state_.screenSetting.out_full_sp_v = defaultEcProjectState.screenSetting.out_full_sp_v;
    ec_project_state_.screenSetting.out_full_sp_w = defaultEcProjectState.screenSetting.out_full_sp_w;
    ec_project_state_.screenSetting.out_full_sp_ts = defaultEcProjectState.screenSetting.out_full_sp_ts;
    ec_project_state_.screenSetting.out_full_sp_co2 = defaultEcProjectState.screenSetting.out_full_sp_co2;
    ec_project_state_.screenSetting.out_full_sp_h2o = defaultEcProjectState.screenSetting.out_full_sp_h2o;
    ec_project_state_.screenSetting.out_full_sp_ch4 = defaultEcProjectState.screenSetting.out_full_sp_ch4;
    ec_project_state_.screenSetting.out_full_sp_n2o = defaultEcProjectState.screenSetting.out_full_sp_n2o;
    ec_project_state_.screenSetting.out_st_1 = defaultEcProjectState.screenSetting.out_st_1;
    ec_project_state_.screenSetting.out_st_2 = defaultEcProjectState.screenSetting.out_st_2;
    ec_project_state_.screenSetting.out_st_3 = defaultEcProjectState.screenSetting.out_st_3;
    ec_project_state_.screenSetting.out_st_4 = defaultEcProjectState.screenSetting.out_st_4;
    ec_project_state_.screenSetting.out_st_5 = defaultEcProjectState.screenSetting.out_st_5;
    ec_project_state_.screenSetting.out_st_6 = defaultEcProjectState.screenSetting.out_st_6;
    ec_project_state_.screenSetting.out_st_7 = defaultEcProjectState.screenSetting.out_st_7;
    ec_project_state_.screenSetting.out_raw_1 = defaultEcProjectState.screenSetting.out_raw_1;
    ec_project_state_.screenSetting.out_raw_2 = defaultEcProjectState.screenSetting.out_raw_2;
    ec_project_state_.screenSetting.out_raw_3 = defaultEcProjectState.screenSetting.out_raw_3;
    ec_project_state_.screenSetting.out_raw_4 = defaultEcProjectState.screenSetting.out_raw_4;
    ec_project_state_.screenSetting.out_raw_5 = defaultEcProjectState.screenSetting.out_raw_5;
    ec_project_state_.screenSetting.out_raw_6 = defaultEcProjectState.screenSetting.out_raw_6;
    ec_project_state_.screenSetting.out_raw_7 = defaultEcProjectState.screenSetting.out_raw_7;
    ec_project_state_.screenSetting.out_raw_u = defaultEcProjectState.screenSetting.out_raw_u;
    ec_project_state_.screenSetting.out_raw_v = defaultEcProjectState.screenSetting.out_raw_v;
    ec_project_state_.screenSetting.out_raw_w = defaultEcProjectState.screenSetting.out_raw_w;
    ec_project_state_.screenSetting.out_raw_ts = defaultEcProjectState.screenSetting.out_raw_ts;
    ec_project_state_.screenSetting.out_raw_co2 = defaultEcProjectState.screenSetting.out_raw_co2;
    ec_project_state_.screenSetting.out_raw_h2o = defaultEcProjectState.screenSetting.out_raw_h2o;
    ec_project_state_.screenSetting.out_raw_ch4 = defaultEcProjectState.screenSetting.out_raw_ch4;
    ec_project_state_.screenSetting.out_raw_gas4 = defaultEcProjectState.screenSetting.out_raw_gas4;
    ec_project_state_.screenSetting.out_raw_tair = defaultEcProjectState.screenSetting.out_raw_tair;
    ec_project_state_.screenSetting.out_raw_pair = defaultEcProjectState.screenSetting.out_raw_pair;
    ec_project_state_.screenSetting.out_full_cosp_u = defaultEcProjectState.screenSetting.out_full_cosp_u;
    ec_project_state_.screenSetting.out_full_cosp_v = defaultEcProjectState.screenSetting.out_full_cosp_v;
    ec_project_state_.screenSetting.out_full_cosp_ts = defaultEcProjectState.screenSetting.out_full_cosp_ts;
    ec_project_state_.screenSetting.out_full_cosp_co2 = defaultEcProjectState.screenSetting.out_full_cosp_co2;
    ec_project_state_.screenSetting.out_full_cosp_h2o = defaultEcProjectState.screenSetting.out_full_cosp_h2o;
    ec_project_state_.screenSetting.out_full_cosp_ch4 = defaultEcProjectState.screenSetting.out_full_cosp_ch4;
    ec_project_state_.screenSetting.out_full_cosp_n2o = defaultEcProjectState.screenSetting.out_full_cosp_n2o;
    ec_project_state_.screenSetting.filter_sr = defaultEcProjectState.screenSetting.filter_sr;
    ec_project_state_.screenSetting.filter_al = defaultEcProjectState.screenSetting.filter_al;
    ec_project_state_.screenSetting.bu_corr = defaultEcProjectState.screenSetting.bu_corr;
    ec_project_state_.screenSetting.bu_multi = defaultEcProjectState.screenSetting.bu_multi;
    ec_project_state_.screenSetting.l_day_bot_gain = defaultEcProjectState.screenSetting.l_day_bot_gain;
    ec_project_state_.screenSetting.l_day_bot_offset = defaultEcProjectState.screenSetting.l_day_bot_offset;
    ec_project_state_.screenSetting.l_day_top_gain = defaultEcProjectState.screenSetting.l_day_top_gain;
    ec_project_state_.screenSetting.l_day_top_offset = defaultEcProjectState.screenSetting.l_day_top_offset;
    ec_project_state_.screenSetting.l_day_spar_gain = defaultEcProjectState.screenSetting.l_day_spar_gain;
    ec_project_state_.screenSetting.l_day_spar_offset = defaultEcProjectState.screenSetting.l_day_spar_offset;
    ec_project_state_.screenSetting.l_night_bot_gain = defaultEcProjectState.screenSetting.l_night_bot_gain;
    ec_project_state_.screenSetting.l_night_bot_offset = defaultEcProjectState.screenSetting.l_night_bot_offset;
    ec_project_state_.screenSetting.l_night_top_gain = defaultEcProjectState.screenSetting.l_night_top_gain;
    ec_project_state_.screenSetting.l_night_top_offset = defaultEcProjectState.screenSetting.l_night_top_offset;
    ec_project_state_.screenSetting.l_night_spar_gain = defaultEcProjectState.screenSetting.l_night_spar_gain;
    ec_project_state_.screenSetting.l_night_spar_offset = defaultEcProjectState.screenSetting.l_night_spar_offset;
    ec_project_state_.screenSetting.m_day_bot1 = defaultEcProjectState.screenSetting.m_day_bot1;
    ec_project_state_.screenSetting.m_day_bot2 = defaultEcProjectState.screenSetting.m_day_bot2;
    ec_project_state_.screenSetting.m_day_bot3 = defaultEcProjectState.screenSetting.m_day_bot3;
    ec_project_state_.screenSetting.m_day_bot4 = defaultEcProjectState.screenSetting.m_day_bot4;
    ec_project_state_.screenSetting.m_day_top1 = defaultEcProjectState.screenSetting.m_day_top1;
    ec_project_state_.screenSetting.m_day_top2 = defaultEcProjectState.screenSetting.m_day_top2;
    ec_project_state_.screenSetting.m_day_top3 = defaultEcProjectState.screenSetting.m_day_top3;
    ec_project_state_.screenSetting.m_day_top4 = defaultEcProjectState.screenSetting.m_day_top4;
    ec_project_state_.screenSetting.m_day_spar1 = defaultEcProjectState.screenSetting.m_day_spar1;
    ec_project_state_.screenSetting.m_day_spar2 = defaultEcProjectState.screenSetting.m_day_spar2;
    ec_project_state_.screenSetting.m_day_spar3 = defaultEcProjectState.screenSetting.m_day_spar3;
    ec_project_state_.screenSetting.m_day_spar4 = defaultEcProjectState.screenSetting.m_day_spar4;
    ec_project_state_.screenSetting.m_night_bot1 = defaultEcProjectState.screenSetting.m_night_bot1;
    ec_project_state_.screenSetting.m_night_bot2 = defaultEcProjectState.screenSetting.m_night_bot2;
    ec_project_state_.screenSetting.m_night_bot3 = defaultEcProjectState.screenSetting.m_night_bot3;
    ec_project_state_.screenSetting.m_night_bot4 = defaultEcProjectState.screenSetting.m_night_bot4;
    ec_project_state_.screenSetting.m_night_top1 = defaultEcProjectState.screenSetting.m_night_top1;
    ec_project_state_.screenSetting.m_night_top2 = defaultEcProjectState.screenSetting.m_night_top2;
    ec_project_state_.screenSetting.m_night_top3 = defaultEcProjectState.screenSetting.m_night_top3;
    ec_project_state_.screenSetting.m_night_top4 = defaultEcProjectState.screenSetting.m_night_top4;
    ec_project_state_.screenSetting.m_night_spar1 = defaultEcProjectState.screenSetting.m_night_spar1;
    ec_project_state_.screenSetting.m_night_spar2 = defaultEcProjectState.screenSetting.m_night_spar2;
    ec_project_state_.screenSetting.m_night_spar3 = defaultEcProjectState.screenSetting.m_night_spar3;
    ec_project_state_.screenSetting.m_night_spar4 = defaultEcProjectState.screenSetting.m_night_spar4;
    ec_project_state_.screenSetting.out_details = defaultEcProjectState.screenSetting.out_details;
    ec_project_state_.screenSetting.power_of_two = defaultEcProjectState.screenSetting.power_of_two;

    // preproc test section
    ec_project_state_.screenTest.test_sr = defaultEcProjectState.screenTest.test_sr;
    ec_project_state_.screenTest.test_ar = defaultEcProjectState.screenTest.test_ar;
    ec_project_state_.screenTest.test_do = defaultEcProjectState.screenTest.test_do;
    ec_project_state_.screenTest.test_al = defaultEcProjectState.screenTest.test_al;
    ec_project_state_.screenTest.test_sk = defaultEcProjectState.screenTest.test_sk;
    ec_project_state_.screenTest.test_ds = defaultEcProjectState.screenTest.test_ds;
    ec_project_state_.screenTest.test_tl = defaultEcProjectState.screenTest.test_tl;
    ec_project_state_.screenTest.test_aa = defaultEcProjectState.screenTest.test_aa;
    ec_project_state_.screenTest.test_ns = defaultEcProjectState.screenTest.test_ns;

    // preproc parameters section
    ec_project_state_.screenParam.aa_lim = defaultEcProjectState.screenParam.aa_lim;
    ec_project_state_.screenParam.aa_max = defaultEcProjectState.screenParam.aa_max;
    ec_project_state_.screenParam.aa_min = defaultEcProjectState.screenParam.aa_min;
    ec_project_state_.screenParam.al_co2_min = defaultEcProjectState.screenParam.al_co2_min;
    ec_project_state_.screenParam.al_co2_max = defaultEcProjectState.screenParam.al_co2_max;
    ec_project_state_.screenParam.al_h2o_min = defaultEcProjectState.screenParam.al_h2o_min;
    ec_project_state_.screenParam.al_h2o_max = defaultEcProjectState.screenParam.al_h2o_max;
    ec_project_state_.screenParam.al_ch4_min = defaultEcProjectState.screenParam.al_ch4_min;
    ec_project_state_.screenParam.al_ch4_max = defaultEcProjectState.screenParam.al_ch4_max;
    ec_project_state_.screenParam.al_n2o_min = defaultEcProjectState.screenParam.al_n2o_min;
    ec_project_state_.screenParam.al_n2o_max = defaultEcProjectState.screenParam.al_n2o_max;
    ec_project_state_.screenParam.al_tson_min = defaultEcProjectState.screenParam.al_tson_min;
    ec_project_state_.screenParam.al_tson_max = defaultEcProjectState.screenParam.al_tson_max;
    ec_project_state_.screenParam.al_u_max = defaultEcProjectState.screenParam.al_u_max;
    ec_project_state_.screenParam.al_w_max = defaultEcProjectState.screenParam.al_w_max;
    ec_project_state_.screenParam.ar_bins = defaultEcProjectState.screenParam.ar_bins;
    ec_project_state_.screenParam.ar_hf_lim = defaultEcProjectState.screenParam.ar_hf_lim;
    ec_project_state_.screenParam.ar_lim = defaultEcProjectState.screenParam.ar_lim;
    ec_project_state_.screenParam.ds_hf_uv = defaultEcProjectState.screenParam.ds_hf_uv;
    ec_project_state_.screenParam.ds_hf_w = defaultEcProjectState.screenParam.ds_hf_w;
    ec_project_state_.screenParam.ds_hf_t = defaultEcProjectState.screenParam.ds_hf_t;
    ec_project_state_.screenParam.ds_hf_co2 = defaultEcProjectState.screenParam.ds_hf_co2;
    ec_project_state_.screenParam.ds_hf_h2o = defaultEcProjectState.screenParam.ds_hf_h2o;
    ec_project_state_.screenParam.ds_hf_ch4 = defaultEcProjectState.screenParam.ds_hf_ch4;
    ec_project_state_.screenParam.ds_hf_n2o = defaultEcProjectState.screenParam.ds_hf_n2o;
    ec_project_state_.screenParam.ds_hf_var = defaultEcProjectState.screenParam.ds_hf_var;
    ec_project_state_.screenParam.ds_sf_uv = defaultEcProjectState.screenParam.ds_sf_uv;
    ec_project_state_.screenParam.ds_sf_w = defaultEcProjectState.screenParam.ds_sf_w;
    ec_project_state_.screenParam.ds_sf_t = defaultEcProjectState.screenParam.ds_sf_t;
    ec_project_state_.screenParam.ds_sf_co2 = defaultEcProjectState.screenParam.ds_sf_co2;
    ec_project_state_.screenParam.ds_sf_h2o = defaultEcProjectState.screenParam.ds_sf_h2o;
    ec_project_state_.screenParam.ds_sf_ch4 = defaultEcProjectState.screenParam.ds_sf_ch4;
    ec_project_state_.screenParam.ds_sf_n2o = defaultEcProjectState.screenParam.ds_sf_n2o;
    ec_project_state_.screenParam.ds_sf_var = defaultEcProjectState.screenParam.ds_sf_var;
    ec_project_state_.screenParam.despike_vm = defaultEcProjectState.screenParam.despike_vm;
    ec_project_state_.screenParam.do_extlim_dw = defaultEcProjectState.screenParam.do_extlim_dw;
    ec_project_state_.screenParam.do_hf1_lim = defaultEcProjectState.screenParam.do_hf1_lim;
    ec_project_state_.screenParam.do_hf2_lim = defaultEcProjectState.screenParam.do_hf2_lim;
    ec_project_state_.screenParam.ns_hf_lim = defaultEcProjectState.screenParam.ns_hf_lim;
    ec_project_state_.screenParam.sk_hf_kumax = defaultEcProjectState.screenParam.sk_hf_kumax;
    ec_project_state_.screenParam.sk_hf_kumin = defaultEcProjectState.screenParam.sk_hf_kumin;
    ec_project_state_.screenParam.sk_hf_skmax = defaultEcProjectState.screenParam.sk_hf_skmax;
    ec_project_state_.screenParam.sk_hf_skmin = defaultEcProjectState.screenParam.sk_hf_skmin;
    ec_project_state_.screenParam.sk_sf_kumax = defaultEcProjectState.screenParam.sk_sf_kumax;
    ec_project_state_.screenParam.sk_sf_kumin = defaultEcProjectState.screenParam.sk_sf_kumin;
    ec_project_state_.screenParam.sk_sf_skmax = defaultEcProjectState.screenParam.sk_sf_skmax;
    ec_project_state_.screenParam.sk_sf_skmin = defaultEcProjectState.screenParam.sk_sf_skmin;
    ec_project_state_.screenParam.sr_num_spk = defaultEcProjectState.screenParam.sr_num_spk;
    ec_project_state_.screenParam.sr_lim_u = defaultEcProjectState.screenParam.sr_lim_u;
    ec_project_state_.screenParam.sr_lim_w = defaultEcProjectState.screenParam.sr_lim_w;
    ec_project_state_.screenParam.sr_lim_co2 = defaultEcProjectState.screenParam.sr_lim_co2;
    ec_project_state_.screenParam.sr_lim_h2o = defaultEcProjectState.screenParam.sr_lim_h2o;
    ec_project_state_.screenParam.sr_lim_ch4 = defaultEcProjectState.screenParam.sr_lim_ch4;
    ec_project_state_.screenParam.sr_lim_n2o = defaultEcProjectState.screenParam.sr_lim_n2o;
    ec_project_state_.screenParam.sr_lim_hf = defaultEcProjectState.screenParam.sr_lim_hf;
    ec_project_state_.screenParam.tl_hf_lim = defaultEcProjectState.screenParam.tl_hf_lim;
    ec_project_state_.screenParam.tl_def_co2 = defaultEcProjectState.screenParam.tl_def_co2;
    ec_project_state_.screenParam.tl_def_h2o = defaultEcProjectState.screenParam.tl_def_h2o;
    ec_project_state_.screenParam.tl_def_ch4 = defaultEcProjectState.screenParam.tl_def_ch4;
    ec_project_state_.screenParam.tl_def_n2o = defaultEcProjectState.screenParam.tl_def_n2o;
    ec_project_state_.screenParam.tl_sf_lim = defaultEcProjectState.screenParam.tl_sf_lim;

    ec_project_state_.spectraSettings.start_sa_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.spectraSettings.end_sa_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.spectraSettings.start_sa_time = QTime(0, 0).toString(QStringLiteral("hh:mm"));
    ec_project_state_.spectraSettings.end_sa_time = QTime(23, 59).toString(QStringLiteral("hh:mm"));
    ec_project_state_.spectraSettings.sa_mode = defaultEcProjectState.spectraSettings.sa_mode;
    ec_project_state_.spectraSettings.sa_file.clear();
    ec_project_state_.spectraSettings.horst_lens = defaultEcProjectState.spectraSettings.horst_lens;
    ec_project_state_.spectraSettings.sa_min_smpl = defaultEcProjectState.spectraSettings.sa_min_smpl;
    ec_project_state_.spectraSettings.sa_fmin_co2 = defaultEcProjectState.spectraSettings.sa_fmin_co2;
    ec_project_state_.spectraSettings.sa_fmin_h2o = defaultEcProjectState.spectraSettings.sa_fmin_h2o;
    ec_project_state_.spectraSettings.sa_fmin_ch4 = defaultEcProjectState.spectraSettings.sa_fmin_ch4;
    ec_project_state_.spectraSettings.sa_fmin_gas4 = defaultEcProjectState.spectraSettings.sa_fmin_gas4;
    ec_project_state_.spectraSettings.sa_fmax_co2 = defaultEcProjectState.spectraSettings.sa_fmax_co2;
    ec_project_state_.spectraSettings.sa_fmax_h2o = defaultEcProjectState.spectraSettings.sa_fmax_h2o;
    ec_project_state_.spectraSettings.sa_fmax_ch4 = defaultEcProjectState.spectraSettings.sa_fmax_ch4;
    ec_project_state_.spectraSettings.sa_fmax_gas4 = defaultEcProjectState.spectraSettings.sa_fmax_gas4;
    ec_project_state_.spectraSettings.sa_hfn_co2_fmin = defaultEcProjectState.spectraSettings.sa_hfn_co2_fmin;
    ec_project_state_.spectraSettings.sa_hfn_h2o_fmin = defaultEcProjectState.spectraSettings.sa_hfn_h2o_fmin;
    ec_project_state_.spectraSettings.sa_hfn_ch4_fmin = defaultEcProjectState.spectraSettings.sa_hfn_ch4_fmin;
    ec_project_state_.spectraSettings.sa_hfn_gas4_fmin = defaultEcProjectState.spectraSettings.sa_hfn_gas4_fmin;
    ec_project_state_.spectraSettings.add_sonic_lptf = defaultEcProjectState.spectraSettings.add_sonic_lptf;
    ec_project_state_.spectraSettings.sa_min_un_ustar = defaultEcProjectState.spectraSettings.sa_min_un_ustar;
    ec_project_state_.spectraSettings.sa_min_un_h = defaultEcProjectState.spectraSettings.sa_min_un_h;
    ec_project_state_.spectraSettings.sa_min_un_le = defaultEcProjectState.spectraSettings.sa_min_un_le;
    ec_project_state_.spectraSettings.sa_min_un_co2 = defaultEcProjectState.spectraSettings.sa_min_un_co2;
    ec_project_state_.spectraSettings.sa_min_un_ch4 = defaultEcProjectState.spectraSettings.sa_min_un_ch4;
    ec_project_state_.spectraSettings.sa_min_un_gas4 = defaultEcProjectState.spectraSettings.sa_min_un_gas4;
    ec_project_state_.spectraSettings.sa_min_st_ustar = defaultEcProjectState.spectraSettings.sa_min_st_ustar;
    ec_project_state_.spectraSettings.sa_min_st_h = defaultEcProjectState.spectraSettings.sa_min_st_h;
    ec_project_state_.spectraSettings.sa_min_st_le = defaultEcProjectState.spectraSettings.sa_min_st_le;
    ec_project_state_.spectraSettings.sa_min_st_co2 = defaultEcProjectState.spectraSettings.sa_min_st_co2;
    ec_project_state_.spectraSettings.sa_min_st_ch4 = defaultEcProjectState.spectraSettings.sa_min_st_ch4;
    ec_project_state_.spectraSettings.sa_min_st_gas4 = defaultEcProjectState.spectraSettings.sa_min_st_gas4;
    ec_project_state_.spectraSettings.sa_max_ustar = defaultEcProjectState.spectraSettings.sa_max_ustar;
    ec_project_state_.spectraSettings.sa_max_h = defaultEcProjectState.spectraSettings.sa_max_h;
    ec_project_state_.spectraSettings.sa_max_le = defaultEcProjectState.spectraSettings.sa_max_le;
    ec_project_state_.spectraSettings.sa_max_co2 = defaultEcProjectState.spectraSettings.sa_max_co2;
    ec_project_state_.spectraSettings.sa_max_ch4 = defaultEcProjectState.spectraSettings.sa_max_ch4;
    ec_project_state_.spectraSettings.sa_max_gas4 = defaultEcProjectState.spectraSettings.sa_max_gas4;
    ec_project_state_.spectraSettings.ex_file.clear();
    ec_project_state_.spectraSettings.sa_bin_spectra.clear();
    ec_project_state_.spectraSettings.sa_full_spectra.clear();
    ec_project_state_.spectraSettings.ex_dir.clear();
    ec_project_state_.spectraSettings.subset = defaultEcProjectState.spectraSettings.subset;
    ec_project_state_.spectraSettings.use_vm_flags = defaultEcProjectState.spectraSettings.use_vm_flags;
    ec_project_state_.spectraSettings.use_foken_low = defaultEcProjectState.spectraSettings.use_foken_low;
    ec_project_state_.spectraSettings.use_foken_mid = defaultEcProjectState.spectraSettings.use_foken_mid;

    ec_project_state_.screenTilt.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.screenTilt.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.screenTilt.start_time = QTime(0, 0).toString(QStringLiteral("hh:mm"));
    ec_project_state_.screenTilt.end_time = QTime(23, 59).toString(QStringLiteral("hh:mm"));
    ec_project_state_.screenTilt.mode = defaultEcProjectState.screenTilt.mode;
    ec_project_state_.screenTilt.north_offset = defaultEcProjectState.screenTilt.north_offset;
    ec_project_state_.screenTilt.min_num_per_sec = defaultEcProjectState.screenTilt.min_num_per_sec;
    ec_project_state_.screenTilt.w_max = defaultEcProjectState.screenTilt.w_max;
    ec_project_state_.screenTilt.u_min = defaultEcProjectState.screenTilt.u_min;
    ec_project_state_.screenTilt.file.clear();
    ec_project_state_.screenTilt.fix_policy = defaultEcProjectState.screenTilt.fix_policy;
    ec_project_state_.screenTilt.angles.clear();
    ec_project_state_.screenTilt.subset = defaultEcProjectState.screenTilt.subset;

    ec_project_state_.timelagOpt.start_date = QDate(2000, 1, 1).toString(Qt::ISODate);
    ec_project_state_.timelagOpt.end_date = QDate::currentDate().toString(Qt::ISODate);
    ec_project_state_.timelagOpt.start_time = QTime(0, 0).toString(QStringLiteral("hh:mm"));
    ec_project_state_.timelagOpt.end_time = QTime(23, 59).toString(QStringLiteral("hh:mm"));
    ec_project_state_.timelagOpt.mode = defaultEcProjectState.timelagOpt.mode;
    ec_project_state_.timelagOpt.file.clear();
    ec_project_state_.timelagOpt.pg_range = defaultEcProjectState.timelagOpt.pg_range;
    ec_project_state_.timelagOpt.le_min_flux = defaultEcProjectState.timelagOpt.le_min_flux;
    ec_project_state_.timelagOpt.to_h2o_nclass = defaultEcProjectState.timelagOpt.to_h2o_nclass;
    ec_project_state_.timelagOpt.co2_min_flux = defaultEcProjectState.timelagOpt.co2_min_flux;
    ec_project_state_.timelagOpt.ch4_min_flux = defaultEcProjectState.timelagOpt.ch4_min_flux;
    ec_project_state_.timelagOpt.gas4_min_flux = defaultEcProjectState.timelagOpt.gas4_min_flux;
    ec_project_state_.timelagOpt.co2_min_lag = defaultEcProjectState.timelagOpt.co2_min_lag;
    ec_project_state_.timelagOpt.co2_max_lag = defaultEcProjectState.timelagOpt.co2_max_lag;
    ec_project_state_.timelagOpt.h2o_min_lag = defaultEcProjectState.timelagOpt.h2o_min_lag;
    ec_project_state_.timelagOpt.h2o_max_lag = defaultEcProjectState.timelagOpt.h2o_max_lag;
    ec_project_state_.timelagOpt.ch4_min_lag = defaultEcProjectState.timelagOpt.ch4_min_lag;
    ec_project_state_.timelagOpt.ch4_max_lag = defaultEcProjectState.timelagOpt.ch4_max_lag;
    ec_project_state_.timelagOpt.gas4_min_lag = defaultEcProjectState.timelagOpt.gas4_min_lag;
    ec_project_state_.timelagOpt.gas4_max_lag = defaultEcProjectState.timelagOpt.gas4_max_lag;
    ec_project_state_.timelagOpt.subset  = defaultEcProjectState.timelagOpt.subset;

    ec_project_state_.projectGeneral.ru_method = defaultEcProjectState.projectGeneral.ru_method;
    ec_project_state_.projectGeneral.its_method = defaultEcProjectState.projectGeneral.its_method;
    ec_project_state_.projectGeneral.its_tlag_max = defaultEcProjectState.projectGeneral.its_tlag_max;
    ec_project_state_.projectGeneral.its_sec_factor = defaultEcProjectState.projectGeneral.its_sec_factor;

    ec_project_state_.biomParam.native_header = defaultEcProjectState.biomParam.native_header;
    ec_project_state_.biomParam.hlines = defaultEcProjectState.biomParam.hlines;
    ec_project_state_.biomParam.separator = defaultEcProjectState.biomParam.separator;
    ec_project_state_.biomParam.tstamp_ref = defaultEcProjectState.biomParam.tstamp_ref;
    ec_project_state_.biomParam.col_ta = defaultEcProjectState.biomParam.col_ta;
    ec_project_state_.biomParam.col_pa = defaultEcProjectState.biomParam.col_pa;
    ec_project_state_.biomParam.col_rh = defaultEcProjectState.biomParam.col_rh;
    ec_project_state_.biomParam.col_ppfd = defaultEcProjectState.biomParam.col_ppfd;
    ec_project_state_.biomParam.col_rg = defaultEcProjectState.biomParam.col_rg;
    ec_project_state_.biomParam.col_lwin = defaultEcProjectState.biomParam.col_lwin;

    setModified(false); // new documents are not in a modified state
    emit ecProjectNew();
}

// Save an ec project
bool EcProject::saveEcProject(const QString &filename)
{
    // try to open file just for checking
    QFile datafile(filename);
    if (!datafile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open file" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Write Project Error"),
                             tr("Cannot write file %1:\n%2")
                             .arg(filename, datafile.errorString()));
        datafile.close();
        return false;
    }
    datafile.close();

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
        project_ini.setValue(EcIni::INI_PROJECT_3, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.project_title));

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
        project_ini.setValue(EcIni::INI_PROJECT_12, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.timeline_file));
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
        project_ini.setValue(EcIni::INI_PROJECT_69, ec_project_state_.projectGeneral.col_diag_anem);
        project_ini.setValue(EcIni::INI_PROJECT_31, QString::number(ec_project_state_.projectGeneral.gas_mw, 'f', 4));
        project_ini.setValue(EcIni::INI_PROJECT_32, QString::number(ec_project_state_.projectGeneral.gas_diff, 'f', 5));
        project_ini.setValue(EcIni::INI_PROJECT_36, ec_project_state_.projectGeneral.col_ts);
        project_ini.setValue(EcIni::INI_PROJECT_39, ec_project_state_.projectGeneral.out_rich);
        project_ini.setValue(EcIni::INI_PROJECT_56, ec_project_state_.projectGeneral.out_md);
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
        project_ini.setValue(EcIni::INI_PROJECT_51, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.out_path));
        project_ini.setValue(EcIni::INI_PROJECT_52, ec_project_state_.projectGeneral.fix_out_format);
        project_ini.setValue(EcIni::INI_PROJECT_53, ec_project_state_.projectGeneral.err_label);
        project_ini.setValue(EcIni::INI_PROJECT_55, ec_project_state_.projectGeneral.qcflag_meth);
        project_ini.setValue(EcIni::INI_PROJECT_34, QVariant(ec_project_state_.projectGeneral.use_biomet).toInt());
        project_ini.setValue(EcIni::INI_PROJECT_35, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.biom_file));
        project_ini.setValue(EcIni::INI_PROJECT_57, QDir::fromNativeSeparators(ec_project_state_.projectGeneral.biom_dir));
        project_ini.setValue(EcIni::INI_PROJECT_58, ec_project_state_.projectGeneral.biom_recurse);
        project_ini.setValue(EcIni::INI_PROJECT_59, QVariant(QString(QLatin1Char('.')) + ec_project_state_.projectGeneral.biom_ext));
        project_ini.setValue(EcIni::INI_PROJECT_60, ec_project_state_.projectGeneral.out_mean_cosp);
        project_ini.setValue(EcIni::INI_PROJECT_61, ec_project_state_.projectGeneral.out_biomet);
        project_ini.setValue(EcIni::INI_PROJECT_62, ec_project_state_.projectGeneral.bin_sp_avail);
        project_ini.setValue(EcIni::INI_PROJECT_63, ec_project_state_.projectGeneral.full_sp_avail);
        project_ini.setValue(EcIni::INI_PROJECT_64, ec_project_state_.projectGeneral.files_found);
        project_ini.setValue(EcIni::INI_PROJECT_65, ec_project_state_.projectGeneral.out_mean_spectra);
        project_ini.setValue(EcIni::INI_PROJECT_66, ec_project_state_.projectGeneral.hf_correct_ghg_ba);
        project_ini.setValue(EcIni::INI_PROJECT_67, ec_project_state_.projectGeneral.hf_correct_ghg_zoh);
        project_ini.setValue(EcIni::INI_PROJECT_68, ec_project_state_.projectGeneral.sonic_output_rate);

        // random uncertainty
        project_ini.setValue(EcIni::INI_RAND_ERROR_0,
                             ec_project_state_.projectGeneral.ru_method);
        project_ini.setValue(EcIni::INI_RAND_ERROR_1,
                             ec_project_state_.projectGeneral.its_method);
        project_ini.setValue(EcIni::INI_RAND_ERROR_2,
                             QString::number(ec_project_state_.projectGeneral.its_tlag_max, 'f', 1));
        // NOTE: temporarly disabled
        //project_ini.setValue(EcIni::INI_RAND_ERROR_3,
        //                     QString::number(ec_project_state_.projectGeneral.its_sec_factor, 'f', 1));
    project_ini.endGroup();

    // spec settings section
    project_ini.beginGroup(EcIni::INIGROUP_SPEC_SETTINGS);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_0, ec_project_state_.spectraSettings.start_sa_date);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_1, ec_project_state_.spectraSettings.end_sa_date);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_50, ec_project_state_.spectraSettings.start_sa_time);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_51, ec_project_state_.spectraSettings.end_sa_time);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_2, ec_project_state_.spectraSettings.sa_mode);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_3, QDir::fromNativeSeparators(ec_project_state_.spectraSettings.sa_file));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_4, ec_project_state_.spectraSettings.sa_min_smpl);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_5, QString::number(ec_project_state_.spectraSettings.sa_fmin_co2, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_6, QString::number(ec_project_state_.spectraSettings.sa_fmin_h2o, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_7, QString::number(ec_project_state_.spectraSettings.sa_fmin_ch4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_8, QString::number(ec_project_state_.spectraSettings.sa_fmin_gas4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_9, QString::number(ec_project_state_.spectraSettings.sa_fmax_co2, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_10, QString::number(ec_project_state_.spectraSettings.sa_fmax_h2o, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_11, QString::number(ec_project_state_.spectraSettings.sa_fmax_ch4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_12, QString::number(ec_project_state_.spectraSettings.sa_fmax_gas4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_13, QString::number(ec_project_state_.spectraSettings.sa_hfn_co2_fmin, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_14, QString::number(ec_project_state_.spectraSettings.sa_hfn_h2o_fmin, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_15, QString::number(ec_project_state_.spectraSettings.sa_hfn_ch4_fmin, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_16, QString::number(ec_project_state_.spectraSettings.sa_hfn_gas4_fmin, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_17, QString::number(ec_project_state_.spectraSettings.sa_min_st_co2, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_18, QString::number(ec_project_state_.spectraSettings.sa_min_st_ch4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_19, QString::number(ec_project_state_.spectraSettings.sa_min_st_gas4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_20, QString::number(ec_project_state_.spectraSettings.sa_min_st_le, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_21, QString::number(ec_project_state_.spectraSettings.sa_min_st_h, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_22, ec_project_state_.spectraSettings.add_sonic_lptf);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_28, ec_project_state_.spectraSettings.horst_lens);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_29, QDir::fromNativeSeparators(ec_project_state_.spectraSettings.ex_file));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_30, QDir::fromNativeSeparators(ec_project_state_.spectraSettings.sa_bin_spectra));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_31, QDir::fromNativeSeparators(ec_project_state_.spectraSettings.sa_full_spectra));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_32, QDir::fromNativeSeparators(ec_project_state_.spectraSettings.ex_dir));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_33, ec_project_state_.spectraSettings.subset);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_34, ec_project_state_.spectraSettings.use_vm_flags);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_35, QString::number(ec_project_state_.spectraSettings.sa_min_st_ustar, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_36, QString::number(ec_project_state_.spectraSettings.sa_min_un_ustar, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_37, QString::number(ec_project_state_.spectraSettings.sa_min_un_h, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_38, QString::number(ec_project_state_.spectraSettings.sa_min_un_le, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_39, QString::number(ec_project_state_.spectraSettings.sa_min_un_co2, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_40, QString::number(ec_project_state_.spectraSettings.sa_min_un_ch4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_41, QString::number(ec_project_state_.spectraSettings.sa_min_un_gas4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_42, QString::number(ec_project_state_.spectraSettings.sa_max_ustar, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_43, QString::number(ec_project_state_.spectraSettings.sa_max_h, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_44, QString::number(ec_project_state_.spectraSettings.sa_max_le, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_45, QString::number(ec_project_state_.spectraSettings.sa_max_co2, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_46, QString::number(ec_project_state_.spectraSettings.sa_max_ch4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_47, QString::number(ec_project_state_.spectraSettings.sa_max_gas4, 'f', 4));
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_48, ec_project_state_.spectraSettings.use_foken_low);
        project_ini.setValue(EcIni::INI_SPEC_SETTINGS_49, ec_project_state_.spectraSettings.use_foken_mid);

        // NOTE: temporary placeholders for SA Groups. Not used right now
        project_ini.setValue(QStringLiteral("sa_co2_g1_start"), 1);
        project_ini.setValue(QStringLiteral("sa_co2_g1_stop"), 12);
        project_ini.setValue(QStringLiteral("sa_ch4_g1_start"), 1);
        project_ini.setValue(QStringLiteral("sa_ch4_g1_stop"), 12);
        project_ini.setValue(QStringLiteral("sa_gas4_g1_start"), 1);
        project_ini.setValue(QStringLiteral("sa_gas4_g1_stop"), 12);
    project_ini.endGroup();

    // screen general section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_GENERAL);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_4, QDir::fromNativeSeparators(ec_project_state_.screenGeneral.data_path));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_6, ec_project_state_.screenGeneral.recurse);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_40, QVariant(ec_project_state_.screenGeneral.use_geo_north).toInt());
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_38, ec_project_state_.screenGeneral.mag_dec);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_39, ec_project_state_.screenGeneral.dec_date);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_8, ec_project_state_.screenGeneral.flag1_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_9, QString::number(ec_project_state_.screenGeneral.flag1_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_10, ec_project_state_.screenGeneral.flag1_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_11, ec_project_state_.screenGeneral.flag2_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_12, QString::number(ec_project_state_.screenGeneral.flag2_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_13, ec_project_state_.screenGeneral.flag2_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_14, ec_project_state_.screenGeneral.flag3_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_15, QString::number(ec_project_state_.screenGeneral.flag3_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_16, ec_project_state_.screenGeneral.flag3_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_17, ec_project_state_.screenGeneral.flag4_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_18, QString::number(ec_project_state_.screenGeneral.flag4_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_19, ec_project_state_.screenGeneral.flag4_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_20, ec_project_state_.screenGeneral.flag5_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_21, QString::number(ec_project_state_.screenGeneral.flag5_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_22, ec_project_state_.screenGeneral.flag5_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_23, ec_project_state_.screenGeneral.flag6_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_24, QString::number(ec_project_state_.screenGeneral.flag6_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_25, ec_project_state_.screenGeneral.flag6_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_26, ec_project_state_.screenGeneral.flag7_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_27, QString::number(ec_project_state_.screenGeneral.flag7_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_28, ec_project_state_.screenGeneral.flag7_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_29, ec_project_state_.screenGeneral.flag8_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_30, QString::number(ec_project_state_.screenGeneral.flag8_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_31, ec_project_state_.screenGeneral.flag8_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_32, ec_project_state_.screenGeneral.flag9_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_33, QString::number(ec_project_state_.screenGeneral.flag9_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_34, ec_project_state_.screenGeneral.flag9_policy);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_35, ec_project_state_.screenGeneral.flag10_col);
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_36, QString::number(ec_project_state_.screenGeneral.flag10_threshold, 'f', 10));
        project_ini.setValue(EcIni::INI_SCREEN_GENERAL_37, ec_project_state_.screenGeneral.flag10_policy);
    project_ini.endGroup();

    // screen settings section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_SETTINGS);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_1, ec_project_state_.screenSetting.max_lack);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_12, ec_project_state_.screenSetting.u_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_13, ec_project_state_.screenSetting.v_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_14, ec_project_state_.screenSetting.w_offset);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_101, ec_project_state_.screenSetting.gill_wm_wboost);
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
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_40, ec_project_state_.screenSetting.filter_sr);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_41, ec_project_state_.screenSetting.filter_al);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_44, ec_project_state_.screenSetting.bu_corr);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_45, ec_project_state_.screenSetting.bu_multi);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_46, QString::number(ec_project_state_.screenSetting.l_day_bot_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_47, QString::number(ec_project_state_.screenSetting.l_day_bot_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_48, QString::number(ec_project_state_.screenSetting.l_day_top_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_49, QString::number(ec_project_state_.screenSetting.l_day_top_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_50, QString::number(ec_project_state_.screenSetting.l_day_spar_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_51, QString::number(ec_project_state_.screenSetting.l_day_spar_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_52, QString::number(ec_project_state_.screenSetting.l_night_bot_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_53, QString::number(ec_project_state_.screenSetting.l_night_bot_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_54, QString::number(ec_project_state_.screenSetting.l_night_top_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_55, QString::number(ec_project_state_.screenSetting.l_night_top_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_56, QString::number(ec_project_state_.screenSetting.l_night_spar_gain, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_57, QString::number(ec_project_state_.screenSetting.l_night_spar_offset, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_58, QString::number(ec_project_state_.screenSetting.m_day_bot1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_59, QString::number(ec_project_state_.screenSetting.m_day_bot2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_60, QString::number(ec_project_state_.screenSetting.m_day_bot3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_61, QString::number(ec_project_state_.screenSetting.m_day_bot4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_62, QString::number(ec_project_state_.screenSetting.m_day_top1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_63, QString::number(ec_project_state_.screenSetting.m_day_top2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_64, QString::number(ec_project_state_.screenSetting.m_day_top3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_65, QString::number(ec_project_state_.screenSetting.m_day_top4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_66, QString::number(ec_project_state_.screenSetting.m_day_spar1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_67, QString::number(ec_project_state_.screenSetting.m_day_spar2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_68, QString::number(ec_project_state_.screenSetting.m_day_spar3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_69, QString::number(ec_project_state_.screenSetting.m_day_spar4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_70, QString::number(ec_project_state_.screenSetting.m_night_bot1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_71, QString::number(ec_project_state_.screenSetting.m_night_bot2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_72, QString::number(ec_project_state_.screenSetting.m_night_bot3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_73, QString::number(ec_project_state_.screenSetting.m_night_bot4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_74, QString::number(ec_project_state_.screenSetting.m_night_top1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_75, QString::number(ec_project_state_.screenSetting.m_night_top2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_76, QString::number(ec_project_state_.screenSetting.m_night_top3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_77, QString::number(ec_project_state_.screenSetting.m_night_top4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_78, QString::number(ec_project_state_.screenSetting.m_night_spar1, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_79, QString::number(ec_project_state_.screenSetting.m_night_spar2, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_80, QString::number(ec_project_state_.screenSetting.m_night_spar3, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_81, QString::number(ec_project_state_.screenSetting.m_night_spar4, 'f', 8));
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_99, ec_project_state_.screenSetting.out_details);
        project_ini.setValue(EcIni::INI_SCREEN_SETTINGS_100, ec_project_state_.screenSetting.power_of_two);
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
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_25, QString::number(ec_project_state_.screenParam.ds_hf_uv, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_26, QString::number(ec_project_state_.screenParam.ds_hf_w, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_27, QString::number(ec_project_state_.screenParam.ds_hf_t, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_28, QString::number(ec_project_state_.screenParam.ds_hf_co2, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_29, QString::number(ec_project_state_.screenParam.ds_hf_h2o, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_50, QString::number(ec_project_state_.screenParam.ds_hf_ch4, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_51, QString::number(ec_project_state_.screenParam.ds_hf_n2o, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_30, QString::number(ec_project_state_.screenParam.ds_hf_var, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_31, QString::number(ec_project_state_.screenParam.ds_sf_uv, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_32, QString::number(ec_project_state_.screenParam.ds_sf_w, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_33, QString::number(ec_project_state_.screenParam.ds_sf_t, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_34, QString::number(ec_project_state_.screenParam.ds_sf_co2, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_35, QString::number(ec_project_state_.screenParam.ds_sf_h2o, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_52, QString::number(ec_project_state_.screenParam.ds_sf_ch4, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_53, QString::number(ec_project_state_.screenParam.ds_sf_n2o, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_36, QString::number(ec_project_state_.screenParam.ds_sf_var, 'f', 2));
        project_ini.setValue(EcIni::INI_SCREEN_PARAM_60, ec_project_state_.screenParam.despike_vm);
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
        project_ini.setValue(EcIni::INI_SCREEN_TILT_12, ec_project_state_.screenTilt.start_time);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_13, ec_project_state_.screenTilt.end_time);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_2, ec_project_state_.screenTilt.mode);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_3, ec_project_state_.screenTilt.north_offset);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_4, ec_project_state_.screenTilt.min_num_per_sec);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_5, QString::number(ec_project_state_.screenTilt.w_max, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_TILT_6, QString::number(ec_project_state_.screenTilt.u_min, 'f', 3));
        project_ini.setValue(EcIni::INI_SCREEN_TILT_7, QDir::fromNativeSeparators(ec_project_state_.screenTilt.file));
        project_ini.setValue(EcIni::INI_SCREEN_TILT_8, ec_project_state_.screenTilt.fix_policy);
        project_ini.setValue(EcIni::INI_SCREEN_TILT_11, ec_project_state_.screenTilt.subset);

        // iterate through angle list
        int k = 0;
        for (const auto &angle : ec_project_state_.screenTilt.angles)
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
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_19, ec_project_state_.timelagOpt.start_time);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_20, ec_project_state_.timelagOpt.end_time);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_2, ec_project_state_.timelagOpt.mode);
        project_ini.setValue(EcIni::INI_TIMELAG_OPT_3, QDir::fromNativeSeparators(ec_project_state_.timelagOpt.file));
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
    return true;
}

// Load a project. Assumes file has been checked with nativeFormat()
bool EcProject::loadEcProject(const QString &filename, bool checkVersion, bool *modified)
{
    auto parent = dynamic_cast<MainWindow*>(this->parent());
    if (parent == nullptr) { return false; }

    bool isVersionCompatible = true;
    QVariant v; // container for conversions

    // open file
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open [loadEcProject()]" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file<br /><p>%1:</p>\n<b>%2</b>")
                             .arg(filename, datafile.errorString()));
        return false;
    }

    QSettings project_ini(filename, QSettings::IniFormat);

    // in case of old non existing file name, use the current existing
    QString projectFilename = project_ini.value(EcIni::INI_PROJECT_2, QString()).toString();
    if (!FileUtils::existsPath(projectFilename))
    {
        projectFilename = filename;
    }

    EcProjectState defaultEcProjectState;
    // general section
    project_ini.beginGroup(EcIni::INIGROUP_PROJECT);
        ec_project_state_.projectGeneral.project_title
            = project_ini.value(EcIni::INI_PROJECT_3,
                                defaultEcProjectState.projectGeneral.project_title).toString();
        ec_project_state_.projectGeneral.project_id
            = project_ini.value(EcIni::INI_PROJECT_6,
                                defaultEcProjectState.projectGeneral.project_id).toString();
        ec_project_state_.projectGeneral.creation_date
            = project_ini.value(EcIni::INI_PROJECT_0,
                                defaultEcProjectState.projectGeneral.creation_date).toString();
        ec_project_state_.projectGeneral.last_change_date
            = project_ini.value(EcIni::INI_PROJECT_1,
                                defaultEcProjectState.projectGeneral.last_change_date).toString();

        v = project_ini.value(EcIni::INI_PROJECT_7,
                              QVariant::fromValue(defaultEcProjectState.projectGeneral.file_type).toInt());
        ec_project_state_.projectGeneral.file_type = static_cast<Defs::RawFileType>(v.toInt());

        ec_project_state_.projectGeneral.use_alt_md_file
                = project_ini.value(EcIni::INI_PROJECT_9,
                                    defaultEcProjectState.projectGeneral.use_alt_md_file).toBool();

        // NOTE: backward compatibility for 'is_express' key
        if (project_ini.contains(EcIni::INI_PROJECT_33_OLD))
        {
            if (checkVersion)
            {
                if (!parent->queryEcProjectImport(filename))
                {
                    return false;
                }

                v = project_ini.value(EcIni::INI_PROJECT_33_OLD,
                         QVariant::fromValue(defaultEcProjectState.projectGeneral.run_mode).toInt());
                ec_project_state_.projectGeneral.run_mode
                    = static_cast<Defs::CurrRunMode>(v.toInt());

                project_ini.remove(EcIni::INI_PROJECT_33_OLD);
                isVersionCompatible = false;
            }
        }
        else
        {
            v = project_ini.value(EcIni::INI_PROJECT_33,
                            QVariant::fromValue(defaultEcProjectState.projectGeneral.run_mode));

            if (v.canConvert<Defs::CurrRunMode>())
            {
                ec_project_state_.projectGeneral.run_mode
                    = v.value<Defs::CurrRunMode>();
            }

            ec_project_state_.projectGeneral.run_fcc
                    = project_ini.value(EcIni::INI_PROJECT_40,
                                        defaultEcProjectState.projectGeneral.run_fcc).toBool();
        }

        ec_project_state_.projectGeneral.file_name = projectFilename;
        ec_project_state_.projectGeneral.sw_version
                = project_ini.value(EcIni::INI_PROJECT_4,
                                    defaultEcProjectState.projectGeneral.sw_version).toString();
        ec_project_state_.projectGeneral.ini_version
                = project_ini.value(EcIni::INI_PROJECT_5,
                                    defaultEcProjectState.projectGeneral.ini_version).toString();
        ec_project_state_.projectGeneral.file_prototype
                = project_ini.value(EcIni::INI_PROJECT_8,
                                    defaultEcProjectState.projectGeneral.file_prototype).toString();
        ec_project_state_.projectGeneral.md_file
                = project_ini.value(EcIni::INI_PROJECT_10,
                                    defaultEcProjectState.projectGeneral.md_file).toString();
        ec_project_state_.projectGeneral.use_tlfile
                = project_ini.value(EcIni::INI_PROJECT_11,
                                    defaultEcProjectState.projectGeneral.use_tlfile).toBool();
        ec_project_state_.projectGeneral.timeline_file
                = project_ini.value(EcIni::INI_PROJECT_12,
                                    defaultEcProjectState.projectGeneral.timeline_file).toString();
        ec_project_state_.projectGeneral.binary_hnlines
                = project_ini.value(EcIni::INI_PROJECT_13,
                                    defaultEcProjectState.projectGeneral.binary_hnlines).toInt();
        ec_project_state_.projectGeneral.binary_eol
                = project_ini.value(EcIni::INI_PROJECT_14,
                                    defaultEcProjectState.projectGeneral.binary_eol).toInt();
        ec_project_state_.projectGeneral.binary_nbytes
                = project_ini.value(EcIni::INI_PROJECT_15,
                                    defaultEcProjectState.projectGeneral.binary_nbytes).toInt();
        ec_project_state_.projectGeneral.binary_little_end
                = project_ini.value(EcIni::INI_PROJECT_16,
                                    defaultEcProjectState.projectGeneral.binary_little_end).toInt();
        ec_project_state_.projectGeneral.master_sonic
                = project_ini.value(EcIni::INI_PROJECT_17,
                                    defaultEcProjectState.projectGeneral.master_sonic).toString();
        ec_project_state_.projectGeneral.col_co2
                = project_ini.value(EcIni::INI_PROJECT_18,
                                    defaultEcProjectState.projectGeneral.col_co2).toInt();
        ec_project_state_.projectGeneral.col_h2o
                = project_ini.value(EcIni::INI_PROJECT_19,
                                    defaultEcProjectState.projectGeneral.col_h2o).toInt();
        ec_project_state_.projectGeneral.col_ch4
                = project_ini.value(EcIni::INI_PROJECT_20,
                                    defaultEcProjectState.projectGeneral.col_ch4).toInt();
        ec_project_state_.projectGeneral.col_n2o
                = project_ini.value(EcIni::INI_PROJECT_21,
                                    defaultEcProjectState.projectGeneral.col_n2o).toInt();
        ec_project_state_.projectGeneral.col_int_t_1
                = project_ini.value(EcIni::INI_PROJECT_22,
                                    defaultEcProjectState.projectGeneral.col_int_t_1).toInt();
        ec_project_state_.projectGeneral.col_int_t_2
                = project_ini.value(EcIni::INI_PROJECT_23,
                                    defaultEcProjectState.projectGeneral.col_int_t_2).toInt();
        ec_project_state_.projectGeneral.col_int_p
                = project_ini.value(EcIni::INI_PROJECT_24,
                                    defaultEcProjectState.projectGeneral.col_int_p).toInt();
        ec_project_state_.projectGeneral.col_air_t
                = project_ini.value(EcIni::INI_PROJECT_25,
                                    defaultEcProjectState.projectGeneral.col_air_t).toInt();
        ec_project_state_.projectGeneral.col_air_p
                = project_ini.value(EcIni::INI_PROJECT_26,
                                    defaultEcProjectState.projectGeneral.col_air_p).toInt();
        ec_project_state_.projectGeneral.col_int_t_c
                = project_ini.value(EcIni::INI_PROJECT_27,
                                    defaultEcProjectState.projectGeneral.col_int_t_c).toInt();
        ec_project_state_.projectGeneral.col_diag_75
                = project_ini.value(EcIni::INI_PROJECT_28,
                                    defaultEcProjectState.projectGeneral.col_diag_75).toInt();
        ec_project_state_.projectGeneral.col_diag_72
                = project_ini.value(EcIni::INI_PROJECT_29,
                                    defaultEcProjectState.projectGeneral.col_diag_72).toInt();
        ec_project_state_.projectGeneral.col_diag_77
                = project_ini.value(EcIni::INI_PROJECT_30,
                                    defaultEcProjectState.projectGeneral.col_diag_77).toInt();
        ec_project_state_.projectGeneral.col_diag_anem
                = project_ini.value(EcIni::INI_PROJECT_69,
                                    defaultEcProjectState.projectGeneral.col_diag_anem).toInt();
        ec_project_state_.projectGeneral.col_ts
                = project_ini.value(EcIni::INI_PROJECT_36,
                                    defaultEcProjectState.projectGeneral.col_ts).toInt();
        ec_project_state_.projectGeneral.gas_mw
                = project_ini.value(EcIni::INI_PROJECT_31,
                                    defaultEcProjectState.projectGeneral.gas_mw).toReal();
        ec_project_state_.projectGeneral.gas_diff
                = project_ini.value(EcIni::INI_PROJECT_32,
                                    defaultEcProjectState.projectGeneral.gas_diff).toReal();

        // NOTE: backward compatibility change for 'out_rich' key
        if (project_ini.value(EcIni::INI_PROJECT_5,
                              defaultEcProjectState.projectGeneral.ini_version).toString() == QLatin1String("1.0"))
        {
            if (checkVersion)
            {
                if (!parent->queryEcProjectImport(filename))
                {
                    return false;
                }
                ec_project_state_.projectGeneral.out_rich = 1;
                isVersionCompatible = false;
            }
        }
        else
        {
            ec_project_state_.projectGeneral.out_rich
                    = project_ini.value(EcIni::INI_PROJECT_39,
                                        defaultEcProjectState.projectGeneral.out_rich).toInt();
        }

        ec_project_state_.projectGeneral.out_md
                = project_ini.value(EcIni::INI_PROJECT_56,
                                    defaultEcProjectState.projectGeneral.out_md).toInt();
        ec_project_state_.projectGeneral.make_dataset
                = project_ini.value(EcIni::INI_PROJECT_41,
                                    defaultEcProjectState.projectGeneral.make_dataset).toBool();

        ec_project_state_.projectGeneral.subset
                = project_ini.value(EcIni::INI_PROJECT_54,
                                    defaultEcProjectState.projectGeneral.subset).toInt();
        ec_project_state_.projectGeneral.start_date
                = project_ini.value(EcIni::INI_PROJECT_42,
                                    QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.projectGeneral.start_time
                = project_ini.value(EcIni::INI_PROJECT_44,
                                    QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.projectGeneral.end_date
                = project_ini.value(EcIni::INI_PROJECT_43,
                                    QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.projectGeneral.end_time
                = project_ini.value(EcIni::INI_PROJECT_45,
                                    QTime(23, 59).toString(QStringLiteral("hh:mm"))).toString();

        ec_project_state_.projectGeneral.hf_meth
                = project_ini.value(EcIni::INI_PROJECT_46,
                                    defaultEcProjectState.projectGeneral.hf_meth).toInt();
        ec_project_state_.projectGeneral.lf_meth
                = project_ini.value(EcIni::INI_PROJECT_47,
                                    defaultEcProjectState.projectGeneral.lf_meth).toInt();
        ec_project_state_.projectGeneral.wpl_meth
                = project_ini.value(EcIni::INI_PROJECT_48,
                                    defaultEcProjectState.projectGeneral.wpl_meth).toInt();
        ec_project_state_.projectGeneral.foot_meth
                = project_ini.value(EcIni::INI_PROJECT_49,
                                    defaultEcProjectState.projectGeneral.foot_meth).toInt();
        ec_project_state_.projectGeneral.tob1_format
                = project_ini.value(EcIni::INI_PROJECT_50,
                                    defaultEcProjectState.projectGeneral.tob1_format).toInt();
        ec_project_state_.projectGeneral.out_path
                = project_ini.value(EcIni::INI_PROJECT_51,
                                    defaultEcProjectState.projectGeneral.out_path).toString();
        ec_project_state_.projectGeneral.fix_out_format
                = project_ini.value(EcIni::INI_PROJECT_52,
                                    defaultEcProjectState.projectGeneral.fix_out_format).toInt();
        ec_project_state_.projectGeneral.err_label
                = project_ini.value(EcIni::INI_PROJECT_53,
                                    defaultEcProjectState.projectGeneral.err_label).toString();
        ec_project_state_.projectGeneral.qcflag_meth
                = project_ini.value(EcIni::INI_PROJECT_55,
                                    defaultEcProjectState.projectGeneral.qcflag_meth).toInt();
        ec_project_state_.projectGeneral.use_biomet
                = project_ini.value(EcIni::INI_PROJECT_34,
                                    defaultEcProjectState.projectGeneral.use_biomet).toInt();
        ec_project_state_.projectGeneral.biom_file
                = project_ini.value(EcIni::INI_PROJECT_35,
                                    defaultEcProjectState.projectGeneral.biom_file).toString();
        ec_project_state_.projectGeneral.biom_dir
                = project_ini.value(EcIni::INI_PROJECT_57,
                                    defaultEcProjectState.projectGeneral.biom_dir).toString();
        ec_project_state_.projectGeneral.biom_recurse
                = project_ini.value(EcIni::INI_PROJECT_58,
                                    defaultEcProjectState.projectGeneral.biom_recurse).toInt();
        ec_project_state_.projectGeneral.biom_ext
                = project_ini.value(EcIni::INI_PROJECT_59,
                                    defaultEcProjectState.projectGeneral.biom_ext).toString().remove(QLatin1Char('.'));
        ec_project_state_.projectGeneral.out_mean_cosp
                = project_ini.value(EcIni::INI_PROJECT_60,
                                    defaultEcProjectState.projectGeneral.out_mean_cosp).toInt();
        ec_project_state_.projectGeneral.out_biomet
                = project_ini.value(EcIni::INI_PROJECT_61,
                                    defaultEcProjectState.projectGeneral.out_biomet).toInt();
        ec_project_state_.projectGeneral.bin_sp_avail
                = project_ini.value(EcIni::INI_PROJECT_62,
                                    defaultEcProjectState.projectGeneral.bin_sp_avail).toInt();
        ec_project_state_.projectGeneral.full_sp_avail
                = project_ini.value(EcIni::INI_PROJECT_63,
                                    defaultEcProjectState.projectGeneral.full_sp_avail).toInt();
        ec_project_state_.projectGeneral.files_found
                = project_ini.value(EcIni::INI_PROJECT_64,
                                    defaultEcProjectState.projectGeneral.files_found).toInt();
        ec_project_state_.projectGeneral.out_mean_spectra
                = project_ini.value(EcIni::INI_PROJECT_65,
                                    defaultEcProjectState.projectGeneral.out_mean_spectra).toInt();
        ec_project_state_.projectGeneral.hf_correct_ghg_ba
                = project_ini.value(EcIni::INI_PROJECT_66,
                                    defaultEcProjectState.projectGeneral.hf_correct_ghg_ba).toInt();
        ec_project_state_.projectGeneral.hf_correct_ghg_zoh
                = project_ini.value(EcIni::INI_PROJECT_67,
                                    defaultEcProjectState.projectGeneral.hf_correct_ghg_zoh).toInt();
        ec_project_state_.projectGeneral.sonic_output_rate
                = project_ini.value(EcIni::INI_PROJECT_68,
                                    defaultEcProjectState.projectGeneral.sonic_output_rate).toInt();
        ec_project_state_.projectGeneral.ru_method
                = project_ini.value(EcIni::INI_RAND_ERROR_0,
                                    defaultEcProjectState.projectGeneral.ru_method).toInt();
        ec_project_state_.projectGeneral.its_method
                = project_ini.value(EcIni::INI_RAND_ERROR_1,
                                    defaultEcProjectState.projectGeneral.its_method).toInt();
        ec_project_state_.projectGeneral.its_tlag_max
                = project_ini.value(EcIni::INI_RAND_ERROR_2,
                                    defaultEcProjectState.projectGeneral.its_tlag_max).toDouble();
    project_ini.endGroup();

    // spec settings section
    project_ini.beginGroup(EcIni::INIGROUP_SPEC_SETTINGS);
        ec_project_state_.spectraSettings.start_sa_date
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_0,
                                    QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.spectraSettings.end_sa_date
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_1,
                                    QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.spectraSettings.start_sa_time
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_50,
                                    QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.spectraSettings.end_sa_time
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_51,
                                    QTime(23, 59).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.spectraSettings.sa_mode
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_2,
                                    defaultEcProjectState.spectraSettings.sa_mode).toInt();
        ec_project_state_.spectraSettings.sa_file
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_3,
                                    defaultEcProjectState.spectraSettings.sa_file).toString();
        ec_project_state_.spectraSettings.sa_min_smpl
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_4,
                                    defaultEcProjectState.spectraSettings.sa_min_smpl).toInt();
        ec_project_state_.spectraSettings.sa_fmin_co2
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_5,
                                    defaultEcProjectState.spectraSettings.sa_fmax_co2).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_h2o
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_6,
                                    defaultEcProjectState.spectraSettings.sa_fmin_h2o).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_ch4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_7,
                                    defaultEcProjectState.spectraSettings.sa_fmin_ch4).toDouble();
        ec_project_state_.spectraSettings.sa_fmin_gas4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_8,
                                    defaultEcProjectState.spectraSettings.sa_fmin_gas4).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_co2
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_9,
                                    defaultEcProjectState.spectraSettings.sa_fmax_co2).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_h2o
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_10,
                                    defaultEcProjectState.spectraSettings.sa_fmax_h2o).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_ch4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_11,
                                    defaultEcProjectState.spectraSettings.sa_fmax_ch4).toDouble();
        ec_project_state_.spectraSettings.sa_fmax_gas4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_12,
                                    defaultEcProjectState.spectraSettings.sa_fmax_gas4).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_co2_fmin
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_13,
                                    defaultEcProjectState.spectraSettings.sa_hfn_co2_fmin).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_h2o_fmin
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_14,
                                    defaultEcProjectState.spectraSettings.sa_hfn_h2o_fmin).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_ch4_fmin
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_15,
                                    defaultEcProjectState.spectraSettings.sa_hfn_ch4_fmin).toDouble();
        ec_project_state_.spectraSettings.sa_hfn_gas4_fmin
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_16,
                                    defaultEcProjectState.spectraSettings.sa_hfn_gas4_fmin).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_ustar
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_36,
                                    defaultEcProjectState.spectraSettings.sa_min_un_ustar).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_h
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_37,
                                    defaultEcProjectState.spectraSettings.sa_min_un_h).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_le
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_38,
                                    defaultEcProjectState.spectraSettings.sa_min_un_le).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_co2
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_39,
                                    defaultEcProjectState.spectraSettings.sa_min_un_co2).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_ch4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_40,
                                    defaultEcProjectState.spectraSettings.sa_min_un_ch4).toDouble();
        ec_project_state_.spectraSettings.sa_min_un_gas4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_41,
                                    defaultEcProjectState.spectraSettings.sa_min_un_gas4).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_ustar
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_35,
                                    defaultEcProjectState.spectraSettings.sa_min_st_ustar).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_h
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_21,
                                    defaultEcProjectState.spectraSettings.sa_min_st_h).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_le
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_20,
                                    defaultEcProjectState.spectraSettings.sa_min_st_le).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_co2
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_17,
                                    defaultEcProjectState.spectraSettings.sa_min_st_co2).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_ch4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_18,
                                    defaultEcProjectState.spectraSettings.sa_min_st_ch4).toDouble();
        ec_project_state_.spectraSettings.sa_min_st_gas4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_19,
                                    defaultEcProjectState.spectraSettings.sa_min_st_gas4).toDouble();
        ec_project_state_.spectraSettings.sa_max_ustar
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_42,
                                    defaultEcProjectState.spectraSettings.sa_max_ustar).toDouble();
        ec_project_state_.spectraSettings.sa_max_h
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_43,
                                    defaultEcProjectState.spectraSettings.sa_max_h).toDouble();
        ec_project_state_.spectraSettings.sa_max_le
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_44,
                                    defaultEcProjectState.spectraSettings.sa_max_le).toDouble();
        ec_project_state_.spectraSettings.sa_max_co2
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_45,
                                    defaultEcProjectState.spectraSettings.sa_max_co2).toDouble();
        ec_project_state_.spectraSettings.sa_max_ch4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_46,
                                    defaultEcProjectState.spectraSettings.sa_max_ch4).toDouble();
        ec_project_state_.spectraSettings.sa_max_gas4
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_47,
                                    defaultEcProjectState.spectraSettings.sa_max_gas4).toDouble();
        ec_project_state_.spectraSettings.add_sonic_lptf
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_22,
                                    defaultEcProjectState.spectraSettings.add_sonic_lptf).toInt();
        ec_project_state_.spectraSettings.horst_lens
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_28,
                                    defaultEcProjectState.spectraSettings.horst_lens).toInt();
        ec_project_state_.spectraSettings.ex_file
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_29,
                                    defaultEcProjectState.spectraSettings.ex_file).toString();
        ec_project_state_.spectraSettings.sa_bin_spectra
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_30,
                                    defaultEcProjectState.spectraSettings.sa_bin_spectra).toString();
        ec_project_state_.spectraSettings.sa_full_spectra
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_31,
                                    defaultEcProjectState.spectraSettings.sa_full_spectra).toString();
        ec_project_state_.spectraSettings.ex_dir
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_32,
                                    defaultEcProjectState.spectraSettings.ex_dir).toString();
        ec_project_state_.spectraSettings.subset
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_33,
                                    defaultEcProjectState.spectraSettings.subset).toInt();
        ec_project_state_.spectraSettings.use_vm_flags
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_34,
                                    defaultEcProjectState.spectraSettings.use_vm_flags).toInt();
        ec_project_state_.spectraSettings.use_foken_low
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_48,
                                    defaultEcProjectState.spectraSettings.use_foken_low).toInt();
        ec_project_state_.spectraSettings.use_foken_mid
                = project_ini.value(EcIni::INI_SPEC_SETTINGS_49,
                                    defaultEcProjectState.spectraSettings.use_foken_mid).toInt();
    project_ini.endGroup();

    // preproc general section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_GENERAL);
        ec_project_state_.screenGeneral.data_path
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_4,
                                    defaultEcProjectState.screenGeneral.data_path).toString();
        ec_project_state_.screenGeneral.recurse
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_6,
                                    defaultEcProjectState.screenGeneral.recurse).toInt();
        ec_project_state_.screenGeneral.use_geo_north
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_40,
                                    defaultEcProjectState.screenGeneral.use_geo_north).toBool();
        ec_project_state_.screenGeneral.mag_dec
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_38,
                                    defaultEcProjectState.screenGeneral.mag_dec).toReal();
        ec_project_state_.screenGeneral.dec_date
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_39,
                                    defaultEcProjectState.projectGeneral.end_date).toString();
        ec_project_state_.screenGeneral.flag1_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_8,
                                    defaultEcProjectState.screenGeneral.flag1_col).toInt();
        ec_project_state_.screenGeneral.flag1_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_9,
                                    defaultEcProjectState.screenGeneral.flag1_threshold).toReal();
        ec_project_state_.screenGeneral.flag1_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_10,
                                    defaultEcProjectState.screenGeneral.flag1_policy).toInt();
        ec_project_state_.screenGeneral.flag2_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_11,
                                    defaultEcProjectState.screenGeneral.flag2_col).toInt();
        ec_project_state_.screenGeneral.flag2_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_12,
                                    defaultEcProjectState.screenGeneral.flag2_threshold).toReal();
        ec_project_state_.screenGeneral.flag2_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_13,
                                    defaultEcProjectState.screenGeneral.flag2_policy).toInt();
        ec_project_state_.screenGeneral.flag3_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_14,
                                    defaultEcProjectState.screenGeneral.flag3_col).toInt();
        ec_project_state_.screenGeneral.flag3_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_15,
                                    defaultEcProjectState.screenGeneral.flag3_threshold).toReal();
        ec_project_state_.screenGeneral.flag3_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_16,
                                    defaultEcProjectState.screenGeneral.flag3_policy).toInt();
        ec_project_state_.screenGeneral.flag4_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_17,
                                    defaultEcProjectState.screenGeneral.flag4_col).toInt();
        ec_project_state_.screenGeneral.flag4_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_18,
                                    defaultEcProjectState.screenGeneral.flag4_threshold).toReal();
        ec_project_state_.screenGeneral.flag4_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_19,
                                    defaultEcProjectState.screenGeneral.flag4_policy).toInt();
        ec_project_state_.screenGeneral.flag5_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_20,
                                    defaultEcProjectState.screenGeneral.flag5_col).toInt();
        ec_project_state_.screenGeneral.flag5_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_21,
                                    defaultEcProjectState.screenGeneral.flag5_threshold).toReal();
        ec_project_state_.screenGeneral.flag5_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_22,
                                    defaultEcProjectState.screenGeneral.flag5_policy).toInt();
        ec_project_state_.screenGeneral.flag6_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_23,
                                    defaultEcProjectState.screenGeneral.flag6_col).toInt();
        ec_project_state_.screenGeneral.flag6_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_24,
                                    defaultEcProjectState.screenGeneral.flag6_threshold).toReal();
        ec_project_state_.screenGeneral.flag6_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_25,
                                    defaultEcProjectState.screenGeneral.flag6_policy).toInt();
        ec_project_state_.screenGeneral.flag7_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_26,
                                    defaultEcProjectState.screenGeneral.flag7_col).toInt();
        ec_project_state_.screenGeneral.flag7_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_27,
                                    defaultEcProjectState.screenGeneral.flag7_threshold).toReal();
        ec_project_state_.screenGeneral.flag7_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_28,
                                    defaultEcProjectState.screenGeneral.flag7_policy).toInt();
        ec_project_state_.screenGeneral.flag8_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_29,
                                    defaultEcProjectState.screenGeneral.flag8_col).toInt();
        ec_project_state_.screenGeneral.flag8_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_30,
                                    defaultEcProjectState.screenGeneral.flag8_threshold).toReal();
        ec_project_state_.screenGeneral.flag8_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_31,
                                    defaultEcProjectState.screenGeneral.flag8_policy).toInt();
        ec_project_state_.screenGeneral.flag9_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_32,
                                    defaultEcProjectState.screenGeneral.flag9_col).toInt();
        ec_project_state_.screenGeneral.flag9_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_33,
                                    defaultEcProjectState.screenGeneral.flag9_threshold).toReal();
        ec_project_state_.screenGeneral.flag9_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_34,
                                    defaultEcProjectState.screenGeneral.flag9_policy).toInt();
        ec_project_state_.screenGeneral.flag10_col
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_35,
                                    defaultEcProjectState.screenGeneral.flag10_col).toInt();
        ec_project_state_.screenGeneral.flag10_threshold
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_36,
                                    defaultEcProjectState.screenGeneral.flag10_threshold).toReal();
        ec_project_state_.screenGeneral.flag10_policy
                = project_ini.value(EcIni::INI_SCREEN_GENERAL_37,
                                    defaultEcProjectState.screenGeneral.flag10_policy).toInt();
    project_ini.endGroup();

    // preproc setup section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_SETTINGS);
        ec_project_state_.screenSetting.max_lack
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_1,
                                    defaultEcProjectState.screenSetting.max_lack).toInt();
        ec_project_state_.screenSetting.u_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_12,
                                    defaultEcProjectState.screenSetting.u_offset).toDouble();
        ec_project_state_.screenSetting.v_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_13,
                                    defaultEcProjectState.screenSetting.v_offset).toDouble();
        ec_project_state_.screenSetting.w_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_14,
                                    defaultEcProjectState.screenSetting.w_offset).toDouble();
        ec_project_state_.screenSetting.cross_wind
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_2,
                                    defaultEcProjectState.screenSetting.cross_wind).toInt();
        ec_project_state_.screenSetting.gill_wm_wboost
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_101,
                                    defaultEcProjectState.screenSetting.gill_wm_wboost).toInt();
        ec_project_state_.screenSetting.flow_distortion
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_3,
                                    defaultEcProjectState.screenSetting.flow_distortion).toInt();
        ec_project_state_.screenSetting.rot_meth
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_4,
                                    defaultEcProjectState.screenSetting.rot_meth).toInt();
        ec_project_state_.screenSetting.detrend_meth
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_5,
                                    defaultEcProjectState.screenSetting.detrend_meth).toInt();
        ec_project_state_.screenSetting.timeconst
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_6,
                                    defaultEcProjectState.screenSetting.timeconst).toDouble();
        ec_project_state_.screenSetting.tlag_meth
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_7,
                                    defaultEcProjectState.screenSetting.tlag_meth).toInt();
        ec_project_state_.screenSetting.tap_win
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_8,
                                    defaultEcProjectState.screenSetting.tap_win).toInt();
        ec_project_state_.screenSetting.nbins
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_9,
                                    defaultEcProjectState.screenSetting.nbins).toInt();
        ec_project_state_.screenSetting.avrg_len
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_10,
                                    defaultEcProjectState.screenSetting.avrg_len).toInt();
        ec_project_state_.screenSetting.out_bin_sp
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_15,
                                    defaultEcProjectState.screenSetting.out_bin_sp).toInt();
        ec_project_state_.screenSetting.out_bin_og
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_38,
                                    defaultEcProjectState.screenSetting.out_bin_og).toInt();
        ec_project_state_.screenSetting.out_full_sp_u
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_16,
                                    defaultEcProjectState.screenSetting.out_full_sp_u).toInt();
        ec_project_state_.screenSetting.out_full_sp_v
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_17,
                                    defaultEcProjectState.screenSetting.out_full_sp_v).toInt();
        ec_project_state_.screenSetting.out_full_sp_w
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_18,
                                    defaultEcProjectState.screenSetting.out_full_sp_w).toInt();
        ec_project_state_.screenSetting.out_full_sp_ts
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_19,
                                    defaultEcProjectState.screenSetting.out_full_sp_ts).toInt();
        ec_project_state_.screenSetting.out_full_sp_co2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_20,
                                    defaultEcProjectState.screenSetting.out_full_sp_co2).toInt();
        ec_project_state_.screenSetting.out_full_sp_h2o
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_21,
                                    defaultEcProjectState.screenSetting.out_full_sp_h2o).toInt();
        ec_project_state_.screenSetting.out_full_sp_ch4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_22,
                                    defaultEcProjectState.screenSetting.out_full_sp_ch4).toInt();
        ec_project_state_.screenSetting.out_full_sp_n2o
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_23,
                                    defaultEcProjectState.screenSetting.out_full_sp_n2o).toInt();
        ec_project_state_.screenSetting.out_full_cosp_u
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_31,
                                    defaultEcProjectState.screenSetting.out_full_cosp_u).toInt();
        ec_project_state_.screenSetting.out_full_cosp_v
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_32,
                                    defaultEcProjectState.screenSetting.out_full_cosp_v).toInt();
        ec_project_state_.screenSetting.out_full_cosp_ts
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_33,
                                    defaultEcProjectState.screenSetting.out_full_cosp_ts).toInt();
        ec_project_state_.screenSetting.out_full_cosp_co2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_34,
                                    defaultEcProjectState.screenSetting.out_full_cosp_co2).toInt();
        ec_project_state_.screenSetting.out_full_cosp_h2o
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_35,
                                    defaultEcProjectState.screenSetting.out_full_cosp_h2o).toInt();
        ec_project_state_.screenSetting.out_full_cosp_ch4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_36,
                                    defaultEcProjectState.screenSetting.out_full_cosp_ch4).toInt();
        ec_project_state_.screenSetting.out_full_cosp_n2o
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_37,
                                    defaultEcProjectState.screenSetting.out_full_cosp_n2o).toInt();
        ec_project_state_.screenSetting.out_st_1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_24,
                                    defaultEcProjectState.screenSetting.out_st_1).toInt();
        ec_project_state_.screenSetting.out_st_2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_25,
                                    defaultEcProjectState.screenSetting.out_st_2).toInt();
        ec_project_state_.screenSetting.out_st_3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_26,
                                    defaultEcProjectState.screenSetting.out_st_3).toInt();
        ec_project_state_.screenSetting.out_st_4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_27,
                                    defaultEcProjectState.screenSetting.out_st_4).toInt();
        ec_project_state_.screenSetting.out_st_5
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_28,
                                    defaultEcProjectState.screenSetting.out_st_5).toInt();
        ec_project_state_.screenSetting.out_st_6
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_29,
                                    defaultEcProjectState.screenSetting.out_st_6).toInt();
        ec_project_state_.screenSetting.out_st_7
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_30,
                                    defaultEcProjectState.screenSetting.out_st_7).toInt();
        ec_project_state_.screenSetting.out_raw_1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_82,
                                    defaultEcProjectState.screenSetting.out_raw_1).toInt();
        ec_project_state_.screenSetting.out_raw_2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_83,
                                    defaultEcProjectState.screenSetting.out_raw_2).toInt();
        ec_project_state_.screenSetting.out_raw_3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_84,
                                    defaultEcProjectState.screenSetting.out_raw_3).toInt();
        ec_project_state_.screenSetting.out_raw_4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_85,
                                    defaultEcProjectState.screenSetting.out_raw_4).toInt();
        ec_project_state_.screenSetting.out_raw_5
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_86,
                                    defaultEcProjectState.screenSetting.out_raw_5).toInt();
        ec_project_state_.screenSetting.out_raw_6
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_87,
                                    defaultEcProjectState.screenSetting.out_raw_6).toInt();
        ec_project_state_.screenSetting.out_raw_7
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_88,
                                    defaultEcProjectState.screenSetting.out_raw_7).toInt();
        ec_project_state_.screenSetting.out_raw_u
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_89,
                                    defaultEcProjectState.screenSetting.out_raw_u).toInt();
        ec_project_state_.screenSetting.out_raw_v
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_90,
                                    defaultEcProjectState.screenSetting.out_raw_v).toInt();
        ec_project_state_.screenSetting.out_raw_w
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_91,
                                    defaultEcProjectState.screenSetting.out_raw_w).toInt();
        ec_project_state_.screenSetting.out_raw_ts
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_92,
                                    defaultEcProjectState.screenSetting.out_raw_ts).toInt();
        ec_project_state_.screenSetting.out_raw_co2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_93,
                                    defaultEcProjectState.screenSetting.out_raw_co2).toInt();
        ec_project_state_.screenSetting.out_raw_h2o
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_94,
                                    defaultEcProjectState.screenSetting.out_raw_h2o).toInt();
        ec_project_state_.screenSetting.out_raw_ch4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_95,
                                    defaultEcProjectState.screenSetting.out_raw_ch4).toInt();
        ec_project_state_.screenSetting.out_raw_gas4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_96,
                                    defaultEcProjectState.screenSetting.out_raw_gas4).toInt();
        ec_project_state_.screenSetting.out_raw_tair
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_97,
                                    defaultEcProjectState.screenSetting.out_raw_tair).toInt();
        ec_project_state_.screenSetting.out_raw_pair
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_98,
                                    defaultEcProjectState.screenSetting.out_raw_pair).toInt();
        ec_project_state_.screenSetting.filter_sr
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_40,
                                    defaultEcProjectState.screenSetting.filter_sr).toInt();
        ec_project_state_.screenSetting.filter_al
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_41,
                                    defaultEcProjectState.screenSetting.filter_al).toInt();
        ec_project_state_.screenSetting.bu_corr
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_44,
                                    defaultEcProjectState.screenSetting.bu_corr).toInt();
        ec_project_state_.screenSetting.bu_multi
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_45,
                                    defaultEcProjectState.screenSetting.bu_multi).toInt();
        ec_project_state_.screenSetting.l_day_bot_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_46,
                                    defaultEcProjectState.screenSetting.l_day_bot_gain).toDouble();
        ec_project_state_.screenSetting.l_day_bot_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_47,
                                    defaultEcProjectState.screenSetting.l_day_bot_offset).toDouble();
        ec_project_state_.screenSetting.l_day_top_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_48,
                                    defaultEcProjectState.screenSetting.l_day_top_gain).toDouble();
        ec_project_state_.screenSetting.l_day_top_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_49,
                                    defaultEcProjectState.screenSetting.l_day_top_offset).toDouble();
        ec_project_state_.screenSetting.l_day_spar_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_50,
                                    defaultEcProjectState.screenSetting.l_day_spar_gain).toDouble();
        ec_project_state_.screenSetting.l_day_spar_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_51,
                                    defaultEcProjectState.screenSetting.l_day_spar_offset).toDouble();
        ec_project_state_.screenSetting.l_night_bot_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_52,
                                    defaultEcProjectState.screenSetting.l_night_bot_gain).toDouble();
        ec_project_state_.screenSetting.l_night_bot_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_53,
                                    defaultEcProjectState.screenSetting.l_night_bot_offset).toDouble();
        ec_project_state_.screenSetting.l_night_top_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_54,
                                    defaultEcProjectState.screenSetting.l_night_top_gain).toDouble();
        ec_project_state_.screenSetting.l_night_top_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_55,
                                    defaultEcProjectState.screenSetting.l_night_top_offset).toDouble();
        ec_project_state_.screenSetting.l_night_spar_gain
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_56,
                                    defaultEcProjectState.screenSetting.l_night_spar_gain).toDouble();
        ec_project_state_.screenSetting.l_night_spar_offset
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_57,
                                    defaultEcProjectState.screenSetting.l_night_spar_offset).toDouble();
        ec_project_state_.screenSetting.m_day_bot1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_58,
                                    defaultEcProjectState.screenSetting.m_day_bot1).toDouble();
        ec_project_state_.screenSetting.m_day_bot2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_59,
                                    defaultEcProjectState.screenSetting.m_day_bot2).toDouble();
        ec_project_state_.screenSetting.m_day_bot3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_60,
                                    defaultEcProjectState.screenSetting.m_day_bot3).toDouble();
        ec_project_state_.screenSetting.m_day_bot4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_61,
                                    defaultEcProjectState.screenSetting.m_day_bot4).toDouble();
        ec_project_state_.screenSetting.m_day_top1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_62,
                                    defaultEcProjectState.screenSetting.m_day_top1).toDouble();
        ec_project_state_.screenSetting.m_day_top2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_63,
                                    defaultEcProjectState.screenSetting.m_day_top2).toDouble();
        ec_project_state_.screenSetting.m_day_top3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_64,
                                    defaultEcProjectState.screenSetting.m_day_top3).toDouble();
        ec_project_state_.screenSetting.m_day_top4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_65,
                                    defaultEcProjectState.screenSetting.m_day_top4).toDouble();
        ec_project_state_.screenSetting.m_day_spar1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_66,
                                    defaultEcProjectState.screenSetting.m_day_spar1).toDouble();
        ec_project_state_.screenSetting.m_day_spar2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_67,
                                    defaultEcProjectState.screenSetting.m_day_spar2).toDouble();
        ec_project_state_.screenSetting.m_day_spar3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_68,
                                    defaultEcProjectState.screenSetting.m_day_spar3).toDouble();
        ec_project_state_.screenSetting.m_day_spar4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_69,
                                    defaultEcProjectState.screenSetting.m_day_spar4).toDouble();
        ec_project_state_.screenSetting.m_night_bot1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_70,
                                    defaultEcProjectState.screenSetting.m_night_bot1).toDouble();
        ec_project_state_.screenSetting.m_night_bot2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_71,
                                    defaultEcProjectState.screenSetting.m_night_bot2).toDouble();
        ec_project_state_.screenSetting.m_night_bot3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_72,
                                    defaultEcProjectState.screenSetting.m_night_bot3).toDouble();
        ec_project_state_.screenSetting.m_night_bot4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_73,
                                    defaultEcProjectState.screenSetting.m_night_bot4).toDouble();
        ec_project_state_.screenSetting.m_night_top1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_74,
                                    defaultEcProjectState.screenSetting.m_night_top1).toDouble();
        ec_project_state_.screenSetting.m_night_top2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_75,
                                    defaultEcProjectState.screenSetting.m_night_top2).toDouble();
        ec_project_state_.screenSetting.m_night_top3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_76,
                                    defaultEcProjectState.screenSetting.m_night_top3).toDouble();
        ec_project_state_.screenSetting.m_night_top4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_77,
                                    defaultEcProjectState.screenSetting.m_night_top4).toDouble();
        ec_project_state_.screenSetting.m_night_spar1
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_78,
                                    defaultEcProjectState.screenSetting.m_night_spar1).toDouble();
        ec_project_state_.screenSetting.m_night_spar2
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_79,
                                    defaultEcProjectState.screenSetting.m_night_spar2).toDouble();
        ec_project_state_.screenSetting.m_night_spar3
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_80,
                                    defaultEcProjectState.screenSetting.m_night_spar3).toDouble();
        ec_project_state_.screenSetting.m_night_spar4
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_81,
                                    defaultEcProjectState.screenSetting.m_night_spar4).toDouble();
        ec_project_state_.screenSetting.out_details =
                project_ini.value(EcIni::INI_SCREEN_SETTINGS_99,
                                  defaultEcProjectState.screenSetting.out_details).toInt();
        ec_project_state_.screenSetting.power_of_two
                = project_ini.value(EcIni::INI_SCREEN_SETTINGS_100,
                                    defaultEcProjectState.screenSetting.power_of_two).toInt();
    project_ini.endGroup();

    // preproc test section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TESTS);
        ec_project_state_.screenTest.test_sr
                = project_ini.value(EcIni::INI_SCREEN_TESTS_0,
                                    defaultEcProjectState.screenTest.test_sr).toInt();
        ec_project_state_.screenTest.test_ar
                = project_ini.value(EcIni::INI_SCREEN_TESTS_1,
                                    defaultEcProjectState.screenTest.test_ar).toInt();
        ec_project_state_.screenTest.test_do
                = project_ini.value(EcIni::INI_SCREEN_TESTS_2,
                                    defaultEcProjectState.screenTest.test_do).toInt();
        ec_project_state_.screenTest.test_al
                = project_ini.value(EcIni::INI_SCREEN_TESTS_3,
                                    defaultEcProjectState.screenTest.test_al).toInt();
        ec_project_state_.screenTest.test_sk
                = project_ini.value(EcIni::INI_SCREEN_TESTS_4,
                                    defaultEcProjectState.screenTest.test_sk).toInt();
        ec_project_state_.screenTest.test_ds
                = project_ini.value(EcIni::INI_SCREEN_TESTS_5,
                                    defaultEcProjectState.screenTest.test_ds).toInt();
        ec_project_state_.screenTest.test_tl
                = project_ini.value(EcIni::INI_SCREEN_TESTS_6,
                                    defaultEcProjectState.screenTest.test_tl).toInt();
        ec_project_state_.screenTest.test_aa
                = project_ini.value(EcIni::INI_SCREEN_TESTS_7,
                                    defaultEcProjectState.screenTest.test_aa).toInt();
        ec_project_state_.screenTest.test_ns
                = project_ini.value(EcIni::INI_SCREEN_TESTS_8,
                                    defaultEcProjectState.screenTest.test_ns).toInt();
    project_ini.endGroup();

    // preproc test section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_PARAM);
        ec_project_state_.screenParam.sr_num_spk
                = project_ini.value(EcIni::INI_SCREEN_PARAM_0,
                                    defaultEcProjectState.screenParam.sr_num_spk).toInt();
        ec_project_state_.screenParam.sr_lim_u
                = project_ini.value(EcIni::INI_SCREEN_PARAM_1,
                                    defaultEcProjectState.screenParam.sr_lim_u).toDouble();
        ec_project_state_.screenParam.sr_lim_w
                = project_ini.value(EcIni::INI_SCREEN_PARAM_45,
                                    defaultEcProjectState.screenParam.sr_lim_w).toDouble();
        ec_project_state_.screenParam.sr_lim_co2
                = project_ini.value(EcIni::INI_SCREEN_PARAM_46,
                                    defaultEcProjectState.screenParam.sr_lim_co2).toDouble();
        ec_project_state_.screenParam.sr_lim_h2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_47,
                                    defaultEcProjectState.screenParam.sr_lim_h2o).toDouble();
        ec_project_state_.screenParam.sr_lim_ch4
                = project_ini.value(EcIni::INI_SCREEN_PARAM_48,
                                    defaultEcProjectState.screenParam.sr_lim_ch4).toDouble();
        ec_project_state_.screenParam.sr_lim_n2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_49,
                                    defaultEcProjectState.screenParam.sr_lim_n2o).toDouble();
        ec_project_state_.screenParam.sr_lim_hf
                = project_ini.value(EcIni::INI_SCREEN_PARAM_2,
                                    defaultEcProjectState.screenParam.sr_lim_hf).toDouble();
        ec_project_state_.screenParam.ar_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_3,
                                    defaultEcProjectState.screenParam.ar_lim).toDouble();
        ec_project_state_.screenParam.ar_bins
                = project_ini.value(EcIni::INI_SCREEN_PARAM_4,
                                    defaultEcProjectState.screenParam.ar_bins).toInt();
        ec_project_state_.screenParam.ar_hf_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_5,
                                    defaultEcProjectState.screenParam.ar_hf_lim).toInt();
        ec_project_state_.screenParam.do_extlim_dw
                = project_ini.value(EcIni::INI_SCREEN_PARAM_6,
                                    defaultEcProjectState.screenParam.do_extlim_dw).toInt();
        ec_project_state_.screenParam.do_hf1_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_7,
                                    defaultEcProjectState.screenParam.do_hf1_lim).toDouble();
        ec_project_state_.screenParam.do_hf2_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_8,
                                    defaultEcProjectState.screenParam.do_hf2_lim).toDouble();
        ec_project_state_.screenParam.al_u_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_9,
                                    defaultEcProjectState.screenParam.al_u_max).toDouble();
        ec_project_state_.screenParam.al_w_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_10,
                                    defaultEcProjectState.screenParam.al_w_max).toDouble();
        ec_project_state_.screenParam.al_tson_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_11,
                                    defaultEcProjectState.screenParam.al_tson_min).toDouble();
        ec_project_state_.screenParam.al_tson_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_12,
                                    defaultEcProjectState.screenParam.al_tson_max).toDouble();
        ec_project_state_.screenParam.al_co2_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_13,
                                    defaultEcProjectState.screenParam.al_co2_min).toDouble();
        ec_project_state_.screenParam.al_co2_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_14,
                                    defaultEcProjectState.screenParam.al_co2_max).toDouble();
        ec_project_state_.screenParam.al_h2o_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_15,
                                    defaultEcProjectState.screenParam.al_h2o_min).toDouble();
        ec_project_state_.screenParam.al_h2o_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_16,
                                    defaultEcProjectState.screenParam.al_h2o_max).toDouble();
        ec_project_state_.screenParam.al_ch4_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_54,
                                    defaultEcProjectState.screenParam.al_ch4_min).toDouble();
        ec_project_state_.screenParam.al_ch4_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_55,
                                    defaultEcProjectState.screenParam.al_ch4_max).toDouble();
        ec_project_state_.screenParam.al_n2o_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_56,
                                    defaultEcProjectState.screenParam.al_n2o_min).toDouble();
        ec_project_state_.screenParam.al_n2o_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_57,
                                    defaultEcProjectState.screenParam.al_n2o_max).toDouble();
        ec_project_state_.screenParam.sk_hf_skmin
                = project_ini.value(EcIni::INI_SCREEN_PARAM_17,
                                    defaultEcProjectState.screenParam.sk_hf_skmin).toDouble();
        ec_project_state_.screenParam.sk_hf_skmax
                = project_ini.value(EcIni::INI_SCREEN_PARAM_18,
                                    defaultEcProjectState.screenParam.sk_hf_skmax).toDouble();
        ec_project_state_.screenParam.sk_sf_skmin
                = project_ini.value(EcIni::INI_SCREEN_PARAM_19,
                                    defaultEcProjectState.screenParam.sk_sf_skmin).toDouble();
        ec_project_state_.screenParam.sk_sf_skmax
                = project_ini.value(EcIni::INI_SCREEN_PARAM_20,
                                    defaultEcProjectState.screenParam.sk_sf_skmax).toDouble();
        ec_project_state_.screenParam.sk_hf_kumin
                = project_ini.value(EcIni::INI_SCREEN_PARAM_21,
                                    defaultEcProjectState.screenParam.sk_hf_kumin).toDouble();
        ec_project_state_.screenParam.sk_hf_kumax
                = project_ini.value(EcIni::INI_SCREEN_PARAM_22,
                                    defaultEcProjectState.screenParam.sk_hf_kumax).toDouble();
        ec_project_state_.screenParam.sk_sf_kumin
                = project_ini.value(EcIni::INI_SCREEN_PARAM_23,
                                    defaultEcProjectState.screenParam.sk_sf_kumin).toDouble();
        ec_project_state_.screenParam.sk_sf_kumax
                = project_ini.value(EcIni::INI_SCREEN_PARAM_24,
                                    defaultEcProjectState.screenParam.sk_sf_kumax).toDouble();
        ec_project_state_.screenParam.ds_hf_uv
                = project_ini.value(EcIni::INI_SCREEN_PARAM_25,
                                    defaultEcProjectState.screenParam.ds_hf_uv).toDouble();
        ec_project_state_.screenParam.ds_hf_w
                = project_ini.value(EcIni::INI_SCREEN_PARAM_26,
                                    defaultEcProjectState.screenParam.ds_hf_w).toDouble();
        ec_project_state_.screenParam.ds_hf_t
                = project_ini.value(EcIni::INI_SCREEN_PARAM_27,
                                    defaultEcProjectState.screenParam.ds_hf_t).toDouble();
        ec_project_state_.screenParam.ds_hf_co2
                = project_ini.value(EcIni::INI_SCREEN_PARAM_28,
                                    defaultEcProjectState.screenParam.ds_hf_co2).toDouble();
        ec_project_state_.screenParam.ds_hf_h2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_29,
                                    defaultEcProjectState.screenParam.ds_hf_h2o).toDouble();
        ec_project_state_.screenParam.ds_hf_ch4
                = project_ini.value(EcIni::INI_SCREEN_PARAM_50,
                                    defaultEcProjectState.screenParam.ds_hf_ch4).toDouble();
        ec_project_state_.screenParam.ds_hf_n2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_51,
                                    defaultEcProjectState.screenParam.ds_hf_n2o).toDouble();
        ec_project_state_.screenParam.ds_hf_var
                = project_ini.value(EcIni::INI_SCREEN_PARAM_30,
                                    defaultEcProjectState.screenParam.ds_hf_var).toDouble();
        ec_project_state_.screenParam.ds_sf_uv
                = project_ini.value(EcIni::INI_SCREEN_PARAM_31,
                                    defaultEcProjectState.screenParam.ds_sf_uv).toDouble();
        ec_project_state_.screenParam.ds_sf_w
                = project_ini.value(EcIni::INI_SCREEN_PARAM_32,
                                    defaultEcProjectState.screenParam.ds_sf_w).toDouble();
        ec_project_state_.screenParam.ds_sf_t
                = project_ini.value(EcIni::INI_SCREEN_PARAM_33,
                                    defaultEcProjectState.screenParam.ds_sf_t).toDouble();
        ec_project_state_.screenParam.ds_sf_co2
                = project_ini.value(EcIni::INI_SCREEN_PARAM_34,
                                    defaultEcProjectState.screenParam.ds_sf_co2).toDouble();
        ec_project_state_.screenParam.ds_sf_h2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_35,
                                    defaultEcProjectState.screenParam.ds_sf_h2o).toDouble();
        ec_project_state_.screenParam.ds_sf_ch4
                = project_ini.value(EcIni::INI_SCREEN_PARAM_52,
                                    defaultEcProjectState.screenParam.ds_sf_ch4).toDouble();
        ec_project_state_.screenParam.ds_sf_n2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_53,
                                    defaultEcProjectState.screenParam.ds_sf_n2o).toDouble();
        ec_project_state_.screenParam.ds_sf_var
                = project_ini.value(EcIni::INI_SCREEN_PARAM_36,
                                    defaultEcProjectState.screenParam.ds_sf_var).toDouble();
        ec_project_state_.screenParam.despike_vm
                = project_ini.value(EcIni::INI_SCREEN_PARAM_60,
                                    defaultEcProjectState.screenParam.despike_vm).toInt();
        ec_project_state_.screenParam.tl_hf_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_37,
                                    defaultEcProjectState.screenParam.tl_hf_lim).toDouble();
        ec_project_state_.screenParam.tl_sf_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_38,
                                    defaultEcProjectState.screenParam.tl_sf_lim).toDouble();
        ec_project_state_.screenParam.tl_def_co2
                = project_ini.value(EcIni::INI_SCREEN_PARAM_39,
                                    defaultEcProjectState.screenParam.tl_def_co2).toDouble();
        ec_project_state_.screenParam.tl_def_h2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_40,
                                    defaultEcProjectState.screenParam.tl_def_h2o).toDouble();
        ec_project_state_.screenParam.tl_def_ch4
                = project_ini.value(EcIni::INI_SCREEN_PARAM_58,
                                    defaultEcProjectState.screenParam.tl_def_ch4).toDouble();
        ec_project_state_.screenParam.tl_def_n2o
                = project_ini.value(EcIni::INI_SCREEN_PARAM_59,
                                    defaultEcProjectState.screenParam.tl_def_n2o).toDouble();
        ec_project_state_.screenParam.aa_min
                = project_ini.value(EcIni::INI_SCREEN_PARAM_41,
                                    defaultEcProjectState.screenParam.aa_min).toDouble();
        ec_project_state_.screenParam.aa_max
                = project_ini.value(EcIni::INI_SCREEN_PARAM_42,
                                    defaultEcProjectState.screenParam.aa_max).toDouble();
        ec_project_state_.screenParam.aa_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_43,
                                    defaultEcProjectState.screenParam.aa_lim).toDouble();
        ec_project_state_.screenParam.ns_hf_lim
                = project_ini.value(EcIni::INI_SCREEN_PARAM_44,
                                    defaultEcProjectState.screenParam.ns_hf_lim).toDouble();
    project_ini.endGroup();

    // planar fit section
    project_ini.beginGroup(EcIni::INIGROUP_SCREEN_TILT);
        ec_project_state_.screenTilt.start_date
                = project_ini.value(EcIni::INI_SCREEN_TILT_0,
                                    QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.screenTilt.end_date
                = project_ini.value(EcIni::INI_SCREEN_TILT_1,
                                    QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.screenTilt.start_time
                = project_ini.value(EcIni::INI_SCREEN_TILT_12,
                                    QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.screenTilt.end_time
                = project_ini.value(EcIni::INI_SCREEN_TILT_13,
                                    QTime(23, 59).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.screenTilt.mode
                = project_ini.value(EcIni::INI_SCREEN_TILT_2,
                                    defaultEcProjectState.screenTilt.mode).toInt();
        ec_project_state_.screenTilt.north_offset
                = project_ini.value(EcIni::INI_SCREEN_TILT_3,
                                    defaultEcProjectState.screenTilt.north_offset).toDouble();
        ec_project_state_.screenTilt.min_num_per_sec
                = project_ini.value(EcIni::INI_SCREEN_TILT_4,
                                    defaultEcProjectState.screenTilt.min_num_per_sec).toInt();
        ec_project_state_.screenTilt.w_max
                = project_ini.value(EcIni::INI_SCREEN_TILT_5,
                                    defaultEcProjectState.screenTilt.w_max).toDouble();
        ec_project_state_.screenTilt.u_min
                = project_ini.value(EcIni::INI_SCREEN_TILT_6,
                                    defaultEcProjectState.screenTilt.u_min).toDouble();
        ec_project_state_.screenTilt.file
                = project_ini.value(EcIni::INI_SCREEN_TILT_7,
                                    defaultEcProjectState.screenTilt.file).toString();
        ec_project_state_.screenTilt.fix_policy
                = project_ini.value(EcIni::INI_SCREEN_TILT_8,
                                    defaultEcProjectState.screenTilt.fix_policy).toInt();
        ec_project_state_.screenTilt.subset
                = project_ini.value(EcIni::INI_SCREEN_TILT_11,
                                    defaultEcProjectState.screenTilt.subset).toInt();

        ec_project_state_.screenTilt.angles.clear();
        int numAngles = countPlanarFitAngles(project_ini.allKeys());
        // iterate through angle list
        for (int k = 0; k < numAngles; ++k)
        {
            QString prefix = EcIni::INI_SCREEN_TILT_PREFIX + QString::number(k + 1) + QStringLiteral("_");

            int exclude = project_ini.value(prefix + EcIni::INI_SCREEN_TILT_10).toInt();
            int include = exclude ? 0 : (exclude + 2);
            auto included = static_cast<Qt::CheckState>(include);

            AngleItem item;
            item.angle_ = project_ini.value(prefix + EcIni::INI_SCREEN_TILT_9).toDouble();
            item.included_ = included;
            item.color_ = WidgetUtils::getColor(k);
            addPlanarFitAngle(item);
        }

    project_ini.endGroup();

    // time lag opt section
    project_ini.beginGroup(EcIni::INIGROUP_TIMELAG_OPT);
        ec_project_state_.timelagOpt.start_date
                = project_ini.value(EcIni::INI_TIMELAG_OPT_0,
                                    QDate(2000, 1, 1).toString(Qt::ISODate)).toString();
        ec_project_state_.timelagOpt.end_date
                = project_ini.value(EcIni::INI_TIMELAG_OPT_1,
                                    QDate::currentDate().toString(Qt::ISODate)).toString();
        ec_project_state_.timelagOpt.start_time
                = project_ini.value(EcIni::INI_TIMELAG_OPT_19,
                                    QTime(0, 0).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.timelagOpt.end_time
                = project_ini.value(EcIni::INI_TIMELAG_OPT_20,
                                    QTime(23, 59).toString(QStringLiteral("hh:mm"))).toString();
        ec_project_state_.timelagOpt.mode
                = project_ini.value(EcIni::INI_TIMELAG_OPT_2,
                                    defaultEcProjectState.timelagOpt.mode).toInt();
        ec_project_state_.timelagOpt.file
                = project_ini.value(EcIni::INI_TIMELAG_OPT_3,
                                    defaultEcProjectState.timelagOpt.file).toString();
        ec_project_state_.timelagOpt.to_h2o_nclass
                = project_ini.value(EcIni::INI_TIMELAG_OPT_17,
                                    defaultEcProjectState.timelagOpt.to_h2o_nclass).toInt();
        ec_project_state_.timelagOpt.co2_min_flux
                = project_ini.value(EcIni::INI_TIMELAG_OPT_4,
                                    defaultEcProjectState.timelagOpt.co2_min_flux).toDouble();
        ec_project_state_.timelagOpt.ch4_min_flux
                = project_ini.value(EcIni::INI_TIMELAG_OPT_5,
                                    defaultEcProjectState.timelagOpt.ch4_min_flux).toDouble();
        ec_project_state_.timelagOpt.gas4_min_flux
                = project_ini.value(EcIni::INI_TIMELAG_OPT_6,
                                    defaultEcProjectState.timelagOpt.gas4_min_flux).toDouble();
        ec_project_state_.timelagOpt.le_min_flux
                = project_ini.value(EcIni::INI_TIMELAG_OPT_7,
                                    defaultEcProjectState.timelagOpt.le_min_flux).toDouble();
        ec_project_state_.timelagOpt.pg_range
                = project_ini.value(EcIni::INI_TIMELAG_OPT_8,
                                    defaultEcProjectState.timelagOpt.pg_range).toDouble();
        ec_project_state_.timelagOpt.co2_min_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_9,
                                    defaultEcProjectState.timelagOpt.co2_min_lag).toDouble();
        ec_project_state_.timelagOpt.co2_max_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_10,
                                    defaultEcProjectState.timelagOpt.co2_max_lag).toDouble();
        ec_project_state_.timelagOpt.h2o_min_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_11,
                                    defaultEcProjectState.timelagOpt.h2o_min_lag).toDouble();
        ec_project_state_.timelagOpt.h2o_max_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_12,
                                    defaultEcProjectState.timelagOpt.h2o_max_lag).toDouble();
        ec_project_state_.timelagOpt.ch4_min_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_13,
                                    defaultEcProjectState.timelagOpt.ch4_min_lag).toDouble();
        ec_project_state_.timelagOpt.ch4_max_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_14,
                                    defaultEcProjectState.timelagOpt.ch4_max_lag).toDouble();
        ec_project_state_.timelagOpt.gas4_min_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_15,
                                    defaultEcProjectState.timelagOpt.gas4_min_lag).toDouble();
        ec_project_state_.timelagOpt.gas4_max_lag
                = project_ini.value(EcIni::INI_TIMELAG_OPT_16,
                                    defaultEcProjectState.timelagOpt.gas4_max_lag).toDouble();
        ec_project_state_.timelagOpt.subset
                = project_ini.value(EcIni::INI_TIMELAG_OPT_18,
                                    defaultEcProjectState.timelagOpt.subset).toInt();
    project_ini.endGroup();

    // random error section
    project_ini.beginGroup(EcIni::INIGROUP_RAND_ERROR);
        ec_project_state_.randomError.method
                = project_ini.value(EcIni::INI_RAND_ERROR_0,
                                    defaultEcProjectState.randomError.method).toInt();
        ec_project_state_.randomError.its_method
                = project_ini.value(EcIni::INI_RAND_ERROR_1,
                                    defaultEcProjectState.randomError.its_method).toInt();
        ec_project_state_.randomError.its_tlag_max
                = project_ini.value(EcIni::INI_RAND_ERROR_2,
                                    defaultEcProjectState.randomError.its_tlag_max).toDouble();
    project_ini.endGroup();

    // biomet section
    project_ini.beginGroup(EcIni::INIGROUP_BIOMET);
        ec_project_state_.biomParam.col_ta
                = project_ini.value(EcIni::INI_BIOMET_4,
                                    defaultEcProjectState.biomParam.col_ta).toInt() + 1000;
        ec_project_state_.biomParam.col_pa
                = project_ini.value(EcIni::INI_BIOMET_5,
                                    defaultEcProjectState.biomParam.col_pa).toInt() + 1000;
        ec_project_state_.biomParam.col_rh
                = project_ini.value(EcIni::INI_BIOMET_6,
                                    defaultEcProjectState.biomParam.col_rh).toInt();
        ec_project_state_.biomParam.col_rg
                = project_ini.value(EcIni::INI_BIOMET_7,
                                    defaultEcProjectState.biomParam.col_rg).toInt();
        ec_project_state_.biomParam.col_lwin
                = project_ini.value(EcIni::INI_BIOMET_8,
                                    defaultEcProjectState.biomParam.col_lwin).toInt();
        ec_project_state_.biomParam.col_ppfd
                = project_ini.value(EcIni::INI_BIOMET_9,
                                    defaultEcProjectState.biomParam.col_ppfd).toInt();
    project_ini.endGroup();

    datafile.close();

    // just loaded projects are not modified
    setModified(false);
    emit ecProjectChanged();

    if (!isVersionCompatible)
        *modified = true;

    return true;
}

bool EcProject::nativeFormat(const QString &filename)
{
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open file: doesn't exists (check the path) "
                   << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename, datafile.errorString()));
        return false;
    }

    // test if the first row of the file start with the correct tag
    // case sensitive by default
    QTextStream in(&datafile);
    QString firstLine;
    in >> firstLine;
    datafile.close();

    // filter out metadata files
    if (firstLine.startsWith(Defs::APP_MD_INI_TAG)
        || firstLine.startsWith(QLatin1String(";ECO2S_METADATA"))
        || firstLine.startsWith(QLatin1String(";ECO2S_DATALOGGING"))
        || firstLine.startsWith(QLatin1String(";ECO2catch"))
        || firstLine.startsWith(QLatin1String(";ECCOCatch")))
    {
        WidgetUtils::warning(nullptr,
                             tr("Load Project Error"),
                             tr("Cannot read file <p>%1:</p>\n"
                                "<b>not in %2 native format.</b>").arg(filename, Defs::APP_NAME));
        return false;
    }

    // filter out other generic files
    if (!firstLine.startsWith(Defs::APP_PD_INI_TAG)
        && !firstLine.startsWith(QLatin1String(";ECO2S_PROCESSING"))
        && !firstLine.startsWith(QLatin1String(";ECO2S_DATAPROCESSING")))
    {
        WidgetUtils::warning(nullptr,
                             tr("Load Error"),
                             tr("Cannot read file <p>%1:</p>\n"
                                "<b>not in %2 native format.</b>")
                             .arg(filename, Defs::APP_NAME));
        return false;
    }

    return true;
}

// prepend a known tag to the project file
bool EcProject::tagProject(const QString &filename)
{
    QString app_tag(Defs::APP_PD_INI_TAG);
    app_tag += QLatin1String("\n");

    return FileUtils::prependToFile(app_tag, filename);
}

void EcProject::setModified(bool mod)
{
    modified_ = mod;
    if (mod)
    {
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

void EcProject::setSpectraMinUnstableUstar(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_ustar = d;
    setModified(true);
}

void EcProject::setSpectraMinUnstableH(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_h = d;
    setModified(true);
}

void EcProject::setSpectraMinUnstableLE(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_le = d;
    setModified(true);
}

void EcProject::setSpectraMinUnstableCo2(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_co2 = d;
    setModified(true);
}

void EcProject::setSpectraMinUnstableCh4(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraMinUnstableGas4(double d)
{
    ec_project_state_.spectraSettings.sa_min_un_gas4 = d;
    setModified(true);
}

void EcProject::setSpectraMinStableUstar(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_ustar = d;
    setModified(true);
}

void EcProject::setSpectraMinStableH(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_h = d;
    setModified(true);
}

void EcProject::setSpectraMinStableLE(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_le = d;
    setModified(true);
}

void EcProject::setSpectraMinStableCo2(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_co2 = d;
    setModified(true);
}

void EcProject::setSpectraMinStableCh4(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraMinStableGas4(double d)
{
    ec_project_state_.spectraSettings.sa_min_st_gas4 = d;
    setModified(true);
}

void EcProject::setSpectraMaxUstar(double d)
{
    ec_project_state_.spectraSettings.sa_max_ustar = d;
    setModified(true);
}

void EcProject::setSpectraMaxH(double d)
{
    ec_project_state_.spectraSettings.sa_max_h = d;
    setModified(true);
}

void EcProject::setSpectraMaxLE(double d)
{
    ec_project_state_.spectraSettings.sa_max_le = d;
    setModified(true);
}

void EcProject::setSpectraMaxCo2(double d)
{
    ec_project_state_.spectraSettings.sa_max_co2 = d;
    setModified(true);
}

void EcProject::setSpectraMaxCh4(double d)
{
    ec_project_state_.spectraSettings.sa_max_ch4 = d;
    setModified(true);
}

void EcProject::setSpectraMaxGas4(double d)
{
    ec_project_state_.spectraSettings.sa_max_gas4 = d;
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

void EcProject::setGeneralOutMeanSpectra(int n)
{
    ec_project_state_.projectGeneral.out_mean_spectra = n;
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

void EcProject::setGeneralHfCorrectGhgBa(int n)
{
    ec_project_state_.projectGeneral.hf_correct_ghg_ba = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralHfCorrectGhgZoh(int n)
{
    ec_project_state_.projectGeneral.hf_correct_ghg_zoh = n;
    setModified(true);
    emit updateInfo();
}

void EcProject::setGeneralSonicOutputRate(int n)
{
    ec_project_state_.projectGeneral.sonic_output_rate = n;
    setModified(true);
    emit updateInfo();
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

void EcProject::setScreenWBoost(int n)
{
    ec_project_state_.screenSetting.gill_wm_wboost = n;
    setModified(true);
}

void EcProject::setScreenFlowDistortion(int n)
{
    ec_project_state_.screenSetting.flow_distortion = n;
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

void EcProject::setScreenParamDespikeVm(int n)
{
    ec_project_state_.screenParam.despike_vm = n;
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

void EcProject::setGeneralColDiagAnem(int n)
{
    ec_project_state_.projectGeneral.col_diag_anem = n;
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

void EcProject::setPlanarFitStartTime(const QString& time)
{
    ec_project_state_.screenTilt.start_time = time;
    setModified(true);
}

void EcProject::setPlanarFitEndTime(const QString& time)
{
    ec_project_state_.screenTilt.end_time = time;
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

void EcProject::setSpectraStartTime(const QString& time)
{
    ec_project_state_.spectraSettings.start_sa_time = time;
    setModified(true);
}

void EcProject::setSpectraEndTime(const QString& time)
{
    ec_project_state_.spectraSettings.end_sa_time = time;
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

void EcProject::setSpectraUseVmFlags(int n)
{
    ec_project_state_.spectraSettings.use_vm_flags = n;
    setModified(true);
}

void EcProject::setSpectraUseFokenLow(int n)
{
    ec_project_state_.spectraSettings.use_foken_low = n;
    setModified(true);
}


void EcProject::setSpectraUseFokenMid(int n)
{
    ec_project_state_.spectraSettings.use_foken_mid = n;
    setModified(true);
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

void EcProject::setTimelagOptStartTime(const QString& time)
{
    ec_project_state_.timelagOpt.start_time = time;
    setModified(true);
}

void EcProject::setTimelagOptEndTime(const QString& time)
{
    ec_project_state_.timelagOpt.end_time = time;
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
    ec_project_state_.projectGeneral.ru_method = n;
    setModified(true);
}

void EcProject::setRandomErrorItsMethod(int n)
{
    ec_project_state_.projectGeneral.its_method = n;
    setModified(true);
}

void EcProject::setRandomErrorItsTlagMax(double d)
{
    ec_project_state_.projectGeneral.its_tlag_max = d;
    setModified(true);
}

void EcProject::setRandomErrorItsSecFactor(double d)
{
    ec_project_state_.projectGeneral.its_sec_factor = d;
    setModified(true);
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
    for (const auto &s : list)
    {
        if (s.contains(QStringLiteral("width")))
        {
            ++i;
        }
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

bool EcProject::isGoodRawFilePrototype(const QString& s)
{
    bool test = !s.isEmpty()
                && s.contains(QStringLiteral("yy"))
                && s.contains(QStringLiteral("dd"))
                && s.contains(QStringLiteral("HH"))
                && s.contains(QStringLiteral("MM"))
                && s.contains(QStringLiteral("."));
    return test;
}
