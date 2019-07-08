/***************************************************************************
  advstatisticaloptions.h
  -----------------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
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

#ifndef ADVSTATISTICALOPTIONS_H
#define ADVSTATISTICALOPTIONS_H

#include <QWidget>

////////////////////////////////////////////////////////////////////////////////
/// \file src/advstatisticaloptions.h
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

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGroupBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabBar;
class QToolBox;

class ClickLabel;
class EcProject;

/// \class AdvStatisticalOptions
/// \brief Class representing the 'Statisticals Tests' tab in the 'RawProcess' page
class AdvStatisticalOptions : public QWidget
{
    Q_OBJECT

public:
    AdvStatisticalOptions(QWidget* parent, EcProject* project);

public slots:
    void reset();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void refresh();

    void on_spikeRemCheckBox_clicked(bool checked);
    void on_amplitudeResCheckBox_clicked(bool checked);
    void on_dropoutsCheckBox_clicked(bool checked);
    void on_absLimCheckBox_clicked(bool checked);
    void on_skewnessCheckBox_clicked(bool checked);
    void on_discontCheckBox_clicked(bool checked);
    void on_timeLagCheckBox_clicked(bool checked);
    void on_attackAngleCheckBox_clicked(bool checked);
    void on_nonSteadyCheckBox_clicked(bool checked);

    void on_defaultValuesButton_clicked();

    void selectAllTest(bool b);
    void updateSelectAllCheckbox();
    void updateRestoreDefault();

    void updateTestSr(bool b);
    void updateTestAr(bool b);
    void updateTestDo(bool b);
    void updateTestAl(bool b);
    void updateTestSk(bool b);
    void updateTestDs(bool b);
    void updateTestTl(bool b);
    void updateTestAa(bool b);
    void updateTestNs(bool b);

    void updateParamSrNumSpk(int n);
    void updateParamSrULim(double n);
    void updateParamSrWLim(double n);
    void updateParamSrCo2Lim(double n);
    void updateParamSrH2oLim(double n);
    void updateParamSrCh4Lim(double n);
    void updateParamSrN2oLim(double n);
    void updateParamSrHfLim(double n);
    void updateDespFilter(bool b);

    void updateParamArLim(double n);
    void updateParamArBins(int n);
    void updateParamArHfLim(int n);

    void updateParamDoExtLimDw(int n);
    void updateParamDoHf1Lim(double n);
    void updateParamDoHf2Lim(double n);

    void updateParamAlUMax(double n);
    void updateParamAlWMax(double n);
    void updateParamAlUMin(double n);
    void updateParamAlWMin(double n);
    void updateParamAlTsonMin(double n);
    void updateParamAlTsonMax(double n);
    void updateParamAlCo2Min(double n);
    void updateParamAlCo2Max(double n);
    void updateParamAlH2oMin(double n);
    void updateParamAlH2oMax(double n);
    void updateParamAlCh4Min(double n);
    void updateParamAlCh4Max(double n);
    void updateParamAlN2oMin(double n);
    void updateParamAlN2oMax(double n);
    void updateAbsLimFilter(bool b);

    void updateParamSkHfSkmin(double n);
    void updateParamSkHfSkmax(double n);
    void updateParamSkSfSkmin(double n);
    void updateParamSkSfSkmax(double n);
    void updateParamSkHfKumin(double n);
    void updateParamSkHfKumax(double n);
    void updateParamSkSfKumin(double n);
    void updateParamSkSfKumax(double n);

    void updateParamDsHfUV(double n);
    void updateParamDsHfW(double n);
    void updateParamDsHfT(double n);
    void updateParamDsHfCo2(double n);
    void updateParamDsHfH2o(double n);
    void updateParamDsHfCh4(double n);
    void updateParamDsHfN2o(double n);
    void updateParamDsHfVar(double n);
    void updateParamDsSfUV(double n);
    void updateParamDsSfW(double n);
    void updateParamDsSfT(double n);
    void updateParamDsSfCo2(double n);
    void updateParamDsSfH2o(double n);
    void updateParamDsSfCh4(double n);
    void updateParamDsSfN2o(double n);
    void updateParamDsSfVar(double n);

    void updateParamTlHfLim(double n);
    void updateParamTlSfLim(double n);
    void updateParamTlDefCo2(double n);
    void updateParamTlDefH2o(double n);
    void updateParamTlDefCh4(double n);
    void updateParamTlDefN2o(double n);

    void updateParamAaMin(double n);
    void updateParamAaMax(double n);
    void updateParamAaLim(double n);

    void updateParamNsHfLim(double n);

    void updateThumbnailGraphLabel(int i);

    void onlineHelpTrigger_1();
    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();
    void onlineHelpTrigger_6();
    void onlineHelpTrigger_7();
    void onlineHelpTrigger_8();
    void onlineHelpTrigger_9();
    void onlineHelpTrigger_10();
    void onlineHelpTrigger_11();

    void updateRandomErrorArea(bool b);
    void onClickRandomMethodLabel();
    void updateRandomMethod(int n);
    void onClickItsDefinitionLabel();
    void updateItsDefinition(int n);
    void onTimelagMaxLabelCLicked();
    void updateTimelagMax(double d);
    void onSecurityCoeffLabelCLicked();
    void updateSecurityCoeff(double d);

    void onClickDespLabel_1();
    void onClickDespLabel_2();
    void onClickDespLabel_3();
    void onClickDespLabel_4();
    void onClickDespLabel_5();
    void onClickDespLabel_6();
    void onClickDespLabel_7();
    void onClickDespLabel_8();

    void onClickAmplResLabel_1();
    void onClickAmplResLabel_2();
    void onClickAmplResLabel_3();

    void onClickDropoutsLabel_1();
    void onClickDropoutsLabel_2();
    void onClickDropoutsLabel_3();

    void onClickAbsLimLabel_1();
    void onClickAbsLimLabel_2();
    void onClickAbsLimLabel_3();
    void onClickAbsLimLabel_5();
    void onClickAbsLimLabel_7();
    void onClickAbsLimLabel_9();
    void onClickAbsLimLabel_11();

    void onClickSkewnessLabel_1();
    void onClickSkewnessLabel_2();
    void onClickSkewnessLabel_5();
    void onClickSkewnessLabel_6();

    void onClickDiscontLabel_1();
    void onClickDiscontLabel_2();
    void onClickDiscontLabel_3();
    void onClickDiscontLabel_4();
    void onClickDiscontLabel_5();
    void onClickDiscontLabel_6();
    void onClickDiscontLabel_7();
    void onClickDiscontLabel_8();

    void onClickTimeLagLabel_1();
    void onClickTimeLagLabel_2();
    void onClickTimeLagLabel_3();
    void onClickTimeLagLabel_4();
    void onClickTimeLagLabel_5();
    void onClickTimeLagLabel_6();

    void onClickAttackAngleLabel_1();
    void onClickAttackAngleLabel_2();
    void onClickAttackAngleLabel_3();

    void onClickNonSteadyLabel_1();

    void updateTooltip(int i);

    void despikingRadioClicked(int b);
    void updateDespikingMethod(int b);

private:
    void createTabWidget();
    bool atLeastOneCheckedTest();
    bool areAllCheckedTests();
    int findClosestEnabledTest(int indexDisabled);
    void hideGraphLabels(bool hidden);
    void showThumbnailGraphLabel(bool visible);
    void createQuestionMark();
    bool requestTestSettingsReset();
    void setTestDefaultValues();

    QToolBox* testToolbox;
    QWidget* tab0;
    QRadioButton *vickersDespikingRadio;
    QRadioButton *mauderDespikingRadio;
    QButtonGroup* despikingRadioGroup;
    QLabel* spikeGraphLabel;
    QSpinBox* despSpin_1;
    QDoubleSpinBox* despSpin_2;
    QDoubleSpinBox* despSpin_3;
    QDoubleSpinBox* despSpin_4;
    QDoubleSpinBox* despSpin_5;
    QDoubleSpinBox* despSpin_6;
    QDoubleSpinBox* despSpin_7;
    QDoubleSpinBox* despSpin_8;
    QCheckBox* despFilterCheckBox;

    QWidget* tab1;
    QLabel* amplResGraphLabel;
    QDoubleSpinBox* amplResSpin_1;
    QSpinBox* amplResSpin_2;
    QSpinBox* amplResSpin_3;

    QWidget* tab2;
    QLabel* dropoutsGraphLabel;
    QSpinBox* dropoutsSpin_1;
    QDoubleSpinBox* dropoutsSpin_2;
    QDoubleSpinBox* dropoutsSpin_3;

    QWidget* tab3;
    QLabel* absLimGraphLabel;
    QDoubleSpinBox* absLimSpin_1;
    QDoubleSpinBox* absLimSpin_2;
    QDoubleSpinBox* absLimSpin_3;
    QDoubleSpinBox* absLimSpin_4;
    QDoubleSpinBox* absLimSpin_5;
    QDoubleSpinBox* absLimSpin_6;
    QDoubleSpinBox* absLimSpin_7;
    QDoubleSpinBox* absLimSpin_8;
    QDoubleSpinBox* absLimSpin_9;
    QDoubleSpinBox* absLimSpin_10;
    QDoubleSpinBox* absLimSpin_11;
    QDoubleSpinBox* absLimSpin_12;
    QDoubleSpinBox* absLimSpin_13;
    QDoubleSpinBox* absLimSpin_14;
    QCheckBox* absLimFilterCheckBox;

    QWidget* tab4;
    QLabel* skewnessGraphLabel;
    QDoubleSpinBox* skewnessSpin_1;
    QDoubleSpinBox* skewnessSpin_2;
    QDoubleSpinBox* skewnessSpin_3;
    QDoubleSpinBox* skewnessSpin_4;
    QDoubleSpinBox* skewnessSpin_5;
    QDoubleSpinBox* skewnessSpin_6;
    QDoubleSpinBox* skewnessSpin_7;
    QDoubleSpinBox* skewnessSpin_8;

    QWidget* tab5;
    QLabel* discontGraphLabel;
    QDoubleSpinBox* discontSpin_1;
    QDoubleSpinBox* discontSpin_2;
    QDoubleSpinBox* discontSpin_3;
    QDoubleSpinBox* discontSpin_4;
    QDoubleSpinBox* discontSpin_5;
    QDoubleSpinBox* discontSpin_6;
    QDoubleSpinBox* discontSpin_7;
    QDoubleSpinBox* discontSpin_8;
    QDoubleSpinBox* discontSpin_9;
    QDoubleSpinBox* discontSpin_10;
    QDoubleSpinBox* discontSpin_11;
    QDoubleSpinBox* discontSpin_12;
    QDoubleSpinBox* discontSpin_13;
    QDoubleSpinBox* discontSpin_14;
    QDoubleSpinBox* discontSpin_15;
    QDoubleSpinBox* discontSpin_16;

    QWidget* tab6;
    QLabel* timelagGraphLabel;
    QDoubleSpinBox* timeLagSpin_1;
    QDoubleSpinBox* timeLagSpin_2;
    QDoubleSpinBox* timeLagSpin_3;
    QDoubleSpinBox* timeLagSpin_4;
    QDoubleSpinBox* timeLagSpin_5;
    QDoubleSpinBox* timeLagSpin_6;

    QWidget* tab7;
    QLabel* attackAngleGraphLabel;
    QDoubleSpinBox* attackAngleSpin_1;
    QDoubleSpinBox* attackAngleSpin_2;
    QDoubleSpinBox* attackAngleSpin_3;

    QWidget* tab8;
    QLabel* nonSteadyGraphLabel;
    QDoubleSpinBox* nonSteadySpin_1;

    QCheckBox* spikeRemCheckBox;
    QCheckBox* amplitudeResCheckBox;
    QCheckBox* dropoutsCheckBox;
    QCheckBox* absLimCheckBox;
    QCheckBox* skewnessCheckBox;
    QCheckBox* discontCheckBox;
    QCheckBox* timeLagCheckBox;
    QCheckBox* attackAngleCheckBox;
    QCheckBox* nonSteadyCheckBox;

    QCheckBox* selectAllCheckBox;
    QPushButton* defaultValuesButton;

    QLabel* thumbnailGraphLabel;

    ClickLabel* despLabel_1;
    ClickLabel* despLabel_2;
    ClickLabel* despLabel_3;
    ClickLabel* despLabel_4;
    ClickLabel* despLabel_5;
    ClickLabel* despLabel_6;
    ClickLabel* despLabel_7;
    ClickLabel* despLabel_8;
    ClickLabel* amplResLabel_1;
    ClickLabel* amplResLabel_2;
    ClickLabel* amplResLabel_3;
    ClickLabel* dropoutsLabel_1;
    ClickLabel* dropoutsLabel_2;
    ClickLabel* dropoutsLabel_3;
    ClickLabel* absLimLabel_1;
    ClickLabel* absLimLabel_2;
    ClickLabel* absLimLabel_3;
    ClickLabel* absLimLabel_5;
    ClickLabel* absLimLabel_7;
    ClickLabel* absLimLabel_9;
    ClickLabel* absLimLabel_11;
    ClickLabel* skewnessLabel_1;
    ClickLabel* skewnessLabel_2;
    ClickLabel* skewnessLabel_5;
    ClickLabel* skewnessLabel_6;
    ClickLabel* discontLabel_1;
    ClickLabel* discontLabel_2;
    ClickLabel* discontLabel_3;
    ClickLabel* discontLabel_4;
    ClickLabel* discontLabel_5;
    ClickLabel* discontLabel_6;
    ClickLabel* discontLabel_7;
    ClickLabel* discontLabel_8;
    ClickLabel* timeLagLabel_1;
    ClickLabel* timeLagLabel_2;
    ClickLabel* timeLagLabel_3;
    ClickLabel* timeLagLabel_4;
    ClickLabel* timeLagLabel_5;
    ClickLabel* timeLagLabel_6;
    ClickLabel* attackAngleLabel_1;
    ClickLabel* attackAngleLabel_2;
    ClickLabel* attackAngleLabel_3;
    ClickLabel* nonSteadyLabel_1;

    QPushButton* questionMark_1;
    QPushButton* questionMark_2;
    QPushButton* questionMark_3;
    QPushButton* questionMark_4;
    QPushButton* questionMark_5;
    QPushButton* questionMark_6;
    QPushButton* questionMark_7;
    QPushButton* questionMark_8;
    QPushButton* questionMark_9;
    QPushButton* questionMark_10;
    QPushButton* questionMark_11;

    QCheckBox* randomErrorCheckBox;
    ClickLabel* randomMethodLabel;
    QComboBox* randomMethodCombo;
    ClickLabel* itsDefinitionLabel;
    QComboBox* itsDefinitionCombo;
    ClickLabel* timelagMaxLabel;
    QDoubleSpinBox* timelagMaxSpin;
    ClickLabel* securityCoeffLabel;
    QDoubleSpinBox* securityCoeffSpin;

    EcProject* ecProject_;
};

#endif // ADVSTATISTICALOPTIONS_H

