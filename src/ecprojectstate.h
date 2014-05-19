/***************************************************************************
  ecprojectstate.h
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
 ***************************************************************************/

#ifndef ECPROJECTSTATE_H
#define ECPROJECTSTATE_H

#include "angleitem.h"
//#include "specgroup.h"
#include "defs.h"

// Structure representing general information for the INI
// processing file
struct ProjectGeneralState
{
    ProjectGeneralState() :
        sw_version(Defs::APP_VERSION_STR),
        ini_version(Defs::PROJECT_VERSION_STR),
        creation_date(QString()),
        last_change_date(QString()),
        run_mode(Defs::CurrRunModeAdv),
        run_fcc(0),
        file_name(QString()),
        project_title(QString()),
        project_id(QString()),
        file_type(0),
        file_prototype(QString()),
        use_alt_md_file(false),
        md_file(QString()),
        use_tlfile(false),
        timeline_file(QString()),
        binary_hnlines(-1),
        binary_eol(-1),
        binary_nbytes(-1),
        binary_little_end(-1),
        master_sonic(QString()),
        col_co2(-1),
        col_h2o(-1),
        col_ch4(-1),
        col_n2o(-1),
        col_int_t_c(-1),
        col_int_t_1(-1),
        col_int_t_2(-1),
        col_int_p(-1),
        col_air_t(-1),
        col_air_p(-1),
        col_diag_75(-1),
        col_diag_72(-1),
        col_diag_77(-1),
        col_ts(-1),
        gas_mw(-1.0),
        gas_diff(-1.0),
        out_ghg_eu(0),
        out_amflux(0),
        out_rich(1),
        make_dataset(false),
        start_date(QString()),
        end_date(QString()),
        start_time(QString()),
        end_time(QString()),
        hf_meth(1),
        lf_meth(1),
        wpl_meth(1),
        foot_meth(1),
        tob1_format(0),
        out_path(QString()),
        fix_out_format(0),
        err_label(QStringLiteral("-9999.0")),
        subset(0),
        qcflag_meth(1),
        out_md(1),
        out_biomet(0),
        use_biomet(0),
        biom_file(QString()),
        biom_dir(QString()),
        biom_recurse(0),
        biom_ext(QStringLiteral("txt")),
        out_mean_cosp(0),
        bin_sp_avail(0),
        full_sp_avail(0),
        files_found(0)
    { ; }
    QString sw_version;
    QString ini_version;
    QString creation_date;
    QString last_change_date;
    int run_mode;
    int run_fcc;
    QString file_name;
    QString project_title;
    QString project_id;
    int file_type;
    QString file_prototype;
    bool use_alt_md_file;
    QString md_file;
    bool use_tlfile;
    QString timeline_file;
    int binary_hnlines;
    int binary_eol;
    int binary_nbytes;
    int binary_little_end;
    QString master_sonic;
    int col_co2;
    int col_h2o;
    int col_ch4;
    int col_n2o;
    int col_int_t_c;
    int col_int_t_1;
    int col_int_t_2;
    int col_int_p;
    int col_air_t;
    int col_air_p;
    int col_diag_75;
    int col_diag_72;
    int col_diag_77;
    int col_ts;
    qreal gas_mw;
    qreal gas_diff;
    int out_ghg_eu;
    int out_amflux;
    int out_rich;
    bool make_dataset;
    QString start_date;
    QString end_date;
    QString start_time;
    QString end_time;
    int hf_meth;
    int lf_meth;
    int wpl_meth;
    int foot_meth;
    int tob1_format;
    QString out_path;
    int fix_out_format;
    QString err_label;
    int subset;
    int qcflag_meth;
    int out_md;
    int out_biomet;
    int use_biomet;
    QString biom_file;
    QString biom_dir;
    int biom_recurse;
    QString biom_ext;
    int out_mean_cosp;
    int bin_sp_avail;
    int full_sp_avail;
    int files_found;
};

