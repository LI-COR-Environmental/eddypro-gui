/***************************************************************************
  dlproject.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#include "dlproject.h"

#include <QDebug>
#include <QRegularExpression>
#include <QSettings>

#include "dbghelper.h"
#include "dlinidefs.h"
#include "fileutils.h"
#include "mainwindow.h"
#include "stringutils.h"
#include "widget_utils.h"

const QString DlProject::ANEM_MANUFACTURER_STRING_0 = QStringLiteral("csi");
const QString DlProject::ANEM_MANUFACTURER_STRING_1 = QStringLiteral("gill");
const QString DlProject::ANEM_MANUFACTURER_STRING_2 = QStringLiteral("metek");
const QString DlProject::ANEM_MANUFACTURER_STRING_3 = QStringLiteral("young");
const QString DlProject::ANEM_MANUFACTURER_STRING_4 = QStringLiteral("other_sonic");

const QString DlProject::ANEM_WIND_FORMAT_STRING_0 = QStringLiteral("uvw");
const QString DlProject::ANEM_WIND_FORMAT_STRING_1 = QStringLiteral("polar_w");
const QString DlProject::ANEM_WIND_FORMAT_STRING_2 = QStringLiteral("axis");

const QString DlProject::ANEM_NORTH_ALIGN_STRING_0 = QStringLiteral("axis");
const QString DlProject::ANEM_NORTH_ALIGN_STRING_1 = QStringLiteral("spar");
const QString DlProject::ANEM_NORTH_ALIGN_STRING_2 = QStringLiteral("na");

const QString DlProject::IRGA_MANUFACTURER_STRING_0 = QStringLiteral("licor");
const QString DlProject::IRGA_MANUFACTURER_STRING_1 = QStringLiteral("other_irga");

const QString DlProject::IRGA_MODEL_STRING_0 = QStringLiteral("li6262");
const QString DlProject::IRGA_MODEL_STRING_1 = QStringLiteral("li7000");
const QString DlProject::IRGA_MODEL_STRING_2 = QStringLiteral("li7500");
const QString DlProject::IRGA_MODEL_STRING_3 = QStringLiteral("li7500a");
const QString DlProject::IRGA_MODEL_STRING_4 = QStringLiteral("li7200");
const QString DlProject::IRGA_MODEL_STRING_5 = QStringLiteral("li7700");
const QString DlProject::IRGA_MODEL_STRING_6 = QStringLiteral("generic_open_path");
const QString DlProject::IRGA_MODEL_STRING_7 = QStringLiteral("generic_closed_path");
const QString DlProject::IRGA_MODEL_STRING_8 = QStringLiteral("open_path_krypton");
const QString DlProject::IRGA_MODEL_STRING_9 = QStringLiteral("open_path_lyman");
const QString DlProject::IRGA_MODEL_STRING_10 = QStringLiteral("closed_path_krypton");
const QString DlProject::IRGA_MODEL_STRING_11 = QStringLiteral("closed_path_lyman");
const QString DlProject::IRGA_MODEL_STRING_12 = QStringLiteral("li7500rs");
const QString DlProject::IRGA_MODEL_STRING_13 = QStringLiteral("li7200rs");

const QString DlProject::IRGA_SW_VERSION_STRING_0 = QStringLiteral("0.0.0");
const QString DlProject::IRGA_SW_VERSION_STRING_1 = QStringLiteral("5.0.3");
const QString DlProject::IRGA_SW_VERSION_STRING_2 = QStringLiteral("6.0.0");
const QString DlProject::IRGA_SW_VERSION_STRING_3 = QStringLiteral("6.4.0");

const QString DlProject::VARIABLE_VAR_STRING_0 = QStringLiteral("u");
const QString DlProject::VARIABLE_VAR_STRING_1 = QStringLiteral("v");
const QString DlProject::VARIABLE_VAR_STRING_2 = QStringLiteral("w");
const QString DlProject::VARIABLE_VAR_STRING_3 = QStringLiteral("ts");
const QString DlProject::VARIABLE_VAR_STRING_4 = QStringLiteral("sos");
const QString DlProject::VARIABLE_VAR_STRING_5 = QStringLiteral("co2");
const QString DlProject::VARIABLE_VAR_STRING_6 = QStringLiteral("h2o");
const QString DlProject::VARIABLE_VAR_STRING_7 = QStringLiteral("ch4");
const QString DlProject::VARIABLE_VAR_STRING_8 = QStringLiteral("n2o");
const QString DlProject::VARIABLE_VAR_STRING_9 = QStringLiteral("int_t_1");
const QString DlProject::VARIABLE_VAR_STRING_10 = QStringLiteral("int_t_2");
const QString DlProject::VARIABLE_VAR_STRING_11 = QStringLiteral("int_p");
const QString DlProject::VARIABLE_VAR_STRING_12 = QStringLiteral("air_t");
const QString DlProject::VARIABLE_VAR_STRING_13 = QStringLiteral("air_p");
const QString DlProject::VARIABLE_VAR_STRING_14 = QStringLiteral("ignore");
const QString DlProject::VARIABLE_VAR_STRING_15 = QStringLiteral("cell_t");
const QString DlProject::VARIABLE_VAR_STRING_16 = QStringLiteral("v");
const QString DlProject::VARIABLE_VAR_STRING_17 = QStringLiteral("u");
const QString DlProject::VARIABLE_VAR_STRING_18 = QStringLiteral("not_numeric");
const QString DlProject::VARIABLE_VAR_STRING_19 = QStringLiteral("co");
const QString DlProject::VARIABLE_VAR_STRING_20 = QStringLiteral("so2");
const QString DlProject::VARIABLE_VAR_STRING_21 = QStringLiteral("o3");
const QString DlProject::VARIABLE_VAR_STRING_22 = QStringLiteral("nh3");
const QString DlProject::VARIABLE_VAR_STRING_23 = QStringLiteral("no");
const QString DlProject::VARIABLE_VAR_STRING_24 = QStringLiteral("no2");
const QString DlProject::VARIABLE_VAR_STRING_25 = QStringLiteral("diag_75");
const QString DlProject::VARIABLE_VAR_STRING_26 = QStringLiteral("diag_72");
const QString DlProject::VARIABLE_VAR_STRING_27 = QStringLiteral("diag_77");
const QString DlProject::VARIABLE_VAR_STRING_28 = QStringLiteral("fast_t");
const QString DlProject::VARIABLE_VAR_STRING_29 = QStringLiteral("flowrate");
const QString DlProject::VARIABLE_VAR_STRING_30 = QStringLiteral("anemometer_diagnostic");

const QString DlProject::VARIABLE_MEASURE_TYPE_STRING_0 = QStringLiteral("molar_density");
const QString DlProject::VARIABLE_MEASURE_TYPE_STRING_1 = QStringLiteral("mole_fraction");
const QString DlProject::VARIABLE_MEASURE_TYPE_STRING_2 = QStringLiteral("mixing_ratio");
const QString DlProject::VARIABLE_MEASURE_TYPE_STRING_3 = QStringLiteral("other");

const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_0 = QStringLiteral("mvolt");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_1 = QStringLiteral("volt");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_2 = QStringLiteral("mm_sec");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_3 = QStringLiteral("cm_sec");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_4 = QStringLiteral("m_sec");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_5 = QStringLiteral("kelvin");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_6 = QStringLiteral("ckelvin");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_7 = QStringLiteral("celsius");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_8 = QStringLiteral("ccelsius");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_9 = QStringLiteral("ppt");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_10 = QStringLiteral("ppm");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_11 = QStringLiteral("ppb");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_12 = QStringLiteral("mmol_m3");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_13 = QStringLiteral("umol_m3");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_14 = QStringLiteral("pa");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_15 = QStringLiteral("hpa");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_16 = QStringLiteral("kpa");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_17 = QStringLiteral("dimensionless");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_18 = QStringLiteral("none");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_19 = QStringLiteral("g_m3");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_20 = QStringLiteral("mg_m3");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_21 = QStringLiteral("ug_m3");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_22 = QStringLiteral("lit_m");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_23 = QStringLiteral("m3_s");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_24 = QStringLiteral("cm3_s");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_25 = QStringLiteral("ft3_s");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_26 = QStringLiteral("in3_s");
const QString DlProject::VARIABLE_MEASURE_UNIT_STRING_27 = QStringLiteral("degrees");

const QString DlProject::VARIABLE_CONVERSION_TYPE_STRING_0 = QStringLiteral("zero_fullscale");
const QString DlProject::VARIABLE_CONVERSION_TYPE_STRING_1 = QStringLiteral("gain_offset");
const QString DlProject::VARIABLE_CONVERSION_TYPE_STRING_2 = QStringLiteral("none");

const QString DlProject::getANEM_MODEL_STRING_0()
{
    static const QString s(QStringLiteral("csat3"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_1()
{
    static const QString s(QStringLiteral("hs_50"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_2()
{
    static const QString s(QStringLiteral("hs_100"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_3()
{
    static const QString s(QStringLiteral("r2"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_4()
{
    static const QString s(QStringLiteral("r3_50"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_5()
{
    static const QString s(QStringLiteral("r3_100"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_6()
{
    static const QString s(QStringLiteral("r3a_100"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_7()
{
    static const QString s(QStringLiteral("wm"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_8()
{
    static const QString s(QStringLiteral("wmpro"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_9()
{
    static const QString s(QStringLiteral("usa1_standard"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_10()
{
    static const QString s(QStringLiteral("usa1_fast"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_11()
{
    static const QString s(QStringLiteral("81000"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_12()
{
    static const QString s(QStringLiteral("generic_sonic"));
    return s;
}

const QString DlProject::getANEM_MODEL_STRING_13()
{
    static const QString s(QStringLiteral("csat3b"));
    return s;
}

DlProject::DlProject(QObject* parent) :
    QObject(parent),
    modified_(false),
    project_state_(ProjectState()),
    project_config_state_(ProjConfigState()),
    isFastTempAvailable_(false)
{ ; }

DlProject::DlProject(QObject* parent, const ProjConfigState &project_config) :
    QObject(parent),
    modified_(false),
    project_state_(ProjectState()),
    project_config_state_(project_config),
    isFastTempAvailable_(false)
{ ; }

DlProject::DlProject(const DlProject& project)
    : QObject(0),
      modified_(project.modified_),
      project_state_(project.project_state_),
      project_config_state_(project.project_config_state_),
      isFastTempAvailable_(false)
{ ; }

DlProject& DlProject::operator=(const DlProject& project)
{
    if (this != &project)
    {
        modified_ = project.modified_;
        project_state_ = project.project_state_;
        project_config_state_ = project.project_config_state_;
        isFastTempAvailable_ = project.isFastTempAvailable_;
    }
    return *this;
}

DlProject::~DlProject()
{ ; }

// Clears project (new document)
void DlProject::newProject(const ProjConfigState& project_config)
{
    DEBUG_FUNC_NAME

    // data e ora in formato ISO
    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString(Qt::ISODate);

    // update project configuration
    project_config_state_ = project_config;
    isFastTempAvailable_ = false;

    project_state_.general.sw_version = Defs::APP_VERSION_STR;
    project_state_.general.ini_version = Defs::METADATA_FILE_VERSION_STR;
    project_state_.general.creation_date = now_str;
    project_state_.general.last_change_date.clear();
    project_state_.general.start_date.clear();
    project_state_.general.end_date.clear();
    project_state_.general.file_name.clear();
    project_state_.general.title.clear();
    project_state_.general.id.clear();

    if (project_config_state_.default_data_path.isEmpty())
    {
        project_state_.files.data_path.clear();
    }
    else
    {
        project_state_.files.data_path = project_config_state_.default_data_path;
    }
    project_state_.files.saved_native = false;
    project_state_.files.timestamp = false;
    project_state_.files.isoformat = false;
    project_state_.files.end_of_line = QStringLiteral("lf");
    project_state_.files.enable_processing = true;
    project_state_.files.tstamp_end = 0;

    project_state_.site.site_name.clear();
    project_state_.site.site_id.clear();
    project_state_.site.altitude = 0.0;
    project_state_.site.latitude = 0.0;
    project_state_.site.longitude = 0.0;
    project_state_.site.canopy_height = 0.0;
    project_state_.site.displacement_height = 0.0;
    project_state_.site.roughness_length = 0.0;

    project_state_.station.name.clear();
    project_state_.station.id.clear();

    project_state_.timing.acquisition_frequency = 10.0;
    project_state_.timing.file_duration = 30;
    project_state_.timing.pc_time_settings = QStringLiteral("local");

    project_state_.anemometerList.clear();
    // add one predefined anememometer as master
    AnemDesc anem;
    anem.setManufacturer(QString());
    anem.setModel(QString());
    anem.setSwVersion(QString());
    anem.setId(QString());
    anem.setHeight(0.1);
    anem.setWindFormat(AnemDesc::getANEM_WIND_FORMAT_STRING_0());
    anem.setNorthAlignment(QString());
    anem.setNorthOffset(0.0);
    anem.setNSeparation(0.0);
    anem.setESeparation(0.0);
    anem.setVSeparation(0.0);
    anem.setVPathLength(1.0);
    anem.setHPathLength(1.0);
    anem.setTau(0.1);
    addAnemometer(anem);

    project_state_.irgaList.clear();
    // add one predefined irga
    IrgaDesc irga;
    irga.setManufacturer(IrgaDesc::getIRGA_MANUFACTURER_STRING_0());
    irga.setModel(QString());
    irga.setSwVersion(QString());
    irga.setId(QString());
    irga.setTubeLength(0.0);
    irga.setTubeDiameter(0.0);
    irga.setTubeFlowRate(0.0);
    irga.setTubeNSeparation(0.0);
    irga.setTubeESeparation(0.0);
    irga.setTubeVSeparation(0.0);
    irga.setVPathLength(1.0);
    irga.setHPathLength(1.0);
    irga.setTau(0.1);
    irga.setKWater(0.15);
    irga.setKOxygen(0.0085);
    addIrga(irga);

    project_state_.varDesc.separator.clear();
    project_state_.varDesc.header_rows = -1;
    project_state_.varDesc.data_label = tr("Not set");

    project_state_.variableList.clear();

    // add one predefined variable
    VariableDesc var;
    var.setIgnore(QStringLiteral("no"));
    var.setNumeric(QStringLiteral("yes"));
    var.setVariable(QString());
    var.setInstrument(QString());
    var.setMeasureType(QString());
    var.setInputUnit(QString());
    var.setMinValue(0.0);
    var.setMaxValue(0.0);
    var.setConversionType(QString());
    var.setOutputUnit(QString());
    var.setAValue(1.0);
    var.setBValue(0.0);
    var.setNomTimelag(0.0);
    var.setMinTimelag(0.0);
    var.setMaxTimelag(0.0);
    addVariable(var);

    setModified(false); // new documents are not in a modified state
    emit projectNew();
}

// Load a project. Assumes file has been checked with nativeFormat()
bool DlProject::loadProject(const QString& filename, bool checkVersion, bool *modified, bool firstReading)
{
    DEBUG_FUNC_NAME

    auto parent = static_cast<MainWindow*>(this->parent());
    if (parent == nullptr) { return false; }

    bool isVersionCompatible = true;
    bool alreadyChecked = false;

    qDebug() << "initial isVersionCompatible:" << isVersionCompatible;
    qDebug() << "initial modified:" << *modified;
    qDebug() << "checkVersion:" << checkVersion;
    qDebug() << "firstReading:" << firstReading;

    // open file
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open [loadProject()]" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Metadata Error"),
                             tr("Cannot read file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename)
                             .arg(datafile.errorString()));
        return false;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString(Qt::ISODate);

    QSettings project_ini(filename, QSettings::IniFormat);

    // in case of old non existing file name, use the current existing
    QString projectFilename = project_ini.value(DlIni::INI_GENE_FILE_NAME, QString()).toString();
    if (!FileUtils::existsPath(projectFilename))
    {
        projectFilename = filename;
    }
    qDebug() << "projectFilename" << projectFilename;

    // general section
    project_ini.beginGroup(DlIni::INIGROUP_PROJECT);
    project_state_.general.title = project_ini.value(DlIni::INI_GENE_TITLE, QString()).toString();
        project_state_.general.id = project_ini.value(DlIni::INI_GENE_ID, QString()).toString();
        project_state_.general.creation_date = project_ini.value(DlIni::INI_GENE_CREATION_DATE, now_str).toString();
        project_state_.general.last_change_date = project_ini.value(DlIni::INI_GENE_LAST_CHANGE_DATE, QString()).toString();
        project_state_.general.start_date = project_ini.value(DlIni::INI_GENE_START_DATE, QString()).toString();
        project_state_.general.end_date = project_ini.value(DlIni::INI_GENE_END_DATE, QString()).toString();
        project_state_.general.file_name = projectFilename;
        project_state_.general.sw_version = project_ini.value(DlIni::INI_GENE_SW_VERSION, Defs::APP_VERSION_STR).toString();
        project_state_.general.ini_version = project_ini.value(DlIni::INI_GENE_INI_VERSION, Defs::METADATA_FILE_VERSION_STR).toString();
    project_ini.endGroup();

    // files section
    project_ini.beginGroup(DlIni::INIGROUP_FILES);
        // test if datapath still exists
        QString datapath = project_ini.value(DlIni::INI_FILE_DATA_PATH, QString()).toString();
        if (FileUtils::existsPath(datapath))
        {
            project_state_.files.data_path = datapath;
        }
        else
        {
            project_state_.files.data_path.clear();
        }
        project_state_.files.saved_native = project_ini.value(DlIni::INI_FILE_SAVED_NATIVE, false).toBool();
        project_state_.files.timestamp = project_ini.value(DlIni::INI_FILE_TIMESTAMP, false).toBool();
        project_state_.files.isoformat = project_ini.value(DlIni::INI_FILE_ISOFORMAT, false).toBool();
        project_state_.files.enable_processing = project_ini.value(DlIni::INI_FILE_ENABLE_PROC, 1).toInt();
        project_state_.files.end_of_line = project_ini.value(DlIni::INI_FILE_END_OF_LINE, QStringLiteral("lf")).toString();
        project_state_.files.tstamp_end = project_ini.value(DlIni::INI_FILE_TIMESTAMP_END, 0).toInt();
    project_ini.endGroup();

    // site section
    project_ini.beginGroup(DlIni::INIGROUP_SITE);
        project_state_.site.site_name = project_ini.value(DlIni::INI_SITE_SITE_NAME, QString()).toString();
        project_state_.site.site_id = project_ini.value(DlIni::INI_SITE_SITE_ID, QString()).toString();
        project_state_.site.altitude = project_ini.value(DlIni::INI_SITE_ALTITUDE, 0.0).toDouble();
        project_state_.site.latitude = project_ini.value(DlIni::INI_SITE_LATITUDE, 0.0).toDouble();
        project_state_.site.longitude = project_ini.value(DlIni::INI_SITE_LONGITUDE, 0.0).toDouble();
        project_state_.site.canopy_height = project_ini.value(DlIni::INI_SITE_CANOPY_HEIGHT, 0.0).toDouble();
        project_state_.site.displacement_height = project_ini.value(DlIni::INI_SITE_DISPLACEMENT_HEIGHT, 0.0).toDouble();
        project_state_.site.roughness_length = project_ini.value(DlIni::INI_SITE_ROUGHNESS_LENGTH, 0.0).toDouble();
    project_ini.endGroup();

    // station section
    project_ini.beginGroup(DlIni::INIGROUP_STATION);
        project_state_.station.name = project_ini.value(DlIni::INI_STATION_NAME, QString()).toString();
        project_state_.station.id = project_ini.value(DlIni::INI_STATION_ID, QString()).toString();
    project_ini.endGroup();

    // timing section
    project_ini.beginGroup(DlIni::INIGROUP_TIMING);
        project_state_.timing.acquisition_frequency = project_ini.value(DlIni::INI_TIME_ACQUISITION_FREQ, 10.0).toDouble();
        project_state_.timing.file_duration = project_ini.value(DlIni::INI_TIME_FILE_DURATION, 30).toInt();
        project_state_.timing.pc_time_settings = project_ini.value(DlIni::INI_TIME_PC_TIME_SET, QString()).toString();
    project_ini.endGroup();

    // instruments section
    project_state_.anemometerList.clear();
    project_state_.irgaList.clear();
    project_ini.beginGroup(DlIni::INIGROUP_INSTRUMENTS);
        // iterate through instrument list
        int numInstr = countInstruments(project_ini.allKeys());

        qDebug() << "number of instrument detected:" << numInstr;

        for (int k = 0; k < numInstr; ++k)
        {
            QString prefix = DlIni::INI_INSTR_PREFIX + QString::number(k + 1) + QStringLiteral("_");

            InstrumentType instrType = getInstrumentType(project_ini, prefix);

            qDebug() << "instrType" << static_cast<int>(instrType);
            // anem case
            if (instrType == InstrumentType::ANEM)
            {
                qDebug() << "anem k" << k;
                AnemDesc anem;
                QString anemModel = project_ini.value(prefix + DlIni::INI_ANEM_2).toString().remove(QRegularExpression(QStringLiteral("_\\d*$")));

                anem.setManufacturer(fromIniAnemManufacturer(project_ini.value(prefix + DlIni::INI_ANEM_1, QString()).toString()));
                anem.setModel(fromIniAnemModel(anemModel));
                anem.setSwVersion(project_ini.value(prefix + DlIni::INI_ANEM_16, QString()).toString().trimmed());
                anem.setId(project_ini.value(prefix + DlIni::INI_ANEM_4, QString()).toString());

                qreal heightVal = project_ini.value(prefix + DlIni::INI_ANEM_5, 0.1).toReal();
                qDebug() << "heightVal" << heightVal;
                if (heightVal >= 0.1)
                {
                    anem.setHeight(heightVal);
                }
                else
                {
                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }
                    anem.setHeight(0.1);
                    isVersionCompatible = false;
                    qDebug() << "anem isVersionCompatible false: height < 0.01";
                }

                anem.setWindFormat(fromIniAnemWindFormat(project_ini.value(prefix + DlIni::INI_ANEM_6, ANEM_WIND_FORMAT_STRING_0).toString()));
                anem.setNorthAlignment(fromIniAnemNorthAlign(anemModel, project_ini.value(prefix + DlIni::INI_ANEM_7, QString()).toString()));
                anem.setNorthOffset(project_ini.value(prefix + DlIni::INI_ANEM_8, 0.0).toReal());
                anem.setNSeparation(project_ini.value(prefix + DlIni::INI_ANEM_10, 0.0).toReal());
                anem.setESeparation(project_ini.value(prefix + DlIni::INI_ANEM_11, 0.0).toReal());
                anem.setVSeparation(project_ini.value(prefix + DlIni::INI_ANEM_12, 0.0).toReal());
                anem.setVPathLength(project_ini.value(prefix + DlIni::INI_ANEM_14, 1.0).toReal());
                anem.setHPathLength(project_ini.value(prefix + DlIni::INI_ANEM_13, 1.0).toReal());
                anem.setTau(project_ini.value(prefix + DlIni::INI_ANEM_15, 0.1).toReal());
                addAnemometer(anem);
            }
            // irga case
            else if (instrType == InstrumentType::IRGA)
            {
                qDebug() << "irga k" << k;
                IrgaDesc irga;
                QString irgaModel = project_ini.value(prefix + DlIni::INI_IRGA_1).toString().remove(QRegularExpression(QStringLiteral("_\\d*$")));

                irga.setManufacturer(fromIniIrgaManufacturer(project_ini.value(prefix + DlIni::INI_IRGA_0, QString()).toString()));
                irga.setModel(fromIniIrgaModel(irgaModel));

                // sw version
                auto sw_version_loading = project_ini.value(prefix + DlIni::INI_IRGA_16, QString()).toString();
                qDebug() << "sw_version_loading" << sw_version_loading;
                auto ini_sw_version = StringUtils::getVersionFromString(project_state_.general.ini_version);
                qDebug() << "ini_sw_version" << ini_sw_version << QT_VERSION_CHECK(3, 1, 0);
                if (ini_sw_version <= QT_VERSION_CHECK(3, 1, 0))
                {
                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }

                    // map previous fixed values version to current free field version
                    if (sw_version_loading == QStringLiteral("0.0.0"))
                    {
                        sw_version_loading.clear();
                    }
                    else if (sw_version_loading == QStringLiteral("5.3.0"))
                    {
                        sw_version_loading = QStringLiteral("0.0.0");
                    }
                    else if (sw_version_loading == QStringLiteral("6.0.0"))
                    {
                        sw_version_loading = QStringLiteral("6.0.0");
                    }
                    else if (sw_version_loading == QStringLiteral("6.4.0"))
                    {
                        sw_version_loading = QStringLiteral("6.5.0");
                    }
                    isVersionCompatible = false;
                    qDebug() << "anem isVersionCompatible false: " << "sw_version_loading:" << sw_version_loading;
                }
                irga.setSwVersion(sw_version_loading);

                irga.setId(project_ini.value(prefix + DlIni::INI_IRGA_3, QString()).toString());
                irga.setTubeLength(project_ini.value(prefix + DlIni::INI_IRGA_5, 0.0).toReal());
                irga.setTubeDiameter(project_ini.value(prefix + DlIni::INI_IRGA_6, 0.0).toReal());
                irga.setTubeFlowRate(project_ini.value(prefix + DlIni::INI_IRGA_7, 0.0).toReal());
                irga.setTubeNSeparation(project_ini.value(prefix + DlIni::INI_IRGA_8, 0.0).toReal());
                irga.setTubeESeparation(project_ini.value(prefix + DlIni::INI_IRGA_9, 0.0).toReal());
                irga.setTubeVSeparation(project_ini.value(prefix + DlIni::INI_IRGA_10, 0.0).toReal());
                irga.setHPathLength(project_ini.value(prefix + DlIni::INI_IRGA_11, 1.0).toReal());
                irga.setVPathLength(project_ini.value(prefix + DlIni::INI_IRGA_12, 1.0).toReal());
                irga.setTau(project_ini.value(prefix + DlIni::INI_IRGA_13, 0.1).toReal());
                irga.setKWater(project_ini.value(prefix + DlIni::INI_IRGA_14, 0.15).toReal());
                irga.setKOxygen(project_ini.value(prefix + DlIni::INI_IRGA_15, 0.0085).toReal());
                addIrga(irga);
            }
        }
    project_ini.endGroup();

    project_state_.variableList.clear();
    // variables section
    project_ini.beginGroup(DlIni::INIGROUP_VARDESC);
    project_state_.varDesc.separator = project_ini.value(DlIni::INI_VARDESC_FIELDSEP, QString()).toString();
        project_state_.varDesc.header_rows = project_ini.value(DlIni::INI_VARDESC_HEADER_ROWS, -1).toInt();
        project_state_.varDesc.data_label = project_ini.value(DlIni::INI_VARDESC_DATA_LABEL, tr("Not set")).toString();

        // count the number of variables described
        int numVar = project_ini.allKeys().filter(DlIni::INI_VARDESC_VAR).size();

        qDebug() << "numVar" << numVar;

        // iterate through variables list
        for (int k = 0; k < numVar; ++k)
        {
            QString prefix = DlIni::INI_VARDESC_PREFIX + QString::number(k + 1) + QStringLiteral("_");
            VariableDesc var;
            QString varStr = project_ini.value(prefix + DlIni::INI_VARDESC_VAR, QString()).toString();

            // ignore yes if (var == 'ignore' || var == 'not_numeric')
            var.setIgnore(StringUtils::fromBool2YesNoString((varStr == VARIABLE_VAR_STRING_14)
                          || (varStr == VARIABLE_VAR_STRING_18)));

            // numeric yes if (var != 'not_numeric')
            var.setNumeric(StringUtils::fromBool2YesNoString(varStr != VARIABLE_VAR_STRING_18));

            // std numeric var
            if (var.ignore() == QLatin1String("no") && var.numeric() == QLatin1String("yes"))
            {
                var.setVariable(fromIniVariableVar(varStr));
            }

            var.setInstrument(fromIniVariableInstrument(project_ini.value(prefix + DlIni::INI_VARDESC_INSTRUMENT, QString()).toString()));

            qDebug() << "varStr" << varStr
                     << VariableDesc::isGasVariable(fromIniVariableVar(varStr))
                     << VariableDesc::isCustomVariable(fromIniVariableVar(varStr));
            if ((!VariableDesc::isGasVariable(fromIniVariableVar(varStr))
                && !VariableDesc::isCustomVariable(fromIniVariableVar(varStr)))
                // TODO: temporary solution before introducing a variable
                // for the ignore field
                && varStr != QLatin1String("ignore"))
            {
                if (!project_ini.value(prefix + DlIni::INI_VARDESC_MEASURE_TYPE, QString()).toString().isEmpty())
                {
                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }
                    var.setMeasureType(QString());
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "measure type isVersionCompatible false: to empty string";
                }
            }
            else
            {
                var.setMeasureType(fromIniVariableMeasureType(project_ini.value(prefix + DlIni::INI_VARDESC_MEASURE_TYPE, QString()).toString()));
            }

            var.setInputUnit(fromIniVariableMeasureUnit(project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_IN, QString()).toString()));
            var.setMinValue(project_ini.value(prefix + DlIni::INI_VARDESC_MIN_VALUE, 0.0).toReal());
            var.setMaxValue(project_ini.value(prefix + DlIni::INI_VARDESC_MAX_VALUE, 0.0).toReal());

            // NOTE: backward compatibility change, conversion type
            qDebug() << "var:" << k << "conversion compatibility change; conversion value:" << project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION).toString();
            // if conversion is min-max
            if (project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString() == VARIABLE_CONVERSION_TYPE_STRING_0)
            {
                if (checkVersion && firstReading && !alreadyChecked)
                {
                    if (!parent->queryDlProjectImport())
                    {
                        return false;
                    }
                    alreadyChecked = true;
                }
                else
                {
                    // silently continue file loading and conversion
                }

                qDebug() << "var:" << k << "scaling: backward compatibility mode on";
                var.setConversionType(VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1());

                qreal minValue = project_ini.value(prefix + DlIni::INI_VARDESC_MIN_VALUE).toReal();
                qreal maxValue = project_ini.value(prefix + DlIni::INI_VARDESC_MAX_VALUE).toReal();
                qreal minMaxValue = maxValue - minValue;

                qDebug() << "minMaxValue" << minMaxValue;
                if (minMaxValue == 0.0)
                {
                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }
                    minMaxValue = 0.000001;
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "minMaxValue isVersionCompatible false: 0.0 -> 0.000001";
                }

                qreal aValue = (project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE).toReal()
                                - project_ini.value(prefix + DlIni::INI_VARDESC_A_VALUE).toReal()) /
                                minMaxValue;

                qreal bValue = (project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE).toReal()
                                    * project_ini.value(prefix + DlIni::INI_VARDESC_MAX_VALUE).toReal()
                                - maxValue * minValue) / minMaxValue;

                var.setOutputUnit(fromIniVariableMeasureUnit(project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_OUT, QString()).toString()));
                var.setAValue(aValue);
                var.setBValue(bValue);
//                isVersionCompatible = false;
                qDebug() << "var:" << k << "var isVersionCompatible false: scaling zero_fullscale";


                // if diagnostic variable, set conversion to empty
                if (VariableDesc::isDiagnosticVar(fromIniVariableVar(varStr)))
                {
                    qDebug() << "diagnostic variable";
                    var.setConversionType(QString());
                    var.setOutputUnit(QString());
                    var.setAValue(aValue);
                    var.setBValue(project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE, 0.0).toReal());
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "aValue isVersionCompatible false: diagnostic variable with gain offset, then set conversion to empty";
                }
            }
            // if conversion is gain-offset
            else if (project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString() == VARIABLE_CONVERSION_TYPE_STRING_1)
            {
//                DEBUG_FUNC_MSG(project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString())
                qreal aValue = project_ini.value(prefix + DlIni::INI_VARDESC_A_VALUE, 1.0).toReal();
                qDebug() << "gain-offset aValue" << aValue;

                if (aValue == 0.0)
                {
                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }
                    aValue = 1.0;
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "aValue isVersionCompatible false: 0.0 -> 1.0";
                }

                // if input unit is empty, set conversion to empty
                if (project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_IN, QString()).toString().isEmpty())
                {
                    qDebug() << "input unit empty";
                    var.setConversionType(QString());
                    var.setOutputUnit(QString());
                    var.setAValue(aValue);
                    var.setBValue(project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE, 0.0).toReal());
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "aValue isVersionCompatible false: input unit is empty, then set conversion to empty";
                }
                else
                {
                    qDebug() << "input unit non empty";
                    var.setConversionType(fromIniVariableConversionType(project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString()));
                    var.setOutputUnit(fromIniVariableMeasureUnit(project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_OUT, QString()).toString()));
                    var.setAValue(aValue);
                    var.setBValue(project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE, 0.0).toReal());
                }

                // if diagnostic variable, set conversion to empty
                if (VariableDesc::isDiagnosticVar(fromIniVariableVar(varStr)))
                {
                    qDebug() << "diagnostic variable";
                    var.setConversionType(QString());
                    var.setOutputUnit(QString());
                    var.setAValue(aValue);
                    var.setBValue(project_ini.value(prefix + DlIni::INI_VARDESC_B_VALUE, 0.0).toReal());
                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "aValue isVersionCompatible false: diagnostic variable with gain offset, then set conversion to empty";
                }
            }
            // if conversion is none or empty
            else if ((project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString() == VARIABLE_CONVERSION_TYPE_STRING_2)
                     || project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString().isEmpty())
            {
                var.setOutputUnit(QString());
                qreal aValue = project_ini.value(prefix + DlIni::INI_VARDESC_A_VALUE, 1.0).toReal();
                qDebug() << "conversion none or empty, aValue" << aValue;
                if (aValue == 0.0)
                {
                    qDebug() << "var:" << k << "queryBeforeMdFileImport";

                    if (checkVersion && firstReading && !alreadyChecked)
                    {
                        if (!parent->queryDlProjectImport())
                        {
                            return false;
                        }
                        alreadyChecked = true;
                    }
                    else
                    {
                        // silently continue file loading and conversion
                    }

                    isVersionCompatible = false;
                    qDebug() << "var:" << k << "aValue isVersionCompatible false: 0.0 -> 1.0";
                }
                else
                {
                    qDebug() << "aValue:" << aValue;
                    qDebug() << "crash ?!";
                }

                // if input unit is dimensionless or none, then set conversion type to gain-offset
                qDebug() << "input unit is dimensionless or none";
                if (project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_IN, QString()).toString() == VARIABLE_MEASURE_UNIT_STRING_17
                    || project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_IN, QString()).toString() == VARIABLE_MEASURE_UNIT_STRING_18)
                {
                    // exclude diagnostic variables
                    if (!VariableDesc::isDiagnosticVar(fromIniVariableVar(varStr)))
                    {
                        if (checkVersion && firstReading && !alreadyChecked)
                        {
                            if (!parent->queryDlProjectImport())
                            {
                                return false;
                            }
                            alreadyChecked = true;
                        }
                        else
                        {
                            // silently continue file loading and conversion
                        }
                        var.setConversionType(VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1());
                        isVersionCompatible = false;
                        qDebug() << "var:" << k << "conversion type isVersionCompatible false: to gain-offset";
                    }
                }
                // if input unit is empty, set conversion to empty
                else if (project_ini.value(prefix + DlIni::INI_VARDESC_UNIT_IN, QString()).toString().isEmpty())
                {
                    var.setConversionType(QString());
                }
                else
                {
                    var.setConversionType(fromIniVariableConversionType(project_ini.value(prefix + DlIni::INI_VARDESC_CONVERSION, QString()).toString()));
                }

                var.setAValue(1.0);
                var.setBValue(0.0);
            }

            var.setNomTimelag(project_ini.value(prefix + DlIni::INI_VARDESC_NOM_TIMELAG, 0.0).toReal());
            var.setMinTimelag(project_ini.value(prefix + DlIni::INI_VARDESC_MIN_TIMELAG, 0.0).toReal());
            var.setMaxTimelag(project_ini.value(prefix + DlIni::INI_VARDESC_MAX_TIMELAG, 0.0).toReal());
            addVariable(var);
        }
    project_ini.endGroup();

    datafile.close();

    hasGoodWindComponentsAndTemperature();

    // just loaded projects are not modified
    qDebug() << "final isVersionCompatible:" << isVersionCompatible;
    if (!isVersionCompatible && checkVersion && !firstReading)
    {
        setModified(true);
    }
    else
    {
        setModified(false);
    }

    emit projectChanged();

    qDebug() << "final modified 1:" << *modified;
    if (!isVersionCompatible)
    {
        *modified = true;
    }
    qDebug() << "final modified 2:" << *modified;

    return true;
}

// test if the project is in native format
bool DlProject::nativeFormat(const QString& filename)
{
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot open file: doesn't exists (check the path) "
                   << filename;
        WidgetUtils::warning(nullptr,
                             tr("Load Metadata Error"),
                             tr("Cannot read file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename, datafile.errorString()));
        return false;
    }

    // test if the first row of the file start with the correct tag
    // case sensitive for default
    QTextStream in(&datafile);
    QString firstLine;
    in >> firstLine;
    datafile.close();
    if (!firstLine.startsWith(QLatin1String(";ECCOCatch"))
        && !firstLine.startsWith(QLatin1String(";ECO2catch"))
        && !firstLine.startsWith(QLatin1String(";ECO2S_DATALOGGING"))
        && !firstLine.startsWith(QLatin1String(";ECO2S_METADATA"))
        && !firstLine.startsWith(Defs::GHG_MD_INI_TAG)
        && !firstLine.startsWith(Defs::APP_MD_INI_TAG))
    {
        WidgetUtils::warning(nullptr,
                             tr("Load Metadata Error"),
                             tr("Cannot read file <p>%1:</p>\n"
                                "<b>not in %2 native format.</b>").arg(filename, Defs::APP_NAME));
        return false;
    }

    return true;
}

// insert tag for native file identification
bool DlProject::tagProject(const QString& filename)
{
    QFile datafile(filename);
    if (!datafile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        // error opening file
        qWarning() << "Error: Cannot tag file" << filename;
        WidgetUtils::warning(nullptr,
                             tr("Write Metadata Error"),
                             tr("Cannot write file <p>%1:</p>\n<b>%2</b>")
                             .arg(filename)
                             .arg(datafile.errorString()));
        datafile.close();
        return false;
    }

    QString app_tag(Defs::GHG_MD_INI_TAG);
    app_tag += QLatin1String("\n");

    QTextStream out(&datafile);
    QString textfile = out.readAll();
    textfile.prepend(app_tag);
    datafile.resize(0);
    out << textfile;
    datafile.close();

    return true;
}

// Save a project
bool DlProject::saveProject(const QString& filename)
{
    DEBUG_FUNC_NAME

    QDateTime now = QDateTime::currentDateTime();
    QString now_str = now.toString(Qt::ISODate);
    QFileInfo fileinfo = QFileInfo(filename);

    QSettings project_ini(filename, QSettings::IniFormat);

    // general section
    project_ini.beginGroup(DlIni::INIGROUP_PROJECT);
        project_ini.setValue(DlIni::INI_GENE_TITLE, project_state_.general.title);
        project_ini.setValue(DlIni::INI_GENE_ID, project_state_.general.id);
        project_ini.setValue(DlIni::INI_GENE_CREATION_DATE, project_state_.general.creation_date);
        project_ini.setValue(DlIni::INI_GENE_LAST_CHANGE_DATE, now_str);
        project_ini.setValue(DlIni::INI_GENE_START_DATE, project_state_.general.start_date);
        project_ini.setValue(DlIni::INI_GENE_END_DATE, project_state_.general.end_date);
        project_ini.setValue(DlIni::INI_GENE_FILE_NAME, fileinfo.absoluteFilePath());

        // update sw version if empty or old
        if (project_state_.general.sw_version.isEmpty()
            || project_state_.general.sw_version != Defs::APP_VERSION_STR)
        {
            project_ini.setValue(DlIni::INI_GENE_SW_VERSION, Defs::APP_VERSION_STR);
        }
        else
        {
            project_ini.setValue(DlIni::INI_GENE_SW_VERSION, project_state_.general.sw_version);
        }

        // update ini version if empty or old
        if (project_state_.general.ini_version.isEmpty()
            || project_state_.general.ini_version != Defs::METADATA_FILE_VERSION_STR)
        {
            project_ini.setValue(DlIni::INI_GENE_INI_VERSION, Defs::METADATA_FILE_VERSION_STR);
        }
        else
        {
            project_ini.setValue(DlIni::INI_GENE_INI_VERSION, project_state_.general.ini_version);
        }
    project_ini.endGroup();

    // files section
    project_ini.beginGroup(DlIni::INIGROUP_FILES);
        project_ini.setValue(DlIni::INI_FILE_DATA_PATH, QDir::fromNativeSeparators(project_state_.files.data_path));
        project_ini.setValue(DlIni::INI_FILE_SAVED_NATIVE, QVariant(project_state_.files.saved_native).toInt());
        project_ini.setValue(DlIni::INI_FILE_TIMESTAMP, QVariant(project_state_.files.timestamp).toInt());
        project_ini.setValue(DlIni::INI_FILE_ISOFORMAT, QVariant(project_state_.files.isoformat).toInt());
        project_ini.setValue(DlIni::INI_FILE_END_OF_LINE, project_state_.files.end_of_line.toLower());
        project_ini.setValue(DlIni::INI_FILE_ENABLE_PROC, QVariant(project_state_.files.enable_processing).toInt());
        project_ini.setValue(DlIni::INI_FILE_TIMESTAMP_END, QVariant(project_state_.files.tstamp_end).toInt());
    project_ini.endGroup();

    // site section
    project_ini.beginGroup(DlIni::INIGROUP_SITE);
        project_ini.setValue(DlIni::INI_SITE_SITE_NAME, project_state_.site.site_name);
        project_ini.setValue(DlIni::INI_SITE_SITE_ID, project_state_.site.site_id);
        project_ini.setValue(DlIni::INI_SITE_ALTITUDE, QString::number(project_state_.site.altitude, 'f', 1));
        project_ini.setValue(DlIni::INI_SITE_LATITUDE, project_state_.site.latitude);
        project_ini.setValue(DlIni::INI_SITE_LONGITUDE, project_state_.site.longitude);
        project_ini.setValue(DlIni::INI_SITE_CANOPY_HEIGHT, project_state_.site.canopy_height);
        project_ini.setValue(DlIni::INI_SITE_DISPLACEMENT_HEIGHT, project_state_.site.displacement_height);
        project_ini.setValue(DlIni::INI_SITE_ROUGHNESS_LENGTH, project_state_.site.roughness_length);
    project_ini.endGroup();

    // station section
    project_ini.beginGroup(DlIni::INIGROUP_STATION);
        project_ini.setValue(DlIni::INI_STATION_NAME, project_state_.station.name);
        project_ini.setValue(DlIni::INI_STATION_ID, project_state_.station.id);
    project_ini.endGroup();

    // timing section
    project_ini.beginGroup(DlIni::INIGROUP_TIMING);
        project_ini.setValue(DlIni::INI_TIME_ACQUISITION_FREQ, QString::number(project_state_.timing.acquisition_frequency, 'f', 3));
        project_ini.setValue(DlIni::INI_TIME_FILE_DURATION, project_state_.timing.file_duration);
        project_ini.setValue(DlIni::INI_TIME_PC_TIME_SET, project_state_.timing.pc_time_settings);
    project_ini.endGroup();

    // instruments section
    project_ini.beginGroup(DlIni::INIGROUP_INSTRUMENTS);
        // firstly remove all previous keys because they are variable in number
        project_ini.remove(QString());

        // iterate through the instrument lists
        int k = 0;
        foreach (const AnemDesc& anem, project_state_.anemometerList)
        {
            QString index = QStringLiteral("_") + QString::number(k + 1);
            QString prefix = StringUtils::insertIndex(DlIni::INI_INSTR_PREFIX, 5, index);
            QString anemModel = toIniAnemModel(anem.model());

            project_ini.setValue(prefix + DlIni::INI_ANEM_1,
                                 toIniAnemManufacturer(anem.manufacturer()));
            if (!anemModel.isEmpty())
            {
                project_ini.setValue(prefix + DlIni::INI_ANEM_2,
                                 QString(anemModel + QStringLiteral("_") + QString::number(k + 1)));
            }
            else
            {
                project_ini.setValue(prefix + DlIni::INI_ANEM_2, QString());
            }
            project_ini.setValue(prefix + DlIni::INI_ANEM_16,
                                 anem.swVersion().trimmed());
            project_ini.setValue(prefix + DlIni::INI_ANEM_4,
                                 anem.id());
            project_ini.setValue(prefix + DlIni::INI_ANEM_5,
                                 QString::number(anem.height(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_ANEM_6,
                                 toIniAnemWindFormat(anem.windFormat()));
            project_ini.setValue(prefix + DlIni::INI_ANEM_7,
                                 toIniAnemNorthAlign(anem.northAlignment()));
            project_ini.setValue(prefix + DlIni::INI_ANEM_8,
                                 QString::number(anem.northOffset(), 'f', 1));
            project_ini.setValue(prefix + DlIni::INI_ANEM_10,
                                 QString::number(anem.nSeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_ANEM_11,
                                 QString::number(anem.eSeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_ANEM_12,
                                 QString::number(anem.vSeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_ANEM_14,
                                 QString::number(anem.vPathLength(), 'f', 4));
            project_ini.setValue(prefix + DlIni::INI_ANEM_13,
                                 QString::number(anem.hPathLength(), 'f', 4));
            project_ini.setValue(prefix + DlIni::INI_ANEM_15,
                                 QString::number(anem.tau(), 'f', 4));
            ++k;
        }

        int j = 0;
        foreach (const IrgaDesc& irga, project_state_.irgaList)
        {
            QString index = QStringLiteral("_") + QString::number(k + 1);
            QString prefix = StringUtils::insertIndex(DlIni::INI_INSTR_PREFIX, 5, index);
            QString irgaModel = toIniIrgaModel(irga.model());

            project_ini.setValue(prefix + DlIni::INI_IRGA_0,
                                 toIniIrgaManufacturer(irga.manufacturer()));
            if (!irgaModel.isEmpty())
            {
                project_ini.setValue(prefix + DlIni::INI_IRGA_1,
                                 QString(irgaModel + QLatin1Char('_') + QString::number(j + 1)));
            }
            else
            {
                project_ini.setValue(prefix + DlIni::INI_IRGA_1, QString());
            }

            // irga sw version
            auto irga_sw_version = irga.swVersion();
            // cleanup input mask effects
            if (irga_sw_version == QStringLiteral(".."))
            {
                irga_sw_version.clear();
            }
            if (irga_sw_version.startsWith(QLatin1Char('.')))
            {
                irga_sw_version.prepend(QLatin1Char('0'));
            }
            if (irga_sw_version.endsWith(QLatin1Char('.')))
            {
                irga_sw_version.append(QLatin1Char('0'));
            }
            project_ini.setValue(prefix + DlIni::INI_IRGA_16, irga_sw_version);

            project_ini.setValue(prefix + DlIni::INI_IRGA_3,
                                 irga.id());
//            project_ini.setValue(prefix + DlIni::INI_IRGA_4,
//                                 QString::number(irga.height(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_IRGA_5,
                                 QString::number(irga.tubeLength(), 'f', 1));
            project_ini.setValue(prefix + DlIni::INI_IRGA_6,
                                 QString::number(irga.tubeDiameter(), 'f', 1));
            project_ini.setValue(prefix + DlIni::INI_IRGA_7,
                                 QString::number(irga.tubeFlowRate(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_IRGA_8,
                                 QString::number(irga.tubeNSeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_IRGA_9,
                                 QString::number(irga.tubeESeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_IRGA_10,
                                 QString::number(irga.tubeVSeparation(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_IRGA_12,
                                 QString::number(irga.vPathLength(), 'f', 4));
            project_ini.setValue(prefix + DlIni::INI_IRGA_11,
                                 QString::number(irga.hPathLength(), 'f', 4));
            project_ini.setValue(prefix + DlIni::INI_IRGA_13,
                                 QString::number(irga.tau(), 'f', 4));
            project_ini.setValue(prefix + DlIni::INI_IRGA_14,
                                 QString::number(irga.kWater(), 'f', 6));
            project_ini.setValue(prefix + DlIni::INI_IRGA_15,
                                 QString::number(irga.kOxygen(), 'f', 6));
            ++k;
            ++j;
        }
    project_ini.endGroup();

    // variables section
    project_ini.beginGroup(DlIni::INIGROUP_VARDESC);
        // firstly remove all previous keys because they are variable in number
        project_ini.remove(QString());
        project_ini.setValue(DlIni::INI_VARDESC_FIELDSEP, project_state_.varDesc.separator);
        project_ini.setValue(DlIni::INI_VARDESC_HEADER_ROWS, project_state_.varDesc.header_rows);
        project_ini.setValue(DlIni::INI_VARDESC_DATA_LABEL, project_state_.varDesc.data_label);

        // iterate through instruments list
        k = 0;
        foreach (const VariableDesc& var, project_state_.variableList)
        {
            QString index = QStringLiteral("_") + QString::number(k + 1);
            QString prefix = StringUtils::insertIndex(DlIni::INI_VARDESC_PREFIX, 3, index);

            // std numeric var
            if ((var.ignore() == QLatin1String("no"))
                && (var.numeric() == QLatin1String("yes")))
            {
                project_ini.setValue(prefix + DlIni::INI_VARDESC_VAR,
                                     toIniVariableVar(var.variable()));
            }
            else if ((var.ignore() == QLatin1String("yes"))
                     && (var.numeric() == QLatin1String("yes")))
            {
                // write ignore
                project_ini.setValue(prefix + DlIni::INI_VARDESC_VAR,
                                     toIniVariableVar(VariableDesc::getVARIABLE_VAR_STRING_14()));
            }
            else
            {
                // write not_numeric
                project_ini.setValue(prefix + DlIni::INI_VARDESC_VAR,
                                     toIniVariableVar(VariableDesc::getVARIABLE_VAR_STRING_18()));
            }

            project_ini.setValue(prefix + DlIni::INI_VARDESC_INSTRUMENT,
                                 toIniVariableInstrument(var.instrument()));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_MEASURE_TYPE,
                                 toIniVariableMeasureType(var.measureType()));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_UNIT_IN,
                                 toIniVariableMeasureUnit(var.inputUnit()));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_MIN_VALUE,
                                 QString::number(var.minValue(), 'f', 6));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_MAX_VALUE,
                                 QString::number(var.maxValue(), 'f', 6));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_CONVERSION,
                                 toIniVariableConversionType(var.conversionType()));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_UNIT_OUT,
                                 toIniVariableMeasureUnit(var.outputUnit()));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_A_VALUE,
                                 QString::number(var.aValue(), 'f', 6));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_B_VALUE,
                                 QString::number(var.bValue(), 'f', 6));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_NOM_TIMELAG,
                                 QString::number(var.nomTimelag(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_MIN_TIMELAG,
                                 QString::number(var.minTimelag(), 'f', 2));
            project_ini.setValue(prefix + DlIni::INI_VARDESC_MAX_TIMELAG,
                                 QString::number(var.maxTimelag(), 'f', 2));
            ++k;
        }
    project_ini.endGroup();

    // sync and close the ini file
    project_ini.sync();

    // native file tag insertion
    bool result = tagProject(filename);
    if (!result)
    {
        WidgetUtils::warning(nullptr,
                             tr("Write Metadata Error"),
                             tr("Unable to tag project file!"));
    }

    hasGoodWindComponentsAndTemperature();

    // project is saved, so set flags accordingly
    setModified(false);

    return true;
}

bool DlProject::modified() const
{
    return modified_;
}

void DlProject::setModified(bool mod)
{
    DEBUG_FUNC_NAME
    qDebug() << "mod" << mod;
    modified_ = mod;
    if (mod)
    {
        emit projectModified();
    }
}

// Add an anemometer to the project
void DlProject::addAnemometer(const AnemDesc& anem)
{
    project_state_.anemometerList.append(anem);
}

// Add an irga to the project
void DlProject::addIrga(const IrgaDesc& irga)
{
    project_state_.irgaList.append(irga);
}

// Add a variable to the project
void DlProject::addVariable(const VariableDesc& var)
{
    project_state_.variableList.append(var);
}

QString DlProject::toIniAnemManufacturer(const QString& s)
{
    if (s == AnemDesc::getANEM_MANUFACTURER_STRING_0())
    {
        return DlProject::ANEM_MANUFACTURER_STRING_0;
    }
    else if (s == AnemDesc::getANEM_MANUFACTURER_STRING_1())
    {
        return DlProject::ANEM_MANUFACTURER_STRING_1;
    }
    else if (s == AnemDesc::getANEM_MANUFACTURER_STRING_2())
    {
        return DlProject::ANEM_MANUFACTURER_STRING_2;
    }
    else if (s == AnemDesc::getANEM_MANUFACTURER_STRING_3())
    {
        return DlProject::ANEM_MANUFACTURER_STRING_3;
    }
    else if (s == AnemDesc::getANEM_MANUFACTURER_STRING_4())
    {
        return DlProject::ANEM_MANUFACTURER_STRING_4;
    }
    else
    {
        return QString();
    }
}

const QString DlProject::toIniAnemModel(const QString& s)
{
    if (s == AnemDesc::getANEM_MODEL_STRING_0())
    {
        return DlProject::getANEM_MODEL_STRING_0();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_1())
    {
        return DlProject::getANEM_MODEL_STRING_1();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_2())
    {
        return DlProject::getANEM_MODEL_STRING_2();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_3())
    {
        return DlProject::getANEM_MODEL_STRING_3();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_4())
    {
        return DlProject::getANEM_MODEL_STRING_4();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_5())
    {
        return DlProject::getANEM_MODEL_STRING_5();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_6())
    {
        return DlProject::getANEM_MODEL_STRING_6();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_7())
    {
        return DlProject::getANEM_MODEL_STRING_7();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_8())
    {
        return DlProject::getANEM_MODEL_STRING_8();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_9())
    {
        return DlProject::getANEM_MODEL_STRING_9();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_10())
    {
        return DlProject::getANEM_MODEL_STRING_10();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_11())
    {
        return DlProject::getANEM_MODEL_STRING_11();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_12())
    {
        return DlProject::getANEM_MODEL_STRING_12();
    }
    else if (s == AnemDesc::getANEM_MODEL_STRING_13())
    {
        return DlProject::getANEM_MODEL_STRING_13();
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniAnemWindFormat(const QString& s)
{
    if (s == AnemDesc::getANEM_WIND_FORMAT_STRING_0())
    {
        return DlProject::ANEM_WIND_FORMAT_STRING_0;
    }
    else if (s == AnemDesc::getANEM_WIND_FORMAT_STRING_1())
    {
        return DlProject::ANEM_WIND_FORMAT_STRING_1;
    }
    else if (s == AnemDesc::getANEM_WIND_FORMAT_STRING_2())
    {
        return DlProject::ANEM_WIND_FORMAT_STRING_2;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniAnemNorthAlign(const QString& s)
{
    if (s == AnemDesc::getANEM_NORTH_ALIGN_STRING_0())
    {
        return DlProject::ANEM_NORTH_ALIGN_STRING_0;
    }
    else if (s == AnemDesc::getANEM_NORTH_ALIGN_STRING_1())
    {
        return DlProject::ANEM_NORTH_ALIGN_STRING_1;
    }
    else if (s == AnemDesc::getANEM_NORTH_ALIGN_STRING_2())
    {
        return DlProject::ANEM_NORTH_ALIGN_STRING_2;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniVariableVar(const QString& s)
{
    if (s == VariableDesc::getVARIABLE_VAR_STRING_0())
    {
        return DlProject::VARIABLE_VAR_STRING_0;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_1())
    {
        return DlProject::VARIABLE_VAR_STRING_1;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_2())
    {
        return DlProject::VARIABLE_VAR_STRING_2;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_3())
    {
        return DlProject::VARIABLE_VAR_STRING_3;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_4())
    {
        return DlProject::VARIABLE_VAR_STRING_4;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_5())
    {
        return DlProject::VARIABLE_VAR_STRING_5;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_6())
    {
        return DlProject::VARIABLE_VAR_STRING_6;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_7())
    {
        return DlProject::VARIABLE_VAR_STRING_7;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_8())
    {
        return DlProject::VARIABLE_VAR_STRING_8;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_9())
    {
        return DlProject::VARIABLE_VAR_STRING_9;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_10())
    {
        return DlProject::VARIABLE_VAR_STRING_10;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_11())
    {
        return DlProject::VARIABLE_VAR_STRING_11;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_12())
    {
        return DlProject::VARIABLE_VAR_STRING_12;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_13())
    {
        return DlProject::VARIABLE_VAR_STRING_13;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_14())
    {
        return DlProject::VARIABLE_VAR_STRING_14;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_15())
    {
        return DlProject::VARIABLE_VAR_STRING_15;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_16())
    {
        return DlProject::VARIABLE_VAR_STRING_16;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_17())
    {
        return DlProject::VARIABLE_VAR_STRING_17;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_18())
    {
        return DlProject::VARIABLE_VAR_STRING_18;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_19())
    {
        return DlProject::VARIABLE_VAR_STRING_19;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_20())
    {
        return DlProject::VARIABLE_VAR_STRING_20;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_21())
    {
        return DlProject::VARIABLE_VAR_STRING_21;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_22())
    {
        return DlProject::VARIABLE_VAR_STRING_22;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_23())
    {
        return DlProject::VARIABLE_VAR_STRING_23;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_24())
    {
        return DlProject::VARIABLE_VAR_STRING_24;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_25())
    {
        return DlProject::VARIABLE_VAR_STRING_25;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_26())
    {
        return DlProject::VARIABLE_VAR_STRING_26;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_27())
    {
        return DlProject::VARIABLE_VAR_STRING_27;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_28())
    {
        return DlProject::VARIABLE_VAR_STRING_28;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_29())
    {
        return DlProject::VARIABLE_VAR_STRING_29;
    }
    else if (s == VariableDesc::getVARIABLE_VAR_STRING_30())
    {
        return DlProject::VARIABLE_VAR_STRING_30;
    }
    else
    {
        return s;
    }
}

const QString DlProject::toIniVariableMeasureType(const QString& s)
{
    if (s == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0())
    {
        return DlProject::VARIABLE_MEASURE_TYPE_STRING_0;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1())
    {
        return DlProject::VARIABLE_MEASURE_TYPE_STRING_1;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2())
    {
        return DlProject::VARIABLE_MEASURE_TYPE_STRING_2;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_3())
    {
        return DlProject::VARIABLE_MEASURE_TYPE_STRING_3;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniVariableMeasureUnit(const QString& s)
{
    if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_0())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_0;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_1())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_1;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_2())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_2;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_3())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_3;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_4())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_4;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_5())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_5;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_6())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_6;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_7())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_7;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_8())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_8;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_9())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_9;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_10())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_10;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_11())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_11;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_12())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_12;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_13())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_13;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_14())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_14;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_15())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_15;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_16())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_16;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_17;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_18())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_18;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_19())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_19;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_20())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_20;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_21())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_21;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_22())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_22;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_23())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_23;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_24())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_24;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_25())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_25;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_26())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_26;
    }
    else if (s == VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_27())
    {
        return DlProject::VARIABLE_MEASURE_UNIT_STRING_27;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniVariableConversionType(const QString& s)
{
    if (s == VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_0())
    {
        return DlProject::VARIABLE_CONVERSION_TYPE_STRING_0;
    }
    else if (s == VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1())
    {
        return DlProject::VARIABLE_CONVERSION_TYPE_STRING_1;
    }
    else if (s == VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_2())
    {
        return DlProject::VARIABLE_CONVERSION_TYPE_STRING_2;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniVariableInstrument(const QString& s)
{
    QString iniInstrumentStr;
    if (!s.isEmpty())
    {
        if (s != QLatin1String("Other"))
        {
            QString numberStr = s.split(QLatin1Char(':')).at(0);
            QString modelStr = s.split(QLatin1Char(':')).at(1);
            QString instrType = numberStr.split(QLatin1Char(' ')).at(0);
            QString number = numberStr.split(QLatin1Char(' ')).at(1);
            QString model;
            if (instrType == tr("Sonic"))
            {
                model = toIniAnemModel(modelStr.trimmed());
            }
            else
            {
                model = toIniIrgaModel(modelStr.trimmed());
            }

            iniInstrumentStr = model + QLatin1Char('_') + number;
        }
        else
        {
            iniInstrumentStr = QStringLiteral("other");
        }
    }
    else
    {
        iniInstrumentStr.clear();
    }
    return iniInstrumentStr;
}

QString DlProject::fromIniVariableInstrument(const QString& s)
{
    DEBUG_FUNC_NAME

    QString tableInstrumentStr;
    if (!s.isEmpty())
    {
        if (s != QStringLiteral("other"))
        {
            int index = s.lastIndexOf(QLatin1Char('_'));
            QString number = s.mid(index + 1);
            QString modelStr = s.left(index);

            InstrumentType instrType = getInstrumentTypeFromModel(modelStr);
            if (instrType == InstrumentType::ANEM)
            {
                tableInstrumentStr = tr("Sonic ") + number + QStringLiteral(": ") + fromIniAnemModel(modelStr);
            }
            else
            {
                tableInstrumentStr = tr("Irga ") + number + QStringLiteral(": ") + fromIniIrgaModel(modelStr);
            }
        }
        else
        {
            tableInstrumentStr = tr("Other");
        }
    }
    else
    {
        tableInstrumentStr.clear();
    }
    return tableInstrumentStr;
}

QString DlProject::fromIniAnemManufacturer(const QString& s)
{
    if (s == DlProject::ANEM_MANUFACTURER_STRING_0)
    {
        return AnemDesc::getANEM_MANUFACTURER_STRING_0();
    }
    else if (s == DlProject::ANEM_MANUFACTURER_STRING_1)
    {
        return AnemDesc::getANEM_MANUFACTURER_STRING_1();
    }
    else if (s == DlProject::ANEM_MANUFACTURER_STRING_2)
    {
        return AnemDesc::getANEM_MANUFACTURER_STRING_2();
    }
    else if (s == DlProject::ANEM_MANUFACTURER_STRING_3)
    {
        return AnemDesc::getANEM_MANUFACTURER_STRING_3();
    }
    else if (s == DlProject::ANEM_MANUFACTURER_STRING_4)
    {
        return AnemDesc::getANEM_MANUFACTURER_STRING_4();
    }
    else
    {
        return QString();
    }
}

const QString DlProject::fromIniAnemModel(const QString& s)
{
    if (s == DlProject::getANEM_MODEL_STRING_0())
    {
        return AnemDesc::getANEM_MODEL_STRING_0();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_1())
    {
        return AnemDesc::getANEM_MODEL_STRING_1();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_2())
    {
        return AnemDesc::getANEM_MODEL_STRING_2();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_3())
    {
        return AnemDesc::getANEM_MODEL_STRING_3();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_4())
    {
        return AnemDesc::getANEM_MODEL_STRING_4();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_5())
    {
        return AnemDesc::getANEM_MODEL_STRING_5();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_6())
    {
        return AnemDesc::getANEM_MODEL_STRING_6();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_7())
    {
        return AnemDesc::getANEM_MODEL_STRING_7();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_8())
    {
        return AnemDesc::getANEM_MODEL_STRING_8();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_9())
    {
        return AnemDesc::getANEM_MODEL_STRING_9();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_10())
    {
        return AnemDesc::getANEM_MODEL_STRING_10();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_11())
    {
        return AnemDesc::getANEM_MODEL_STRING_11();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_12())
    {
        return AnemDesc::getANEM_MODEL_STRING_12();
    }
    else if (s == DlProject::getANEM_MODEL_STRING_13())
    {
        return AnemDesc::getANEM_MODEL_STRING_13();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniAnemWindFormat(const QString& s)
{
    if (s == DlProject::ANEM_WIND_FORMAT_STRING_0)
    {
        return AnemDesc::getANEM_WIND_FORMAT_STRING_0();
    }
    else if (s == DlProject::ANEM_WIND_FORMAT_STRING_1)
    {
        return AnemDesc::getANEM_WIND_FORMAT_STRING_1();
    }
    else if (s == DlProject::ANEM_WIND_FORMAT_STRING_2)
    {
        return AnemDesc::getANEM_WIND_FORMAT_STRING_2();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniAnemNorthAlign(const QString &model, const QString &s)
{
    // axis
    if (s == DlProject::ANEM_NORTH_ALIGN_STRING_0)
    {
        return AnemDesc::getANEM_NORTH_ALIGN_STRING_0();
    }
    // spar
    else if (s == DlProject::ANEM_NORTH_ALIGN_STRING_1)
    {
        // NOTE: hack for bogus 'spar' value with csat/metek/young
        if (model == DlProject::getANEM_MODEL_STRING_0()
            || model == DlProject::getANEM_MODEL_STRING_13())
        {
            return AnemDesc::getANEM_NORTH_ALIGN_STRING_2();
        }
        else
        {
            return AnemDesc::getANEM_NORTH_ALIGN_STRING_1();
        }
    }
    // na
    else if (s == DlProject::ANEM_NORTH_ALIGN_STRING_2)
    {
        return AnemDesc::getANEM_NORTH_ALIGN_STRING_2();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniVariableVar(const QString& s)
{
    if (s == DlProject::VARIABLE_VAR_STRING_0)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_0();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_1)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_1();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_2)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_2();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_3)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_3();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_4)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_4();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_5)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_5();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_6)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_6();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_7)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_7();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_8)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_8();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_9)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_9();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_10)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_10();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_11)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_11();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_12)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_12();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_13)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_13();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_14)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_14();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_15)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_15();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_16)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_16();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_17)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_17();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_18)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_18();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_19)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_19();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_20)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_20();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_21)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_21();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_22)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_22();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_23)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_23();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_24)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_24();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_25)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_25();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_26)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_26();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_27)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_27();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_28)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_28();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_29)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_29();
    }
    else if (s == DlProject::VARIABLE_VAR_STRING_30)
    {
        return VariableDesc::getVARIABLE_VAR_STRING_30();
    }
    else
    {
        return s;
    }
}

const QString DlProject::fromIniVariableMeasureType(const QString& s)
{
    if (s == DlProject::VARIABLE_MEASURE_TYPE_STRING_0)
    {
        return VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0();
    }
    else if (s == DlProject::VARIABLE_MEASURE_TYPE_STRING_1)
    {
        return VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1();
    }
    else if (s == DlProject::VARIABLE_MEASURE_TYPE_STRING_2)
    {
        return VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2();
    }
    else if (s == DlProject::VARIABLE_MEASURE_TYPE_STRING_3)
    {
        return VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_3();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniVariableMeasureUnit(const QString& s)
{
    if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_0)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_0();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_1)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_1();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_2)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_2();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_3)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_3();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_4)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_4();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_5)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_5();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_6)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_6();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_7)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_7();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_8)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_8();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_9)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_9();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_10)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_10();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_11)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_11();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_12)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_12();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_13)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_13();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_14)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_14();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_15)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_15();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_16)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_16();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_17)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_17();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_18)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_18();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_19)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_19();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_20)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_20();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_21)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_21();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_22)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_22();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_23)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_23();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_24)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_24();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_25)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_25();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_26)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_26();
    }
    else if (s == DlProject::VARIABLE_MEASURE_UNIT_STRING_27)
    {
        return VariableDesc::getVARIABLE_MEASURE_UNIT_STRING_27();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniVariableConversionType(const QString& s)
{
    if (s == DlProject::VARIABLE_CONVERSION_TYPE_STRING_0)
    {
        return VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_0();
    }
    else if (s == DlProject::VARIABLE_CONVERSION_TYPE_STRING_1)
    {
        return VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_1();
    }
    else if (s == DlProject::VARIABLE_CONVERSION_TYPE_STRING_2)
    {
        return QString();
    }
    else
    {
        return QString();
    }
}

int DlProject::countInstruments(const QStringList& list)
{
    int i = 0;
    foreach (const QString& s, list)
    {
        if (s.contains(DlIni::INI_ANEM_1))
            ++i;
    }
    return i;
}

DlProject::InstrumentType DlProject::getInstrumentType(const QSettings& iniGroup, const QString& prefix)
{
    if (iniGroup.contains(prefix + DlIni::INI_ANEM_6))
    {
        return InstrumentType::ANEM;
    }
    else if (iniGroup.contains(prefix + DlIni::INI_IRGA_5))
    {
        return InstrumentType::IRGA;
    }
    return InstrumentType::UNDEFINED;
}

DlProject::InstrumentType DlProject::getInstrumentTypeFromModel(const QString& model)
{
    if (model.contains(QRegularExpression(QStringLiteral("li*")))
        || model.contains(QRegularExpression(QStringLiteral("open_path*")))
        || model.contains(QRegularExpression(QStringLiteral("closed_path*")))
        || model.contains(QRegularExpression(QStringLiteral("generic*path"))))
    {
        return InstrumentType::IRGA;
    }
    return InstrumentType::ANEM;
}

QString DlProject::fromIniIrgaManufacturer(const QString& s)
{
    if (s == DlProject::IRGA_MANUFACTURER_STRING_0)
    {
        return IrgaDesc::getIRGA_MANUFACTURER_STRING_0();
    }
    else if (s == DlProject::IRGA_MANUFACTURER_STRING_1)
    {
        return IrgaDesc::getIRGA_MANUFACTURER_STRING_1();
    }
    else
    {
        return QString();
    }
}

QString DlProject::fromIniIrgaModel(const QString& s)
{
    if (s == DlProject::IRGA_MODEL_STRING_0)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_0();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_1)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_1();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_2)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_2();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_3)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_3();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_4)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_4();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_5)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_5();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_6)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_6();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_7)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_7();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_8)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_8();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_9)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_9();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_10)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_10();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_11)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_11();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_12)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_12();
    }
    else if (s == DlProject::IRGA_MODEL_STRING_13)
    {
        return IrgaDesc::getIRGA_MODEL_STRING_13();
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniIrgaManufacturer(const QString& s)
{
    if (s == IrgaDesc::getIRGA_MANUFACTURER_STRING_0())
    {
        return DlProject::IRGA_MANUFACTURER_STRING_0;
    }
    else if (s == IrgaDesc::getIRGA_MANUFACTURER_STRING_1())
    {
        return DlProject::IRGA_MANUFACTURER_STRING_1;
    }
    else
    {
        return QString();
    }
}

QString DlProject::toIniIrgaModel(const QString& s)
{
    if (s == IrgaDesc::getIRGA_MODEL_STRING_0())
    {
        return DlProject::IRGA_MODEL_STRING_0;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_1())
    {
        return DlProject::IRGA_MODEL_STRING_1;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_2())
    {
        return DlProject::IRGA_MODEL_STRING_2;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_3())
    {
        return DlProject::IRGA_MODEL_STRING_3;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_4())
    {
        return DlProject::IRGA_MODEL_STRING_4;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_5())
    {
        return DlProject::IRGA_MODEL_STRING_5;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_6())
    {
        return DlProject::IRGA_MODEL_STRING_6;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_7())
    {
        return DlProject::IRGA_MODEL_STRING_7;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_8())
    {
        return DlProject::IRGA_MODEL_STRING_8;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_9())
    {
        return DlProject::IRGA_MODEL_STRING_9;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_10())
    {
        return DlProject::IRGA_MODEL_STRING_10;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_11())
    {
        return DlProject::IRGA_MODEL_STRING_11;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_12())
    {
        return DlProject::IRGA_MODEL_STRING_12;
    }
    else if (s == IrgaDesc::getIRGA_MODEL_STRING_13())
    {
        return DlProject::IRGA_MODEL_STRING_13;
    }
    else
    {
        return QString();
    }
}

bool DlProject::hasOneFastTemperature()
{
    return isFastTempAvailable_;
}

bool DlProject::hasGoodIrgaNames()
{
    DEBUG_FUNC_NAME

    auto test = true;
    foreach (const IrgaDesc& irga, project_state_.irgaList)
    {
        test &= IrgaDesc::isWellNamed(irga);
    }
    return test;
}

bool DlProject::hasGoodIrgaSeparations()
{
    DEBUG_FUNC_NAME

    auto test = true;
    foreach (const IrgaDesc& irga, project_state_.irgaList)
    {
        test &= IrgaDesc::hasGoodSeparations(irga);
    }
    return test;
}

bool DlProject::hasGoodIrgaClosedPath()
{
    DEBUG_FUNC_NAME

    auto test = true;
    foreach (const IrgaDesc& irga, project_state_.irgaList)
    {
        test &= IrgaDesc::isAGoodClosedPath(irga);
    }
    return test;
}

bool DlProject::hasGoodIrgaGeneric()
{
    DEBUG_FUNC_NAME

    auto test = true;
    foreach (const IrgaDesc& irga, project_state_.irgaList)
    {
        test &= IrgaDesc::hasGoodPathLength(irga);
    }
    return test;
}

bool DlProject::masterAnemHasFwVersion()
{
    auto test = true;
    if (!project_state_.anemometerList.isEmpty())
    {
        if (project_state_.anemometerList.first().swVersion().trimmed().isEmpty())
        {
            test = false;
        }
    }
    return test;
}

bool DlProject::masterAnemHasGoodWindmasterFwVersion()
{
    DEBUG_FUNC_NAME

    auto test = true;
    if (!project_state_.anemometerList.isEmpty())
    {
        // if Gill
        if (project_state_.anemometerList.first().manufacturer()
                == AnemDesc::getANEM_MANUFACTURER_STRING_1())
        {
            // and if Windmaster/Pro
            if (project_state_.anemometerList.first().model() ==
                    AnemDesc::getANEM_MODEL_STRING_7() or
                project_state_.anemometerList.first().model() ==
                    AnemDesc::getANEM_MODEL_STRING_8())
            {
                auto anem_version = project_state_.anemometerList.first().swVersion().trimmed();
                if (!anem_version.isEmpty())
                {
                    QRegularExpression re(QStringLiteral("^\\d+[.|-]\\d+[.|-]\\d+$"));
                    qDebug() << "re.valid:" << re.isValid();

                    if (!re.match(anem_version).hasMatch())
                    {
                        test = false;
                    }
                }
                else
                {
                    test = false;
                }
            }
        }
    }
    return test;
}

bool DlProject::hasOneGoodAnemometer()
{
    DEBUG_FUNC_NAME

    bool test = false;
    foreach (const AnemDesc& anem, project_state_.anemometerList)
    {
        if (AnemDesc::isGoodAnemometer(anem))
        {
            test = true;
            return test;
        }
    }
    return test;
}

bool DlProject::hasGoodWindComponentsAndTemperature()
{
    DEBUG_FUNC_NAME

    bool testCompResult = false;
    isFastTempAvailable_ = false;

    QMultiHash<QString, int> var_hash;
    QMultiHash<QString, int> sonic_hash;
    QList<QHash<QString, int>> comp_hash_list;

    int i = 0;
    foreach (const VariableDesc& var, *variables())
    {
        // populate a hash table with all the well described
        // anemometers variables
        if (var.ignore() == QLatin1String("no")
            && var.numeric() == QLatin1String("yes")
            && (VariableDesc::isGoodWindComponent(var)
                || VariableDesc::isGoodSonicTempOrSpeed(var)))
        {
            var_hash.insert(var.instrument().split(QLatin1Char(':')).at(0).trimmed() + QLatin1Char('|') +
                            var.variable(), i);
        }

        // test for at least a well described fast ambient temperature
        if (var.ignore() == QLatin1String("no")
            && var.numeric() == QLatin1String("yes")
            && var.variable().contains(VariableDesc::getVARIABLE_VAR_STRING_28())
            && VariableDesc::isGoodTemperature(var, VariableDesc::AnalogType::FAST))
        {
            isFastTempAvailable_ = true;
            qDebug() << "isFastTempAvailable_:" << isFastTempAvailable_ << i;
        }
        qDebug() << "hash table population, step i:" << i;
        ++i;
    }

    // populate a new hash of anemometers which have variables, to list and
    // count the unique anememeters
    foreach (int value, var_hash)
    {
        QString sonicKey = var_hash.key(value);
        sonicKey = sonicKey.split(QLatin1Char('|')).at(0);
        sonic_hash.insert(sonicKey, value);
        qDebug() << "var_hash(key, value):" << var_hash.key(value) << value;
    }
    QStringList uniqueAnems = sonic_hash.uniqueKeys();
    uniqueAnems.sort();
    int anemsNum = uniqueAnems.size();
    qDebug() << "sonic_hash" << sonic_hash;
    qDebug() << "uniqueAnems" << uniqueAnems;

    // create an ordered list of hash tables of variables, one for each
    // anemometer which has variables
    for (int k = 0; k < anemsNum; ++k)
    {
        QMultiHash<QString, int> hash;

        foreach (int value, sonic_hash)
        {
            if (sonic_hash.key(value) == uniqueAnems.at(k))
            {
                hash.insert(var_hash.key(value).split(QLatin1Char('|')).at(1), value);
            }
        }
        comp_hash_list << hash;
    }

    // test each hash in the previous list
    for (int j = 0; j < comp_hash_list.size(); ++j)
    {
        qDebug() << "hash i" << j << ":" << comp_hash_list.at(j);

        bool hasTemperature = false;
        if (checkAnemVars(comp_hash_list.at(j), isFastTempAvailable_, &hasTemperature))
        {
            testCompResult = true;
            qDebug() << "hasTemperature" << hasTemperature;

            int k = 0;
            foreach (AnemDesc anem, project_state_.anemometerList)
            {
                int checkedAnemColumn = QVariant(uniqueAnems.at(j).split(QLatin1Char(' ')).at(1)).toInt();
                int checkedAnemIndex = checkedAnemColumn - 1;
                qDebug() << "checkedAnemColumn" << checkedAnemColumn << "checkedAnemIndex" << checkedAnemIndex;
                if (k == checkedAnemIndex)
                {
                    qDebug() << "k" << k << "checkedAnemIndex" << checkedAnemIndex;
                    anem.setHasGoodWindComponents(true);
                    anem.setHasGoodTemp(hasTemperature);
                    project_state_.anemometerList.replace(k, anem);
                }
                ++k;
            }
        }
        else
        {
            int k = 0;
            foreach (AnemDesc anem, project_state_.anemometerList)
            {
                int checkedAnemColumn = QVariant(uniqueAnems.at(j).split(QLatin1Char(' ')).at(1)).toInt();
                int checkedAnemIndex = checkedAnemColumn - 1;
                if (k == checkedAnemIndex)
                {
                    qDebug() << "k" << k << "checkedAnemIndex" << checkedAnemIndex;
                    anem.setHasGoodWindComponents(false);
                    anem.setHasGoodTemp(hasTemperature);
                    project_state_.anemometerList.replace(k, anem);
                }
                ++k;
            }
        }
    }
    qDebug() << "testCompResult" << testCompResult;

    return testCompResult;
}

bool DlProject::checkAnemVars(const AnemComponents &hash, bool isFastTempAvailable, bool *anemHasTemp)
{
    DEBUG_FUNC_NAME

    int uNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_0()).size();
    int vNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_1()).size();
    int wNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_2()).size();
    int tsNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_3()).size();
    int sosNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_4()).size();
    int rhoNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_16()).size();
    int thetaNum = hash.values(VariableDesc::getVARIABLE_VAR_STRING_17()).size();

    qDebug() << "isFastTempAvailable:" << isFastTempAvailable;
    qDebug() << "uNum:" << uNum;
    qDebug() << "vNum:" << vNum;
    qDebug() << "wNum:" << wNum;
    qDebug() << "tsNum:" << tsNum;
    qDebug() << "sosNum:" << sosNum;
    qDebug() << "rhoNum:" << rhoNum;
    qDebug() << "thetaNum:" << thetaNum;

    bool test_u = (uNum == 1);
    bool test_v = (vNum == 1);
    bool test_w = (wNum == 1);
    bool test_Ts = (tsNum == 1);
    bool test_Sos = (sosNum == 1);
    bool test_rho = (rhoNum == 1);
    bool test_theta = (thetaNum == 1);

    *anemHasTemp = (test_Ts || test_Sos);

    bool res = (test_u && test_v && test_w && (test_Ts || test_Sos || isFastTempAvailable))
                ||
               (test_rho && test_theta && test_w && (test_Ts || test_Sos || isFastTempAvailable));

    qDebug() << "anemHasTemp" << *anemHasTemp;
    qDebug() << "res" << res;

    return res;
}

bool DlProject::masterAnemContainsGillWindmaster()
{
    const QString masterAnemModel = project_state_.anemometerList.first().model();

    qDebug() << "masterAnemModel" << masterAnemModel;
    if (masterAnemModel == AnemDesc::getANEM_MODEL_STRING_7()
        || masterAnemModel == AnemDesc::getANEM_MODEL_STRING_8())
    {
        return true;
    }
    return false;
}
