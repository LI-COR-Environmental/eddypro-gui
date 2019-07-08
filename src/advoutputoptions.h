/***************************************************************************
  advoutputoptions.h
  -------------------
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#ifndef ADVOUTPUTOPTIONS_H
#define ADVOUTPUTOPTIONS_H

#include <QWidget>

#include <vector>

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QRadioButton;

class ClickLabel;
struct ConfigState;
class EcProject;
class RichTextCheckBox;

class AdvOutputOptions : public QWidget
{
    Q_OBJECT

public:
    explicit AdvOutputOptions(QWidget* parent,
                              EcProject* ecProject,
                              ConfigState* config);

    void setSmartfluxUI();

public slots:
    void updateOutputs(int n);
    void checkMetadataOutput();
    void reset();
    void setOutputBiomet();

private slots:
    void refresh();

    void updateOutBinSpectra(bool b);
    void updateBinSpectra(bool b);

    void updateFixedOuputFormat(int n);
    void updateErrorLabel(const QString& s);

    void checkFullSpectraAll(bool b);
    void checkFullCospectraAll(bool b);

    void updateOutputRaw1(bool b);
    void updateOutputRaw2(bool b);
    void updateOutputRaw3(bool b);
    void updateOutputRaw4(bool b);
    void updateOutputRaw5(bool b);
    void updateOutputRaw6(bool b);
    void updateOutputRaw7(bool b);
    void updateOutputRawU(bool b);
    void updateOutputRawV(bool b);
    void updateOutputRawW(bool b);
    void updateOutputRawTs(bool b);
    void updateOutputRawCo2(bool b);
    void updateOutputRawH2o(bool b);
    void updateOutputRawCh4(bool b);
    void updateOutputRawGas4(bool b);
    void updateOutputRawTair(bool b);
    void updateOutputRawPair(bool b);
    void checkStAll(bool b);
    void checkTimeSeriesAll(bool b);
    void checkVarsAll(bool b);
    void updateSelectAllCheckbox();

    void selectMin();
    void selectTypical();
    void selectFull();

    void onlineHelpTrigger_1();
    void onlineHelpTrigger_2();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();
    void onlineHelpTrigger_6();
    void onlineHelpTrigger_7();
    void onlineHelpTrigger_8();
    void onlineHelpTrigger_9();

    void onClickerrorFormatLabel();

    void openToviHomepage();

private:
    bool areAllCheckedVars();
    void setVarsAvailable(bool ok);
    bool areTimeSeriesChecked();
    void updateVarsAvailable();
    void restoreOutputs();

    void createQuestionMark();

    std::vector<bool> oldEnabled{};
    std::vector<bool> oldVisible{};

    QPushButton* fullSelectionButton;
    QLabel* fullSelectionDesc;
    QPushButton* minSelectionButton;
    QLabel* minSelectionDesc;
    QPushButton* typicalSelectionButton;
    QLabel* typicalSelectionDesc;

    QCheckBox* outBinSpectraCheckBox;
    QCheckBox* outBinOgivesCheckBox;
    QCheckBox* outMeanSpectraCheckBox;
    QCheckBox* outMeanCospCheckBox;

    RichTextCheckBox* outFullSpectraCheckBoxU;
    RichTextCheckBox* outFullSpectraCheckBoxV;
    RichTextCheckBox* outFullSpectraCheckBoxW;
    RichTextCheckBox* outFullSpectraCheckBoxTs;
    RichTextCheckBox* outFullSpectraCheckBoxCo2;
    RichTextCheckBox* outFullSpectraCheckBoxH2o;
    RichTextCheckBox* outFullSpectraCheckBoxCh4;
    RichTextCheckBox* outFullSpectraCheckBoxN2o;

    RichTextCheckBox* outFullCospectraCheckBoxU;
    RichTextCheckBox* outFullCospectraCheckBoxV;
    RichTextCheckBox* outFullCospectraCheckBoxTs;
    RichTextCheckBox* outFullCospectraCheckBoxCo2;
    RichTextCheckBox* outFullCospectraCheckBoxH2o;
    RichTextCheckBox* outFullCospectraCheckBoxCh4;
    RichTextCheckBox* outFullCospectraCheckBoxN2o;

    QCheckBox* outFullCospectraAll;
    QCheckBox* outGhgEuCheckBox;
    QCheckBox* outAmFluxCheckBox;
    QCheckBox* outDetailsCheckBox;
    QCheckBox* outMdCheckBox;
    QCheckBox* outBiometCheckBox;
    QCheckBox* createDatasetCheckBox;
    QCheckBox* outFullCheckBox;
    ClickLabel* fullOutformatLabel;
    QRadioButton* fixedVarsOutputRadio;
    QRadioButton* variableVarsOutputRadio;
    QButtonGroup* outputFormatRadioGroup;
    ClickLabel* errorFormatLabel;
    QComboBox* errorFormatCombo;
    ClickLabel* level1Label;
    ClickLabel* level2Label;
    ClickLabel* level3Label;
    ClickLabel* level4Label;
    ClickLabel* level5Label;
    ClickLabel* level6Label;
    ClickLabel* level7Label;
    QCheckBox* outSt1CheckBox;
    QCheckBox* outSt2CheckBox;
    QCheckBox* outSt3CheckBox;
    QCheckBox* outSt4CheckBox;
    QCheckBox* outSt5CheckBox;
    QCheckBox* outSt6CheckBox;
    QCheckBox* outSt7CheckBox;
    QCheckBox* outRaw1CheckBox;
    QCheckBox* outRaw2CheckBox;
    QCheckBox* outRaw3CheckBox;
    QCheckBox* outRaw4CheckBox;
    QCheckBox* outRaw5CheckBox;
    QCheckBox* outRaw6CheckBox;
    QCheckBox* outRaw7CheckBox;
    RichTextCheckBox* outRawUCheckBox;
    RichTextCheckBox* outRawVCheckBox;
    RichTextCheckBox* outRawWCheckBox;
    RichTextCheckBox* outRawTsCheckBox;
    RichTextCheckBox* outRawCo2CheckBox;
    RichTextCheckBox* outRawH2oCheckBox;
    RichTextCheckBox* outRawCh4CheckBox;
    RichTextCheckBox* outRawGas4CheckBox;
    RichTextCheckBox* outRawTairCheckBox;
    RichTextCheckBox* outRawPairCheckBox;
    QCheckBox* outVarsAllCheckBox;

    QPushButton* questionMark_1;
    QPushButton* questionMark_2;
    QPushButton* questionMark_3;
    QPushButton* questionMark_4;
    QPushButton* questionMark_5;
    QPushButton* questionMark_6;
    QPushButton* questionMark_7;
    QPushButton* questionMark_8;
    QPushButton* questionMark_9;

    QLabel* hrLabel_1;

    EcProject* ecProject_;
    ConfigState* configState_;
};

#endif // ADVOUTPUTOPTIONS_H
