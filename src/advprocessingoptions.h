/***************************************************************************
  advprocessingoptions.h
  -------------------
  Copyright (C) 2007-2011 Eco2s team. Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

class QSpinBox;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QRadioButton;
class QButtonGroup;
class QTabWidget;
class QGroupBox;

class QwwResetLineEdit;

class ClickLabel;
class PlanarFitSettingsDialog;
class TimeLagSettingsDialog;
class DlProject;
class EcProject;
struct ConfigState;

class AdvProcessingOptions : public QWidget
{
    Q_OBJECT

public:
    AdvProcessingOptions(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config);
    ~AdvProcessingOptions();

    void setEnabledAndCheckAngleOfAttack(int gillGroup);

    PlanarFitSettingsDialog* getPlanarFitSettingsDialog() { return pfDialog_; }
    TimeLagSettingsDialog* getTimeLagSettingsDialog() { return tlDialog_; }

    void setSmartfluxUI();

public slots:
    void reset();
    void updateAngleOfAttack(const QString &s);

private:
    QLabel *windOffsetLabel;
    ClickLabel *uLabel;
    ClickLabel *vLabel;
    ClickLabel *wLabel;
    QDoubleSpinBox *uOffsetSpin;
    QDoubleSpinBox *vOffsetSpin;
    QDoubleSpinBox *wOffsetSpin;
    QCheckBox *aoaCheckBox;
    ClickLabel* aoaLabel;
    ClickLabel *aoaMethLabel;
    QComboBox *aoaMethCombo;
    QCheckBox *rotCheckBox;
    ClickLabel *rotMethLabel;
    QComboBox *rotMethCombo;
    QPushButton *pfSettingsButton;
    QLabel *detrendLabel;
    ClickLabel *detrendMethLabel;
    QComboBox *detrendCombo;
    QCheckBox *timeLagCheckBox;
    ClickLabel *timeConstantLabel;
    QDoubleSpinBox *timeConstantSpin;
    ClickLabel *timeLagMethodLabel;
    QComboBox *timeLagMethodCombo;
    QPushButton *tlSettingsButton;
    QCheckBox* fftCheckBox;
    QCheckBox* qcCheckBox;
    ClickLabel *qcLabel;
    QComboBox *qcMethodCombo;
    QCheckBox* fpCheckBox;
    ClickLabel *fpLabel;
    QComboBox *fpMethodCombo;

    ClickLabel *filterLabel;
    QComboBox *filterCombo;

    ClickLabel *nBinsLabel;
    QSpinBox *nBinsSpin;

    QCheckBox *wplCheckBox;
    ClickLabel *wplMethLabel;
    QComboBox *wplMethCombo;
    ClickLabel* burbaLabel;
    QCheckBox *burbaCorrCheckBox;
    ClickLabel *burbaTypeLabel;
    QRadioButton *burbaSimpleRadio;
    QRadioButton *burbaMultiRadio;
    QPushButton *setDefaultsButton;
    QButtonGroup *burbaRadioGroup;
    QWidget *burbaSimpleDay;
    QWidget *burbaSimpleNight;
    QWidget *burbaMultiDay;
    QWidget *burbaMultiNight;
    QTabWidget *burbaSimpleTab;
    QTabWidget *burbaMultiTab;
    QStackedWidget *burbaParamWidget;

    QwwResetLineEdit *lDayBotGain;
    QwwResetLineEdit *lDayBotOffset;
    QwwResetLineEdit *lDayTopGain;
    QwwResetLineEdit *lDayTopOffset;
    QwwResetLineEdit *lDaySparGain;
    QwwResetLineEdit *lDaySparOffset;
    QwwResetLineEdit *lNightBotGain;
    QwwResetLineEdit *lNightBotOffset;
    QwwResetLineEdit *lNightTopGain;
    QwwResetLineEdit *lNightTopOffset;
    QwwResetLineEdit *lNightSparGain;
    QwwResetLineEdit *lNightSparOffset;

    QwwResetLineEdit *mDayBot1;
    QwwResetLineEdit *mDayBot2;
    QwwResetLineEdit *mDayBot3;
    QwwResetLineEdit *mDayBot4;
    QwwResetLineEdit *mDayTop1;
    QwwResetLineEdit *mDayTop2;
    QwwResetLineEdit *mDayTop3;
    QwwResetLineEdit *mDayTop4;
    QwwResetLineEdit *mDaySpar1;
    QwwResetLineEdit *mDaySpar2;
    QwwResetLineEdit *mDaySpar3;
    QwwResetLineEdit *mDaySpar4;
    QwwResetLineEdit *mNightBot1;
    QwwResetLineEdit *mNightBot2;
    QwwResetLineEdit *mNightBot3;
    QwwResetLineEdit *mNightBot4;
    QwwResetLineEdit *mNightTop1;
    QwwResetLineEdit *mNightTop2;
    QwwResetLineEdit *mNightTop3;
    QwwResetLineEdit *mNightTop4;
    QwwResetLineEdit *mNightSpar1;
    QwwResetLineEdit *mNightSpar2;
    QwwResetLineEdit *mNightSpar3;
    QwwResetLineEdit *mNightSpar4;

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

    DlProject *dlProject_;
    EcProject *ecProject_;
    ConfigState* configState_;

    PlanarFitSettingsDialog *pfDialog_;
    TimeLagSettingsDialog *tlDialog_;

    int previousDetrendMethod;

    void createTlSettingsDialog();
    void createPfSettingsDialog();
    void createBurbaParamItems();
    void createQuestionMark();
    void setBurbaDefaultValues();

    int detectAngleOfAttackAnemGroups(const QString& anemModel);
    void setEnabledAngleOfAttack();

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
    void updateFilter(int n);
    void updateNBins(int n);
    void updatePowerOfTwo(bool b);

    void onClickDetrendCombo(int detrendMethod);
    void onClickDetrendLabel();
    void onClickTimeConstantLabel();
    void onClickAoaMethLabel();
    void onClickRotMethLabel();
    void onClickTimeLagMethLabel();
    void onClickFilterLabel();
    void onNBinsLabelClicked();
    void onULabelClicked();
    void onVLabelClicked();
    void onWLabelClicked();

    void onClickQcMethodLabel();
    void updateQcMeth_1(bool b);
    void updateQcMeth_2(int n);
    void onClickFpMethodLabel();
    void updateFpMeth_1(bool b);
    void updateFpMeth_2(int n);

    void onClickWplMethLabel();
    void updateWplMeth_1(bool b);
    void updateToMixingRatio(int n);
    void updateBurbaGroup(bool b);
    void updateBurbaType_1(bool b);
    void updateBurbaType_2(int n);
    void enableBurbaCorrectionArea(bool b);

    void setLDayBotGain(QString s);
    void setLDayBotOffset(QString s);
    void setLDayTopGain(QString s);
    void setLDayTopOffset(QString s);
    void setLDaySparGain(QString s);
    void setLDaySparOffset(QString s);
    void setLNightBotGain(QString s);
    void setLNightBotOffset(QString s);
    void setLNightTopGain(QString s);
    void setLNightTopOffset(QString s);
    void setLNightSparGain(QString s);
    void setLNightSparOffset(QString s);
    void setMDayBot1(QString s);
    void setMDayBot2(QString s);
    void setMDayBot3(QString s);
    void setMDayBot4(QString s);
    void setMDayTop1(QString s);
    void setMDayTop2(QString s);
    void setMDayTop3(QString s);
    void setMDayTop4(QString s);
    void setMDaySpar1(QString s);
    void setMDaySpar2(QString s);
    void setMDaySpar3(QString s);
    void setMDaySpar4(QString s);
    void setMNightBot1(QString s);
    void setMNightBot2(QString s);
    void setMNightBot3(QString s);
    void setMNightBot4(QString s);
    void setMNightTop1(QString s);
    void setMNightTop2(QString s);
    void setMNightTop3(QString s);
    void setMNightTop4(QString s);
    void setMNightSpar1(QString s);
    void setMNightSpar2(QString s);
    void setMNightSpar3(QString s);
    void setMNightSpar4(QString s);

    void on_setDefaultsButton_clicked();

    void refresh();
    void refreshAngleOfAttack();

    void showPfSettingsDialog();
    void showTlSettingsDialog();

    void updateTooltip(int i);

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

signals:

};

#endif // ADVPROCESSINGOPTIONS_H
