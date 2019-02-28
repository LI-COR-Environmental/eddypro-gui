/***************************************************************************
  projectpage.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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

#include "projectpage.h"

#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>
#include <QUrl>

#include "binarysettingsdialog.h"
#include "clicklabel.h"
#include "configstate.h"
#include "customclearlineedit.h"
#include "dirbrowsewidget.h"
#include "dlinidialog.h"
#include "dlproject.h"
#include "dlrawfiledesctab.h"
#include "ecproject.h"
#include "filebrowsewidget.h"
#include "fileutils.h"
#include "globalsettings.h"
#include "mytabwidget.h"
#include "smartfluxbar.h"
#include "splitter.h"
#include "widget_utils.h"

ProjectPage::ProjectPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    metadataFileBrowse(nullptr),
    dynamicMdFileBrowse(nullptr),
    biometExtFileBrowse(nullptr),
    biometExtDirBrowse(nullptr),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config),
    helpGroup_(nullptr),
    dlIniDialog_(nullptr),
    isMetadataEditorOn_(false),
    previousFileType_(0),
    currentFileType_(0),
    previousMetadataFile_(QString()),
    currentMetadataFile_(QString()),
    binDialog_(nullptr)
{
    titleLabel = new ClickLabel(tr("Project name :"));
    titleLabel->setProperty("optionalField", true);
    titleLabel->setToolTip(tr("<b>Project name:</b> Enter a name for the flux computation project. This will be the default file name for this project, but you can edit it while saving the project. This field is optional."));

    titleEdit = new CustomClearLineEdit();
    titleEdit->setToolTip(titleLabel->toolTip());
    titleEdit->setMaxLength(255);

    fileTypeLabel = new ClickLabel;
    fileTypeLabel->setText(tr("Raw file format :"));
    fileTypeLabel->setToolTip(tr("<b>Raw file format:</b> Select the format of your raw files."
                                 "<br /><b><i>LI-COR GHG</i></b> is the format of files created by LI-COR data logging software."
                                 "<br /><b><i>ASCII plain text</i></b> includes virtually any text file organized in data columns. "
                                 "<br /><b><i>Generic Binary</i></b> is any fixed-length words binary format used to store single precision variables. Use the <b><i>Settings...</i></b> dialogue to further specify the format. "
                                 "<br /><b><i>TOB1</i></b> is a proprietary format by Campbell<sup>&reg;</sup> Scientific, used to collect raw files on its data loggers. "
                                 "<br />The two <b><i>SLT are binary</i></b> formats of raw files created by the EddySoft and EdiSol data collection tools."));

    ghgFileTyperRadio = new QRadioButton(tr("LI-COR GHG"));
    ghgFileTyperRadio->setChecked(true);
    ghgFileTyperRadio->setToolTip(tr("<b>LI-COR GHG:</b> A custom LI-COR raw file format. Each GHG file is an archive containing a data and a metadata file, both in readable text format. GHG files have extension \".ghg\""));
    asciiFileTypeRadio = new QRadioButton(tr("ASCII plain text"));
    asciiFileTypeRadio->setToolTip(tr("<b>ASCII plain text:</b> Any text file organized in data columns, with or without a header. All typical field separators (tab, space, comma, and semicolon) are supported. Campbell<sup>&reg;</sup> Scientific TOA5 format is an example of a supported ASCII data file."));
    tobFileTypeRadio = new QRadioButton(tr("TOB1"));
    tobFileTypeRadio->setToolTip(tr("<b>TOB1:</b> Raw files in the Campbell<sup>&reg;</sup> Scientific binary format. Support of TOB1 format is limited to files containing only ULONG and IEEE4 fields, or ULONG and FP2 fields. In the second case, FP2 fields must follow any ULONG field, while for ULONG and IEEE4 there is no such limitation. Note also that ULONG fields are not interpreted by EddyPro, thus they can only be defined as <i>Ignore</i> variables."));
    sltEddysoftFileTypeRadio = new QRadioButton(tr("SLT (EddySoft)"));
    sltEddysoftFileTypeRadio->setToolTip(tr("<b>SLT (EddySoft):</b> Format of binary files created by EddyMeas, the data acquisition tool of the EddySoft suite, by O. Kolle and C. Rebmann (Max Planck Institute, Jena, Germany). This is a fixed-length binary format. It includes a binary header in each file that needs to be interpreted to correctly retrieve data. EddyPro interprets such header automatically, but you still need to describe the file content in the Raw File Description tab of the Metadata File Editor."));
    sltEdisolFileTypeRadio = new QRadioButton(tr("SLT (EdiSol)"));
    sltEdisolFileTypeRadio->setToolTip(tr("<b>SLT (EdiSol):</b> Format of binary files created by EdiSol, the data acquisition tool developed by Univ. of Edinburg, UK. This is a fixed-length binary format. It includes a binary header in each file that needs to be interpreted to correctly retrieve data. EddyPro interprets such header automatically, but you still need to describe the file content in the Raw File Description tab of the Metadata File Editor."));
    binaryFileTypeRadio = new QRadioButton(tr("Generic binary"));
    binaryFileTypeRadio->setToolTip(tr("<b>Generic binary:</b> Generic binary (unformatted) raw data files. Limited to fixed-length binary words that contain data stored as single precision (real) numbers."));

    fileTypeRadioGroup = new QButtonGroup(this);
    fileTypeRadioGroup->addButton(ghgFileTyperRadio, 0);
    fileTypeRadioGroup->addButton(asciiFileTypeRadio, 1);
    fileTypeRadioGroup->addButton(tobFileTypeRadio, 2);
    fileTypeRadioGroup->addButton(sltEddysoftFileTypeRadio, 3);
    fileTypeRadioGroup->addButton(sltEdisolFileTypeRadio, 4);
    fileTypeRadioGroup->addButton(binaryFileTypeRadio, 5);

    tobSettingsCombo = new QComboBox(this);
    tobSettingsCombo->addItem(tr("Detect automatically"));
    tobSettingsCombo->addItem(tr("Only ULONG and IEEE4 fields"));
    tobSettingsCombo->addItem(tr("Only ULONG and FP2 fields"));
    tobSettingsCombo->setItemData(0, tr("<b>Detect automatically:</b> Let EddyPro figure out whether TOB1 files contain (ULONG and) IEEE4 fields or (ULONG and) FP2 fields."), Qt::ToolTipRole);
    tobSettingsCombo->setItemData(1, tr("<b>Only ULONG and IEEE4 fields:</b> Choose this option to specify that your TOB1 files contain only IEEE4 fields and possibly ULONG fields. Note that EddyPro does not interpret ULONG fields. This means that any variable stored in ULONG format must be marked with the <i>Ignore</i> option in the <b><i>Raw File Description</i></b> table. Typically ULONG format is used for timestamp information."), Qt::ToolTipRole);
    tobSettingsCombo->setItemData(2, tr("<b>Only ULONG and FP2 fields:</b> Choose this option to specify that your TOB1 files contain only FP2 fields and possibly ULONG fields. Note that ULONG fields, if present, must come first in the sequence of fields. Note also that EddyPro does not interpret ULONG fields. This means that any variable stored in ULONG format must be marked with the <i>Ignore</i> option in the <b><i>Raw File Description</i></b> table. Typically ULONG format is used for timestamp information."), Qt::ToolTipRole);
    tobSettingsCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    binSettingsButton = new QPushButton(tr("Settings..."));
    binSettingsButton->setProperty("commonButton", true);
    binSettingsButton->setMaximumWidth(binSettingsButton->sizeHint().width());
    binSettingsButton->setToolTip(tr("<b>Generic Binary Settings:</b> Use the <b><i>Settings...</i></b> button to provide specifications of the binary format, to help EddyPro to correctly read the files."));

    createMetadataEditor();

    metadataEditors = new MyTabWidget;
    metadataEditors->setObjectName(QStringLiteral("metadataEditorsTabwidget"));
    metadataEditors->getTabBarAsPublic()->setObjectName(QStringLiteral("metadataEditorsTabbar"));
    metadataEditors->addTab(metadataTab, QStringLiteral("Metadata File Editor"));
//    metadataEditors->addTab(slowMeasuresTab, QStringLiteral("Meteo and ecological measurements files"));

    metadataLabel = new ClickLabel;
    metadataLabel->setText(tr("Metadata file :"));
    metadataLabel->setToolTip(tr("<b>Metadata:</b> Choose whether to use metadata files embedded into GHG files or to bypass them by using an alternative metadata file. Only applicable to raw files in LI-COR GHG format."));
    embMetadataFileRadio = new QRadioButton(tr("Use embedded file"));
    embMetadataFileRadio->setToolTip(tr("<b>Use embedded metadata file:</b> Select this option to use file-specific meta-information, retrieved from the metadata file residing inside the GHG archive."));
    embMetadataFileRadio->setStyleSheet(tr(" QRadioButton { padding-left: 2px; } "));
    altMetadataFileRadio = new QRadioButton(tr("Use alternative file: "));
    altMetadataFileRadio->setToolTip(tr("<b>Use alternative metadata file:</b> Select this option to use an alternative metadata file. Note that in this case all GHG files are processed using the same meta-information, retrieved from the alternative metadata file. This file is created and/or edited in the <b><i>Metadata File Editor</i></b>. If you are about to process GHG files, you can speed up the completion of the alternative METADATA by unzipping any raw file and loading the extracted METADATA from the Metadata file: Use alternative file <b><i>Load</i></b> button. Make changes if needed and save the file."));
    altMetadataFileRadio->setStyleSheet(tr(" QRadioButton { padding-left: 0px; } "));

    metadataRadioGroup = new QButtonGroup(this);
    metadataRadioGroup->addButton(embMetadataFileRadio, 0);
    metadataRadioGroup->addButton(altMetadataFileRadio, 1);

    metadataFileBrowse = new FileBrowseWidget;
    metadataFileBrowse->disableClearAction();
    metadataFileBrowse->setToolTip(tr("<b>Load:</b> Load an existing metadata file to edit it in the <i><b>Metadata File Editor</i></b>. If you use the <i><b>Metadata File Editor</i></b> to create and save a new metadata file from scratch, its path will appear here."));
    metadataFileBrowse->setDialogTitle(tr("Select the Metadata File"));
    metadataFileBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());
    metadataFileBrowse->setDialogFilter(tr("%1 Metadata Files (*.metadata);;All Files (*.*)").arg(Defs::APP_NAME));

    createQuestionMark();

    dynamicMdCheckBox = new QCheckBox;
    dynamicMdCheckBox->setText(tr("Use dynamic metadata file :"));
    dynamicMdCheckBox->setToolTip(tr("<b>Use dynamic metadata file:</b> "
                                     "Check this option and provide the "
                                     "corresponding path to instruct "
                                     "EddyPro to use an externally-created "
                                     "file that contains time changing "
                                     "metadata, such as canopy height, "
                                     "instrument separations and more."
                                     "Click on the question mark at the right "
                                     "side of the <b>Load</b> button to learn "
                                     "more about dynamic metadata on the "
                                     "EddyPro Help."));
    dynamicMdCheckBox->setProperty("optionalField", true);

    dynamicMdFileBrowse = new FileBrowseWidget;
    dynamicMdFileBrowse->setToolTip(tr("<b>Load:</b> Load an existing dynamic metadata file."));
    dynamicMdFileBrowse->setDialogTitle(tr("Select the Dynamic Metadata File"));
    dynamicMdFileBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());
    dynamicMdFileBrowse->setDialogFilter(tr("All Files (*.*)"));

    biomDataCheckBox = new QCheckBox;
    biomDataCheckBox->setText(tr("Biomet data :"));
    biomDataCheckBox->setProperty("optionalField", true);
    biomDataCheckBox->setToolTip(tr("<b>Biomet data:</b> Select this option and choose the source of biomet data. Biomet data are slow (< 1 Hz) measurements of biological and meteorological variables that complement eddy covariance measurements. Some biomet measurements can be used to improve flux results (ambient temperature, relative humidity and pressure, global radiation, PAR and long-wave incoming radiation). All biomet data available are screened for physical plausibility, averaged on the same time scale of the fluxes, and provided in a separate output file."));

    biomEmbFileRadio = new QRadioButton(tr("Use embedded files "));
    biomEmbFileRadio->setChecked(true);
    biomEmbFileRadio->setToolTip(tr("<b>Use embedded files:</b> Choose this option to use data from biomet files embedded in the LI-COR GHG files. This option is only available for GHG files collected with the LI-7550 embedded software version 6.0.0 or newer, provided a biomet system was used during data collection. EddyPro will automatically read biomet files from the GHG bundles, interpret them and extract relevant variables."));

    biomExtFileRadio = new QRadioButton(tr("Use external file:"));
    biomExtFileRadio->setToolTip(tr("<b>Use external file:</b> Select this option if you have all biomet data collected in one only external file, and provide the path to this file by using the <b><i>Load...</i></b> button. <br /><b>IMPORTANT:</b> The biomet file must be formatted according the guidelines that you can find in EddyPro Help and User\'s Guide. Click on the question mark at the right side of the <b><i>Load...</i></b> button to access the guidelines on EddyPro Help."));

    biometExtFileBrowse = new FileBrowseWidget;
    biometExtFileBrowse->setToolTip(tr("<b>Load:</b> Load an existing biomet external file"));
    biometExtFileBrowse->setDialogTitle(tr("Select the Biomet File"));
    biometExtFileBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());
#if defined(Q_OS_MACOS)
    biometExtFileBrowse->setDialogFilter(tr("All Files (*.*)"));
#else
    biometExtFileBrowse->setDialogFilter(tr("%1 Biomet Files (*.csv);;All Files (*.*)").arg(Defs::APP_NAME));
#endif

    biomExtDirRadio = new QRadioButton(tr("Use external directory:"));
    biomExtDirRadio->setToolTip(tr("<b>Use external directory:</b> Select this option if you have biomet data collected in more than one external file, and provide the path to the directory that contains those files by using the <b><i>Browse...</i></b> button. <br /><b>IMPORTANT:</b> All biomet files must be formatted according the guidelines that you can find in EddyPro Help and User\'s Guide. Click on the question mark at the right side of the <b><i>Browse...</i></b> button to access the guidelines page on EddyPro Help."));

    biometExtDirBrowse = new DirBrowseWidget;
    biometExtDirBrowse->setToolTip(tr("<b>Browse :</b> Use to specify the "
                                      "folder that contains the external biomet "
                                      "data. If data are also contained in "
                                      "subfolders, select the <i>Search in "
                                      "subfolders</i> box."));
    biometExtDirBrowse->setDialogTitle(tr("Select the Biomet Files Directory"));
    biometExtDirBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());

    biomExtDirRecCheckBox = new QCheckBox;
    biomExtDirRecCheckBox->setText(tr("Search in subfolders"));
    biomExtDirRecCheckBox->setToolTip(tr("<b>Search in subfolders:</b> Check this box if biomet files are in subfolders in the selected directory."));

    biomExtDirSuffixLabel = new ClickLabel(tr("Files extension :"), this);
    biomExtDirSuffixLabel->setToolTip(tr("<b>Files extension :</b> Select or directly edit the extension of the biomet files in the selected folder. We recommend removing any other file with the same extension from this folder (and possibly its subfolders) to avoid conflicts."));
    biomExtDirCombo = new QComboBox;
    biomExtDirCombo->setEditable(true);
    biomExtDirCombo->addItems(QStringList()
                              << QStringLiteral("txt")
                              << QStringLiteral("dat")
                              << QStringLiteral("csv")
                              << QStringLiteral("met")
                              << QStringLiteral("meteo"));
    biomExtDirCombo->setStyleSheet(QStringLiteral("QComboBox {min-width: 72px;"
                                                 "max-width: 72px;}"));
    biomExtDirCombo->setToolTip(biomExtDirSuffixLabel->toolTip());

    biomRadioGroup = new QButtonGroup(this);
    biomRadioGroup->addButton(biomEmbFileRadio, 0);
    biomRadioGroup->addButton(biomExtFileRadio, 1);
    biomRadioGroup->addButton(biomExtDirRadio, 2);

    auto altMetadataFileRadioBox = new QHBoxLayout;
    altMetadataFileRadioBox->addWidget(altMetadataFileRadio);
    altMetadataFileRadioBox->addWidget(questionMark_3);
    altMetadataFileRadioBox->addStretch();
//    altMetadataFileRadioBox->setSpacing(0);
//    altMetadataFileRadioBox->setContentsMargins(0, 0, 0, 0);

    auto gridLeft = new QGridLayout;
    gridLeft->addWidget(titleLabel, 0, 0, Qt::AlignRight);
    gridLeft->addWidget(titleEdit, 0, 1, 1, 3);
    gridLeft->addWidget(fileTypeLabel, 1, 0, Qt::AlignRight);
    gridLeft->addWidget(ghgFileTyperRadio, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_1, 1, 2, Qt::AlignLeft);
    gridLeft->addWidget(asciiFileTypeRadio, 2, 1, Qt::AlignLeft);
    gridLeft->addWidget(binaryFileTypeRadio, 3, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_5, 3, 2, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(binSettingsButton, 3, 3);
    gridLeft->addWidget(tobFileTypeRadio, 4, 1, Qt::AlignLeft);
    gridLeft->addWidget(questionMark_4, 4, 2, 1, 1, Qt::AlignLeft);
    gridLeft->addWidget(tobSettingsCombo, 4, 3);
    gridLeft->addWidget(sltEddysoftFileTypeRadio, 5, 1, Qt::AlignLeft);
    gridLeft->addWidget(sltEdisolFileTypeRadio, 6, 1, Qt::AlignLeft);
    gridLeft->setRowMinimumHeight(1, 21);
    gridLeft->setRowMinimumHeight(2, 21);
    gridLeft->setRowMinimumHeight(3, 21);
    gridLeft->setRowMinimumHeight(4, 21);
    gridLeft->setRowMinimumHeight(5, 21);
    gridLeft->setRowMinimumHeight(6, 21);
    gridLeft->setRowStretch(7, 1);
    gridLeft->setColumnStretch(4, 1);
    gridLeft->setVerticalSpacing(3);

    auto gridRight = new QGridLayout;
    gridRight->addWidget(metadataLabel, 1, 0, Qt::AlignRight);
    gridRight->addWidget(embMetadataFileRadio, 1, 1, 1, 1, Qt::AlignLeft);
    gridRight->addLayout(altMetadataFileRadioBox, 2, 1);
    gridRight->addWidget(metadataFileBrowse, 3, 1, 1, 3);
    gridRight->addWidget(dynamicMdCheckBox, 5, 0, Qt::AlignRight);
    gridRight->addWidget(dynamicMdFileBrowse, 5, 1, 1, 3);
    gridRight->addWidget(questionMark_2, 5, 4);
    gridRight->addWidget(biomDataCheckBox, 7, 0, Qt::AlignRight);
    gridRight->addWidget(biomEmbFileRadio, 7, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biomExtFileRadio, 8, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biometExtFileBrowse, 8, 2, 1, 2);
    gridRight->addWidget(questionMark_6, 8, 4);
    gridRight->addWidget(biomExtDirRadio, 9, 1, 1, 1, Qt::AlignLeft);
    gridRight->addWidget(biometExtDirBrowse, 9, 2, 1, 2);
    gridRight->addWidget(biomExtDirRecCheckBox, 10, 1, Qt::AlignRight);
    gridRight->addWidget(biomExtDirSuffixLabel, 10, 2, Qt::AlignRight);
    gridRight->addWidget(biomExtDirCombo, 10, 3);
    gridRight->setRowMinimumHeight(4, 44);
    gridRight->setRowStretch(11, 1);
    gridRight->setRowMinimumHeight(1, 21);
    gridRight->setRowMinimumHeight(2, 21);
    gridRight->setRowMinimumHeight(3, 21);
    gridRight->setRowMinimumHeight(4, 21);
    gridRight->setRowMinimumHeight(5, 21);
    gridRight->setRowMinimumHeight(6, 21);
    gridRight->setRowMinimumHeight(7, 21);
    gridRight->setRowMinimumHeight(8, 21);
    gridRight->setRowMinimumHeight(9, 21);
    gridRight->setRowMinimumHeight(10, 21);
    gridRight->setVerticalSpacing(3);
    gridRight->setColumnStretch(2, 1);
    gridRight->setContentsMargins(0, 0, 50, 0);

    auto hBox = new QGridLayout;
    hBox->addLayout(gridLeft, 0, 0);
    hBox->addLayout(gridRight, 0, 1);
    hBox->setColumnStretch(0, 1);
    hBox->setColumnStretch(1, 1);

    auto upGroupTitle = new QLabel(tr("Project Info"));
    upGroupTitle->setProperty("groupTitle", true);

    auto splitter = new Splitter(Qt::Vertical, this);
    splitter->addWidget(WidgetUtils::getContainerScrollArea(this, hBox));
    splitter->addWidget(metadataEditors);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->handle(1)->setToolTip(tr("Handle the separator."));
    splitter->setContentsMargins(15, 15, 15, 10);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(smartfluxBar_);
    mainLayout->addWidget(upGroupTitle);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(ecProject_, &EcProject::ecProjectNew,
            this, &ProjectPage::reset);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &ProjectPage::refresh);

    connect(dlIniDialog_, &DlIniDialog::metadataFileSaved,
            this, &ProjectPage::updateMetadataFileBrowse);
    connect(dlIniDialog_, &DlIniDialog::mdFileEditClearRequest,
            metadataFileBrowse, &FileBrowseWidget::clear);

    connect(titleLabel, &ClickLabel::clicked,
            this, &ProjectPage::onTitleLabelClicked);
    connect(titleEdit, &QLineEdit::textChanged, [=](const QString& s)
            { ecProject_->setGeneralTitle(s); });

    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateFileType(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fileTypeRadioClicked_1(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fileTypeRadioClicked_2(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUseMetadataFile_1(int)));
    connect(fileTypeRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(selectWidget(int)));

    connect(tobSettingsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(tobSettingsUpdate(int)));
    connect(tobSettingsCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTooltip(int)));

    connect(binSettingsButton, &QPushButton::clicked,
            this, &ProjectPage::binSettingsDialog);

    connect(metadataRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(metadataRadioClicked(int)));
    connect(metadataRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUseMetadataFile_2(int)));

    connect(metadataFileBrowse, &FileBrowseWidget::clearRequested,
            this, &ProjectPage::mdResetRequest);
    connect(metadataFileBrowse, &FileBrowseWidget::pathChanged,
            this, &ProjectPage::updateMetadataFile);
    connect(metadataFileBrowse, &FileBrowseWidget::pathSelected,
            this, &ProjectPage::metadataFileSelected);

    connect(dynamicMdCheckBox, &QCheckBox::toggled, [=](bool checked)
            { dynamicMdFileBrowse->setEnabled(checked); });
    connect(dynamicMdCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setGeneralUseTimelineFile(checked); });
    connect(dynamicMdFileBrowse, &FileBrowseWidget::pathChanged,
            this, &ProjectPage::updateTimelineFile);
    connect(dynamicMdFileBrowse, &FileBrowseWidget::pathSelected,
            this, &ProjectPage::dynamicMdFileSelected);

    connect(biomDataCheckBox, &QCheckBox::toggled,
            this, &ProjectPage::on_biomDataCheckBox_clicked);
    connect(biomRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(on_biomRadioGroup_clicked_1(int)));
    connect(biomRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(on_biomRadioGroup_clicked_2(int)));

    connect(biometExtFileBrowse, &FileBrowseWidget::pathChanged,
            this, &ProjectPage::updateBiomFile);
    connect(biometExtFileBrowse, &FileBrowseWidget::pathSelected,
            this, &ProjectPage::biomExtFileSelected);

    connect(biometExtDirBrowse, &DirBrowseWidget::pathChanged,
            this, &ProjectPage::updateBiomDir);
    connect(biometExtDirBrowse, &DirBrowseWidget::pathSelected,
            this, &ProjectPage::biometExtDirSelected);

    connect(biomExtDirRecCheckBox, &QCheckBox::toggled,
            this, &ProjectPage::updateExtDirRec);

    connect(biomExtDirSuffixLabel, &ClickLabel::clicked,
            this, &ProjectPage::onBiomExtDirSuffixLabelClicked);
    connect(biomExtDirCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateExtDirSuffix(QString)));
    connect(biomExtDirCombo, &QComboBox::editTextChanged,
            this, &ProjectPage::updateExtDirSuffix);

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));

    QTimer::singleShot(0, this, SLOT(reset()));
}

ProjectPage::~ProjectPage()
{
    delete binDialog_;
}

// modeless dialog
void ProjectPage::createMetadataEditor()
{
    auto helpLabel = new QLabel;
    helpLabel->setText(tr("This dialog will activate if you: <br /><ul><li>select a "
                          "Raw file format different than LI-COR GHG</li><br />"
                          "<li>select Metadata file: Use alternative file</li></ul>"));
    helpLabel->setObjectName(QStringLiteral("helpLabel"));

    auto helpLayout = new QVBoxLayout;
    helpLayout->addWidget(helpLabel);
    helpLayout->addStretch();

    helpGroup_ = new QFrame;
    helpGroup_->setLayout(helpLayout);

    dlIniDialog_ = new DlIniDialog(this, dlProject_, configState_);
    dlIniDialog_->setObjectName(QStringLiteral("DlIniDialog"));

    metadataTab = new QStackedWidget;
    metadataTab->addWidget(helpGroup_);
    metadataTab->addWidget(dlIniDialog_);
    metadataTab->setCurrentIndex(0);
}

void ProjectPage::selectWidget(int filetype)
{
    auto type = static_cast<Defs::RawFileType>(filetype);
    auto previousType = static_cast<Defs::RawFileType>(previousFileType_);

    if (filetype != previousFileType_)
    {
        if (type == Defs::RawFileType::GHG)
        {
            if (!isMetadataEditorOn_)
            {
                helpGroup_->show();
                dlIniDialog_->hide();
            }
        }
        else if (previousType == Defs::RawFileType::GHG)
        {
            helpGroup_->hide();
            dlIniDialog_->show();
        }
    }
}

void ProjectPage::metadataFileSelected(const QString& file_path)
{
    QFileInfo mdDir(file_path);
    auto mdPath = mdDir.canonicalPath();
    configState_->window.last_data_path = mdPath;
    GlobalSettings::updateLastDatapath(mdPath);

    auto embedded = false;
    if (dlIniDialog_->openFile(file_path, embedded))
    {
        updateMetadataFileBrowse(file_path);
        altMetadataFileRadio->setChecked(true);
        isMetadataEditorOn_ = true;
        metadataTab->setCurrentIndex(1);
    }
}

void ProjectPage::biomExtFileSelected(const QString& fp)
{
    biometExtFileBrowse->setPath(fp);
    updateMetadataLoading();

    QFileInfo mdDir(fp);
    auto mdPath = mdDir.canonicalPath();
    configState_->window.last_data_path = mdPath;
    GlobalSettings::updateLastDatapath(mdPath);
}

void ProjectPage::onTitleLabelClicked()
{
    titleEdit->setFocus();
    titleEdit->selectAll();
}

void ProjectPage::updateMetadataFileBrowse(const QString &filename)
{
    metadataFileBrowse->setPath(filename);
    updateMetadataLoading();
}

void ProjectPage::updateFileType(int filetype)
{
    ecProject_->setGeneralFileType(static_cast<Defs::RawFileType>(filetype));
}

void ProjectPage::updateUseMetadataFile_1(int filetype)
{
    auto type = static_cast<Defs::RawFileType>(filetype);

    if (type == Defs::RawFileType::GHG)
    {
        ecProject_->setGeneralUseAltMdFile(altMetadataFileRadio->isChecked());
    }
    else
    {
        ecProject_->setGeneralUseAltMdFile(true);
    }
    updateMetadataLoading();
}

void ProjectPage::updateUseMetadataFile_2(int radio)
{
    ecProject_->setGeneralUseAltMdFile(radio);
    updateMetadataLoading();
}

void ProjectPage::updateMetadataFile(const QString& fp)
{
    previousMetadataFile_ = currentMetadataFile_;
    currentMetadataFile_ = QDir::cleanPath(fp);

    ecProject_->setGeneralMdFilepath(currentMetadataFile_);
}

void ProjectPage::updateBiomFile(const QString& fp)
{
    ecProject_->setGeneralBiomFile(QDir::cleanPath(fp));
}

void ProjectPage::updateBiomDir(const QString& fp)
{
    ecProject_->setGeneralBiomDir(QDir::cleanPath(fp));
}

void ProjectPage::onBiomExtDirSuffixLabelClicked()
{
    biomExtDirCombo->setFocus();
    biomExtDirCombo->showPopup();
}

void ProjectPage::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    titleEdit->clear();
    fileTypeRadioGroup->buttons().at(0)->setChecked(true);
    binSettingsButton->setEnabled(false);
    tobSettingsCombo->setEnabled(false);
    WidgetUtils::resetComboToItem(tobSettingsCombo, 0);

    embMetadataFileRadio->setEnabled(true);
    embMetadataFileRadio->setChecked(true);
    altMetadataFileRadio->setChecked(false);
    metadataRadioClicked(0);

    mdEditorReset();
    metadataFileBrowse->setEnabled(false);

    dynamicMdCheckBox->setChecked(false);
    dynamicMdFileBrowse->setEnabled(false);
    dynamicMdFileBrowse->clear();

    biomDataCheckBox->setChecked(false);
    biomEmbFileRadio->setChecked(true);
    biometExtFileBrowse->clear();
    biometExtDirBrowse->clear();
    biomExtDirRecCheckBox->setChecked(false);
    biomExtDirCombo->setCurrentIndex(0);
    on_biomDataCheckBox_clicked(false);

    refreshMetadata();
    updateMetadataLoading();

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void ProjectPage::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    if (titleEdit->text() != ecProject_->generalTitle())
        titleEdit->setText(ecProject_->generalTitle());

    fileTypeRadioGroup
            ->buttons().at(static_cast<int>(ecProject_->generalFileType()))
            ->setChecked(true);
    fileTypeRadioClicked_2(static_cast<int>(ecProject_->generalFileType()));

    tobSettingsCombo->setCurrentIndex(ecProject_->generalTob1Format());

    embMetadataFileRadio->setChecked(!ecProject_->generalUseAltMdFile());
    altMetadataFileRadio->setChecked(ecProject_->generalUseAltMdFile());
    metadataRadioClicked(ecProject_->generalUseAltMdFile());

    refreshMetadata();

    dynamicMdCheckBox->setChecked(ecProject_->generalUseTimelineFile());
    dynamicMdFileBrowse->setPath(ecProject_->generalTimelineFilepath());
    dynamicMdFileBrowse->setEnabled(dynamicMdCheckBox->isChecked());

    int useBiom = ecProject_->generalUseBiomet();
    biomDataCheckBox->setChecked(useBiom);
    if (useBiom)
    {
        biomRadioGroup->buttons().at(useBiom - 1)->setChecked(true);
    }
    else
    {
        if ((ecProject_->generalFileType() == Defs::RawFileType::GHG))
        {
            biomEmbFileRadio->setChecked(true);
        }
        else
        {
            biomExtFileRadio->click();
//            biomRadioGroup->buttons().at(1)->setChecked(true);
        }
    }
    biometExtFileBrowse->setPath(ecProject_->generalBiomFile());
    biometExtDirBrowse->setPath(ecProject_->generalBiomDir());
    biomExtDirRecCheckBox->setChecked(ecProject_->generalBiomRecurse());

    QString s(ecProject_->generalBiomExt());
    if (!s.isEmpty())
    {
        int n = biomExtDirCombo->findText(s);
        if (n < 0)
        {
            biomExtDirCombo->addItem(s);
            biomExtDirCombo->setCurrentIndex(biomExtDirCombo->findText(s));
        }
        else
        {
            biomExtDirCombo->setCurrentIndex(n);
        }
    }
    else
    {
        biomExtDirCombo->setCurrentIndex(0);
    }
    on_biomDataCheckBox_clicked(biomDataCheckBox->isChecked());

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

// triggered by reset() and refresh()
void ProjectPage::refreshMetadata()
{
    QString mdFile(ecProject_->generalMdFilepath());
    if (mdFile != metadataFileBrowse->path())
    {
        if (!mdFile.isEmpty())
        {
            if (QFile::exists(mdFile))
            {
                bool embedded = !ecProject_->generalUseAltMdFile();
                if (dlIniDialog_->openFile(mdFile, embedded))
                {
                    updateMetadataFileBrowse(mdFile);
                }
                // silently discard and clean metadata editor
                else
                {
                    mdEditorReset();
                }
            }
            else
            {
                // schedule a silent cleanup request
                // at the end of a project file loading (refresh)
                emit mdCleanupRequest();
            }
        }
        // silently discard and clean metadata editor
        else
        {
            mdEditorReset();
        }
    }

    if (ecProject_->generalUseAltMdFile())
    {
        isMetadataEditorOn_ = true;
        metadataTab->setCurrentIndex(1);
    }
    else
    {
        isMetadataEditorOn_ = false;
        metadataTab->setCurrentIndex(0);
    }
}

void ProjectPage::updateMetadataLoading()
{
    updateMetadataFile(metadataFileBrowse->path());

    if (!currentMetadataFile_.isEmpty())
    {
        if (QFile::exists(currentMetadataFile_))
        {
            dlIniDialog_->setNewFlag(false);
            emit updateMetadataReadRequest();
        }
    }
}

void ProjectPage::fileTypeRadioClicked_1(int fileType)
{
    auto type = static_cast<Defs::RawFileType>(fileType);
    if (type == Defs::RawFileType::GHG)
    {
        if (ecProject_->generalMdFilepath().isEmpty())
        {
            isMetadataEditorOn_ = false;
            metadataTab->setCurrentIndex(0);
        }
        else
        {
            isMetadataEditorOn_ = true;
            metadataTab->setCurrentIndex(1);
        }
    }
    else
    {
        isMetadataEditorOn_ = true;
        metadataTab->setCurrentIndex(1);
    }

    previousFileType_ = currentFileType_;
    currentFileType_ = static_cast<int>(type);
}

void ProjectPage::fileTypeRadioClicked_2(int fileType)
{
    auto type = static_cast<Defs::RawFileType>(fileType);

    // if licor
    if (type == Defs::RawFileType::GHG)
    {
        embMetadataFileRadio->setEnabled(true);
        embMetadataFileRadio->setChecked(metadataFileBrowse->path().isEmpty());
        altMetadataFileRadio->setChecked(!(metadataFileBrowse->path()).isEmpty());

        metadataFileBrowse->setEnabled(altMetadataFileRadio->isChecked());

        biomEmbFileRadio->setEnabled(biomDataCheckBox->isChecked());
    }
    else
    {
        embMetadataFileRadio->setChecked(false);
        embMetadataFileRadio->setEnabled(false);
        altMetadataFileRadio->setChecked(true);
        metadataFileBrowse->setEnabled(true);

        biomEmbFileRadio->setEnabled(false);
        if (biomEmbFileRadio->isChecked())
        {
            biomExtFileRadio->click();
        }
    }

    tobSettingsCombo->setEnabled(type == Defs::RawFileType::TOB1);

    binSettingsButton->setEnabled(type == Defs::RawFileType::BIN);

    if (type == Defs::RawFileType::SLT1 || type == Defs::RawFileType::SLT2)
    {
        dlIniDialog_->rawFileDescTab()->rawSettingsButton->setEnabled(false);
    }
    else
    {
        dlIniDialog_->rawFileDescTab()->rawSettingsButton->setEnabled(true);
    }
}

void ProjectPage::metadataRadioClicked(int b)
{
    isMetadataEditorOn_ = b;
    metadataTab->setCurrentIndex(b);

    // licor file type
    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        // with or without alternative metadata
        metadataFileBrowse->setEnabled(b);
    }
    else
    {
        metadataFileBrowse->setEnabled(true);
    }
}

void ProjectPage::createQuestionMark()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_2 = new QPushButton;
    questionMark_2->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_3 = new QPushButton;
    questionMark_3->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_4 = new QPushButton;
    questionMark_4->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_5 = new QPushButton;
    questionMark_5->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_6 = new QPushButton;
    questionMark_6->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_2);
    connect(questionMark_2, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_3);
    connect(questionMark_3, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_4);
    connect(questionMark_4, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_5);
    connect(questionMark_5, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_6);
    connect(questionMark_6, &QPushButton::clicked,
            this, &ProjectPage::onlineHelpTrigger_7);
}

// used to avoid loading projects with no ghg or with
// alternative metadata which could block the basic settings page.
// moreover, set the biomet embedded case
void ProjectPage::setSmartfluxUI()
{
    bool on = configState_->project.smartfluxMode;

    // block project modified() signal
    auto oldmod = false;
    if (!on)
    {
        // save the modified flag to prevent side effects of setting widgets
        oldmod = ecProject_->modified();
        ecProject_->blockSignals(true);
    }

    if (on)
    {
        fileTypeRadioGroup->button(0)->click();
        metadataRadioGroup->button(0)->click();

        dynamicMdCheckBox->setChecked(false);
        dynamicMdFileBrowse->clear();

        biomDataCheckBox->setChecked(true);
        biomRadioGroup->button(0)->click();
    }

    // restore project modified() signal
    if (!on)
    {
        // restore modified flag
        ecProject_->setModified(oldmod);
        ecProject_->blockSignals(false);
    }
}

void ProjectPage::onlineHelpTrigger_2()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/ghg-file-format.html")));
}

void ProjectPage::onlineHelpTrigger_3()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/dynamic-metadata.html")));
}

void ProjectPage::onlineHelpTrigger_4()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/metadata-file-use-alternative.html")));
}

void ProjectPage::onlineHelpTrigger_5()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/tob1-files.html")));
}

void ProjectPage::onlineHelpTrigger_6()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/generic-binary-files.html")));
}

void ProjectPage::onlineHelpTrigger_7()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/biomet-data-format.html")));
}

void ProjectPage::binSettingsDialog()
{
    if (!binDialog_)
    {
        binDialog_ = new BinarySettingsDialog(this, ecProject_);
        emit connectBinarySettingsRequest();
    }
    binDialog_->refresh();

    binDialog_->show();
    binDialog_->raise();
    binDialog_->activateWindow();
}

void ProjectPage::tobSettingsUpdate(int n)
{
    ecProject_->setGeneralTob1Format(n);
}

void ProjectPage::updateTimelineFile(const QString& fp)
{
    ecProject_->setGeneralTimelineFilepath(QDir::cleanPath(fp));
}

void ProjectPage::dynamicMdFileSelected(const QString& fp)
{
    dynamicMdFileBrowse->setPath(fp);

    QFileInfo paramFilePath(fp);
    auto lastPath = paramFilePath.canonicalPath();
    configState_->window.last_data_path = lastPath;
    GlobalSettings::updateLastDatapath(lastPath);
}

void ProjectPage::mdResetRequest()
{
    if (!dlIniDialog()->requestMetadataReset()) { return; }

    mdEditorReset();
    metadataFileBrowse->clear();
    emit requestBasicSettingsClear();
}

void ProjectPage::mdEditorReset()
{
    dlIniDialog_->defaults_2();
}

void ProjectPage::updateTooltip(int i)
{
    auto senderCombo = qobject_cast<QComboBox *>(sender());

    WidgetUtils::updateComboItemTooltip(senderCombo, i);
}

void ProjectPage::on_biomDataCheckBox_clicked(bool clicked)
{
    auto radio_list = QWidgetList() << biomExtFileRadio
                                   << biomExtDirRadio;
    for (auto w : radio_list)
    {
        w->setEnabled(clicked);
    }

    if (clicked)
    {
        ecProject_->setGeneralUseBiomet(biomRadioGroup->checkedId() + 1);

        biomEmbFileRadio->setEnabled(ecProject_->generalFileType()
                                     == Defs::RawFileType::GHG);
        if (ecProject_->generalFileType() != Defs::RawFileType::GHG)
        {
            if (ecProject_->generalUseBiomet() == 1)
            {
                biomExtFileRadio->click();
            }
        }
        emit setOutputBiometRequest();
    }
    else
    {
        ecProject_->setGeneralUseBiomet(0);
        biomEmbFileRadio->setEnabled(false);
    }

    biometExtFileBrowse->setEnabled(ecProject_->generalUseBiomet() == 2);
    biometExtDirBrowse->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirRecCheckBox->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirSuffixLabel->setEnabled(ecProject_->generalUseBiomet() == 3);
    biomExtDirCombo->setEnabled(ecProject_->generalUseBiomet() == 3);
}

void ProjectPage::on_biomRadioGroup_clicked_1(int button)
{
    ecProject_->setGeneralUseBiomet(button + 1);
}

void ProjectPage::on_biomRadioGroup_clicked_2(int button)
{
    switch (button)
    {
        case 0:
            biometExtFileBrowse->setEnabled(false);
            biometExtDirBrowse->setEnabled(false);
            biomExtDirRecCheckBox->setEnabled(false);
            biomExtDirSuffixLabel->setEnabled(false);
            biomExtDirCombo->setEnabled(false);
            break;
        case 1:
            biometExtFileBrowse->setEnabled(true);
            biometExtDirBrowse->setEnabled(false);
            biomExtDirRecCheckBox->setEnabled(false);
            biomExtDirSuffixLabel->setEnabled(false);
            biomExtDirCombo->setEnabled(false);
            break;
        case 2:
            biometExtFileBrowse->setEnabled(false);
            biometExtDirBrowse->setEnabled(true);
            biomExtDirRecCheckBox->setEnabled(true);
            biomExtDirSuffixLabel->setEnabled(true);
            biomExtDirCombo->setEnabled(true);
            break;
        default:
            break;
    }
}

void ProjectPage::updateExtDirRec(bool b)
{
    ecProject_->setGeneralBiomRecurse(b);
}

void ProjectPage::biometExtDirSelected(const QString& dir_path)
{
    biometExtDirBrowse->setPath(dir_path);

    QDir dataDir(dir_path);
    auto canonicalDataDir = dataDir.canonicalPath();
    configState_->window.last_data_path = canonicalDataDir;
    GlobalSettings::updateLastDatapath(canonicalDataDir);
}

void ProjectPage::updateExtDirSuffix(const QString& s)
{
    if (s.isEmpty())
    {
        WidgetUtils::warning(this,
                             tr("Files suffix"),
                             tr("Enter a non empty string."));

        biomExtDirCombo->setCurrentIndex(0);
        return;
    }
    ecProject_->setGeneralBiomExt(s);
}

void ProjectPage::updateSmartfluxBar()
{
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}
