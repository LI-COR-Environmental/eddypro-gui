/***************************************************************************
  ecproject.h
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

#ifndef ECPROJECT_H
#define ECPROJECT_H

#include <QObject>
#include "configstate.h"
#include "ecprojectstate.h"
  
class EcProject : public QObject
{
    Q_OBJECT

public:
    EcProject(QObject *parent, ProjConfigState& project_config);
    EcProject(const EcProject &project);

    virtual ~EcProject();

    EcProject& operator=(const EcProject &project);

    void newEcProject(const ProjConfigState &project_config);

    // save the current project
    bool saveEcProject(const QString &filename);

    // load a project
    bool loadEcProject(const QString &filename, bool checkVersion = true, bool *modified = 0);

    // is the file in native format?
    bool nativeFormat(const QString &filename);

    // field comparison for previous data assessment
    bool fuzzyCompare(const EcProject& previousProject);

    // set project
    void setGeneralRunMode(int n);
    void setGeneralRunFcc(int n);
    void setGeneralTitle(const QString &s);
    void setGeneralFileName(const QString &id);
    void setGeneralId(const QString &id);
    void setGeneralFileType(int n);
    void setGeneralBinaryHNLines(int n);
    void setGeneralBinaryEol(int n);
    void setGeneralBinaryNBytes(int n);
    void setGeneralBinaryLittleEnd(int n);
    void setGeneralFilePrototype(const QString &f);
    void setGeneralUseAltMdFile(bool b);
    void setGeneralMdFilepath(const QString &p);
    void setGeneralCustomFile(const QString& fp);
    void setGeneralUseTimelineFile(bool b);
    void setGeneralTimelineFilepath(const QString &p);
    void setGeneralColMasterSonic(const QString &p);
    void setGeneralColCo2(int n);
    void setGeneralColH2o(int n);
    void setGeneralColCh4(int n);
    void setGeneralColGas4(int n);
    void setGeneralColIntTc(int n);
    void setGeneralColIntT1(int n);
    void setGeneralColIntT2(int n);
    void setGeneralColIntP(int n);
    void setGeneralColAirT(int n);
    void setGeneralColAirP(int n);
    void setGeneralColDiag75(int n);
    void setGeneralColDiag72(int n);
    void setGeneralColDiag77(int n);
    void setGeneralColTs(int n);
    void setGeneralColGasMw(double n);
    void setGeneralColGasDiff(double n);
    void setGeneralMakeDataset(bool b);
    void setGeneralStartDate(const QString &d);
    void setGeneralEndDate(const QString &d);
    void setGeneralStartTime(const QString &t);
    void setGeneralEndTime(const QString &t);
    void setGeneralHfMethod(int n);
    void setGeneralLfMethod(int n);
    void setGeneralWplMeth( int n);
    void setGeneralFpMeth(int n);
    void setGeneralOutGhgEu(int n);
    void setGeneralOutAmFluxOut(int n);
    void setGeneralOutRich(int n);
    void setGeneralOutMd(int n);
    void setGeneralOutBiomet(int n);
    void setGeneralTob1Format(int n);
    void setGeneralOutPath(const QString &p);
    void setGeneralFixedOutFormat(int n);
    void setGeneralErrorLabel(const QString &p);
    void setGeneralSubset(int n);
    void setGeneralQcMeth(int n);
    void setGeneralUseBiomet(int n);
    void setGeneralBiomFile(const QString &p);
    void setGeneralBiomDir(const QString &p);
    void setGeneralBiomRecurse(int n);
    void setGeneralBiomExt(const QString &p);

    void setMetGeneralFileType(int n);
    void setMetGeneralDataPath(const QString &s);
    void setMetSettingsExtension(const QString &s);
    void setMetSettingsDateFormat(const QString &s);
    void setMetSettingsTimeFormat(const QString &s);
    void setMetSettingsLoggerId(const QString &s);
    void setMetSettingsFieldSep(int n);
    void setMetSettingsSepPolicy(int n);
    void setMetGeneralTimestampEnd(bool n);

    void setScreenDataPath(const QString &p);
    void setScreenRecurse(int n);
    void setScreenUseGeoNorth(bool b);
    void setScreenMagDec(double n);
    void setScreenDecDate(const QString &d);
    void setScreenNFiles(int n);
    void setScreenAvrgLen(int n);
    void setScreenFlag1Col(int n);
    void setScreenFlag1Threshold(double n);
    void setScreenFlag1Policy(int n);
    void setScreenFlag2Col(int n);
    void setScreenFlag2Threshold(double n);
    void setScreenFlag2Policy(int n);
    void setScreenFlag3Col(int n);
    void setScreenFlag3Threshold(double n);
    void setScreenFlag3Policy(int n);
    void setScreenFlag4Col(int n);
    void setScreenFlag4Threshold(double n);
    void setScreenFlag4Policy(int n);
    void setScreenFlag5Col(int n);
    void setScreenFlag5Threshold(double n);
    void setScreenFlag5Policy(int n);
    void setScreenFlag6Col(int n);
    void setScreenFlag6Threshold(double n);
    void setScreenFlag6Policy(int n);
    void setScreenFlag7Col(int n);
    void setScreenFlag7Threshold(double n);
    void setScreenFlag7Policy(int n);
    void setScreenFlag8Col(int n);
    void setScreenFlag8Threshold(double n);
    void setScreenFlag8Policy(int n);
    void setScreenFlag9Col(int n);
    void setScreenFlag9Threshold(double n);
    void setScreenFlag9Policy(int n);
    void setScreenFlag10Col(int n);
    void setScreenFlag10Threshold(double n);
    void setScreenFlag10Policy(int n);

    void setScreenMaxLack(int n);
    void setScreenUOffset(double d);
    void setScreenVOffset(double d);
    void setScreenWOffset(double d);
    void setScreenFlowDistortion(int n);
    void setScreenCrossWind(int n);
    void setScreenRotMethod(int n);
    void setScreenDetrendMeth(int n);
    void setScreenTimeConst(double n);
    void setScreenTlagMeth(int n);
    void setScreenTapWin(int n);
    void setScreenNBins(int n);
    void setScreenOutBinSpectra(int n);
    void setScreenOutBinOgives(int n);
    void setGeneralOutMeanCosp(int n);
    void setGeneralBinSpectraAvail(int n);
    void setGeneralFullSpectraAvail(int n);
    void setGeneralFilesFound(int n);
    void setScreenOutFullSpectraU(int n);
    void setScreenOutFullSpectraV(int n);
    void setScreenOutFullSpectraW(int n);
    void setScreenOutFullSpectraTs(int n);
    void setScreenOutFullSpectraCo2(int n);
    void setScreenOutFullSpectraH2o(int n);
    void setScreenOutFullSpectraCh4(int n);
    void setScreenOutFullSpectraN2o(int n);
    void setScreenOutSt1(int n);
    void setScreenOutSt2(int n);
    void setScreenOutSt3(int n);
    void setScreenOutSt4(int n);
    void setScreenOutSt5(int n);
    void setScreenOutSt6(int n);
    void setScreenOutSt7(int n);
    void setScreenOutRaw1(int n);
    void setScreenOutRaw2(int n);
    void setScreenOutRaw3(int n);
    void setScreenOutRaw4(int n);
    void setScreenOutRaw5(int n);
    void setScreenOutRaw6(int n);
    void setScreenOutRaw7(int n);
    void setScreenOutRawU(int n);
    void setScreenOutRawV(int n);
    void setScreenOutRawW(int n);
    void setScreenOutRawTs(int n);
    void setScreenOutRawCo2(int n);
    void setScreenOutRawH2o(int n);
    void setScreenOutRawCh4(int n);
    void setScreenOutRawGas4(int n);
    void setScreenOutRawTair(int n);
    void setScreenOutRawPair(int n);
    void setScreenOutFullCospectraU(int n);
    void setScreenOutFullCospectraV(int n);
    void setScreenOutFullCospectraTs(int n);
    void setScreenOutFullCospectraCo2(int n);
    void setScreenOutFullCospectraH2o(int n);
    void setScreenOutFullCospectraCh4(int n);
    void setScreenOutFullCospectraN2o(int n);
    void setScreenlOutDetails(int n);
    void setScreenlOutSlowVars(int n);
    void setScreenToMixRatio(int n);
    void setScreenFilterSr(int n);
    void setScreenFilterAl(int n);
    void setScreenBuCorr(int n);
    void setScreenBuMulti(int n);
    void setScreenLDayBotGain(double d);
    void setScreenLDayBotOffset(double d);
    void setScreenLDayTopGain(double d);
    void setScreenLDayTopOffset(double d);
    void setScreenLDaySparGain(double d);
    void setScreenLDaySparOffset(double d);
    void setScreenLNightBotGain(double d);
    void setScreenLNightBotOffset(double d);
    void setScreenLNightTopGain(double d);
    void setScreenLNightTopOffset(double d);
    void setScreenLNightSparGain(double d);
    void setScreenLNightSparOffset(double d);
    void setScreenMDayBot1(double d);
    void setScreenMDayBot2(double d);
    void setScreenMDayBot3(double d);
    void setScreenMDayBot4(double d);
    void setScreenMDayTop1(double d);
    void setScreenMDayTop2(double d);
    void setScreenMDayTop3(double d);
    void setScreenMDayTop4(double d);
    void setScreenMDaySpar1(double d);
    void setScreenMDaySpar2(double d);
    void setScreenMDaySpar3(double d);
    void setScreenMDaySpar4(double d);
    void setScreenMNightBot1(double d);
    void setScreenMNightBot2(double d);
    void setScreenMNightBot3(double d);
    void setScreenMNightBot4(double d);
    void setScreenMNightTop1(double d);
    void setScreenMNightTop2(double d);
    void setScreenMNightTop3(double d);
    void setScreenMNightTop4(double d);
    void setScreenMNightSpar1(double d);
    void setScreenMNightSpar2(double d);
    void setScreenMNightSpar3(double d);
    void setScreenMNightSpar4(double d);
    void setScreenTimelagFilePath(const QString &p);
    void setScreenlPowerOfTwo(int n);

    void setScreenTestSr(int l);
    void setScreenTestAr(int l);
    void setScreenTestDo(int l);
    void setScreenTestAl(int l);
    void setScreenTestSk(int l);
    void setScreenTestDs(int l);
    void setScreenTestTl(int l);
    void setScreenTestAa(int l);
    void setScreenTestNs(int l);

    void setScreenParamSrNumSpk(int n);
    void setScreenParamSrHfLim(double n);
    void setScreenParamSrULim(double n);
    void setScreenParamSrWLim(double n);
    void setScreenParamSrCo2Lim(double n);
    void setScreenParamSrH2oLim(double n);
    void setScreenParamSrCh4Lim(double n);
    void setScreenParamSrN2oLim(double n);
    void setScreenParamArLim(double n);
    void setScreenParamArBins(int n);
    void setScreenParamArHfLim(int n);
    void setScreenParamDoExtLimDw(int n);
    void setScreenParamDoHf1Lim(double n);
    void setScreenParamDoHf2Lim(double n);
    void setScreenParamAlUMax(double n);
    void setScreenParamAlWMax(double n);
    void setScreenParamAlTsonMin(double n);
    void setScreenParamAlTsonMax(double n);
    void setScreenParamAlCo2Min(double n);
    void setScreenParamAlCo2Max(double n);
    void setScreenParamAlH2oMin(double n);
    void setScreenParamAlH2oMax(double n);
    void setScreenParamAlCh4Min(double n);
    void setScreenParamAlCh4Max(double n);
    void setScreenParamAlN2oMin(double n);
    void setScreenParamAlN2oMax(double n);
    void setScreenParamSkHfSkmin(double n);
    void setScreenParamSkHfSkmax(double n);
    void setScreenParamSkSfSkmin(double n);
    void setScreenParamSkSfSkmax(double n);
    void setScreenParamSkHfKumin(double n);
    void setScreenParamSkHfKumax(double n);
    void setScreenParamSkSfKumin(double n);
    void setScreenParamSkSfKumax(double n);
    void setScreenParamDsHfUV(double n);
    void setScreenParamDsHfW(double n);
    void setScreenParamDsHfT(double n);
    void setScreenParamDsHfCo2(double n);
    void setScreenParamDsHfH2o(double n);
    void setScreenParamDsHfCh4(double n);
    void setScreenParamDsHfN2o(double n);
    void setScreenParamDsHfVar(double n);
    void setScreenParamDsSfUV(double n);
    void setScreenParamDsSfW(double n);
    void setScreenParamDsSfT(double n);
    void setScreenParamDsSfCo2(double n);
    void setScreenParamDsSfH2o(double n);
    void setScreenParamDsSfCh4(double n);
    void setScreenParamDsSfN2o(double n);
    void setScreenParamDsSfVar(double n);
    void setScreenParamTlHfLim(double n);
    void setScreenParamTlSfLim(double n);
    void setScreenParamTlDefCo2(double n);
    void setScreenParamTlDefH2o(double n);
    void setScreenParamTlDefCh4(double n);
    void setScreenParamTlDefN2o(double n);
    void setScreenParamAaMin(double n);
    void setScreenParamAaMax(double n);
    void setScreenParamAaLim(double n);
    void setScreenParamNsHfLim(double n);

    void setPlanarFitStartDate(const QString& date);
    void setPlanarFitEndDate(const QString& date);
    void setPlanarFitMode(int i);
    void setPlanarFitNorthOffset(double d);
    void setPlanarFitItemPerSector(int i);
    void setPlanarFitWmax(double d);
    void setPlanarFitUmin(double d);
    void setPlanarFitFile(const QString& p);
    void setPlanarFitPolicy(int i);
    void setPlanarFitSubset(int n);

    void setSpectraMode(int i);
    void setSpectraStartDate(const QString& date);
    void setSpectraEndDate(const QString& date);
    void setSpectraFile(const QString& p);
    void setSpectraMinSmpl(int l);
    void setSpectraHorst(int n);
    void setSpectraAddSonic(int n);
    void setSpectraFminCo2(double d);
    void setSpectraFminH2o(double d);
    void setSpectraFminCh4(double d);
    void setSpectraFminGas4(double d);
    void setSpectraFmaxCo2(double d);
    void setSpectraFmaxH2o(double d);
    void setSpectraFmaxCh4(double d);
    void setSpectraFmaxGas4(double d);
    void setSpectraHfnCo2(double d);
    void setSpectraHfnH2o(double d);
    void setSpectraHfnCh4(double d);
    void setSpectraHfnGas4(double d);
    void setSpectraMinCo2(double d);
    void setSpectraMinCh4(double d);
    void setSpectraMinGas4(double d);
    void setSpectraMinLe(double d);
    void setSpectraMinH(double d);
    void setSpectraF10Co2(double d);
    void setSpectraF10Ch4(double d);
    void setSpectraF10Gas4(double d);
    void setSpectraF10Le(double d);
    void setSpectraF10H(double d);
    void setSpectraExFile(const QString& p);
    void setSpectraBinSpectra(const QString& p);
    void setSpectraFullSpectra(const QString& p);
    void setSpectraExDir(const QString& p);
    void setSpectraSubset(int n);

    void setTimelagOptStartDate(const QString& date);
    void setTimelagOptEndDate(const QString& date);
    void setTimelagOptMode(int i);
    void setTimelagOptFile(const QString& p);
    void setTimelagOptH2oNClass(int n);
    void setTimelagOptCo2MinFlux(double d);
    void setTimelagOptCh4MinFlux(double d);
    void setTimelagOptGas4MinFlux(double d);
    void setTimelagOptLeMinFlux(double d);
    void setTimelagOptPgRange(double d);
    void setTimelagOptCo2MinLag(double d);
    void setTimelagOptCo2MaxLag(double d);
    void setTimelagOptH2oMinLag(double d);
    void setTimelagOptH2oMaxLag(double d);
    void setTimelagOptCh4MinLag(double d);
    void setTimelagOptCh4MaxLag(double d);
    void setTimelagOptGas4MinLag(double d);
    void setTimelagOptGas4MaxLag(double d);
    void setTimelagOptSubset(int i);

    void setRandomErrorMethod(int n);
    void setRandomErrorItsMethod(int n);
    void setRandomErrorItsTlagMax(double d);
    void setRandomErrorItsSecFactor(double d);

    void setBiomParamColAirT(int n);
    void setBiomParamColAirP(int n);
    void setBiomParamColRh(int n);
    void setBiomParamColRg(int n);
    void setBiomParamColLwin(int n);
    void setBiomParamColPpfd(int n);

    // get project
    int generalRunMode() const { return ec_project_state_.projectGeneral.run_mode; }
    int generalRunFcc() const { return ec_project_state_.projectGeneral.run_fcc; }
    const QString& generalTitle() const { return ec_project_state_.projectGeneral.project_title; }
    const QString& generalFileName() const { return ec_project_state_.projectGeneral.file_name; }
    const QString& generalId() const { return ec_project_state_.projectGeneral.project_id; }
    const QString& generalFilePrototype() const { return ec_project_state_.projectGeneral.file_prototype; }
    bool generalUseAltMdFile() const { return ec_project_state_.projectGeneral.use_alt_md_file; }
    int generalFileType() const { return ec_project_state_.projectGeneral.file_type; }
    int generalBinaryHNLines() const { return ec_project_state_.projectGeneral.binary_hnlines; }
    int generalBinaryEol() const { return ec_project_state_.projectGeneral.binary_eol; }
    int generalBinaryNBytes() const { return ec_project_state_.projectGeneral.binary_nbytes; }
    int generalBinaryLittleEnd() const { return ec_project_state_.projectGeneral.binary_little_end; }
    const QString& generalMdFilepath() const { return ec_project_state_.projectGeneral.md_file; }
    bool generalUseTimelineFile() const { return ec_project_state_.projectGeneral.use_tlfile; }
    const QString& generalTimelineFilepath() const { return ec_project_state_.projectGeneral.timeline_file; }
    const QString& generalColMasterSonic() const { return ec_project_state_.projectGeneral.master_sonic; }
    int generalColCo2() const { return ec_project_state_.projectGeneral.col_co2; }
    int generalColH2o() const { return ec_project_state_.projectGeneral.col_h2o; }
    int generalColCh4() const { return ec_project_state_.projectGeneral.col_ch4; }
    int generalColGas4() const { return ec_project_state_.projectGeneral.col_n2o; }
    int generalColIntTc() const { return ec_project_state_.projectGeneral.col_int_t_c; }
    int generalColIntT1() const { return ec_project_state_.projectGeneral.col_int_t_1; }
    int generalColIntT2() const { return ec_project_state_.projectGeneral.col_int_t_2; }
    int generalColIntP() const { return ec_project_state_.projectGeneral.col_int_p; }
    int generalColAirT() const { return ec_project_state_.projectGeneral.col_air_t; }
    int generalColAirP() const { return ec_project_state_.projectGeneral.col_air_p; }
    int generalColDiag75() const { return ec_project_state_.projectGeneral.col_diag_75; }
    int generalColDiag72() const { return ec_project_state_.projectGeneral.col_diag_72; }
    int generalColDiag77() const { return ec_project_state_.projectGeneral.col_diag_77; }
    int generalColTs() const { return ec_project_state_.projectGeneral.col_ts; }
    qreal generalGasMw() const { return ec_project_state_.projectGeneral.gas_mw; }
    qreal generalGasDiff() const { return ec_project_state_.projectGeneral.gas_diff; }
    int generalOutGhgEu() const { return ec_project_state_.projectGeneral.out_ghg_eu; }
    int generalOutAmFlux() const { return ec_project_state_.projectGeneral.out_amflux; }
    int generalOutRich() const { return ec_project_state_.projectGeneral.out_rich; }
    int generalOutMd() const { return ec_project_state_.projectGeneral.out_md; }
    int generalOutBiomet() const { return ec_project_state_.projectGeneral.out_biomet; }
    bool generalMakeDataset() const { return ec_project_state_.projectGeneral.make_dataset; }
    const QString& generalStartDate() const { return ec_project_state_.projectGeneral.start_date; }
    const QString& generalEndDate() const { return ec_project_state_.projectGeneral.end_date; }
    const QString& generalStartTime() const { return ec_project_state_.projectGeneral.start_time; }
    const QString& generalEndTime() const { return ec_project_state_.projectGeneral.end_time; }
    int generalHfMethod() const { return ec_project_state_.projectGeneral.hf_meth; }
    int generalLfMethod() const { return ec_project_state_.projectGeneral.lf_meth; }
    int generalWplMeth() const { return ec_project_state_.projectGeneral.wpl_meth; }
    int generalFpMeth() const { return ec_project_state_.projectGeneral.foot_meth; }
    int generalTob1Format() const { return ec_project_state_.projectGeneral.tob1_format; }
    const QString& generalOutPath() const { return ec_project_state_.projectGeneral.out_path; }
    int generalFixedOutFormat() const { return ec_project_state_.projectGeneral.fix_out_format; }
    const QString& generalErroLabel() const { return ec_project_state_.projectGeneral.err_label; }
    int generalSubset() const { return ec_project_state_.projectGeneral.subset; }
    int generalQcfMeth() const { return ec_project_state_.projectGeneral.qcflag_meth; }
    int generalUseBiomet() const { return ec_project_state_.projectGeneral.use_biomet; }
    const QString& generalBiomFile() const { return ec_project_state_.projectGeneral.biom_file; }
    const QString& generalBiomDir() const { return ec_project_state_.projectGeneral.biom_dir; }
    int generalBiomRecurse() const { return ec_project_state_.projectGeneral.biom_recurse; }
    const QString& generalBiomExt() const { return ec_project_state_.projectGeneral.biom_ext; }
    int generalOutMeanCosp() const { return ec_project_state_.projectGeneral.out_mean_cosp; }
    int generalBinSpectraAvail() const { return ec_project_state_.projectGeneral.bin_sp_avail; }
    int generalFullSpectraAvail() const { return ec_project_state_.projectGeneral.full_sp_avail; }
    int generalFilesFound() const { return ec_project_state_.projectGeneral.files_found; }

    const QString& screenDataPath() const { return ec_project_state_.screenGeneral.data_path; }
    int screenRecurse() const { return ec_project_state_.screenGeneral.recurse; }
    bool screenUseGeoNorth() const { return ec_project_state_.screenGeneral.use_geo_north; }
    double screenMagDec() const { return ec_project_state_.screenGeneral.mag_dec; }
    const QString& screenDecDate() const { return ec_project_state_.screenGeneral.dec_date; }
    int screenNFiles() const { return ec_project_state_.screenSetting.nfiles; }
    int screenAvrgLen() const { return ec_project_state_.screenSetting.avrg_len; }
    int screenFlag1Col() const { return ec_project_state_.screenGeneral.flag1_col; }
    double screenFlag1Threshold() const { return ec_project_state_.screenGeneral.flag1_threshold; }
    int screenFlag1Upper() const { return ec_project_state_.screenGeneral.flag1_policy; }
    int screenFlag2Col() const { return ec_project_state_.screenGeneral.flag2_col; }
    double screenFlag2Threshold() const { return ec_project_state_.screenGeneral.flag2_threshold; }
    int screenFlag2Upper() const { return ec_project_state_.screenGeneral.flag2_policy; }
    int screenFlag3Col() const { return ec_project_state_.screenGeneral.flag3_col; }
    double screenFlag3Threshold() const { return ec_project_state_.screenGeneral.flag3_threshold; }
    int screenFlag3Upper() const { return ec_project_state_.screenGeneral.flag3_policy; }
    int screenFlag4Col() const { return ec_project_state_.screenGeneral.flag4_col; }
    double screenFlag4Threshold() const { return ec_project_state_.screenGeneral.flag4_threshold; }
    int screenFlag4Upper() const { return ec_project_state_.screenGeneral.flag4_policy; }
    int screenFlag5Col() const { return ec_project_state_.screenGeneral.flag5_col; }
    double screenFlag5Threshold() const { return ec_project_state_.screenGeneral.flag5_threshold; }
    int screenFlag5Upper() const { return ec_project_state_.screenGeneral.flag5_policy; }
    int screenFlag6Col() const { return ec_project_state_.screenGeneral.flag6_col; }
    double screenFlag6Threshold() const { return ec_project_state_.screenGeneral.flag6_threshold; }
    int screenFlag6Upper() const { return ec_project_state_.screenGeneral.flag6_policy; }
    int screenFlag7Col() const { return ec_project_state_.screenGeneral.flag7_col; }
    double screenFlag7Threshold() const { return ec_project_state_.screenGeneral.flag7_threshold; }
    int screenFlag7Upper() const { return ec_project_state_.screenGeneral.flag7_policy; }
    int screenFlag8Col() const { return ec_project_state_.screenGeneral.flag8_col; }
    double screenFlag8Threshold() const { return ec_project_state_.screenGeneral.flag8_threshold; }
    int screenFlag8Upper() const { return ec_project_state_.screenGeneral.flag8_policy; }
    int screenFlag9Col() const { return ec_project_state_.screenGeneral.flag9_col; }
    double screenFlag9Threshold() const { return ec_project_state_.screenGeneral.flag9_threshold; }
    int screenFlag9Upper() const { return ec_project_state_.screenGeneral.flag9_policy; }
    int screenFlag10Col() const { return ec_project_state_.screenGeneral.flag10_col; }
    double screenFlag10Threshold() const { return ec_project_state_.screenGeneral.flag10_threshold; }
    int screenFlag10Upper() const { return ec_project_state_.screenGeneral.flag10_policy; }

    int screenMaxLack() const { return ec_project_state_.screenSetting.max_lack; }
    double screenUOffset() const { return ec_project_state_.screenSetting.u_offset; }
    double screenVOffset() const { return ec_project_state_.screenSetting.v_offset; }
    double screenWOffset() const { return ec_project_state_.screenSetting.w_offset; }
    int screenFlowDistortion() const { return ec_project_state_.screenSetting.flow_distortion; }
    int screenCrossWind() const { return ec_project_state_.screenSetting.cross_wind; }
    int screenRotMethod() const { return ec_project_state_.screenSetting.rot_meth; }
    int screenDetrendMeth() const { return ec_project_state_.screenSetting.detrend_meth; }
    double screenTimeConst() const { return ec_project_state_.screenSetting.timeconst; }
    int screenTlagMeth() const { return ec_project_state_.screenSetting.tlag_meth; }
    int screenTapWin() const { return ec_project_state_.screenSetting.tap_win; }
    int screenNBins() const { return ec_project_state_.screenSetting.nbins; }
    int screenOutBinSpectra() const { return ec_project_state_.screenSetting.out_bin_sp; }
    int screenOutBinOgives() const { return ec_project_state_.screenSetting.out_bin_og; }
    int screenOutFullSpectraU() const { return ec_project_state_.screenSetting.out_full_sp_u; }
    int screenOutFullSpectraV() const { return ec_project_state_.screenSetting.out_full_sp_v; }
    int screenOutFullSpectraW() const { return ec_project_state_.screenSetting.out_full_sp_w; }
    int screenOutFullSpectraTs() const { return ec_project_state_.screenSetting.out_full_sp_ts; }
    int screenOutFullSpectraCo2() const { return ec_project_state_.screenSetting.out_full_sp_co2; }
    int screenOutFullSpectraH2o() const { return ec_project_state_.screenSetting.out_full_sp_h2o; }
    int screenOutFullSpectraCh4() const { return ec_project_state_.screenSetting.out_full_sp_ch4; }
    int screenOutFullSpectraN2o() const { return ec_project_state_.screenSetting.out_full_sp_n2o; }
    int screenOutSt1() const { return ec_project_state_.screenSetting.out_st_1; }
    int screenOutSt2() const { return ec_project_state_.screenSetting.out_st_2; }
    int screenOutSt3() const { return ec_project_state_.screenSetting.out_st_3; }
    int screenOutSt4() const { return ec_project_state_.screenSetting.out_st_4; }
    int screenOutSt5() const { return ec_project_state_.screenSetting.out_st_5; }
    int screenOutSt6() const { return ec_project_state_.screenSetting.out_st_6; }
    int screenOutSt7() const { return ec_project_state_.screenSetting.out_st_7; }
    int screenOutRaw1() const { return ec_project_state_.screenSetting.out_raw_1; }
    int screenOutRaw2() const { return ec_project_state_.screenSetting.out_raw_2; }
    int screenOutRaw3() const { return ec_project_state_.screenSetting.out_raw_3; }
    int screenOutRaw4() const { return ec_project_state_.screenSetting.out_raw_4; }
    int screenOutRaw5() const { return ec_project_state_.screenSetting.out_raw_5; }
    int screenOutRaw6() const { return ec_project_state_.screenSetting.out_raw_6; }
    int screenOutRaw7() const { return ec_project_state_.screenSetting.out_raw_7; }
    int screenOutRawU() const { return ec_project_state_.screenSetting.out_raw_u; }
    int screenOutRawV() const { return ec_project_state_.screenSetting.out_raw_v; }
    int screenOutRawW() const { return ec_project_state_.screenSetting.out_raw_w; }
    int screenOutRawTs() const { return ec_project_state_.screenSetting.out_raw_ts; }
    int screenOutRawCo2() const { return ec_project_state_.screenSetting.out_raw_co2; }
    int screenOutRawH2o() const { return ec_project_state_.screenSetting.out_raw_h2o; }
    int screenOutRawCh4() const { return ec_project_state_.screenSetting.out_raw_ch4; }
    int screenOutRawGas4() const { return ec_project_state_.screenSetting.out_raw_gas4; }
    int screenOutRawTair() const { return ec_project_state_.screenSetting.out_raw_tair; }
    int screenOutRawPair() const { return ec_project_state_.screenSetting.out_raw_pair; }
    int screenOutFullCospectraU() const { return ec_project_state_.screenSetting.out_full_cosp_u; }
    int screenOutFullCospectraV() const { return ec_project_state_.screenSetting.out_full_cosp_v; }
    int screenOutFullCospectraTs() const { return ec_project_state_.screenSetting.out_full_cosp_ts; }
    int screenOutFullCospectraCo2() const { return ec_project_state_.screenSetting.out_full_cosp_co2; }
    int screenOutFullCospectraH2o() const { return ec_project_state_.screenSetting.out_full_cosp_h2o; }
    int screenOutFullCospectraCh4() const { return ec_project_state_.screenSetting.out_full_cosp_ch4; }
    int screenOutFullCospectraN2o() const { return ec_project_state_.screenSetting.out_full_cosp_n2o; }
    int screenToMixRatio() const { return ec_project_state_.screenSetting.to_mixratio; }
    int screenFilterSr() const { return ec_project_state_.screenSetting.filter_sr; }
    int screenFilterAl() const { return ec_project_state_.screenSetting.filter_al; }
    int screenBuCorr() const { return ec_project_state_.screenSetting.bu_corr; }
    int screenBuMulti() const { return ec_project_state_.screenSetting.bu_multi; }
    double screenLDayBotGain() const { return ec_project_state_.screenSetting.l_day_bot_gain; }
    double screenLDayBotOffset() const { return ec_project_state_.screenSetting.l_day_bot_offset; }
    double screenLDayTopGain() const { return ec_project_state_.screenSetting.l_day_top_gain; }
    double screenLDayTopOffset() const { return ec_project_state_.screenSetting.l_day_top_offset; }
    double screenLDaySparGain() const { return ec_project_state_.screenSetting.l_day_spar_gain; }
    double screenLDaySparOffset() const { return ec_project_state_.screenSetting.l_day_spar_offset; }
    double screenLNightBotGain() const { return ec_project_state_.screenSetting.l_night_bot_gain; }
    double screenLNightBotOffset() const { return ec_project_state_.screenSetting.l_night_bot_offset; }
    double screenLNightTopGain() const { return ec_project_state_.screenSetting.l_night_top_gain; }
    double screenLNightTopOffset() const { return ec_project_state_.screenSetting.l_night_top_offset; }
    double screenLNightSparGain() const { return ec_project_state_.screenSetting.l_night_spar_gain; }
    double screenLNightSparOffset() const { return ec_project_state_.screenSetting.l_night_spar_offset; }
    double screenMDayBot1() const { return ec_project_state_.screenSetting.m_day_bot1; }
    double screenMDayBot2() const { return ec_project_state_.screenSetting.m_day_bot2; }
    double screenMDayBot3() const { return ec_project_state_.screenSetting.m_day_bot3; }
    double screenMDayBot4() const { return ec_project_state_.screenSetting.m_day_bot4; }
    double screenMDayTop1() const { return ec_project_state_.screenSetting.m_day_top1; }
    double screenMDayTop2() const { return ec_project_state_.screenSetting.m_day_top2; }
    double screenMDayTop3() const { return ec_project_state_.screenSetting.m_day_top3; }
    double screenMDayTop4() const { return ec_project_state_.screenSetting.m_day_top4; }
    double screenMDaySpar1() const { return ec_project_state_.screenSetting.m_day_spar1; }
    double screenMDaySpar2() const { return ec_project_state_.screenSetting.m_day_spar2; }
    double screenMDaySpar3() const { return ec_project_state_.screenSetting.m_day_spar3; }
    double screenMDaySpar4() const { return ec_project_state_.screenSetting.m_day_spar4; }
    double screenMNightBot1() const { return ec_project_state_.screenSetting.m_night_bot1; }
    double screenMNightBot2() const { return ec_project_state_.screenSetting.m_night_bot2; }
    double screenMNightBot3() const { return ec_project_state_.screenSetting.m_night_bot3; }
    double screenMNightBot4() const { return ec_project_state_.screenSetting.m_night_bot4; }
    double screenMNightTop1() const { return ec_project_state_.screenSetting.m_night_top1; }
    double screenMNightTop2() const { return ec_project_state_.screenSetting.m_night_top2; }
    double screenMNightTop3() const { return ec_project_state_.screenSetting.m_night_top3; }
    double screenMNightTop4() const { return ec_project_state_.screenSetting.m_night_top4; }
    double screenMNightSpar1() const { return ec_project_state_.screenSetting.m_night_spar1; }
    double screenMNightSpar2() const { return ec_project_state_.screenSetting.m_night_spar2; }
    double screenMNightSpar3() const { return ec_project_state_.screenSetting.m_night_spar3; }
    double screenMNightSpar4() const { return ec_project_state_.screenSetting.m_night_spar4; }
    int screenOutDetails() const { return ec_project_state_.screenSetting.out_details; }
    int screenOutSlowVars() const { return ec_project_state_.screenSetting.out_biomet; }
    int screenPowerOfTwo() const { return ec_project_state_.screenSetting.power_of_two; }

    int screenTestSr() const { return ec_project_state_.screenTest.test_sr; }
    int screenTestAr() const { return ec_project_state_.screenTest.test_ar; }
    int screenTestDo() const { return ec_project_state_.screenTest.test_do; }
    int screenTestAl() const { return ec_project_state_.screenTest.test_al; }
    int screenTestSk() const { return ec_project_state_.screenTest.test_sk; }
    int screenTestDs() const { return ec_project_state_.screenTest.test_ds; }
    int screenTestTl() const { return ec_project_state_.screenTest.test_tl; }
    int screenTestAa() const { return ec_project_state_.screenTest.test_aa; }
    int screenTestNs() const { return ec_project_state_.screenTest.test_ns; }

    int screenParamSrNumSpk() const { return ec_project_state_.screenParam.sr_num_spk; }
    double screenParamSrULim() const { return ec_project_state_.screenParam.sr_lim_u; }
    double screenParamSrWLim() const { return ec_project_state_.screenParam.sr_lim_w; }
    double screenParamSrCo2Lim() const { return ec_project_state_.screenParam.sr_lim_co2; }
    double screenParamSrH2oLim() const { return ec_project_state_.screenParam.sr_lim_h2o; }
    double screenParamSrCh4Lim() const { return ec_project_state_.screenParam.sr_lim_ch4; }
    double screenParamSrN2oLim() const { return ec_project_state_.screenParam.sr_lim_n2o; }
    double screenParamSrHfLim() const { return ec_project_state_.screenParam.sr_lim_hf; }

    double screenParamArLim() const { return ec_project_state_.screenParam.ar_lim; }
    int screenParamArBins() const { return ec_project_state_.screenParam.ar_bins; }
    int screenParamArHfLim() const { return ec_project_state_.screenParam.ar_hf_lim; }

    int screenParamDoExtLimDw() const { return ec_project_state_.screenParam.do_extlim_dw; }
    double screenParamDoHf1Lim() const { return ec_project_state_.screenParam.do_hf1_lim; }
    double screenParamDoHf2Lim() const { return ec_project_state_.screenParam.do_hf2_lim; }

    double screenParamAlUMax() const { return ec_project_state_.screenParam.al_u_max; }
    double screenParamAlWMax() const { return ec_project_state_.screenParam.al_w_max; }
    double screenParamAlTsonMin() const { return ec_project_state_.screenParam.al_tson_min; }
    double screenParamAlTsonMax() const { return ec_project_state_.screenParam.al_tson_max; }
    double screenParamAlCo2Min() const { return ec_project_state_.screenParam.al_co2_min; }
    double screenParamAlCo2Max() const { return ec_project_state_.screenParam.al_co2_max; }
    double screenParamAlH2oMin() const { return ec_project_state_.screenParam.al_h2o_min; }
    double screenParamAlH2oMax() const { return ec_project_state_.screenParam.al_h2o_max; }
    double screenParamAlCh4Min() const { return ec_project_state_.screenParam.al_ch4_min; }
    double screenParamAlCh4Max() const { return ec_project_state_.screenParam.al_ch4_max; }
    double screenParamAlN2oMin() const { return ec_project_state_.screenParam.al_n2o_min; }
    double screenParamAlN2oMax() const { return ec_project_state_.screenParam.al_n2o_max; }

    double screenParamSkHfSkmin() const { return ec_project_state_.screenParam.sk_hf_skmin; }
    double screenParamSkHfSkmax() const { return ec_project_state_.screenParam.sk_hf_skmax; }
    double screenParamSkSfSkmin() const { return ec_project_state_.screenParam.sk_sf_skmin; }
    double screenParamSkSfSkmax() const { return ec_project_state_.screenParam.sk_sf_skmax; }
    double screenParamSkHfKumin() const { return ec_project_state_.screenParam.sk_hf_kumin; }
    double screenParamSkHfKumax() const { return ec_project_state_.screenParam.sk_hf_kumax; }
    double screenParamSkSfKumin() const { return ec_project_state_.screenParam.sk_sf_kumin; }
    double screenParamSkSfKumax() const { return ec_project_state_.screenParam.sk_sf_kumax; }

    double screenParamDsHfUV() const { return ec_project_state_.screenParam.ds_hf_uv; }
    double screenParamDsHfW() const { return ec_project_state_.screenParam.ds_hf_w; }
    double screenParamDsHfT() const { return ec_project_state_.screenParam.ds_hf_t; }
    double screenParamDsHfCo2() const { return ec_project_state_.screenParam.ds_hf_co2; }
    double screenParamDsHfH2o() const { return ec_project_state_.screenParam.ds_hf_h2o; }
    double screenParamDsHfCh4() const { return ec_project_state_.screenParam.ds_hf_ch4; }
    double screenParamDsHfN2o() const { return ec_project_state_.screenParam.ds_hf_n2o; }
    double screenParamDsHfVar() const { return ec_project_state_.screenParam.ds_hf_var; }
    double screenParamDsSfUV() const { return ec_project_state_.screenParam.ds_sf_uv; }
    double screenParamDsSfW() const { return ec_project_state_.screenParam.ds_sf_w; }
    double screenParamDsSfT() const { return ec_project_state_.screenParam.ds_sf_t; }
    double screenParamDsSfCo2() const { return ec_project_state_.screenParam.ds_sf_co2; }
    double screenParamDsSfH2o() const { return ec_project_state_.screenParam.ds_sf_h2o; }
    double screenParamDsSfCh4() const { return ec_project_state_.screenParam.ds_sf_ch4; }
    double screenParamDsSfN2o() const { return ec_project_state_.screenParam.ds_sf_n2o; }
    double screenParamDsSfVar() const { return ec_project_state_.screenParam.ds_sf_var; }

    double screenParamTlHfLim() const { return ec_project_state_.screenParam.tl_hf_lim; }
    double screenParamTlSfLim() const { return ec_project_state_.screenParam.tl_sf_lim; }
    double screenParamTlDefCo2() const { return ec_project_state_.screenParam.tl_def_co2; }
    double screenParamTlDefH2o() const { return ec_project_state_.screenParam.tl_def_h2o; }
    double screenParamTlDefCh4() const { return ec_project_state_.screenParam.tl_def_ch4; }
    double screenParamTlDefN2o() const { return ec_project_state_.screenParam.tl_def_n2o; }

    double screenParamAaMin() const { return ec_project_state_.screenParam.aa_min; }
    double screenParamAaMax() const { return ec_project_state_.screenParam.aa_max; }
    double screenParamAaLim() const { return ec_project_state_.screenParam.aa_lim; }

    double screenParamNsHfLim() const { return ec_project_state_.screenParam.ns_hf_lim; }

    const QString& planarFitStartDate() const { return ec_project_state_.screenTilt.start_date; }
    const QString& planarFitEndDate() const { return ec_project_state_.screenTilt.end_date; }
    int planarFitMode() const { return ec_project_state_.screenTilt.mode; }
    double planarFitNorthOffset() const { return ec_project_state_.screenTilt.north_offset; }
    int planarFitItemPerSector() const { return ec_project_state_.screenTilt.min_num_per_sec; }
    double planarFitWmax() const { return ec_project_state_.screenTilt.w_max; }
    double planarFitUmin() const { return ec_project_state_.screenTilt.u_min; }
    const QString& planarFitFile() const { return ec_project_state_.screenTilt.file; }
    int planarFitFixPolicy() const { return ec_project_state_.screenTilt.fix_policy; }
    int planarFitSubset() const { return ec_project_state_.screenTilt.subset; }

    int spectraMode() const { return ec_project_state_.spectraSettings.sa_mode; }
    const QString& spectraStartDate() const { return ec_project_state_.spectraSettings.start_sa_date; }
    const QString& spectraEndDate() const { return ec_project_state_.spectraSettings.end_sa_date; }
    const QString& spectraFile() const { return ec_project_state_.spectraSettings.sa_file; }
    int spectraMinSmpl() const { return ec_project_state_.spectraSettings.sa_min_smpl; }
    int spectraHorst() const { return ec_project_state_.spectraSettings.horst_lens; }
    double spectraFminCo2() const { return ec_project_state_.spectraSettings.sa_fmin_co2; }
    double spectraFminH2o() const { return ec_project_state_.spectraSettings.sa_fmin_h2o; }
    double spectraFminCh4() const { return ec_project_state_.spectraSettings.sa_fmin_ch4; }
    double spectraFminGas4() const { return ec_project_state_.spectraSettings.sa_fmin_gas4; }
    double spectraFmaxCo2() const { return ec_project_state_.spectraSettings.sa_fmax_co2; }
    double spectraFmaxH2o() const { return ec_project_state_.spectraSettings.sa_fmax_h2o; }
    double spectraFmaxCh4() const { return ec_project_state_.spectraSettings.sa_fmax_ch4; }
    double spectraFmaxGas4() const { return ec_project_state_.spectraSettings.sa_fmax_gas4; }
    double spectraHfnCo2() const { return ec_project_state_.spectraSettings.sa_hfn_co2_fmin; }
    double spectraHfnH2o() const { return ec_project_state_.spectraSettings.sa_hfn_h2o_fmin; }
    double spectraHfnCh4() const { return ec_project_state_.spectraSettings.sa_hfn_ch4_fmin; }
    double spectraHfnGas4() const { return ec_project_state_.spectraSettings.sa_hfn_gas4_fmin; }
    int spectraAddSonic() const { return ec_project_state_.spectraSettings.add_sonic_lptf; }
    double spectraMinCo2() const { return ec_project_state_.spectraSettings.sa_min_co2; }
    double spectraMinCh4() const { return ec_project_state_.spectraSettings.sa_min_ch4; }
    double spectraMinGas4() const { return ec_project_state_.spectraSettings.sa_min_gas4; }
    double spectraMinLe() const { return ec_project_state_.spectraSettings.sa_min_le; }
    double spectraMinH() const { return ec_project_state_.spectraSettings.sa_min_h; }
    double spectraF10Co2() const { return ec_project_state_.spectraSettings.f10_co2_trshld; }
    double spectraF10Ch4() const { return ec_project_state_.spectraSettings.f10_ch4_trshld; }
    double spectraF10Gas4() const { return ec_project_state_.spectraSettings.f10_gas4_trshld; }
    double spectraF10Le() const { return ec_project_state_.spectraSettings.f10_le_trshld; }
    double spectraF10H() const { return ec_project_state_.spectraSettings.f10_h_trshld; }
    const QString& spectraExFile() const { return ec_project_state_.spectraSettings.ex_file; }
    const QString& spectraBinSpectra() const { return ec_project_state_.spectraSettings.sa_bin_spectra; }
    const QString& spectraFullSpectra() const { return ec_project_state_.spectraSettings.sa_full_spectra; }
    const QString& spectraExDir() const { return ec_project_state_.spectraSettings.ex_dir; }
    int spectraSubset() const { return ec_project_state_.spectraSettings.subset; }

    const QString& timelagOptStartDate() const { return ec_project_state_.timelagOpt.start_date; }
    const QString& timelagOptEndDate() const { return ec_project_state_.timelagOpt.end_date; }
    int timelagOptMode() const { return ec_project_state_.timelagOpt.mode; }
    const QString& timelagOptFile() const { return ec_project_state_.timelagOpt.file; }
    int timelagOptH2oNClass() const { return ec_project_state_.timelagOpt.to_h2o_nclass; }
    double timelagOptCo2MinFlux() const { return ec_project_state_.timelagOpt.co2_min_flux; }
    double timelagOptCh4MinFlux() const { return ec_project_state_.timelagOpt.ch4_min_flux; }
    double timelagOptGas4MinFlux() const { return ec_project_state_.timelagOpt.gas4_min_flux; }
    double timelagOptLeMinFlux() const { return ec_project_state_.timelagOpt.le_min_flux; }
    double timelagOptPgRange() const { return ec_project_state_.timelagOpt.pg_range; }
    double timelagOptCo2MinLag() const { return ec_project_state_.timelagOpt.co2_min_lag; }
    double timelagOptCo2MaxLag() const { return ec_project_state_.timelagOpt.co2_max_lag; }
    double timelagOptH2oMinLag() const { return ec_project_state_.timelagOpt.h2o_min_lag; }
    double timelagOptH2oMaxLag() const { return ec_project_state_.timelagOpt.h2o_max_lag; }
    double timelagOptCh4MinLag() const { return ec_project_state_.timelagOpt.ch4_min_lag; }
    double timelagOptCh4MaxLag() const { return ec_project_state_.timelagOpt.ch4_max_lag; }
    double timelagOptGas4MinLag() const { return ec_project_state_.timelagOpt.gas4_min_lag; }
    double timelagOptGas4MaxLag() const { return ec_project_state_.timelagOpt.gas4_max_lag; }
    int timelagOptSubset() const { return ec_project_state_.timelagOpt.subset; }

    int randErrorMethod() const { return ec_project_state_.randomError.method; }
    int randErrorItsMehod() const { return ec_project_state_.randomError.its_method; }
    double randErrorTlagMax() const { return ec_project_state_.randomError.its_tlag_max; }
    double randErrorSecFactor() const { return ec_project_state_.randomError.its_sec_factor; }

    int biomParamColAirT() const { return ec_project_state_.biomParam.col_ta; }
    int biomParamColAirP() const { return ec_project_state_.biomParam.col_pa; }
    int biomParamColRh() const { return ec_project_state_.biomParam.col_rh; }
    int biomParamColRg() const { return ec_project_state_.biomParam.col_rg; }
    int biomParamColLwin() const { return ec_project_state_.biomParam.col_lwin; }
    int biomParamColPpfd() const { return ec_project_state_.biomParam.col_ppfd; }

    bool planarFitSectorDefined();

    void addPlanarFitAngle(const AngleItem& angle);

    // is the project modified?
    bool modified() const;

    QList<AngleItem>* planarFitAngles();
    bool hasPlanarFitFullAngle();

//    SpecGroupList *specGroups();
//    void addSpecGroup(const SpecGroup &sg);

    const QString getFilenamePrototype() const;
    bool isEngineStep2Needed();

public slots:
    void setModified(bool mod);

signals:
    // send that the project has been created
    void ecProjectNew();
    // send that the project has been loaded
    void ecProjectChanged();
    // send that the project has been modified
    void ecProjectModified();

    void updateInfo();

private:
    bool modified_;
    EcProjectState ec_project_state_;
    ProjConfigState project_config_state_;

    // insert tag for native format files
    bool tagProject(const QString &filename);

    bool previousFileNameCompare(const QString &currentPath, const QString &previousPath);
    bool previousSettingsCompare(bool current, bool previous);
    bool previousFourthGasCompare(int currentGas, double currGasMw, double currGasDiff,
                                  int previousGas, double previousGasMw, double previousGasDiff);
    bool compareDates(const QString& currStartDate, const QString& prevStartDate,
                      const QString& currStartTime, const QString& prevStartTime,
                      const QString& currEndDate, const QString& prevEndDate,
                      const QString& currEndTime, const QString& prevEndTime);

    int countPlanarFitAngles(const QStringList& list);
};

#endif // ECPROJECT_H
