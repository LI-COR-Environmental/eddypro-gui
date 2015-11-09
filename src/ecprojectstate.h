/***************************************************************************
  ecprojectstate.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#ifndef ECPROJECTSTATE_H
#define ECPROJECTSTATE_H

#include "angle_item.h"
#include "calibration.h"
#include "defs.h"

/// \struct GeneralProjectState
/// \brief Structure representing general information for the INI
/// processing file
struct ProjectGeneralState
{
    QString sw_version = Defs::APP_VERSION_STR;
    QString ini_version = Defs::PROJECT_FILE_VERSION_STR;
    QString creation_date = QString();
    QString last_change_date = QString();
    Defs::CurrRunMode run_mode = Defs::CurrRunMode::Advanced;
    bool run_fcc = false;
    QString file_name = QString();
    QString project_title = QString();
//    QString project_id = QString();
    QString project_id = QStringLiteral("gerardo");
    Defs::RawFileType file_type = Defs::RawFileType::GHG;
    QString file_prototype = QString();
    bool use_alt_md_file = false;
    QString md_file = QString();
    bool use_tlfile = false;
    QString timeline_file = QString();
    int binary_hnlines = -1;
    int binary_eol = -1;
    int binary_nbytes = -1;
    int binary_little_end = -1;
    QString master_sonic = QString();
    int col_co2 = -1;
    int col_h2o = -1;
    int col_ch4 = -1;
    int col_n2o = -1;
    int col_int_t_c = -1;
    int col_int_t_1 = -1;
    int col_int_t_2 = -1;
    int col_int_p = -1;
    int col_air_t = -1;
    int col_air_p = -1;
    int col_diag_75 = -1;
    int col_diag_72 = -1;
    int col_diag_77 = -1;
    int col_ts = -1;
    qreal gas_mw = -1.0;
    qreal gas_diff = -1.0;
    int out_ghg_eu = 0;
    int out_amflux = 0;
    int out_rich = 1;
    bool make_dataset = false;
    int subset = 0;
    QString start_date = QString();
    QString end_date = QString();
    QString start_time = QString();
    QString end_time = QString();
    int hf_meth = 1;
    int lf_meth = 1;
    int wpl_meth = 1;
    int foot_meth = 1;
    int tob1_format = 0;
    QString out_path = QString();
    int fix_out_format = 0;
    QString err_label = QStringLiteral("-9999.0");
    int qcflag_meth = 1;
    int out_md = 1;
    int out_biomet = 0;
    int use_biomet = 0;
    QString biom_file = QString();
    QString biom_dir = QString();
    int biom_recurse = 0;
    QString biom_ext = QStringLiteral("txt");
    int out_mean_spectra = 0;
    int out_mean_cosp = 0;
    int bin_sp_avail = 0;
    int full_sp_avail = 0;
    int files_found = 0;
    int hf_correct_ghg_ba = 1;
    int hf_correct_ghg_zoh = 1;
    int sonic_output_rate = -1;
};

/// \struct SpectraSettingsState
/// \brief Structure representing spectral settings information for the INI
/// processing file
struct SpectraSettingsState
{
    int sa_mode = 1;
    int horst_lens = 2;
    QString start_sa_date = QString();
    QString end_sa_date = QString();
    QString start_sa_time = QString();
    QString end_sa_time = QString();
    QString sa_file = QString();
    int sa_min_smpl = 10;
    qreal sa_fmin_co2 = 0.005;
    qreal sa_fmin_h2o = 0.005;
    qreal sa_fmin_ch4 = 0.005;
    qreal sa_fmin_gas4 = 0.005;
    qreal sa_fmax_co2 = 2.0;
    qreal sa_fmax_h2o = 2.0;
    qreal sa_fmax_ch4 = 2.0;
    qreal sa_fmax_gas4 = 2.0;
    qreal sa_hfn_co2_fmin = 1.0;
    qreal sa_hfn_h2o_fmin = 1.0;
    qreal sa_hfn_ch4_fmin = 1.0;
    qreal sa_hfn_gas4_fmin = 1.0;
    qreal sa_min_un_ustar = 0.2;
    qreal sa_min_un_h = 20.0;
    qreal sa_min_un_le = 20.0;
    qreal sa_min_un_co2 = 2.0;
    qreal sa_min_un_ch4 = 0.01;
    qreal sa_min_un_gas4 = 0.01;
    qreal sa_min_st_ustar = 0.05;
    qreal sa_min_st_h = 5.0;
    qreal sa_min_st_le = 3.0;
    qreal sa_min_st_co2 = 0.5;
    qreal sa_min_st_ch4 = 0.005;
    qreal sa_min_st_gas4 = 0.005;
    qreal sa_max_ustar = 5.0;
    qreal sa_max_h = 1000.0;
    qreal sa_max_le = 1000.0;
    qreal sa_max_co2 = 100.0;
    qreal sa_max_ch4 = 20.0;
    qreal sa_max_gas4 = 20.0;
    int add_sonic_lptf = 1;
    QString ex_file = QString();
    QString sa_bin_spectra = QString();
    QString sa_full_spectra = QString();
    QString ex_dir = QString();
    int subset = 0;
    int use_vm_flags = 1;
    int use_foken_low = 1;
    int use_foken_mid = 0;
};

/// \struct ScreenGeneralState
/// \brief Structure representing raw processing general information for the INI
/// processing file
struct ScreenGeneralState
{
    QString data_path = QString();
    QString spectra_path = QString();
    QString stat_file = QString();
    int nfile = 1;
    QString start_run = QString();
    QString end_run = QString();
    bool timestamp_end = false;
    QString flux_file = QString();
    QString in1_file = QString();
    QString in2_file = QString();
    int recurse = 1;
    bool use_geo_north = false;
    qreal mag_dec = 0.0;
    QString dec_date = QString();
    int flag1_col = -1;
    qreal flag1_threshold = -9999.0;
    int flag1_policy = 1;
    int flag2_col = -1;
    qreal flag2_threshold = -9999.0;
    int flag2_policy = 1;
    int flag3_col = -1;
    qreal flag3_threshold = -9999.0;
    int flag3_policy = 1;
    int flag4_col = -1;
    qreal flag4_threshold = -9999.0;
    int flag4_policy = 1;
    int flag5_col = -1;
    qreal flag5_threshold = -9999.0;
    int flag5_policy = 1;
    int flag6_col = -1;
    qreal flag6_threshold = -9999.0;
    int flag6_policy = 1;
    int flag7_col = -1;
    qreal flag7_threshold = -9999.0;
    int flag7_policy = 1;
    int flag8_col = -1;
    qreal flag8_threshold = -9999.0;
    int flag8_policy = 1;
    int flag9_col = -1;
    qreal flag9_threshold = -9999.0;
    int flag9_policy = 1;
    int flag10_col = -1;
    qreal flag10_threshold = -9999.0;
    int flag10_policy = 1;
};

/// \struct ScreenSettingState
/// \brief Structure representing raw processing settings information for the INI
/// processing file
struct ScreenSettingState
{
    int max_lack = 10;
    int cross_wind = 0;
    int flow_distortion = 3;
    int rot_meth = 1;
    int detrend_meth = 0;
    qreal timeconst = 250.0;
    int tlag_meth = 2;
    int tap_win = 3;
    int nbins = 50;
    int avrg_len = 30;
    qreal u_offset = 0.0;
    qreal v_offset = 0.0;
    qreal w_offset = 0.0;
    int out_bin_sp = 1;
    int out_bin_og = 0;
    int out_full_sp_u = 0;
    int out_full_sp_v = 0;
    int out_full_sp_w = 0;
    int out_full_sp_ts = 0;
    int out_full_sp_co2 = 0;
    int out_full_sp_h2o = 0;
    int out_full_sp_ch4 = 0;
    int out_full_sp_n2o = 0;
    int out_full_cosp_u = 0;
    int out_full_cosp_v = 0;
    int out_full_cosp_ts = 1;
    int out_full_cosp_co2 = 0;
    int out_full_cosp_h2o = 0;
    int out_full_cosp_ch4 = 0;
    int out_full_cosp_n2o = 0;
    int out_st_1 = 1;
    int out_st_2 = 0;
    int out_st_3 = 0;
    int out_st_4 = 0;
    int out_st_5 = 0;
    int out_st_6 = 0;
    int out_st_7 = 0;
    int out_raw_1 = 0;
    int out_raw_2 = 0;
    int out_raw_3 = 0;
    int out_raw_4 = 0;
    int out_raw_5 = 0;
    int out_raw_6 = 0;
    int out_raw_7 = 0;
    int out_raw_u = 0;
    int out_raw_v = 0;
    int out_raw_w = 0;
    int out_raw_ts = 0;
    int out_raw_co2 = 0;
    int out_raw_h2o = 0;
    int out_raw_ch4 = 0;
    int out_raw_gas4 = 0;
    int out_raw_tair = 0;
    int out_raw_pair = 0;
    int filter_sr = 1;
    int filter_al = 1;
    int bu_corr = 0;
    int bu_multi = 0;
    qreal l_day_bot_gain = 0.944;
    qreal l_day_bot_offset = 2.57;
    qreal l_day_top_gain = 1.005;
    qreal l_day_top_offset = 0.24;
    qreal l_day_spar_gain = 1.010;
    qreal l_day_spar_offset = 0.36;
    qreal l_night_bot_gain = 0.883;
    qreal l_night_bot_offset = 2.17;
    qreal l_night_top_gain = 1.008;
    qreal l_night_top_offset = -0.41;
    qreal l_night_spar_gain = 1.010;
    qreal l_night_spar_offset = -0.17;
    qreal m_day_bot1 = 2.8;
    qreal m_day_bot2 = -0.0681;
    qreal m_day_bot3 = 0.0021;
    qreal m_day_bot4 = -0.334;
    qreal m_day_top1 = -0.1;
    qreal m_day_top2 = -0.0044;
    qreal m_day_top3 = 0.0011;
    qreal m_day_top4 = -0.022;
    qreal m_day_spar1 = 0.3;
    qreal m_day_spar2 = -0.0007;
    qreal m_day_spar3 = 0.0006;
    qreal m_day_spar4 = -0.044;
    qreal m_night_bot1 = 0.5;
    qreal m_night_bot2 = -0.1160;
    qreal m_night_bot3 = 0.0087;
    qreal m_night_bot4 = -0.206;
    qreal m_night_top1 = -1.7;
    qreal m_night_top2 = -0.0160;
    qreal m_night_top3 = 0.0051;
    qreal m_night_top4 = -0.029;
    qreal m_night_spar1 = -2.1;
    qreal m_night_spar2 = -0.0200;
    qreal m_night_spar3 = 0.0070;
    qreal m_night_spar4 = 0.026;
    int out_details = 0;
    int power_of_two = 1;
};

/// \struct ScreenTestState
/// \brief Structure representing tests information for the INI
/// processing file
struct ScreenTestState
{
    int test_sr = 1;
    int test_ar = 1;
    int test_do = 1;
    int test_al = 1;
    int test_sk = 1;
    int test_ds = 0;
    int test_tl = 0;
    int test_aa = 0;
    int test_ns = 0;
};

/// \struct ScreenParamState
/// \brief Structure representing tests parameters information for the INI
/// processing file
struct ScreenParamState
{
    int sr_num_spk = 3;
    qreal sr_lim_u = 3.5;
    qreal sr_lim_w = 5.0;
    qreal sr_lim_co2 = 3.5;
    qreal sr_lim_h2o = 3.5;
    qreal sr_lim_ch4 = 8.0;
    qreal sr_lim_n2o = 8.0;
    qreal sr_lim_hf = 1.0;
    qreal ar_lim = 7.0;
    int ar_bins = 100;
    int ar_hf_lim = 70;
    int do_extlim_dw = 10;
    qreal do_hf1_lim = 10.0;
    qreal do_hf2_lim = 6.0;
    qreal al_u_max = 30.0;
    qreal al_w_max = 5.0;
    qreal al_tson_min = -40.0;
    qreal al_tson_max = 50.0;
    qreal al_co2_min = 200.0;
    qreal al_co2_max = 900.0;
    qreal al_h2o_min = 0.0;
    qreal al_h2o_max = 40.0;
    qreal al_ch4_min = 0.17;
    qreal al_ch4_max = 1000.0;
    qreal al_n2o_min = 0.032;
    qreal al_n2o_max = 1000.0;
    qreal sk_hf_skmin = -2.0;
    qreal sk_hf_skmax = 2.0;
    qreal sk_sf_skmin = -1.0;
    qreal sk_sf_skmax = 1.0;
    qreal sk_hf_kumin = 1.0;
    qreal sk_hf_kumax = 8.0;
    qreal sk_sf_kumin = 2.0;
    qreal sk_sf_kumax = 5.0;
    qreal ds_hf_uv = 4.0;
    qreal ds_hf_w = 2.0;
    qreal ds_hf_t = 4.0;
    qreal ds_hf_co2 = 40.0;
    qreal ds_hf_h2o = 3.26;
    qreal ds_hf_ch4 = 40.0;
    qreal ds_hf_n2o = 40.0;
    qreal ds_hf_var = 3.0;
    qreal ds_sf_uv = 2.7;
    qreal ds_sf_w = 1.3;
    qreal ds_sf_t = 2.7;
    qreal ds_sf_co2 = 27.0;
    qreal ds_sf_h2o = 2.2;
    qreal ds_sf_ch4 = 30.0;
    qreal ds_sf_n2o = 30.0;
    qreal ds_sf_var = 2.0;
    qreal tl_hf_lim = 20.0;
    qreal tl_sf_lim = 10.0;
    qreal tl_def_co2 = 0.0;
    qreal tl_def_h2o = 0.0;
    qreal tl_def_ch4 = 0.0;
    qreal tl_def_n2o = 0.0;
    qreal aa_min = -30.0;
    qreal aa_max = 30.0;
    qreal aa_lim = 10.0;
    qreal ns_hf_lim = 0.5;
    int despike_vm = 0;
};

/// \struct ScreenTiltState
/// \brief Structure representing ...
struct ScreenTiltState
{
    QString start_date = QString();
    QString end_date = QString();
    QString start_time = QString();
    QString end_time = QString();
    int mode = 1;
    qreal north_offset = 0.0;
    int min_num_per_sec = 0;
    qreal w_max = 0.099;
    qreal u_min = -0.001;
    QString file = QString();
    int fix_policy = 0;
    QList<AngleItem> angles = QList<AngleItem>();
    int subset = 0;
};

struct TimelagOptState
{
    QString start_date = QString();
    QString end_date = QString();
    QString start_time = QString();
    QString end_time = QString();
    int mode = 1;
    QString file = QString();
    int to_h2o_nclass = 10;
    qreal co2_min_flux = 2.0;
    qreal ch4_min_flux = 0.2;
    qreal gas4_min_flux = 0.02;
    qreal le_min_flux = 20.0;
    qreal pg_range = 1.5;
    qreal co2_min_lag = -1000.1;
    qreal co2_max_lag = -1000.1;
    qreal h2o_min_lag = -1000.1;
    qreal h2o_max_lag = -1000.1;
    qreal ch4_min_lag = -1000.1;
    qreal ch4_max_lag = -1000.1;
    qreal gas4_min_lag = -1000.1;
    qreal gas4_max_lag = -1000.1;
    int subset = 0;
};

struct RandomErrorState
{
    int method = 0;
    int its_method = 1;
    qreal its_tlag_max = 10.0;
    qreal its_sec_factor = 20.0;
};

struct BiometState
{
    int native_header = 1;
    int hlines = 2;
    QString separator = QStringLiteral("comma");
    QString tstamp_ref = QStringLiteral("end");
    int col_ta = 999;
    int col_pa = 999;
    int col_rh = -1;
    int col_rg = -1;
    int col_lwin = -1;
    int col_ppfd = -1;
};

/// \struct EcProjectState
/// \brief Container structure representing information for the INI
/// processing file
struct EcProjectState
{
    ProjectGeneralState projectGeneral;
    SpectraSettingsState spectraSettings;
    ScreenGeneralState screenGeneral;
    ScreenSettingState screenSetting;
    ScreenTestState screenTest;
    ScreenParamState screenParam;
    ScreenTiltState screenTilt;
    TimelagOptState timelagOpt;
    RandomErrorState randomError;
    BiometState biomParam;
    Calibration driftCorr;
};

#endif // ECPROJECTSTATE_H