// Structure representing spectral settings information for the INI
// processing file
struct SpectraSettingsState
{
    SpectraSettingsState() :
        sa_mode(1),
        horst_lens(2),
        start_sa_date(QString()),
        end_sa_date(QString()),
        sa_file(QString()),
        sa_min_smpl(10),
        sa_fmin_co2(0.0006),
        sa_fmin_h2o(0.0006),
        sa_fmin_ch4(0.0006),
        sa_fmin_gas4(0.0006),
        sa_fmax_co2(5.0),
        sa_fmax_h2o(5.0),
        sa_fmax_ch4(5.0),
        sa_fmax_gas4(5.0),
        sa_hfn_co2_fmin(5.0),
        sa_hfn_h2o_fmin(5.0),
        sa_hfn_ch4_fmin(5.0),
        sa_hfn_gas4_fmin(5.0),
        sa_min_co2(2.0),
        sa_min_ch4(0.2),
        sa_min_gas4(0.02),
        sa_min_le(20.0),
        sa_min_h(-20.0),
        add_sonic_lptf(1),
        f10_co2_trshld(2.0),
        f10_ch4_trshld(0.2),
        f10_gas4_trshld(0.02),
        f10_le_trshld(10.0),
        f10_h_trshld(10.0),
        ex_file(QString()),
        sa_bin_spectra(QString()),
        sa_full_spectra(QString()),
        ex_dir(QString()),
        subset(0)
    { ; }
    int sa_mode;
    int horst_lens;
    QString start_sa_date;
    QString end_sa_date;
    QString sa_file;
    int sa_min_smpl;
    qreal sa_fmin_co2;
    qreal sa_fmin_h2o;
    qreal sa_fmin_ch4;
    qreal sa_fmin_gas4;
    qreal sa_fmax_co2;
    qreal sa_fmax_h2o;
    qreal sa_fmax_ch4;
    qreal sa_fmax_gas4;
    qreal sa_hfn_co2_fmin;
    qreal sa_hfn_h2o_fmin;
    qreal sa_hfn_ch4_fmin;
    qreal sa_hfn_gas4_fmin;
    qreal sa_min_co2;
    qreal sa_min_ch4;
    qreal sa_min_gas4;
    qreal sa_min_le;
    qreal sa_min_h;
    int add_sonic_lptf;
    qreal f10_co2_trshld;
    qreal f10_ch4_trshld;
    qreal f10_gas4_trshld;
    qreal f10_le_trshld;
    qreal f10_h_trshld;
    QString ex_file;
    QString sa_bin_spectra;
    QString sa_full_spectra;
    QString ex_dir;
    int subset;
};

