/***************************************************************************
  dlproject.h
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

#ifndef DLPROJECT_H
#define DLPROJECT_H

#include <QMultiHash>
#include <QObject>

#include "anem_desc.h"      // NOTE: for AnemDescList, maybe to fix
#include "configstate.h"
#include "dlprojectstate.h"
#include "irga_desc.h"      // NOTE: for IrgaDescList, maybe to fix
#include "variable_desc.h"  // NOTE: for VariableDescList, maybe to fix

class QSettings;

using AnemComponents = QMultiHash<QString, int>;

class DlProject : public QObject
{
    Q_OBJECT

public:
    explicit DlProject(QObject* parent);
    DlProject(QObject* parent, const ProjConfigState& project_config);
    DlProject(const DlProject& project);
    DlProject& operator=(const DlProject& project);
    virtual ~DlProject();

    bool hasOneGoodAnemometer();
    bool hasGoodWindComponentsAndTemperature();
    bool hasOneFastTemperature();
    bool hasGoodIrgaNames();
    bool hasGoodIrgaSeparations();
    bool hasGoodIrgaClosedPath();
    bool hasGoodIrgaGeneric();
    bool masterAnemHasFwVersion();
    bool masterAnemHasGoodWindmasterFwVersion();

    // start a new project
    void newProject(const ProjConfigState &project_config);
    // load a project
    bool loadProject(const QString& filename, bool checkVersion = true, bool *modified = nullptr, bool firstReading = false);
    // save the current project
    bool saveProject(const QString& filename);
    // insert tag for native format files
    bool tagProject(const QString& filename);
    // is the file in native format?
    bool nativeFormat(const QString& filename);

    // get and set project state
    const QString& title() const;
    void setTitle(const QString& t);
    const QString& id() const;
    void setId(const QString& t);
    const QString& startDate() const;
    void setStartDate(const QString& d);
    const QString& endDate() const;
    void setEndDate(const QString& d);
    const QString& filename() const;
    void setFileName(const QString& f);

    // get and set files state
    const QString& datapath() const;
    void setDataPath(const QString& p);
    bool savedNative() const;
    void setSavedNative(bool t);
    bool header() const;
    void setHeader(bool t);
    bool timestamp() const;
    void setTimestamp(bool t);
    bool isoformat() const;
    void setIsoformat(bool t);
    const QString& endOfLine() const;
    void setEndOfLine(const QString& s);
    bool enableProc() const;
    void setEnableProc(bool p);
    int format() const;
    void setFormat(int f);
    const QString& filesNotes() const;
    void setFilesNotes(const QString& n);
    int timestampEnd() const;
    void setTimestampEnd(int f);

    // get and set site state
    const QString& siteName() const;
    void setSiteName(const QString& f);
    const QString& siteId() const;
    void setSiteId(const QString& n);
    double siteAltitude() const;
    void setSiteAltitude(double a);
    double siteLatitude() const;
    void setSiteLatitude(double l);
    double siteLongitude() const;
    void setSiteLongitude(double l);
    double siteCanopyHeight() const;
    void setSiteCanopyHeight(double c);
    double siteDisplacementHeight() const;
    void setSiteDisplacementHeight(double c);
    double siteRoughnessLength() const;
    void setSiteRoughnessLength(double c);

    // get and set station state
    const QString& stationName() const;
    void setStationName(const QString& n);
    const QString& stationId() const;
    void setStationId(const QString& i);
    int stationAnemNr() const;
    void setStationAnemNr(int n);
    int stationIrgaNr() const;
    void setStationIrgaNr(int n);
    double stationAnemometerHeight() const;
    void setStationAnemometerHeight(double h);

    // get and set timing state
    double acquisitionFrequency() const;
    void setAcquisitionFrequency(double f);
    int fileDuration() const;
    void setFileDuration(int n);
    const QString& pcTimeSettings() const;
    void setPcTimeSettings(const QString& t);

    const QString& fieldSep() const;
    void setFieldSep(const QString& t);
    int headerRows() const;
    void setHeaderRows(int n);
    bool notNumeric() const;
    void setNotNumeric(bool p);
    const QString& dataLabel() const;
    void setDataLabel(const QString& t);

    void addAnemometer(const AnemDesc& am);
    void addIrga(const IrgaDesc& irga);
    void addVariable(const VariableDesc& var);

    static const QString toIniAnemModel(const QString& s);
    static const QString fromIniAnemModel(const QString& s);
    static const QString toIniVariableMeasureType(const QString& s);
    static const QString fromIniVariableMeasureType(const QString& s);

    // is the project modified?
    bool modified() const;
    // set project as modified
    void setModified(bool m);

    // return pointer to anemometer measure list
    AnemDescList* anems();
    IrgaDescList* irgas();
    VariableDescList* variables();

    bool masterAnemContainsGillWindmaster();

    static const QString getANEM_MODEL_STRING_0();
    static const QString getANEM_MODEL_STRING_1();
    static const QString getANEM_MODEL_STRING_2();
    static const QString getANEM_MODEL_STRING_3();
    static const QString getANEM_MODEL_STRING_4();
    static const QString getANEM_MODEL_STRING_5();
    static const QString getANEM_MODEL_STRING_6();
    static const QString getANEM_MODEL_STRING_7();
    static const QString getANEM_MODEL_STRING_8();
    static const QString getANEM_MODEL_STRING_9();
    static const QString getANEM_MODEL_STRING_10();
    static const QString getANEM_MODEL_STRING_11();
    static const QString getANEM_MODEL_STRING_12();
    static const QString getANEM_MODEL_STRING_13();

signals:
    // send that a new project has been created
    void projectNew();
    // send that a project has been loaded
    void projectChanged();
    // send that a project has been modified
    void projectModified();

private:
    enum class InstrumentType { UNDEFINED, ANEM, IRGA };

    QString toIniAnemManufacturer(const QString& s);
    QString toIniAnemWindFormat(const QString& s);
    QString toIniAnemNorthAlign(const QString& s);
    QString toIniVariableVar(const QString& s);
    QString toIniVariableMeasureUnit(const QString& s);
    QString toIniVariableConversionType(const QString& s);
    QString toIniVariableInstrument(const QString& s);
    QString toIniBool(const QString& s);
    QString fromIniAnemManufacturer(const QString& s);
    QString fromIniAnemWindFormat(const QString& s);
    QString fromIniAnemNorthAlign(const QString &model, const QString& s);
    QString fromIniVariableVar(const QString& s);
    QString fromIniVariableMeasureUnit(const QString& s);
    QString fromIniVariableConversionType(const QString& s);
    QString fromIniVariableInstrument(const QString& s);

    int countInstruments(const QStringList& list);
    InstrumentType getInstrumentType(const QSettings& iniGroup, const QString& prefix);
    InstrumentType getInstrumentTypeFromModel(const QString& model);

    QString fromIniIrgaManufacturer(const QString& s);
    QString fromIniIrgaModel(const QString& s);
    QString toIniIrgaManufacturer(const QString& s);
    QString toIniIrgaModel(const QString& s);

    bool checkAnemVars(const AnemComponents& hash, bool isFastTempAvailable, bool *anemHasTemp = nullptr);

private:
    bool modified_;
    ProjectState project_state_;
    ProjConfigState project_config_state_;
    bool isFastTempAvailable_;

    static const QString VARIABLE_VAR_STRING_0;
    static const QString VARIABLE_VAR_STRING_1;
    static const QString VARIABLE_VAR_STRING_2;
    static const QString VARIABLE_VAR_STRING_3;
    static const QString VARIABLE_VAR_STRING_4;
    static const QString VARIABLE_VAR_STRING_5;
    static const QString VARIABLE_VAR_STRING_6;
    static const QString VARIABLE_VAR_STRING_7;
    static const QString VARIABLE_VAR_STRING_8;
    static const QString VARIABLE_VAR_STRING_9;
    static const QString VARIABLE_VAR_STRING_10;
    static const QString VARIABLE_VAR_STRING_11;
    static const QString VARIABLE_VAR_STRING_12;
    static const QString VARIABLE_VAR_STRING_13;
    static const QString VARIABLE_VAR_STRING_14;
    static const QString VARIABLE_VAR_STRING_15;
    static const QString VARIABLE_VAR_STRING_16;
    static const QString VARIABLE_VAR_STRING_17;
    static const QString VARIABLE_VAR_STRING_18;
    static const QString VARIABLE_VAR_STRING_19;
    static const QString VARIABLE_VAR_STRING_20;
    static const QString VARIABLE_VAR_STRING_21;
    static const QString VARIABLE_VAR_STRING_22;
    static const QString VARIABLE_VAR_STRING_23;
    static const QString VARIABLE_VAR_STRING_24;
    static const QString VARIABLE_VAR_STRING_25;
    static const QString VARIABLE_VAR_STRING_26;
    static const QString VARIABLE_VAR_STRING_27;
    static const QString VARIABLE_VAR_STRING_28;
    static const QString VARIABLE_VAR_STRING_29;

    static const QString VARIABLE_MEASURE_TYPE_STRING_0;
    static const QString VARIABLE_MEASURE_TYPE_STRING_1;
    static const QString VARIABLE_MEASURE_TYPE_STRING_2;
    static const QString VARIABLE_MEASURE_TYPE_STRING_3;

    static const QString ANEM_MANUFACTURER_STRING_0;
    static const QString ANEM_MANUFACTURER_STRING_1;
    static const QString ANEM_MANUFACTURER_STRING_2;
    static const QString ANEM_MANUFACTURER_STRING_3;
    static const QString ANEM_MANUFACTURER_STRING_4;

    static const QString ANEM_WIND_FORMAT_STRING_0;
    static const QString ANEM_WIND_FORMAT_STRING_1;
    static const QString ANEM_WIND_FORMAT_STRING_2;

    static const QString ANEM_NORTH_ALIGN_STRING_0;
    static const QString ANEM_NORTH_ALIGN_STRING_1;
    static const QString ANEM_NORTH_ALIGN_STRING_2;

    static const QString IRGA_MANUFACTURER_STRING_0;
    static const QString IRGA_MANUFACTURER_STRING_1;

    static const QString IRGA_MODEL_STRING_0;
    static const QString IRGA_MODEL_STRING_1;
    static const QString IRGA_MODEL_STRING_2;
    static const QString IRGA_MODEL_STRING_3;
    static const QString IRGA_MODEL_STRING_4;
    static const QString IRGA_MODEL_STRING_5;
    static const QString IRGA_MODEL_STRING_6;
    static const QString IRGA_MODEL_STRING_7;
    static const QString IRGA_MODEL_STRING_8;
    static const QString IRGA_MODEL_STRING_9;
    static const QString IRGA_MODEL_STRING_10;
    static const QString IRGA_MODEL_STRING_11;
    static const QString IRGA_MODEL_STRING_12;
    static const QString IRGA_MODEL_STRING_13;

    static const QString IRGA_SW_VERSION_STRING_0;
    static const QString IRGA_SW_VERSION_STRING_1;
    static const QString IRGA_SW_VERSION_STRING_2;
    static const QString IRGA_SW_VERSION_STRING_3;

    static const QString VARIABLE_MEASURE_UNIT_STRING_0;
    static const QString VARIABLE_MEASURE_UNIT_STRING_1;
    static const QString VARIABLE_MEASURE_UNIT_STRING_2;
    static const QString VARIABLE_MEASURE_UNIT_STRING_3;
    static const QString VARIABLE_MEASURE_UNIT_STRING_4;
    static const QString VARIABLE_MEASURE_UNIT_STRING_5;
    static const QString VARIABLE_MEASURE_UNIT_STRING_6;
    static const QString VARIABLE_MEASURE_UNIT_STRING_7;
    static const QString VARIABLE_MEASURE_UNIT_STRING_8;
    static const QString VARIABLE_MEASURE_UNIT_STRING_9;
    static const QString VARIABLE_MEASURE_UNIT_STRING_10;
    static const QString VARIABLE_MEASURE_UNIT_STRING_11;
    static const QString VARIABLE_MEASURE_UNIT_STRING_12;
    static const QString VARIABLE_MEASURE_UNIT_STRING_13;
    static const QString VARIABLE_MEASURE_UNIT_STRING_14;
    static const QString VARIABLE_MEASURE_UNIT_STRING_15;
    static const QString VARIABLE_MEASURE_UNIT_STRING_16;
    static const QString VARIABLE_MEASURE_UNIT_STRING_17;
    static const QString VARIABLE_MEASURE_UNIT_STRING_18;
    static const QString VARIABLE_MEASURE_UNIT_STRING_19;
    static const QString VARIABLE_MEASURE_UNIT_STRING_20;
    static const QString VARIABLE_MEASURE_UNIT_STRING_21;
    static const QString VARIABLE_MEASURE_UNIT_STRING_22;
    static const QString VARIABLE_MEASURE_UNIT_STRING_23;
    static const QString VARIABLE_MEASURE_UNIT_STRING_24;
    static const QString VARIABLE_MEASURE_UNIT_STRING_25;
    static const QString VARIABLE_MEASURE_UNIT_STRING_26;
    static const QString VARIABLE_MEASURE_UNIT_STRING_27;

    static const QString VARIABLE_CONVERSION_TYPE_STRING_0;
    static const QString VARIABLE_CONVERSION_TYPE_STRING_1;
    static const QString VARIABLE_CONVERSION_TYPE_STRING_2;
};

// general section
inline const QString& DlProject::title() const
   { return project_state_.general.title; }
inline void DlProject::setTitle(const QString& t)
{ project_state_.general.title = t; setModified(true); }

inline const QString& DlProject::id() const
   { return project_state_.general.id; }
inline void DlProject::setId(const QString& t)
{ project_state_.general.id = t; setModified(true); }

inline const QString& DlProject::startDate() const
   { return project_state_.general.start_date; }
inline void DlProject::setStartDate(const QString& d)
   { project_state_.general.start_date = d; setModified(true); }

inline const QString& DlProject::endDate() const
   { return project_state_.general.end_date; }
inline void DlProject::setEndDate(const QString& d)
   { project_state_.general.end_date = d; setModified(true); }

inline const QString& DlProject::filename() const
   { return project_state_.general.file_name; }
inline void DlProject::setFileName(const QString& f)
   { project_state_.general.file_name = f; setModified(true); }

// files section
inline const QString& DlProject::datapath() const
   { return project_state_.files.data_path; }
inline void DlProject::setDataPath(const QString& p)
   { project_state_.files.data_path = p; setModified(true); }

inline bool DlProject::savedNative() const
    { return project_state_.files.saved_native; }
inline void DlProject::setSavedNative(bool t)
    { project_state_.files.saved_native = t; setModified(true); }

inline bool DlProject::timestamp() const
    { return project_state_.files.timestamp; }
inline void DlProject::setTimestamp(bool t)
    { project_state_.files.timestamp = t; setModified(true); }

inline bool DlProject::isoformat() const
    { return project_state_.files.isoformat; }
inline void DlProject::setIsoformat(bool t)
    { project_state_.files.isoformat = t; setModified(true); }

inline const QString& DlProject::endOfLine() const
   { return project_state_.files.end_of_line; }
inline void DlProject::setEndOfLine(const QString& s)
   { project_state_.files.end_of_line = s; setModified(true); }

inline bool DlProject::enableProc() const
    { return project_state_.files.enable_processing; }
inline void DlProject::setEnableProc(bool p)
    { project_state_.files.enable_processing = p; setModified(true); }

inline int DlProject::timestampEnd() const
    { return project_state_.files.tstamp_end; }
inline void DlProject::setTimestampEnd(int f)
    { project_state_.files.tstamp_end = f; setModified(true); }

// site section
inline const QString& DlProject::siteName() const
   { return project_state_.site.site_name; }
inline void DlProject::setSiteName(const QString& f)
   { project_state_.site.site_name = f; setModified(true); }

inline const QString& DlProject::siteId() const
   { return project_state_.site.site_id; }
inline void DlProject::setSiteId(const QString& n)
   { project_state_.site.site_id = n; setModified(true); }

inline double DlProject::siteAltitude() const
   { return project_state_.site.altitude; }
inline void DlProject::setSiteAltitude(double a)
   { project_state_.site.altitude = a; setModified(true); }

inline double DlProject::siteLatitude() const
   { return project_state_.site.latitude; }
inline void DlProject::setSiteLatitude(double l)
   { project_state_.site.latitude = l; setModified(true); }

inline double DlProject::siteLongitude() const
   { return project_state_.site.longitude; }
inline void DlProject::setSiteLongitude(double l)
   { project_state_.site.longitude = l; setModified(true); }

inline double DlProject::siteCanopyHeight() const
   { return project_state_.site.canopy_height; }
inline void DlProject::setSiteCanopyHeight(double c)
   { project_state_.site.canopy_height = c; setModified(true); }

inline double DlProject::siteDisplacementHeight() const
   { return project_state_.site.displacement_height; }
inline void DlProject::setSiteDisplacementHeight(double c)
   { project_state_.site.displacement_height = c; setModified(true); }

inline double DlProject::siteRoughnessLength() const
   { return project_state_.site.roughness_length; }
inline void DlProject::setSiteRoughnessLength(double c)
   { project_state_.site.roughness_length = c; setModified(true); }

// station section
inline const QString& DlProject::stationName() const
   { return project_state_.station.name; }
inline void DlProject::setStationName(const QString& n)
   { project_state_.station.name = n; setModified(true); }

inline const QString& DlProject::stationId() const
   { return project_state_.station.id; }
inline void DlProject::setStationId(const QString& i)
   { project_state_.station.id = i; setModified(true); }

// timing section
inline double DlProject::acquisitionFrequency() const
   { return project_state_.timing.acquisition_frequency; }
inline void DlProject::setAcquisitionFrequency(double f)
   { project_state_.timing.acquisition_frequency = f; setModified(true); }

inline int DlProject::fileDuration() const
   { return project_state_.timing.file_duration; }
inline void DlProject::setFileDuration(int n)
   { project_state_.timing.file_duration = n; setModified(true); }

inline const QString& DlProject::pcTimeSettings() const
   { return project_state_.timing.pc_time_settings; }
inline void DlProject::setPcTimeSettings(const QString& t)
   { project_state_.timing.pc_time_settings = t; setModified(true); }

inline AnemDescList* DlProject::anems()
    { return &project_state_.anemometerList; }

inline IrgaDescList* DlProject::irgas()
    { return &project_state_.irgaList; }

inline VariableDescList* DlProject::variables()
    { return &project_state_.variableList; }

inline const QString& DlProject::fieldSep() const
   { return project_state_.varDesc.separator; }
inline void DlProject::setFieldSep(const QString& s)
   { project_state_.varDesc.separator = s; setModified(true); }

inline int DlProject::headerRows() const
   { return project_state_.varDesc.header_rows; }
inline void DlProject::setHeaderRows(int n)
   { project_state_.varDesc.header_rows = n; setModified(true); }

inline const QString& DlProject::dataLabel() const
   { return project_state_.varDesc.data_label; }
inline void DlProject::setDataLabel(const QString& s)
   { project_state_.varDesc.data_label = s; setModified(true); }

#endif // DLPROJECT_H
