/***************************************************************************
  defs.h
  -------------------
  Global application resources. Constants, enumerations and strings.
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

#ifndef DEFS_H
#define DEFS_H

#include <QDataStream>
#include <QDebug>

class QChar;
class QString;
class QStringLiteral;

////////////////////////////////////////////////////////////////////////////////
/// \file src/defs.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

/// \namespace Defs
/// \brief Contains global string literals
namespace Defs
{
    const auto APP_VERSION = 0x060100;
    const auto APP_VERSION_STR = QStringLiteral("6.1.0");
    const auto APP_STAGE_STR = QStringLiteral("beta5");

    const auto PROJECT_FILE_VERSION = 0x040401;
    const auto PROJECT_FILE_VERSION_STR = QStringLiteral("4.4.1");

    const auto METADATA_FILE_VERSION = 0x030200;
    const auto METADATA_FILE_VERSION_STR = QStringLiteral("3.2.0");

    const auto WIN_COMPILER = QStringLiteral("MinGW 4.9.2 x86-32 on Windows<sup>&reg;</sup> 10");
    const auto MAC_COMPILER = QStringLiteral("Apple LLVM 7.0.0 (clang-700.1.76) x86-64 on Mac OS X<sup>&reg;</sup> 10.11.1");

    const auto CURRENT_COPYRIGHT_YEAR = QStringLiteral("2015");

    enum class CurrPage
    {
        Welcome,
        ProjectCreation,
        BasicSettings,
        AdvancedSettings,
        Run
    };

    enum class CurrStatus
    {
        Ready,
        Run,
        Pause
    };

    enum class CurrRunStatus
    {
        Express,
        Advanced_RP,
        Advanced_FCC,
        Retriever
    };

    enum class CurrRunMode
    {
        Advanced,
        Express,
        Retriever
    };

    enum class RawFileType
    {
        GHG,
        ASCII,
        TOB1,
        SLT1,
        SLT2,
        BIN
    };

    enum class HfSpectraMethod
    {
        Moncrieff,
        Massmann,
        Horst,
        Ibrom,
        Fratini
    };

    // debug helper to show Defs::RawFileType enum values
    QDebug operator<<(QDebug dbg, const RawFileType& fileType);

    // NOTE: necessary if streaming directly Defs::RawFileType in QSettings
//    QDataStream& operator<<(QDataStream& out, const Defs::RawFileType& fileType);
//    QDataStream& operator>>(QDataStream& in, Defs::RawFileType& fileType);

    void qt_registerCustomTypes();

#if defined(Q_OS_WIN)
    const auto EXEC_FILE_EXT = QStringLiteral(".exe");
    const auto HOST_OS = QStringLiteral("win");
#elif defined(Q_OS_MAC)
    const auto EXEC_FILE_EXT = QString();
    const auto HOST_OS = QStringLiteral("mac");
#elif defined(Q_OS_LINUX)
    const auto EXEC_FILE_EXT = QString();
    const auto HOST_OS = QStringLiteral("linux");
#endif

    // strings
    const auto APP_NAME       = QStringLiteral("EddyPro");
    const auto APP_NAME_LCASE = QStringLiteral("eddypro");
    const auto APP_NAME_UCASE = QStringLiteral("EDDYPRO");
    const auto ORG_DOMAIN     = QStringLiteral("licor.com");
    const auto ORG_NAME       = QStringLiteral("LI-COR");
    const auto APP_URL
        = QStringLiteral("http://www.licor.com/env/products/"
                         "eddy_covariance/eddypro.html");
    const auto CALIBRATION_API_URL
        = QStringLiteral("http://envprod.licor.com:8080/api/2012-04/instruments/");

    // UTF-16 hex codes
    const auto DEGREE      = QChar(0x00B0);
    const auto PLUSMINUS   = QChar(0x00B1);
    const auto MID_DOT     = QChar(0x2219);
    const auto SQUARE      = QChar(0x00B2);
    const auto CUBE        = QChar(0x00B3);
    const auto MICRO       = QChar(0x00B5);
    const auto SIGMA       = QChar(0x03C3);
    const auto O_SLASH     = QChar(0x00F8);
    const auto A_DIAERESIS = QChar(0x00E4);
    const auto NBPS        = QChar(0x00A0);
    const auto ALPHA       = QChar(0x03B1);
    const auto RHO         = QChar(0x03C1);
    const auto THETA       = QChar(0x03B8);
    const auto SUBTWO      = QChar(0x2082);
    const auto SUBTHREE    = QChar(0x2083);
    const auto SUBFOUR     = QChar(0x2084);
    const auto DEGREE_C    = QChar(0x2103);
    const auto DEGREE_K    = QChar(0x212A);
    const auto MAC_COMMAND_KEY    = QChar(0x2318);
    const auto TRADEMARK_SYMBOL   = QChar(0x2122);

    const QString CDEGREE_C_STRING = QStringLiteral("c") + DEGREE_C;
    const QString CDEGREE_K_STRING = QStringLiteral("c") + DEGREE_K;
    const auto MMOL_MOL_STRING = QStringLiteral("mmol/mol");
    const QString UMOL_MOL_STRING = MICRO + QStringLiteral("mol/mol");
    const auto MMOL_M_STRING = QStringLiteral("mmol/m");
    const QString UMOL_M_STRING = MICRO + QStringLiteral("mol/m");
    const auto NMOL_MOL_STRING = QStringLiteral("nmol/mol");
    const QString MMOL_M3_STRING = MMOL_M_STRING + CUBE;
    const QString UMOL_M3_STRING = UMOL_M_STRING + CUBE;
    const QString UMOL_M2S_STRING = MICRO + QStringLiteral("mol/m") + SQUARE + QStringLiteral("s");
    const QString W_M2_STRING = QStringLiteral("W/m") + SQUARE;
    const QString G_M3_STRING = QStringLiteral("g/m") + CUBE;
    const QString MG_M3_STRING = QStringLiteral("mg/m") + CUBE;
    const QString UG_M3_STRING = MICRO + QStringLiteral("g/m") + CUBE;
    const QString M3_G_CM_STRING = QStringLiteral("m") + CUBE + QStringLiteral("/(g*cm)");
    const QString CM2_S_STRING = QStringLiteral("cm") + SQUARE + QStringLiteral("/s");
    const QString L_MIN_STRING = QStringLiteral("l/min");
    const QString M3_S_STRING = QStringLiteral("m") + CUBE + QStringLiteral("/s");
    const QString CM3_S_STRING = QStringLiteral("cm") + CUBE + QStringLiteral("/s");
    const QString FT3_S_STRING = QStringLiteral("ft") + CUBE + QStringLiteral("/s");
    const QString IN3_S_STRING = QStringLiteral("in") + CUBE + QStringLiteral("/s");

    const auto CO2_STRING   = QStringLiteral("CO<sub>2</sub>");
    const auto H2O_STRING   = QStringLiteral("H<sub>2</sub>O");
    const auto CH4_STRING   = QStringLiteral("CH<sub>4</sub>");
    const auto N2O_STRING   = QStringLiteral("N<sub>2</sub>O");
    const auto O3_STRING    = QStringLiteral("O<sub>3</sub>");
    const auto TSON_STRING  = QStringLiteral("T<sub>s</sub>");
    const auto GAS4_STRING  = QStringLiteral("4<sup>th</sup>");

    const auto READY      = QObject::tr("Ready");
    const auto ABOUT_DESC = QObject::tr("LI-COR EddyPro software suite.");

    // dirs
    const auto BIN_FILE_DIR         = QStringLiteral("bin");
    const auto LOG_FILE_DIR         = QStringLiteral("log");
    const auto INI_FILE_DIR         = QStringLiteral("ini");
    const auto TMP_FILE_DIR         = QStringLiteral("tmp");
    const auto SMF_FILE_DIR         = QStringLiteral("smf");
    const auto CAL_FILE_DIR         = QStringLiteral("cal");
    const auto TRANSLATION_FILE_DIR = QStringLiteral("tra");
    const auto DOC_DIR              = QStringLiteral("docs");
    const auto TEMPLATE_FILE_DIR    = QStringLiteral("file-templates/");
    const auto OUT_BINNED_COSPECTRA_DIR    = QStringLiteral("/eddypro_binned_cospectra");

#if defined(Q_OS_WIN)
    const auto LICOR_ENV_DIR        = QStringLiteral(".licor");
#elif defined(Q_OS_MAC)
    const auto LICOR_ENV_DIR        = QStringLiteral("licor");
#elif defined(Q_OS_LINUX)
    const auto LICOR_ENV_DIR        = QStringLiteral("licor");
#endif

    // file extensions
    const auto METADATA_FILE_EXT        = QStringLiteral("metadata");
    const auto PROJECT_FILE_EXT         = APP_NAME_LCASE;
    const auto SMARTFLUX_FILE_EXT       = QStringLiteral("smartflux");
    const auto AUX_FILE_EXT             = QStringLiteral("ini");
    const auto LOG_FILE_EXT             = QStringLiteral("log");
    const auto RAW_DATA_FILE_EXT        = QStringLiteral("raw");
    const auto TMP_NATIVE_DATA_FILE_EXT = QStringLiteral("tmp");
    const auto RAW_NATIVE_DATA_FILE_EXT = QStringLiteral("raw");
    const auto CSV_NATIVE_DATA_FILE_EXT = QStringLiteral("csv");
    const auto ARJ_NATIVE_DATA_FILE_EXT = QStringLiteral("arj");
    const auto ZIP_NATIVE_DATA_FILE_EXT = QStringLiteral("zip");
    const auto GHG_NATIVE_DATA_FILE_EXT = QStringLiteral("ghg");
    const auto SLT_NATIVE_DATA_FILE_EXT = QStringLiteral("slt");
    const auto TOB1_NATIVE_DATA_FILE_EXT = QStringLiteral("tob1");
    const auto CSS_FILE_EXT             = QStringLiteral("qss");
    const auto TEMPLATE_FILE_EXT        = QStringLiteral(".txt");

    // eddypro engine
    const auto ENGINE_RP  = QStringLiteral("%1_rp%2").arg(APP_NAME_LCASE).arg(EXEC_FILE_EXT);
    const auto ENGINE_FCC = QStringLiteral("%1_fcc%2").arg(APP_NAME_LCASE).arg(EXEC_FILE_EXT);

    // helper tools
    const auto COMPRESSOR_BIN        = QStringLiteral("7z%1").arg(EXEC_FILE_EXT);
#if defined(Q_OS_WIN)
    const auto FREEZER_BIN           = QStringLiteral("pausep%1").arg(EXEC_FILE_EXT);
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    const auto FREEZER_BIN           = QStringLiteral("kill");
#endif

    // auxiliary ini files
    const auto RS_INI = QStringLiteral("rssetup.%1").arg(AUX_FILE_EXT);
    const auto PF_INI = QStringLiteral("pfsetup.%1").arg(AUX_FILE_EXT);
    const auto SA_INI = QStringLiteral("sasetup.%1").arg(AUX_FILE_EXT);
    const auto EC_INI = QStringLiteral("ecsetup.%1").arg(AUX_FILE_EXT);

    const auto GHG_MD_INI_TAG = QStringLiteral(";GHG_METADATA");
    const auto APP_MD_INI_TAG = QStringLiteral(";%1_METADATA").arg(APP_NAME_UCASE);
    const auto APP_PD_INI_TAG = QStringLiteral(";%1_PROCESSING").arg(APP_NAME_UCASE);

    // file names
    const auto DEFAULT_METADATA_FILENAME = QStringLiteral("%1.%2").arg(APP_NAME_LCASE).arg(METADATA_FILE_EXT);
    const auto DEFAULT_PROCESSING_FILENAME = QStringLiteral("processing.%1").arg(PROJECT_FILE_EXT);
    const auto DEFAULT_PROJECT_FILENAME = QObject::tr("project-untitled.%1").arg(PROJECT_FILE_EXT);
    const auto DEFAULT_BIOMET_SUFFIX = QStringLiteral("-biomet");
    const auto DEFAULT_SMARTFLUX_SUFFIX = QStringLiteral("-smartflux");
    const auto DEFAULT_SMARTFLUX_PROJECT_FILENAME = QObject::tr("project-untitled%1.%2").arg(DEFAULT_SMARTFLUX_SUFFIX).arg(PROJECT_FILE_EXT);

    const auto SPECTRA_FILE_TEMPLATE = QStringLiteral("eddypro_sample_spectral_assessment");
    const auto PLANAR_FIT_FILE_TEMPLATE = QStringLiteral("eddypro_sample_planar_fit");
    const auto TIME_LAG_FILE_TEMPLATE = QStringLiteral("eddypro_sample_timelag_opt");

    // register configuration strings
    const auto CONFGROUP_GENERAL      = QStringLiteral("/general");
    const auto CONF_GEN_LOOK_FEEL     = QStringLiteral("/lookfeel");
    const auto CONF_GEN_SHOW_SPLASH   = QStringLiteral("/showsplash");
    const auto CONF_GEN_LOADLAST      = QStringLiteral("/loadlast");
    const auto CONF_GEN_RECENTFILES   = QStringLiteral("/recentfiles");
    const auto CONF_GEN_RECENTNUM     = QStringLiteral("/recentnum");
    const auto CONF_GEN_ENV           = QStringLiteral("/env");

    const auto CONFGROUP_PROJECT        = QStringLiteral("/project");
    const auto CONF_PROJ_DEFAULT_PATH   = QStringLiteral("/default_data_path");
    const auto CONF_PROJ_CUSTOM_VARS    = QStringLiteral("/custom_variables");
    const auto CONF_PROJ_SMARTFLUX      = QStringLiteral("/smartflux");
    const auto CONF_PROJ_SMARTFLUX_FILENAME = QStringLiteral("/smartflux_filename");
    const auto CONF_PROJ_SMARTFLUX_FILEPATH = QStringLiteral("/smartflux_filepath");

    const auto CONFGROUP_WINDOW          = QStringLiteral("/window");
    const auto CONF_WIN_STATUSBAR        = QStringLiteral("/status_bar");
    const auto CONF_WIN_FULLSCREEN       = QStringLiteral("/full_screen");
    const auto CONF_WIN_CONSOLEDOCK      = QStringLiteral("/console_dock");
    const auto CONF_WIN_TOOLTIPS         = QStringLiteral("/tooltips");
    const auto CONF_WIN_INFODOCK         = QStringLiteral("/info_dock");
    const auto CONF_WIN_MAINWIN_STATE    = QStringLiteral("/mainwin_state");
    const auto CONF_WIN_MAINWIN_GEOMETRY = QStringLiteral("/mainwin_geometry");
    const auto CONF_WIN_AUTOHELP         = QStringLiteral("/auto_choose_help");
    const auto CONF_WIN_OFFLINEHELP      = QStringLiteral("/offline_help");
    const auto CONF_WIN_LAST_DATAPATH    = QStringLiteral("/last_datapath");
    const auto CONF_WIN_RUN_EXP_MSG      = QStringLiteral("/run_exp_msg");
    const auto CONF_WIN_RUN_ADV_MSG      = QStringLiteral("/run_adv_msg");
    const auto CONF_WIN_RUN_RET_MSG      = QStringLiteral("/run_ret_msg");
    const auto CONF_WIN_BASIC_SETTINGS_CLEARING_MSG = QStringLiteral("/basic_settings_cleaning_msg");
    const auto CONF_WIN_SMARTFLUX_CONFIG_MSG = QStringLiteral("/smartflux_conf_msg");
    const auto CONF_WIN_NOAA_WEBSITE_MSG = QStringLiteral("/noaa_website_msg");
    const auto CONF_WIN_AOA_SELECTION_MSG = QStringLiteral("/aoa_selection_msg");

    const auto LICOR_EP_VERSION_FILE = QStringLiteral("http://dl.licor.com/EddyPro/version");
    const auto EP_SAMPLE_DATA_FILES  = QStringLiteral("http://dl.licor.com/EddyPro/data");
    const QString EP_USER_AGENT = QStringLiteral("EddyPro ") + APP_VERSION_STR;

    const auto GHG_TIMESTAMP_FORMAT = QStringLiteral("yyyy-mm-ddTHHMM??_");

    const auto CONF_WIN_DATES_CHECK_MSG = QStringLiteral("/dates_range_check_msg");
} // namespace Defs

Q_DECLARE_METATYPE(Defs::CurrRunMode)
Q_DECLARE_METATYPE(Defs::RawFileType)

#endif // DEFS_H