// Structure representing raw processing general information for the INI
// processing file
struct ScreenGeneralState
{
    ScreenGeneralState() :
        data_path(QString()),
        spectra_path(QString()),
        stat_file(QString()),
        nfile(1),
        start_run(QString()),
        end_run(QString()),
        timestamp_end(false),
        flux_file(QString()),
        in1_file(QString()),
        in2_file(QString()),
        recurse(1),
        use_geo_north(false),
        mag_dec(0.0),
        dec_date(QString()),
        flag1_col(-1),
        flag1_threshold(-9999.0),
        flag1_policy(1),
        flag2_col(-1),
        flag2_threshold(-9999.0),
        flag2_policy(1),
        flag3_col(-1),
        flag3_threshold(-9999.0),
        flag3_policy(1),
        flag4_col(-1),
        flag4_threshold(-9999.0),
        flag4_policy(1),
        flag5_col(-1),
        flag5_threshold(-9999.0),
        flag5_policy(1),
        flag6_col(-1),
        flag6_threshold(-9999.0),
        flag6_policy(1),
        flag7_col(-1),
        flag7_threshold(-9999.0),
        flag7_policy(1),
        flag8_col(-1),
        flag8_threshold(-9999.0),
        flag8_policy(1),
        flag9_col(-1),
        flag9_threshold(-9999.0),
        flag9_policy(1),
        flag10_col(-1),
        flag10_threshold(-9999.0),
        flag10_policy(1)
    { ; }
    QString data_path;
    QString spectra_path;
    QString stat_file;
    int nfile;
    QString start_run;
    QString end_run;
    bool timestamp_end;
    QString flux_file;
    QString in1_file;
    QString in2_file;
    int recurse;
    bool use_geo_north;
    qreal mag_dec;
    QString dec_date;
    int flag1_col;
    qreal flag1_threshold;
    int flag1_policy;
    int flag2_col;
    qreal flag2_threshold;
    int flag2_policy;
    int flag3_col;
    qreal flag3_threshold;
    int flag3_policy;
    int flag4_col;
    qreal flag4_threshold;
    int flag4_policy;
    int flag5_col;
    qreal flag5_threshold;
    int flag5_policy;
    int flag6_col;
    qreal flag6_threshold;
    int flag6_policy;
    int flag7_col;
    qreal flag7_threshold;
    int flag7_policy;
    int flag8_col;
    qreal flag8_threshold;
    int flag8_policy;
    int flag9_col;
    qreal flag9_threshold;
    int flag9_policy;
    int flag10_col;
    qreal flag10_threshold;
    int flag10_policy;
};

