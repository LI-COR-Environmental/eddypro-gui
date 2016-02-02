/***************************************************************************
  advprocessingoptions.h
  -------------------
  Copyright (C) 2007-2011 Eco2s team. Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
  All rights reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef ADVPROCESSINGOPTIONS_H
#define ADVPROCESSINGOPTIONS_H

#include <QWidget>

////////////////////////////////////////////////////////////////////////////////
/// \file src/advprocessingoptions.h
/// \brief
/// \version
/// \date
/// \author Antonio Forgione
/// \note
/// \sa PreProcessing
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStackedWidget;
class QTabWidget;

class CalibrationDialog;
class ClickLabel;
struct ConfigState;
class CustomResetLineEdit;
class DlProject;
class EcProject;
class PlanarFitSettingsDialog;
class RichTextCheckBox;
class TimeLagSettingsDialog;

#include "calibration.h"
#include "calibrationapi.h"
#include "calibrationinfo.h"

/// \class AdvlProcessingOptions
/// \brief Class representing the 'Settings' tab in the 'RawProcess' page
class AdvProcessingOptions : public QWidget
{
    Q_OBJECT

public:
    AdvProcessingOptions(QWidget* parent,
                         DlProject* dlProject,
                         EcProject* ecProject,
                         ConfigState* config);
    ~AdvProcessingOptions();

    PlanarFitSettingsDialog* getPlanarFitSettingsDialog() { return pfDialog_; }
    TimeLagSettingsDialog* getTimeLagSettingsDialog() { return tlDialog_; }
    CalibrationDialog* getCalibrationDialog() { return calibDialog_; }

public slots:
    void reset();

private slots:
    void updateUOffset(double d);
    void updateVOffset(double d);
    void updateWOffset(double d);
    void updateAoaMethod_1(bool b);
    void updateAoaMethod_2(int n);
    void updateRotMethod_1(bool b);
    void updateRotMethod_2(int n);
    void updatePfSettingsButton(bool b);
    void updateDetrendMeth(int l);
    void updateTimeConst(double l);
    void updateTlagMeth_1(bool b);
    void updateTlagMeth_2(int n);
    void updateTlSettingsButton(bool b);

    void onClickDetrendCombo(int detrendMethod);
    void onClickDetrendLabel();
    void onClickTimeConstantLabel();
    void updateWBoost(bool b);
    void onClickAoaMethLabel();
    void onClickRotMethLabel();
    void onClickTimeLagMethLabel();
    void onULabelClicked();
    void onVLabelClicked();
    void onWLabelClicked();

    void onClickQcMethodLabel();
    void updateQcMeth_1(bool b);
    void updateQcMeth_2(int n);
    void onClickFpMethodLabel();
    void updateFpMeth_1(bool b);
    void updateFpMeth_2(int n);

    void updateWplMeth_1(bool b);
    void updateBurbaGroup(bool b);
    void updateBurbaType_2(int n);
    void enableBurbaCorrectionArea(bool b);

    void on_setDefaultsButton_clicked();

    void refresh();

    void showPfSettingsDialog();
    void showTlSettingsDialog();
    void showCalibDialog();

    void updateTooltip(int i);

    void onlineHelpTrigger_1();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_11();

    void fetchCalibration();
    void parseCalibrationInfo(const QByteArray &data);
    void parseCalibrationFile();

private:
    enum class DetrendMethod {
        BlockAverage,
        LinearDetrending,
        RunningMean,
        ExponentialRunningMean
    };

    void createPfSettingsDialog();
    void createTlSettingsDialog();
    void createCalibDialog();
    void createBurbaParamItems();
    void createQuestionMark();
    bool requestBurbaSettingsReset();
    void setBurbaDefaultValues();

    void computeInverseCoefficients(Calibration &cal);

    QLabel* windOffsetLabel;
    ClickLabel* uLabel;
    ClickLabel* vLabel;
    ClickLabel* wLabel;
    QDoubleSpinBox* uOffsetSpin;
    QDoubleSpinBox* vOffsetSpin;
    QDoubleSpinBox* wOffsetSpin;
    RichTextCheckBox* wBoostCheckBox;
    RichTextCheckBox* aoaCheckBox;
    ClickLabel* aoaMethLabel;
    QComboBox* aoaMethCombo;
    RichTextCheckBox* rotCheckBox;
    ClickLabel* rotMethLabel;
    QComboBox* rotMethCombo;
    QPushButton* pfSettingsButton;
    QLabel* detrendLabel;
    ClickLabel* detrendMethLabel;
    QComboBox* detrendCombo;
    RichTextCheckBox* timeLagCheckBox;
    ClickLabel* timeConstantLabel;
    QDoubleSpinBox* timeConstantSpin;
    ClickLabel* timeLagMethodLabel;
    QComboBox* timeLagMethodCombo;
    QPushButton* tlSettingsButton;
    RichTextCheckBox* qcCheckBox;
    ClickLabel* qcLabel;
    QComboBox* qcMethodCombo;
    RichTextCheckBox* fpCheckBox;
    ClickLabel* fpLabel;
    QComboBox* fpMethodCombo;

    RichTextCheckBox* wplCheckBox;
    RichTextCheckBox* burbaCorrCheckBox;
    ClickLabel* burbaTypeLabel;
    QRadioButton* burbaSimpleRadio;
    QRadioButton* burbaMultiRadio;
    QPushButton* setDefaultsButton;
    QButtonGroup* burbaRadioGroup;
    QWidget* burbaSimpleDay;
    QWidget* burbaSimpleNight;
    QWidget* burbaMultiDay;
    QWidget* burbaMultiNight;
    QTabWidget* burbaSimpleTab;
    QTabWidget* burbaMultiTab;
    QStackedWidget* burbaParamWidget;

    CustomResetLineEdit* lDayBotGain;
    CustomResetLineEdit* lDayBotOffset;
    CustomResetLineEdit* lDayTopGain;
    CustomResetLineEdit* lDayTopOffset;
    CustomResetLineEdit* lDaySparGain;
    CustomResetLineEdit* lDaySparOffset;
    CustomResetLineEdit* lNightBotGain;
    CustomResetLineEdit* lNightBotOffset;
    CustomResetLineEdit* lNightTopGain;
    CustomResetLineEdit* lNightTopOffset;
    CustomResetLineEdit* lNightSparGain;
    CustomResetLineEdit* lNightSparOffset;

    CustomResetLineEdit* mDayBot1;
    CustomResetLineEdit* mDayBot2;
    CustomResetLineEdit* mDayBot3;
    CustomResetLineEdit* mDayBot4;
    CustomResetLineEdit* mDayTop1;
    CustomResetLineEdit* mDayTop2;
    CustomResetLineEdit* mDayTop3;
    CustomResetLineEdit* mDayTop4;
    CustomResetLineEdit* mDaySpar1;
    CustomResetLineEdit* mDaySpar2;
    CustomResetLineEdit* mDaySpar3;
    CustomResetLineEdit* mDaySpar4;
    CustomResetLineEdit* mNightBot1;
    CustomResetLineEdit* mNightBot2;
    CustomResetLineEdit* mNightBot3;
    CustomResetLineEdit* mNightBot4;
    CustomResetLineEdit* mNightTop1;
    CustomResetLineEdit* mNightTop2;
    CustomResetLineEdit* mNightTop3;
    CustomResetLineEdit* mNightTop4;
    CustomResetLineEdit* mNightSpar1;
    CustomResetLineEdit* mNightSpar2;
    CustomResetLineEdit* mNightSpar3;
    CustomResetLineEdit* mNightSpar4;

    QRadioButton* noDriftCorrectionRadio;
    QRadioButton* linearDriftCorrectionRadio;
    QRadioButton* rssiDriftCorrectionRadio;
    QButtonGroup* driftCorrectionRadioGroup;
    QPushButton* retrieveCalibrationButton;
    QPushButton* editCalibrationButton;
    QLineEdit* serialNumberEdit;

    QPushButton* questionMark_1;
    QPushButton* questionMark_4;
    QPushButton* questionMark_11;

    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;

    PlanarFitSettingsDialog* pfDialog_{};
    TimeLagSettingsDialog* tlDialog_{};
    CalibrationDialog* calibDialog_{};

    DetrendMethod previousDetrendMethod_{DetrendMethod::BlockAverage};

    CalibrationAPI* calibration_api_;
    CalibrationInfo calibration_info_;
    QString calibration_file_;
    Calibration calibration_;
};

#endif // ADVPROCESSINGOPTIONS_H
