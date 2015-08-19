/***************************************************************************
  advspectraloptions.h
  -------------------
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

class QwwButtonLineEdit;

class AncillaryFileTest;
class ClickLabel;
class DlProject;
class EcProject;

class AdvSpectralOptions : public QWidget
{
    Q_OBJECT
public:
    explicit AdvSpectralOptions(QWidget* parent,
                                DlProject* dlProject,
                                EcProject* ecProject,
                                ConfigState* config);
    ~AdvSpectralOptions();

    void setSmartfluxUI();

signals:
    void updateOutputsRequest(int n);

public slots:
    void reset();

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void refresh();
    void updateSpectraFile(const QString& fp);
    void updateBinnedSpectraFile(const QString& fp);
    void updateFullSpectraFile(const QString& fp);
    void spectraFileLoad_clicked();
    void binnedSpectraDirBrowse_clicked();
    void fullSpectraDirBrowse_clicked();
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
    void onMinLeLabelClicked();
    void onMinHLabelClicked();

    void updateMinCo2(double d);
    void updateMinCh4(double d);
    void updateMinGas4(double d);
    void updateMinLe(double d);
    void updateMinH(double d);

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

    void updateF10Co2(double d);
    void updateF10Ch4(double d);
    void updateF10Gas4(double d);
    void updateF10Le(double d);
    void updateF10H(double d);

    void onF10Co2LabelClicked();
    void onF10Ch4LabelClicked();
    void onF10Gas4LabelClicked();
    void onF10LeLabelClicked();
    void onF10HLabelClicked();

    void onSpin10LabelClicked();
    void onSpin20LabelClicked();
    void onSpin30LabelClicked();

    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate& d);
    void updateEndDate(const QDate& d);

    void onMinSmplLabelClicked();
    void updateMinSmpl(int n);

    void onlineHelpTrigger_1();
    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();

    void updateTooltip(int i);
    void clearSpectraFileEdit();
    void clearBinnedSpectraDirEdit();
    void clearFullSpectraDirEdit();

    void updateSubsetSelection(bool b);
    void updateSuggestedFrequencyRanges();

private:
    bool isHorstIbromFratini();
    bool isIbrom();
    bool isFratini();
    void forceEndDatePolicy();

    void createQuestionMark();

    double getLowestFrequencyValue();
    double getHighestFrequencyValue();

    void setHfMethod(int hfMethComboIndex);

    QRadioButton* spectraExistingRadio;
    QRadioButton* spectraNonExistingRadio;
    QRadioButton* binnedSpectraExistingRadio;
    QRadioButton* binnedSpectraNonExistingRadio;
    QCheckBox* subsetCheckBox;
    ClickLabel* startDateLabel;
    QDateEdit* startDateEdit;
    QLabel* lockedIcon;
    ClickLabel* endDateLabel;
    QDateEdit* endDateEdit;
    QwwButtonLineEdit* spectraFileEdit;
    QPushButton* spectraFileLoad;
    QButtonGroup* spectraRadioGroup;
    QwwButtonLineEdit* binnedSpectraDirEdit;
    QPushButton* binnedSpectraDirBrowse;
    QButtonGroup* binnedSpectraRadioGroup;
    QCheckBox* lfMethodCheck;
    QCheckBox* hfMethodCheck;
    ClickLabel* hfMethLabel;
    QComboBox* hfMethCombo;
    QCheckBox* horstCheck;
    ClickLabel* horstMethodLabel;
    QComboBox* horstCombo;
    ClickLabel* minSmplLabel;
    QSpinBox* minSmplSpin;
    ClickLabel* minSpinCo2Label;
    ClickLabel* minSpinCh4Label;
    ClickLabel* minSpinGas4Label;
    ClickLabel* minSpinLeLabel;
    ClickLabel* minSpinHLabel;
    QDoubleSpinBox* minSpinCo2;
    QDoubleSpinBox* minSpinCh4;
    QDoubleSpinBox* minSpinGas4;
    QDoubleSpinBox* minSpinLe;
    QDoubleSpinBox* minSpinH;
    QLabel* spin11Label;
    QLabel* spin12Label;
    QLabel* spin13Label;
    QLabel* spin14Label;
    ClickLabel* spin10Label;
    ClickLabel* spin20Label;
    ClickLabel* spin30Label;
    QLabel* minMaxFreqLabel;
    QLabel* noiseFreqLabel;
    QLabel* thresholdLabel;
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
    QwwButtonLineEdit* fullSpectraDirEdit;
    QPushButton* fullSpectraDirBrowse;
    QButtonGroup* fullSpectraRadioGroup;
    QLabel* fratiniTitle;
    QCheckBox* addSonicCheck;
    ClickLabel* f10Spin1Label;
    ClickLabel* f10Spin2Label;
    ClickLabel* f10Spin3Label;
    ClickLabel* f10Spin4Label;
    ClickLabel* f10Spin5Label;
    QDoubleSpinBox* f10Co2Spin;
    QDoubleSpinBox* f10Ch4Spin;
    QDoubleSpinBox* f10Gas4Spin;
    QDoubleSpinBox* f10LeSpin;
    QDoubleSpinBox* f10HSpin;

    QLabel* settingsGroupTitle;
    QLabel* lowFreqTitle;
    QLabel* highFreqTitle;
    QLabel* freqAttenuationTitle;

    QPushButton* questionMark_1;
    QPushButton* questionMark_2;
    QPushButton* questionMark_3;
    QPushButton* questionMark_4;

    QPushButton* suggestedRangeButton;

    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;

    AncillaryFileTest* test_ {};
    bool spectraNonExistingRadioOldEnabled = false;
};

#endif // ADVSPECTRALOPTIONS_H