// Structure representing raw processing settings information for the INI
// processing file
struct ScreenSettingState
{
    ScreenSettingState() :
        nfiles(1),
        max_lack(10),
        cross_wind(0),
        flow_distortion(0),
        rot_meth(1),
        detrend_meth(0),
        timeconst(250.0),
        tlag_meth(2),
        tap_win(3),
        nbins(90),
        avrg_len(30),
        u_offset(0.0),
        v_offset(0.0),
        w_offset(0.0),
        out_bin_sp(1),
        out_bin_og(0),
        out_full_sp_u(0),
        out_full_sp_v(0),
        out_full_sp_w(0),
        out_full_sp_ts(0),
        out_full_sp_co2(0),
        out_full_sp_h2o(0),
        out_full_sp_ch4(0),
        out_full_sp_n2o(0),
        out_full_cosp_u(0),
        out_full_cosp_v(0),
        out_full_cosp_ts(1),
        out_full_cosp_co2(0),
        out_full_cosp_h2o(0),
        out_full_cosp_ch4(0),
        out_full_cosp_n2o(0),
        out_st_1(1),
        out_st_2(0),
        out_st_3(0),
        out_st_4(0),
        out_st_5(0),
        out_st_6(0),
        out_st_7(0),
        out_raw_1(0),
        out_raw_2(0),
        out_raw_3(0),
        out_raw_4(0),
        out_raw_5(0),
        out_raw_6(0),
        out_raw_7(0),
        out_raw_u(0),
        out_raw_v(0),
        out_raw_w(0),
        out_raw_ts(0),
        out_raw_co2(0),
        out_raw_h2o(0),
        out_raw_ch4(0),
        out_raw_gas4(0),
        out_raw_tair(0),
        out_raw_pair(0),
        to_mixratio(1),
        filter_sr(1),
        filter_al(1),
        bu_corr(-1),
        bu_multi(0),
        l_day_bot_gain(0.944),
        l_day_bot_offset(2.57),
        l_day_top_gain(1.005),
        l_day_top_offset(0.24),
        l_day_spar_gain(1.010),
        l_day_spar_offset(0.36),
        l_night_bot_gain(0.883),
        l_night_bot_offset(2.17),
        l_night_top_gain(1.008),
        l_night_top_offset(-0.41),
        l_night_spar_gain(1.010),
        l_night_spar_offset(-0.17),
        m_day_bot1(2.8),
        m_day_bot2(-0.0681),
        m_day_bot3(0.0021),
        m_day_bot4(-0.334),
        m_day_top1(-0.1),
        m_day_top2(-0.0044),
        m_day_top3(0.0011),
        m_day_top4(-0.022),
        m_day_spar1(0.3),
        m_day_spar2(-0.0007),
        m_day_spar3(0.0006),
        m_day_spar4(-0.044),
        m_night_bot1(0.5),
        m_night_bot2(-0.1160),
        m_night_bot3(0.0087),
        m_night_bot4(-0.206),
        m_night_top1(-1.7),
        m_night_top2(-0.1160),
        m_night_top3(0.0051),
        m_night_top4(-0.029),
        m_night_spar1(-2.1),
        m_night_spar2(-0.0200),
        m_night_spar3(0.0070),
        m_night_spar4(-0.026),
        out_details(0),
        out_biomet(0),
        power_of_two(1)
    { ; }
    int nfiles;
    int max_lack;
    int cross_wind;
    int flow_distortion;
    int rot_meth;
    int detrend_meth;
    qreal timeconst;
    int tlag_meth;
    int tap_win;
    int nbins;
    int avrg_len;
    qreal u_offset;
    qreal v_offset;
    qreal w_offset;
    int out_bin_sp;
    int out_bin_og;
    int out_full_sp_u;
    int out_full_sp_v;
    int out_full_sp_w;
    int out_full_sp_ts;
    int out_full_sp_co2;
    int out_full_sp_h2o;
    int out_full_sp_ch4;
    int out_full_sp_n2o;
    int out_full_cosp_u;
    int out_full_cosp_v;
    int out_full_cosp_ts;
    int out_full_cosp_co2;
    int out_full_cosp_h2o;
    int out_full_cosp_ch4;
    int out_full_cosp_n2o;
    int out_st_1;
    int out_st_2;
    int out_st_3;
    int out_st_4;
    int out_st_5;
    int out_st_6;
    int out_st_7;
    int out_raw_1;
    int out_raw_2;
    int out_raw_3;
    int out_raw_4;
    int out_raw_5;
    int out_raw_6;
    int out_raw_7;
    int out_raw_u;
    int out_raw_v;
    int out_raw_w;
    int out_raw_ts;
    int out_raw_co2;
    int out_raw_h2o;
    int out_raw_ch4;
    int out_raw_gas4;
    int out_raw_tair;
    int out_raw_pair;
    int to_mixratio;
    int filter_sr;
    int filter_al;
    int bu_corr;
    int bu_multi;
    qreal l_day_bot_gain;
    qreal l_day_bot_offset;
    qreal l_day_top_gain;
    qreal l_day_top_offset;
    qreal l_day_spar_gain;
    qreal l_day_spar_offset;
    qreal l_night_bot_gain;
    qreal l_night_bot_offset;
    qreal l_night_top_gain;
    qreal l_night_top_offset;
    qreal l_night_spar_gain;
    qreal l_night_spar_offset;
    qreal m_day_bot1;
    qreal m_day_bot2;
    qreal m_day_bot3;
    qreal m_day_bot4;
    qreal m_day_top1;
    qreal m_day_top2;
    qreal m_day_top3;
    qreal m_day_top4;
    qreal m_day_spar1;
    qreal m_day_spar2;
    qreal m_day_spar3;
    qreal m_day_spar4;
    qreal m_night_bot1;
    qreal m_night_bot2;
    qreal m_night_bot3;
    qreal m_night_bot4;
    qreal m_night_top1;
    qreal m_night_top2;
    qreal m_night_top3;
    qreal m_night_top4;
    qreal m_night_spar1;
    qreal m_night_spar2;
    qreal m_night_spar3;
    qreal m_night_spar4;
    int out_details;
    int out_biomet;
    int power_of_two;
};

