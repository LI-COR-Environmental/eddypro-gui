/***************************************************************************
  defs.h
  -------------------
  Global application resources. Constants, enumerations and strings.
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

#ifndef DEFS_H
#define DEFS_H

class QChar;
class QString;
class QStringLiteral;

// Contains global string literals
namespace Defs
{
    const QString APP_VERSION_STR      = QStringLiteral("5.1.0");
    const QString APP_STAGE_STR        = QStringLiteral("");
    const QString PROJECT_VERSION_STR  = QStringLiteral("4.2");
    const QString METADATA_VERSION_STR = QStringLiteral("3.1");
    const QString WIN_COMPILER         = QStringLiteral("MinGW 4.8.0 x86_32");
    const QString MAC_COMPILER         = QStringLiteral("Clang 4.2 x86_64");

    enum CurrPage
    {
        CurrPageWelcome,          // 0, welcome page
        CurrPageProjectCreation,  // 1, project creation page
        CurrPageBasicSettings,    // 2, basic settings page
        CurrPageAdvancedSettings, // 3, advanced settings page
        CurrPageRun               // 4, run page
    };

    enum CurrStatus
    {
        CurrStatusReady, // 0, ready
        CurrStatusRun,   // 1, running
        CurrStatusPause  // 2, pause
    };

    enum CurrRunStatus
    {
        CurrRunStatusExp, // 0, running express
        CurrRunStatusAdv, // 1, running advanced
        CurrRunStatusRet  // 2, running retriever
    };

    enum CurrRunMode
    {
        CurrRunModeAdv, // 0, running advanced
        CurrRunModeExp, // 1, running express
        CurrRunModeRet  // 2, running retriever
    };

    enum RawFileType
    {
        RawFileTypeGHG,   // 0, GHG
        RawFileTypeASCII, // 1, ASCII
        RawFileTypeTOB1,  // 2, TOB1
        RawFileTypeSLT1,  // 3, SLT1
        RawFileTypeSLT2,  // 4, SLT2
        RawFileTypeBIN    // 5, BIN
    };

#if defined(Q_OS_WIN)
    const QString EXEC_FILE_EXT = QStringLiteral(".exe");
    const QString HOST_OS = QStringLiteral("win");
#elif defined(Q_OS_MAC)
    const QString EXEC_FILE_EXT = QStringLiteral(".app");
    const QString HOST_OS = QStringLiteral("mac");
#elif defined(Q_OS_LINUX)
    const QString EXEC_FILE_EXT = QStringLiteral("");
    const QString HOST_OS = QStringLiteral("lin");
#endif

    // strings
    const QString APP_NAME       = QStringLiteral("EddyPro");
    const QString APP_NAME_LCASE = QStringLiteral("eddypro");
    const QString APP_NAME_UCASE = QStringLiteral("EDDYPRO");
    const QString ORG_DOMAIN     = QStringLiteral("licor.com");
    const QString ORG_NAME       = QStringLiteral("LI-COR");

    // UTF-16 hex codes
    const QChar DEGREE      = QChar(0x00B0);
    const QChar PLUSMINUS   = QChar(0x00B1);
    const QChar MID_DOT     = QChar(0x2219);
    const QChar SQUARE      = QChar(0x00B2);
    const QChar CUBE        = QChar(0x00B3);
    const QChar MICRO       = QChar(0x00B5);
    const QChar SIGMA       = QChar(0x03C3);
    const QChar O_SLASH     = QChar(0x00F8);
    const QChar A_DIAERESIS = QChar(0x00E4);
    const QChar NBPS        = QChar(0x00A0);
    const QChar ALPHA       = QChar(0x03B1);
    const QChar RHO         = QChar(0x03C1);
    const QChar THETA       = QChar(0x03B8);
    const QChar SUBTWO            = QChar(0x2082);
    const QChar SUBTHREE          = QChar(0x2083);
    const QChar SUBFOUR           = QChar(0x2084);
    // const QChar DEGREE_C          = QChar(2103);  // UNICODE

    const QString DEGREE_C        = DEGREE + QStringLiteral("C");
    const QString CDEGREE_C       = QStringLiteral("c") + DEGREE_C;
    const QString MMOL_MOL_STRING = QStringLiteral("mmol/mol");
    const QString UMOL_MOL_STRING = MICRO + QStringLiteral("mol/mol");
    const QString MMOL_M_STRING   = QStringLiteral("mmol/m");
    const QString UMOL_M_STRING   = MICRO + QStringLiteral("mol/m");
    const QString NMOL_MOL_STRING = QStringLiteral("nmol/mol");
    const QString MMOL_M3_STRING  = MMOL_M_STRING + Defs::CUBE;
    const QString UMOL_M3_STRING  = UMOL_M_STRING + Defs::CUBE;
    const QString UMOL_M2S_STRING = MICRO + QStringLiteral("mol/m") + SQUARE + QStringLiteral("s");
    const QString W_M2_STRING     = QStringLiteral("W/m") + SQUARE;
    const QString G_M3_STRING     =  QStringLiteral("g/m") + Defs::CUBE;
    const QString MG_M3_STRING    =  QStringLiteral("mg/m") + Defs::CUBE;
    const QString UG_M3_STRING    =  MICRO + QStringLiteral("g/m") + Defs::CUBE;
    const QString M3_G_CM_STRING  =  QStringLiteral("m") + Defs::CUBE + QStringLiteral("/(g*cm)");
    const QString CM2_S_STRING    = QStringLiteral("cm") + SQUARE + QStringLiteral("/s");
    const QString L_MIN_STRING    =  QStringLiteral("l/min");
    const QString M3_S_STRING     =  QStringLiteral("m") + Defs::CUBE + QStringLiteral("/s");
    const QString CM3_S_STRING    =  QStringLiteral("cm") + Defs::CUBE + QStringLiteral("/s");
    const QString FT3_S_STRING    =  QStringLiteral("ft") + Defs::CUBE + QStringLiteral("/s");
    const QString IN3_S_STRING    =  QStringLiteral("in") + Defs::CUBE + QStringLiteral("/s");

    const QString CO2_STRING   = QStringLiteral("CO<sub>2</sub>");
    const QString H2O_STRING   = QStringLiteral("H<sub>2</sub>O");
    const QString CH4_STRING   = QStringLiteral("CH<sub>4</sub>");
    const QString N2O_STRING   = QStringLiteral("N<sub>2</sub>O");
    const QString TSON_STRING  = QStringLiteral("T<sub>s</sub>");
    const QString GAS4_STRING  = QStringLiteral("4<sup>th</sup>");
    const QString C_DEG_STRING = DEGREE + QStringLiteral("C");

    const QString READY      = QT_TR_NOOP(QStringLiteral("Ready"));
    const QString ABOUT_DESC = QT_TR_NOOP(QStringLiteral("LI-COR EddyPRO software suite."));

    // dirs
    const QString DATA_FILE_DIR        = QStringLiteral("ene");
    const QString BIN_FILE_DIR         = QStringLiteral("bin");
    const QString LOG_FILE_DIR         = QStringLiteral("log");
    const QString INI_FILE_DIR         = QStringLiteral("ini");
    const QString TMP_FILE_DIR         = QStringLiteral("tmp");
    const QString SMF_FILE_DIR         = QStringLiteral("smf");
    const QString TRANSLATION_FILE_DIR = QStringLiteral("tra");

#if defined(Q_OS_WIN)
    const QString LICOR_ENV_DIR        = QStringLiteral(".licor");
#elif defined(Q_OS_MAC)
    const QString LICOR_ENV_DIR        = QStringLiteral("licor");
#endif

    // file extensions
    const QString METADATA_FILE_EXT        = QStringLiteral("metadata");
    const QString PROJECT_FILE_EXT         = APP_NAME_LCASE;
    const QString SMARTFLUX_FILE_EXT       = QStringLiteral("smartflux");
    const QString AUX_FILE_EXT             = QStringLiteral("ini");
    const QString LOG_FILE_EXT             = QStringLiteral("log");
    const QString DATA_FILE_EXT            = QStringLiteral("ene");
    const QString RAW_DATA_FILE_EXT        = QStringLiteral("raw");
    const QString TMP_NATIVE_DATA_FILE_EXT = QStringLiteral("tmp");
    const QString RAW_NATIVE_DATA_FILE_EXT = QStringLiteral("raw");
    const QString CSV_NATIVE_DATA_FILE_EXT = QStringLiteral("csv");
    const QString ARJ_NATIVE_DATA_FILE_EXT = QStringLiteral("arj");
    const QString ZIP_NATIVE_DATA_FILE_EXT = QStringLiteral("zip");
    const QString GHG_NATIVE_DATA_FILE_EXT = QStringLiteral("ghg");
    const QString CSS_FILE_EXT             = QStringLiteral("qss");

    // auxiliary programs
    const QString COMPRESSOR_BIN        = QString(QStringLiteral("7z%1")).arg(EXEC_FILE_EXT);
    const QString PRE_PROCESSOR_BIN     = QString(QStringLiteral("preprocessor%1")).arg(EXEC_FILE_EXT);
    const QString PLANAR_FIT_BIN        = QString(QStringLiteral("planarfit%1")).arg(EXEC_FILE_EXT);
    const QString SPECTRAL_ANALYSIS_BIN = QString(QStringLiteral("specanalysis%1")).arg(EXEC_FILE_EXT);
    const QString FLUX_COMP_BIN         = QString(QStringLiteral("fluxcomputation%1")).arg(EXEC_FILE_EXT);
    const QString TIMELAG_OPT_BIN       = QString(QStringLiteral("timelag_optimizer%1")).arg(EXEC_FILE_EXT);
    const QString ENGINE_1_BIN          = QString(QStringLiteral("%1_rp%2")).arg(APP_NAME_LCASE).arg(EXEC_FILE_EXT);
    const QString ENGINE_2_BIN          = QString(QStringLiteral("%1_fcc%2")).arg(APP_NAME_LCASE).arg(EXEC_FILE_EXT);
    const QString FREEZER_BIN           = QString(QStringLiteral("pausep%1")).arg(EXEC_FILE_EXT);

    // auxiliary ini files
    const QString RS_INI = QString(QStringLiteral("rssetup.%1")).arg(AUX_FILE_EXT);
    const QString PF_INI = QString(QStringLiteral("pfsetup.%1")).arg(AUX_FILE_EXT);
    const QString SA_INI = QString(QStringLiteral("sasetup.%1")).arg(AUX_FILE_EXT);
    const QString EC_INI = QString(QStringLiteral("ecsetup.%1")).arg(AUX_FILE_EXT);

    const QString MD_INI_TAG     = QStringLiteral(";GHG_METADATA");
    const QString APP_MD_INI_TAG = QString(QStringLiteral(";%1_METADATA")).arg(APP_NAME_UCASE);
    const QString APP_PD_INI_TAG = QString(QStringLiteral(";%1_PROCESSING")).arg(APP_NAME_UCASE);

    // file names
    const QString DEFAULT_METADATA_FILENAME = QString(QStringLiteral("%1.%2")).arg(APP_NAME_LCASE).arg(METADATA_FILE_EXT);
    const QString DEFAULT_PROCESSING_FILENAME = QString(QStringLiteral("processing.%1")).arg(PROJECT_FILE_EXT);
    const QString DEFAULT_PROJECT_FILENAME = QT_TR_NOOP(QStringLiteral("project-untitled.%1").arg(PROJECT_FILE_EXT));
    const QString DEFAULT_BIOMET_SUFFIX = QStringLiteral("-biomet");
    const QString DEFAULT_SMARTFLUX_SUFFIX = QStringLiteral("-smartflux");
    const QString DEFAULT_SMARTFLUX_PROJECT_FILENAME = QT_TR_NOOP(QStringLiteral("project-untitled%1.%2").arg(DEFAULT_SMARTFLUX_SUFFIX).arg(PROJECT_FILE_EXT));
    const QString ENGINE_RP_LOGFILE        = QString(QStringLiteral("log/%1_rp.%2")).arg(APP_NAME_LCASE).arg(LOG_FILE_EXT);

    // register configuration strings
    const QString CONFGROUP_GENERAL      = QStringLiteral("/general");
    const QString CONF_GEN_LOOK_FEEL     = QStringLiteral("/lookfeel");
    const QString CONF_GEN_SHOW_SPLASH   = QStringLiteral("/showsplash");
    const QString CONF_GEN_LOADLAST      = QStringLiteral("/loadlast");
    const QString CONF_GEN_RECENTFILES   = QStringLiteral("/recentfiles");
    const QString CONF_GEN_RECENTNUM     = QStringLiteral("/recentnum");
    const QString CONF_GEN_ENV           = QStringLiteral("/env");

    const QString CONFGROUP_PROJECT        = QStringLiteral("/project");
    const QString CONF_PROJ_DEFAULT_PATH   = QStringLiteral("/default_data_path");
    const QString CONF_PROJ_CUSTOM_VARS    = QStringLiteral("/custom_variables");
    const QString CONF_PROJ_SMARTFLUX      = QStringLiteral("/smartflux");
    const QString CONF_PROJ_SMARTFLUX_FILENAME = QStringLiteral("/smartflux_filename");
    const QString CONF_PROJ_SMARTFLUX_FILEPATH = QStringLiteral("/smartflux_filepath");

    const QString CONFGROUP_WINDOW          = QStringLiteral("/window");
    const QString CONF_WIN_STATUSBAR        = QStringLiteral("/status_bar");
    const QString CONF_WIN_FULLSCREEN       = QStringLiteral("/full_screen");
    const QString CONF_WIN_CONSOLEDOCK      = QStringLiteral("/console_dock");
    const QString CONF_WIN_TOOLTIPS         = QStringLiteral("/tooltips");
    const QString CONF_WIN_INFODOCK         = QStringLiteral("/info_dock");
    const QString CONF_WIN_MAINWIN_STATE    = QStringLiteral("/mainwin_state");
    const QString CONF_WIN_MAINWIN_GEOMETRY = QStringLiteral("/mainwin_geometry");
    const QString CONF_WIN_AUTOHELP         = QStringLiteral("/auto_choose_help");
    const QString CONF_WIN_OFFLINEHELP      = QStringLiteral("/offline_help");
    const QString CONF_WIN_LAST_DATAPATH    = QStringLiteral("/last_datapath");
    const QString CONF_WIN_RUN_EXP_MSG      = QStringLiteral("/run_exp_msg");
    const QString CONF_WIN_RUN_ADV_MSG      = QStringLiteral("/run_adv_msg");
    const QString CONF_WIN_RUN_RET_MSG      = QStringLiteral("/run_ret_msg");
    const QString CONF_WIN_BASIC_SETTINGS_CLEARING_MSG = QStringLiteral("/basic_settings_cleaning_msg");
    const QString CONF_WIN_SMARTFLUX_CONFIG_MSG = QStringLiteral("/smartflux_conf_msg");
    const QString CONF_WIN_NOAA_WEBSITE_MSG = QStringLiteral("/noaa_website_msg");
} // namespace Defs

#endif // DEFS_H
