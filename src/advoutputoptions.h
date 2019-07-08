/***************************************************************************
  advoutputoptions.h
  ------------------
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
    AdvOutputOptions(QWidget* parent,
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
    QCheckBox* fluxnetStdBiometCheckBox;
    QCheckBox* fluxnetErrLabelCheckBox;

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

    QLabel* hrLabel_1;

    EcProject* ecProject_;
    ConfigState* configState_;
};

#endif // ADVOUTPUTOPTIONS_H