// Structure representing tests information for the INI
// processing file
struct ScreenTestState
{
    ScreenTestState() :
        test_sr(1),
        test_ar(1),
        test_do(1),
        test_al(1),
        test_sk(1),
        test_ds(0),
        test_tl(0),
        test_aa(0),
        test_ns(0)
    { ; }
    int test_sr;
    int test_ar;
    int test_do;
    int test_al;
    int test_sk;
    int test_ds;
    int test_tl;
    int test_aa;
    int test_ns;
};

// Structure representing tests parameters information for the INI
// processing file
struct ScreenParamState
{
    ScreenParamState() :
        sr_num_spk(3),
        sr_lim_u(3.5),
        sr_lim_w(5.0),
        sr_lim_co2(3.5),
        sr_lim_h2o(3.5),
        sr_lim_ch4(8.0),
        sr_lim_n2o(8.0),
        sr_lim_hf(1.0),
        ar_lim(7.0),
        ar_bins(100),
        ar_hf_lim(70),
        do_extlim_dw(10),
        do_hf1_lim(10.0),
        do_hf2_lim(6.0),
        al_u_max(30.0),
        al_w_max(5.0),
        al_tson_min(-40.0),
        al_tson_max(50.0),
        al_co2_min(200.0),
        al_co2_max(900.0),
        al_h2o_min(0.0),
        al_h2o_max(40.0),
        al_ch4_min(0.17),
        al_ch4_max(1000.0),
        al_n2o_min(0.032),
        al_n2o_max(1000.0),
        sk_hf_skmin(-2.0),
        sk_hf_skmax(2.0),
        sk_sf_skmin(-1.0),
        sk_sf_skmax(1.0),
        sk_hf_kumin(1.0),
        sk_hf_kumax(8.0),
        sk_sf_kumin(2.0),
        sk_sf_kumax(5.0),
        ds_hf_uv(4.0),
        ds_hf_w(2.0),
        ds_hf_t(4.0),
        ds_hf_co2(40.0),
        ds_hf_h2o(3.26),
        ds_hf_ch4(40.0),
        ds_hf_n2o(40.0),
        ds_hf_var(3.0),
        ds_sf_uv(2.7),
        ds_sf_w(1.3),
        ds_sf_t(2.7),
        ds_sf_co2(27.0),
        ds_sf_h2o(2.2),
        ds_sf_ch4(27.0),
        ds_sf_n2o(27.0),
        ds_sf_var(2.0),
        tl_hf_lim(20.0),
        tl_sf_lim(10.0),
        tl_def_co2(3.5),
        tl_def_h2o(2.5),
        tl_def_ch4(3.5),
        tl_def_n2o(2.5),
        aa_min(-30.0),
        aa_max(30.0),
        aa_lim(10.0),
        ns_hf_lim(0.5)
    { ; }
    int sr_num_spk;
    qreal sr_lim_u;
    qreal sr_lim_w;
    qreal sr_lim_co2;
    qreal sr_lim_h2o;
    qreal sr_lim_ch4;
    qreal sr_lim_n2o;
    qreal sr_lim_hf;
    qreal ar_lim;
    int ar_bins;
    int ar_hf_lim;
    int do_extlim_dw;
    qreal do_hf1_lim;
    qreal do_hf2_lim;
    qreal al_u_max;
    qreal al_w_max;
    qreal al_tson_min;
    qreal al_tson_max;
    qreal al_co2_min;
    qreal al_co2_max;
    qreal al_h2o_min;
    qreal al_h2o_max;
    qreal al_ch4_min;
    qreal al_ch4_max;
    qreal al_n2o_min;
    qreal al_n2o_max;
    qreal sk_hf_skmin;
    qreal sk_hf_skmax;
    qreal sk_sf_skmin;
    qreal sk_sf_skmax;
    qreal sk_hf_kumin;
    qreal sk_hf_kumax;
    qreal sk_sf_kumin;
    qreal sk_sf_kumax;
    qreal ds_hf_uv;
    qreal ds_hf_w;
    qreal ds_hf_t;
    qreal ds_hf_co2;
    qreal ds_hf_h2o;
    qreal ds_hf_ch4;
    qreal ds_hf_n2o;
    qreal ds_hf_var;
    qreal ds_sf_uv;
    qreal ds_sf_w;
    qreal ds_sf_t;
    qreal ds_sf_co2;
    qreal ds_sf_h2o;
    qreal ds_sf_ch4;
    qreal ds_sf_n2o;
    qreal ds_sf_var;
    qreal tl_hf_lim;
    qreal tl_sf_lim;
    qreal tl_def_co2;
    qreal tl_def_h2o;
    qreal tl_def_ch4;
    qreal tl_def_n2o;
    qreal aa_min;
    qreal aa_max;
    qreal aa_lim;
    qreal ns_hf_lim;
};

