/***************************************************************************
  projectpage.h
  -------------
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

#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include <QPointer>
#include <QWidget>

class QAction;
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

class BinarySettingsDialog;
class ClickLabel;
struct ConfigState;
class CustomClearLineEdit;
class DirBrowseWidget;
class DlIniDialog;
class DlProject;
class EcProject;
class FileBrowseWidget;
class MyTabWidget;
class SmartFluxBar;

class ProjectPage : public QWidget
{
    Q_OBJECT

public:
    ProjectPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config);
    ~ProjectPage() override;

    inline DlIniDialog* dlIniDialog() { return dlIniDialog_; }
    BinarySettingsDialog* getBinarySettingsDialog() { return binDialog_; }

    void updateSmartfluxBar();
    void setSmartfluxUI();

private:
    ClickLabel *titleLabel;
    CustomClearLineEdit *titleEdit;

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
    FileBrowseWidget *metadataFileBrowse;

    QCheckBox *dynamicMdCheckBox;
    FileBrowseWidget *dynamicMdFileBrowse;

    QCheckBox *biomDataCheckBox;
    QRadioButton *biomEmbFileRadio;
    QRadioButton *biomExtFileRadio;
    FileBrowseWidget* biometExtFileBrowse;
    QRadioButton *biomExtDirRadio;
    DirBrowseWidget* biometExtDirBrowse;
    QCheckBox* biomExtDirRecCheckBox;
    ClickLabel* biomExtDirSuffixLabel;
    QComboBox* biomExtDirCombo;
    QButtonGroup *biomRadioGroup;

    QPushButton *questionMark_1;
    QPushButton *questionMark_2;
    QPushButton *questionMark_3;
    QPushButton *questionMark_4;
    QPushButton *questionMark_5;
    QPushButton *questionMark_6;

    DlProject *dlProject_;
    EcProject *ecProject_;
    ConfigState* configState_;

    QFrame *helpGroup_;
    DlIniDialog *dlIniDialog_;
    QStackedWidget *metadataTab;

    MyTabWidget* metadataEditors;

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

    void fileTypeRadioClicked_1(int fileType);
    void fileTypeRadioClicked_2(int fileType);
    void selectWidget(int filetype);
    void metadataFileSelected(const QString& file_path);
    void onTitleLabelClicked();
    void updateMetadataFileBrowse(const QString &filename);

    void updateFileType(int filetype);
    void updateUseMetadataFile_1(int filetype);
    void updateUseMetadataFile_2(int radio);
    void updateMetadataFile(const QString& fp);
    void updateBiomFile(const QString& fp);
    void updateBiomDir(const QString& fp);
    void updateExtDirRec(bool b);
    void biometExtDirSelected(const QString &dir_path);
    void updateExtDirSuffix(const QString& s);
    void onBiomExtDirSuffixLabelClicked();

    void reset();
    void refresh();
    void refreshMetadata();
    void updateMetadataLoading();

    void metadataRadioClicked(int b);

    void onlineHelpTrigger_2();
    void onlineHelpTrigger_3();
    void onlineHelpTrigger_4();
    void onlineHelpTrigger_5();
    void onlineHelpTrigger_6();
    void onlineHelpTrigger_7();

    void tobSettingsUpdate(int n);
    void binSettingsDialog();

    void updateTimelineFile(const QString& fp);
    void dynamicMdFileSelected(const QString &fp);
    void mdEditorReset();
    void mdResetRequest();
    void updateTooltip(int i);

    void on_biomDataCheckBox_clicked(bool clicked);
    void on_biomRadioGroup_clicked_1(int button);
    void on_biomRadioGroup_clicked_2(int button);
    void biomExtFileSelected(const QString &fp);

signals:
    void updateMetadataReadRequest();
    void connectBinarySettingsRequest();
    void setOutputBiometRequest();
    void requestBasicSettingsClear();
    void mdCleanupRequest();
};

#endif // PROJECTPAGE_H
