/***************************************************************************
  projectpage.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <QPointer>
#include <QWidget>

#include "faderwidget.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QStackedWidget;

class QwwButtonLineEdit;
class QwwClearLineEdit;

class BinarySettingsDialog;
class ClickLabel;
struct ConfigState;
class DlIniDialog;
class DlProject;
class EcProject;
class MyTabWidget;
//class SlowMeasureTab;
class SmartFluxBar;

class ProjectPage : public QWidget
{
    Q_OBJECT

public:
    ProjectPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config);
    ~ProjectPage();

    inline DlIniDialog* dlIniDialog() { return dlIniDialog_; }
    BinarySettingsDialog* getBinarySettingsDialog() { return binDialog_; }
    void updateSmartfluxBar();

    void setSmartfluxUI();

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private:
    ClickLabel *titleLabel;
    QwwClearLineEdit *titleEdit;

    ClickLabel *fileTypeLabel;
    QButtonGroup *fileTypeRadioGroup;
    QRadioButton *ghgFileTyperRadio;
    QRadioButton *asciiFileTypeRadio;
    QRadioButton *binaryFileTypeRadio;
    QRadioButton *tobFileTypeRadio;
    QRadioButton *sltEddysoftFileTypeRadio;
    QRadioButton *sltEdisolFileTypeRadio;
    QComboBox* tobSettingsCombo;
    QPushButton *binSettingsButton;

    QRadioButton *altMetadataFileRadio;
    QRadioButton *embMetadataFileRadio;
    ClickLabel *metadataLabel;
    QButtonGroup *metadataRadioGroup;
    QwwButtonLineEdit *metadataFileEdit;
    QPushButton *metadataFileLoad;

    QCheckBox *dynamicMdCheckBox;
    QwwButtonLineEdit *dynamicMdEdit;
    QPushButton *dynamicMdLoad;

    QCheckBox *biomDataCheckBox;
    QRadioButton *biomEmbFileRadio;
    QRadioButton *biomExtFileRadio;
    QwwButtonLineEdit *biomExtFileEdit;
    QPushButton *biomExtFileLoad;
    QRadioButton *biomExtDirRadio;
    QwwButtonLineEdit *biomExtDirEdit;
    QPushButton *biomExtDirBrowse;
    QCheckBox* biomExtDirRecCheckBox;
    ClickLabel* biomExtDirSuffixLabel;
    QComboBox* biomExtDirCombo;
    QButtonGroup *biomRadioGroup;

    QPushButton *questionMark_2;
    QPushButton *questionMark_3;
    QPushButton *questionMark_4;
    QPushButton *questionMark_5;
    QPushButton *questionMark_6;
    QPushButton *questionMark_7;
    QPushButton *questionMark_8;

    DlProject *dlProject_;
    EcProject *ecProject_;
    ConfigState* configState_;

    QFrame *helpGroup_;
    DlIniDialog *dlIniDialog_;
    QStackedWidget *metadataTab;

//    QStackedWidget* slowMeasuresTab;

    MyTabWidget* metadataEditors;

    QPointer<FaderWidget> faderWidget_;
    bool fadingOn_;
    bool isMetadataEditorOn_;
    int previousFileType_;
    int currentFileType_;

    QString previousMetadataFile_;
    QString currentMetadataFile_;

    void createQuestionMark();

    BinarySettingsDialog* binDialog_;
    SmartFluxBar* smartfluxBar_;

private slots:
    void createMetadataEditor();
    void createSlowMeasuresEditor();

    void fileTypeRadioClicked_1(int fileType);
    void fileTypeRadioClicked_2(int fileType);
    void fadeInWidget(int filetype);
    void metadataFileLoad_clicked();
    void onTitleLabelClicked();
    void updateMetadataFileEdit(const QString &filename);
    void updateBiomFileEdit(const QString &filename);

    void updateFileType(int filetype);
    void updateUseMetadataFile_1(int filetype);
    void updateUseMetadataFile_2(int radio);
    void updateMetadataFile(const QString& fp);
    void updateBiomFile(const QString& fp);
    void updateBiomDir(const QString& fp);
    void updateExtDirRec(bool b);
    void on_biomDirBrowse_clicked();
    void updateExtDirSuffix(const QString& s);
    void onBiomExtDirSuffixLabelClicked();

    void reset();
    void refresh();
    void refreshMetadata();
    void updateMetadataLoading();

    void loginiFileNew_clicked();
    void metadataRadioClicked(int b);

    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();
    void onlineHelpTrigger_6();
    void onlineHelpTrigger_7();
    void onlineHelpTrigger_8();

    void tobSettingsUpdate(int n);
    void binSettingsDialog();

    void timelineFileLoad_clicked();
    void onTimelineFileCheckBoxClicked(bool b);
    void updateUseTimelineFile(bool b);
    void updateTimelineFile(const QString& fp);

    void mdReset();
    void mdResetRequest();
    void updateTooltip(int i);
    void clearDynamicMdEdit();
    void clearBiomExtFileEdit();
    void clearBiomExtDirEdit();

    void on_biomDataCheckBox_clicked(bool clicked);
    void on_biomRadioGroup_clicked_1(int button);
    void on_biomRadioGroup_clicked_2(int button);
    void on_biomFileLoad_clicked();

signals:
    void updateMetadataReadRequest();
    void updateRawFilenameFormatRequest();
    void connectBinarySettingsRequest();
    void setOutputBiometRequest();
    void requestBasicSettingsClear();
};

#endif // PROJECTPAGE_H