struct ScreenTiltState
{
    ScreenTiltState() :
        start_date(QString()),
        end_date(QString()),
        mode(1),
        north_offset(0.0),
        min_num_per_sec(0),
        w_max(0.099),
        u_min(-0.001),
        file(QString()),
        fix_policy(0),
        angles(),
        subset(0)
    { ; }
    QString start_date;
    QString end_date;
    int mode;
    qreal north_offset;
    int min_num_per_sec;
    qreal w_max;
    qreal u_min;
    QString file;
    int fix_policy;
    QList<AngleItem> angles;
    int subset;
};

struct TimelagOptState
{
    TimelagOptState() :
        start_date(QString()),
        end_date(QString()),
        mode(1),
        file(QString()),
        to_h2o_nclass(10),
        co2_min_flux(2.0),
        ch4_min_flux(0.2),
        gas4_min_flux(0.02),
        le_min_flux(20.0),
        pg_range(1.5),
        co2_min_lag(-1000.1),
        co2_max_lag(-1000.1),
        h2o_min_lag(-1000.1),
        h2o_max_lag(-1000.1),
        ch4_min_lag(-1000.1),
        ch4_max_lag(-1000.1),
        gas4_min_lag(-1000.1),
        gas4_max_lag(-1000.1),
        subset(0)
    { ; }
    QString start_date;
    QString end_date;
    int mode;
    QString file;
    int to_h2o_nclass;
    qreal co2_min_flux;
    qreal ch4_min_flux;
    qreal gas4_min_flux;
    qreal le_min_flux;
    qreal pg_range;
    qreal co2_min_lag;
    qreal co2_max_lag;
    qreal h2o_min_lag;
    qreal h2o_max_lag;
    qreal ch4_min_lag;
    qreal ch4_max_lag;
    qreal gas4_min_lag;
    qreal gas4_max_lag;
    int subset;
};

struct RandomErrorState
{
    RandomErrorState() :
        method(0),
        its_method(1),
        its_tlag_max(10.0),
        its_sec_factor(20.0)
    { ; }
    int method;
    int its_method;
    qreal its_tlag_max;
    qreal its_sec_factor;
};

struct BiometState
{
    BiometState() :
        native_header(1),
        hlines(2),
        separator(QStringLiteral("comma")),
        tstamp_ref(QStringLiteral("end")),
        col_ta(999),
        col_pa(999),
        col_rh(-1),
        col_rg(-1),
        col_lwin(-1),
        col_ppfd(-1)
    { ; }
    int native_header;
    int hlines;
    QString separator;
    QString tstamp_ref;
    int col_ta;
    int col_pa;
    int col_rh;
    int col_rg;
    int col_lwin;
    int col_ppfd;
};

// Container structure representing information for the INI
// processing file
struct EcProjectState
{
    EcProjectState() :
        projectGeneral(),
        spectraSettings(),
        screenGeneral(),
        screenSetting(),
        screenTest(),
        screenParam(),
        screenTilt(),
        timelagOpt(),
        randomError(),
        biomParam()
    { ; }
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
};

#endif // ECPROJECTSTATE_H
