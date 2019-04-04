/***************************************************************************
  advspectraloptions.h
  --------------------
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef ADVSPECTRALOPTIONS_H
#define ADVSPECTRALOPTIONS_H

#include <QWidget>

#include "configstate.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDate;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTimeEdit;

class AncillaryFileTest;
class ClickLabel;
class CustomClearLineEdit;
class DirBrowseWidget;
class DlProject;
class EcProject;
class FileBrowseWidget;

class AdvSpectralOptions : public QWidget
{
    Q_OBJECT
public:
    AdvSpectralOptions(QWidget* parent,
                       DlProject* dlProject,
                       EcProject* ecProject,
                       ConfigState* config);

    void setSmartfluxUI();

signals:
    void updateOutputsRequest(int n);

public slots:
    void reset();
    void partialRefresh();

private slots:
    void refresh();
    void updateSpectraFile(const QString& fp);
    void updateBinnedSpectraFile(const QString& fp);
    void updateFullSpectraFile(const QString& fp);
    void testSelectedSpectraFile(const QString &fp);
    void binnedSpectraDirSelected(const QString &dir_path);
    void fullSpectraDirSelected(const QString &dir_path);
    void spectraRadioClicked(int radioButton);
    void binnedSpectraRadioClicked(int radioButton);
    void fullSpectraRadioClicked(int radioButton);

    void onClickHfMethLabel();
    void updateHfMethod_1(bool b);
    void updateHfMethod_2(int n);

    void onClickHorstLabel();
    void updateHorst_1(bool b);
    void updateHorst_2(int n);

    void onMinCo2LabelClicked();
    void onMinCh4LabelClicked();
    void onMinGas4LabelClicked();

    void updateFminCo2(double d);
    void updateFminH2o(double d);
    void updateFminCh4(double d);
    void updateFminGas4(double d);
    void updateFmaxCo2(double d);
    void updateFmaxH2o(double d);
    void updateFmaxCh4(double d);
    void updateFmaxGas4(double d);
    void updateHfnCo2(double d);
    void updateHfnH2o(double d);
    void updateHfnCh4(double d);
    void updateHfnGas4(double d);

    void onSpin10LabelClicked();
    void onSpin20LabelClicked();
    void onSpin30LabelClicked();

    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate& d);
    void updateStartTime(const QTime& t);
    void updateEndDate(const QDate& d);
    void updateEndTime(const QTime& t);

    void onMinSmplLabelClicked();
    void updateMinSmpl(int n);

    void onlineHelpTrigger_11();
    void onlineHelpTrigger_1();
    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();

    void updateTooltip(int i);

    void onSubsetCheckboxToggled(bool b);

    void updateFilter(int n);
    void updateNBins(int n);

private:
    bool isHorstIbromFratini();
    bool isIbrom();
    bool isFratini();
    void forceEndDatePolicy();
    void forceEndTimePolicy();

    void createQuestionMarks();
    void setHfMethod(int hfMethComboIndex);

    QCheckBox* vmFlagsCheckBox;
    QCheckBox* lowQualityCheckBox;
    QCheckBox* moderateQualityCheckBox;

    ClickLabel* filterLabel;
    QComboBox* filterCombo;
    ClickLabel* nBinsLabel;
    QSpinBox* nBinsSpin;
    QCheckBox* fftCheckBox;

    QRadioButton* spectraExistingRadio;
    QRadioButton* spectraNonExistingRadio;
    QRadioButton* binnedSpectraExistingRadio;
    QRadioButton* binnedSpectraNonExistingRadio;
    QCheckBox* subsetCheckBox;
    ClickLabel* startDateLabel;
    QDateEdit* startDateEdit;
    QTimeEdit* startTimeEdit;
    QLabel* lockedIcon;
    ClickLabel* endDateLabel;
    QDateEdit* endDateEdit;
    QTimeEdit* endTimeEdit;
    FileBrowseWidget* spectraFileBrowse;
    QButtonGroup* spectraRadioGroup;
    DirBrowseWidget* binnedSpectraDirBrowse;
    QButtonGroup* binnedSpectraRadioGroup;
    QCheckBox* lfMethodCheck;
    QCheckBox* hfMethodCheck;
    ClickLabel* hfMethLabel;
    QComboBox* hfMethCombo;
    QCheckBox* horstCheck;
    ClickLabel* horstMethodLabel;
    QComboBox* horstCombo;
    QCheckBox* hfCorrectGhgBaCheck;
    QCheckBox* hfCorrectGhgZohCheck;
    ClickLabel* sonicFrequencyLabel;
    QSpinBox* sonicFrequency;
    ClickLabel* minSmplLabel;
    QSpinBox* minSmplSpin;
    ClickLabel* minUnstableLabel;
    ClickLabel* minStableLabel;
    ClickLabel* maxLabel;
    QLabel* spin31Label;
    QLabel* spin32Label;
    QLabel* spin33Label;
    QLabel* spin34Label;
    QDoubleSpinBox* qcMinUnstableUstarSpin;
    QDoubleSpinBox* qcMinUnstableHSpin;
    QDoubleSpinBox* qcMinUnstableLESpin;
    QDoubleSpinBox* qcMinUnstableCo2Spin;
    QDoubleSpinBox* qcMinUnstableCh4Spin;
    QDoubleSpinBox* qcMinUnstableGas4Spin;
    QDoubleSpinBox* qcMinStableUstarSpin;
    QDoubleSpinBox* qcMinStableHSpin;
    QDoubleSpinBox* qcMinStableLESpin;
    QDoubleSpinBox* qcMinStableCo2Spin;
    QDoubleSpinBox* qcMinStableCh4Spin;
    QDoubleSpinBox* qcMinStableGas4Spin;
    QDoubleSpinBox* qcMaxUstarSpin;
    QDoubleSpinBox* qcMaxHSpin;
    QDoubleSpinBox* qcMaxLESpin;
    QDoubleSpinBox* qcMaxCo2Spin;
    QDoubleSpinBox* qcMaxCh4Spin;
    QDoubleSpinBox* qcMaxGas4Spin;
    QLabel* spin11Label;
    QLabel* spin12Label;
    QLabel* spin13Label;
    QLabel* spin14Label;
    ClickLabel* spin10Label;
    ClickLabel* spin20Label;
    ClickLabel* spin30Label;
    QLabel* minMaxFreqLabel;
    QLabel* noiseFreqLabel;
    QDoubleSpinBox* spin11;
    QDoubleSpinBox* spin12;
    QDoubleSpinBox* spin13;
    QDoubleSpinBox* spin14;
    QDoubleSpinBox* spin21;
    QDoubleSpinBox* spin22;
    QDoubleSpinBox* spin23;
    QDoubleSpinBox* spin24;
    QDoubleSpinBox* spin31;
    QDoubleSpinBox* spin32;
    QDoubleSpinBox* spin33;
    QDoubleSpinBox* spin34;
    QRadioButton* fullSpectraExistingRadio;
    QRadioButton* fullSpectraNonExistingRadio;
    DirBrowseWidget* fullSpectraDirBrowse;
    QButtonGroup* fullSpectraRadioGroup;
    QLabel* fratiniTitle;
    QCheckBox* addSonicCheck;

    ClickLabel* frictionVelocityLabel;
    ClickLabel* sensibleHeatLabel;
    ClickLabel* latentHeatLabel;
    ClickLabel* qcCo2Label;
    ClickLabel* qcCh4Label;
    ClickLabel* qcGas4Label;

    QLabel* settingsGroupTitle_1;
    QLabel* lowFreqTitle;
    QLabel* highFreqTitle;
    QLabel* ghgSystemCorrectionTitle;
    QLabel* freqAttenuationTitle;

    QPushButton* questionMark_1;
    QPushButton* questionMark_2;
    QPushButton* questionMark_3;
    QPushButton* questionMark_4;
    QPushButton* questionMark_5;
    QPushButton* questionMark_6;

    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;

    bool spectraNonExistingRadioOldEnabled = false;
};

#endif // ADVSPECTRALOPTIONS_H
