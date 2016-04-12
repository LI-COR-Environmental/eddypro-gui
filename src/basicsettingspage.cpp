/***************************************************************************
  basicsettingspage.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#include "basicsettingspage.h"

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QNetworkReply>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QScrollArea>
#include <QSpinBox>
#include <QtConcurrentRun>
#include <QTimeEdit>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

#include <cmath>

#if defined(Q_OS_MAC)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
#endif

#include <boost/math/common_factor.hpp>

#if defined(Q_OS_MAC)
#pragma clang diagnostic pop
#endif

#include "QProgressIndicator.h"

#include "advancedsettingspage.h"
#include "advprocessingoptions.h"
#include "advsettingscontainer.h"
#include "biommetadatareader.h"
#include "clicklabel.h"
#include "configstate.h"
#include "customclearlineedit.h"
#include "dbghelper.h"
#include "defs.h"
#include "dirbrowsewidget.h"
#include "dlproject.h"
#include "ecproject.h"
#include "fileutils.h"
#include "fileformatwidget.h"
#include "globalsettings.h"
#include "infomessage.h"
#include "process.h"
#include "rawfilenamedialog.h"
#include "smartfluxbar.h"
#include "splitter.h"
#include "widget_utils.h"

// for the qobject_cast in handleCrossWindAndAngleOfAttackUpdate()
#include "mainwidget.h"

const QString BasicSettingsPage::FLAG_POLICY_STRING_0 = QObject::tr("Above threshold");
const QString BasicSettingsPage::FLAG_POLICY_STRING_1 = QObject::tr("Below threshold");

BasicSettingsPage::BasicSettingsPage(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    findFileProgressWidget(nullptr),
    progressWidget_2(nullptr),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config),
    rawFilenameDialog(nullptr),
    suffixList_(QStringList()),
    httpManager_(nullptr),
    httpReply_(nullptr),
    progressWidget_3(nullptr),
    currentRawDataList_(QStringList()),
    currentFilteredRawDataList_(QStringList()),
    biomList_(QList<BiomItem>())
{
    DEBUG_FUNC_NAME

    findFileProgressWidget = new QProgressIndicator;
    findFileProgressWidget->setAnimationDelay(40);
    findFileProgressWidget->setDisplayedWhenStopped(false);
    findFileProgressWidget->setFixedSize(21, 21);
    findFileProgressWidget->setColor(QColor(46, 98, 152));

    progressWidget_2 = new QProgressIndicator;
    progressWidget_2->setAnimationDelay(40);
    progressWidget_2->setDisplayedWhenStopped(false);
    progressWidget_2->setFixedSize(21, 21);
    progressWidget_2->setColor(QColor(46, 98, 152));

    progressWidget_3 = new QProgressIndicator;
    progressWidget_3->setAnimationDelay(40);
    progressWidget_3->setDisplayedWhenStopped(false);
    progressWidget_3->setFixedSize(21, 21);
    progressWidget_3->setColor(QColor(46, 98, 152));

    datapathLabel = new ClickLabel(tr("Raw data directory :"), this);
    datapathLabel->setToolTip(tr("<b>Raw data directory:</b> Use the <i>Browse...</i> button to specify the folder that contains the raw data. If data are also contained in subfolders, select the <i>Search in subfolders</i> box."));

    datapathBrowse = new DirBrowseWidget;
    datapathBrowse->disableClearAction();
    datapathBrowse->setToolTip(datapathLabel->toolTip());
    datapathBrowse->setDialogTitle(tr("Select the Raw Data Directory"));
    datapathBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());

    recursionCheckBox = new QCheckBox;
    recursionCheckBox->setText(tr("Search in subfolders"));
    recursionCheckBox->setToolTip(tr("<b>Search in subfolders:</b> Check this box if data are in subfolders in the selected directory. EddyPro will process files that are in the <i>Raw data directory</i> and <i>subfolders</i> if you check this box."));

    filesFound = new QLabel;
    filesFound->setProperty("greyLabel", true);

    filePrototypeLabel = new ClickLabel;
    filePrototypeLabel->setText(tr("Raw file name format :"));
    filePrototypeLabel->setToolTip(tr("<b>Raw file name format:</b> For raw files other than GHG, your entry in this field should provide a template of the file names that EddyPro uses to retrieve the timestamp. You must indicate which part of the file name represent the year (<i>yy</i> or <i>yyyy</i>), month (<i>mm</i>, if using <i>dd</i> for the day, omit if using <i>ddd</i>), day (<i>dd</i> for the day of the month, <i>ddd</i> for the day of the year), hour (<i>HH</i>), minute (<i>MM</i>), and the extension of the file. The question mark can match any single character. For example, for a file name of the type: '2015-05-27_1030_mysite-12.raw', a valid raw file name format is: 'yyyy-mm-dd_HHMM_mysite-??.raw'. Remember to include the file extension!"));

    filePrototypeEdit = new FileFormatWidget;
    filePrototypeEdit->setReadOnly(false);
    filePrototypeEdit->setToolTip(tr("Open the <i>Raw file name format</i> dialog box to change the raw file name format."));
    filePrototypeEdit->setButtonText(tr("Set..."));
    filePrototypeEdit->disableClickAction();

    outpathLabel = new ClickLabel(tr("Output directory :"), this);
    outpathLabel->setToolTip(tr("<b>Output directory:</b> Specify where the output files will be stored. Click the <i>Browse...</i> button and navigate to the desired directory. You can also type/edit it directly from this text box. Note that the software will create subfolders inside the selected output directory."));

    outpathBrowse = new DirBrowseWidget;
    outpathBrowse->setReadOnly(false);
    outpathBrowse->setToolTip(outpathLabel->toolTip());
    outpathBrowse->setDialogTitle(tr("Select the Output Directory"));
    outpathBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());

    previousDatapathLabel = new ClickLabel(tr("Previous results directory :"), this);
    previousDatapathLabel->setToolTip(tr("<b>Previous results directory:</b> Path of the directory containing results from previous run(s). EddyPro will attempt to speed up the flux computation by checking for any partial results obtained from previous run(s). If settings used in the previous runs match the current settings, EddyPro will use the results as an intermediate starting point in the current data processing session."));
    previousDatapathLabel->setProperty("optionalField", true);

    previousDatapathBrowse = new DirBrowseWidget;
    previousDatapathBrowse->setToolTip(previousDatapathLabel->toolTip());
    previousDatapathBrowse->setDialogTitle(tr("Select the Previous Results Directory"));
    previousDatapathBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());

    idLabel = new ClickLabel(tr("Output ID :"));
    idLabel->setToolTip(tr("<b>Output ID:</b> Enter the ID. This string will be appended to each output file name so a short ID is recommended. Note that characters that result in file names that are unacceptable to the commonest operating systems (this includes | \\ / : ; ? * ' \" < > CR LF TAB SPACE and other non readable characters) are not permitted."));

    idEdit = new CustomClearLineEdit;
    idEdit->setToolTip(idLabel->toolTip());
    idEdit->setMaxLength(255);
    idEdit->setMaximumWidth(outpathBrowse->returnLineEditWidth());

    // prevent filesystem's illegal characters and whitespace insertion:
    // exclude the first 33 (from 0 to 32) ASCII chars, including
    // '\0'(NUL),'\a'(BEL),'\b'(BS),'\t'(TAB),'\n'(LF),'\v'(VT),'\f'(FF),'\r'(CR) and ' '(SPACE)
    // plus the following:
    // '|', '\', '/', ':', ';', '?', '*', '"', ''', '`', '<', '>'
    QString idRegexp = QStringLiteral("[^\\000-\\040|\\\\/:;\\?\\*\"'`<>]+");

    idEdit->setRegExp(idRegexp);

    avgIntervalLabel = new ClickLabel(tr("Flux averaging interval :"), this);
    avgIntervalLabel->setToolTip(tr("<b>Flux averaging interval:</b> This is the time span over which fluxes will be averaged. The flux averaging interval can be shorter than, equal to, or longer than the raw file duration. Set <i>0</i> to use the input file duration as the flux averaging interval, in which case <i>File as is</i> appears in the field."));
    avgIntervalSpin = new QSpinBox(this);
    avgIntervalSpin->setRange(0, 1440);
    avgIntervalSpin->setSingleStep(1);
    avgIntervalSpin->setValue(30);
    avgIntervalSpin->setSuffix(QStringLiteral(" [min]"));
    avgIntervalSpin->setAccelerated(true);
    avgIntervalSpin->setSpecialValueText(tr("File as is"));
    avgIntervalSpin->setToolTip(avgIntervalLabel->toolTip());
    avgIntervalSpin->setMinimumWidth(110);
    avgIntervalSpin->setMaximumWidth(125);

    maxLackLabel = new ClickLabel(tr("Missing samples allowance :"), this);
    maxLackLabel->setToolTip(tr("<b>Missing sample allowance:</b> Enter the "
                                "maximum percentage of missing data allowed "
                                "for each variable, for each averaging interval. "
                                "If the percentage of missing values exceeds "
                                "this threshold for a given variable, all "
                                "results that need that variable will not "
                                "be computed. Data can be 'missing' either "
                                "because absent in the raw data files, or "
                                "because eliminated during one of the raw "
                                "data screening procedures, e.g. de-spiking."));
    maxLackSpin = new QSpinBox;
    maxLackSpin->setRange(0, 40);
    maxLackSpin->setSingleStep(1);
    maxLackSpin->setValue(10);
    maxLackSpin->setAccelerated(true);
    maxLackSpin->setSuffix(tr("  [%]", "Percentage"));
    maxLackSpin->setToolTip(maxLackLabel->toolTip());
    maxLackSpin->setMinimumWidth(110);
    maxLackSpin->setMaximumWidth(125);

    lockedIcon = new QLabel;
    auto pixmap_2x = QPixmap(QStringLiteral(":/icons/vlink-locked"));
#if defined(Q_OS_MAC)
    pixmap_2x.setDevicePixelRatio(2.0);
#endif
    lockedIcon->setPixmap(pixmap_2x);

    startDateLabel = new ClickLabel(this);
    startDateLabel->setText(tr("Start :"));
    startDateLabel->setToolTip(tr("<b>Start:</b> Starting date of the dataset to process. This may or may not coincide with the date of the first raw file. It is used to select a subset of available raw data for processing."));
    startDateEdit = new QDateEdit;
    startDateEdit->setToolTip(startDateLabel->toolTip());
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setMinimumWidth(70);
    startDateEdit->setMaximumWidth(70);
    WidgetUtils::customizeCalendar(startDateEdit->calendarWidget());

    startTimeEdit = new QTimeEdit;
    startTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    startTimeEdit->setMinimumWidth(70);
    startTimeEdit->setMaximumWidth(70);
    startTimeEdit->setAccelerated(true);

    auto startDateContainer = new QHBoxLayout;
    startDateContainer->addWidget(startDateEdit);
    startDateContainer->insertSpacing(1, 10);
    startDateContainer->addWidget(startTimeEdit);
    startDateContainer->addStretch();
    startDateContainer->setContentsMargins(0, 0, 0, 0);

    endDateLabel = new ClickLabel(this);
    endDateLabel->setText(tr("End :"));
    endDateLabel->setToolTip(tr("<b>End:</b> Ending date of the dataset to process. This may or may not coincide with the date of the last raw file. It is used to select a subset of available raw data for processing."));
    endDateEdit = new QDateEdit;
    endDateEdit->setToolTip(endDateLabel->toolTip());
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setMinimumWidth(70);
    endDateEdit->setMaximumWidth(70);
    WidgetUtils::customizeCalendar(endDateEdit->calendarWidget());

    endTimeEdit = new QTimeEdit;
    endTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    endTimeEdit->setMinimumWidth(70);
    endTimeEdit->setMaximumWidth(70);
    endTimeEdit->setAccelerated(true);

    auto endDateContainer = new QHBoxLayout;
    endDateContainer->addWidget(endDateEdit);
    endDateContainer->insertSpacing(1, 10);
    endDateContainer->addWidget(endTimeEdit);
    endDateContainer->addStretch();
    endDateContainer->setContentsMargins(0, 0, 0, 0);

    subsetCheckBox = new QCheckBox;
    subsetCheckBox->setText(tr("Select a different period"));
    subsetCheckBox->setToolTip(tr("<b>Select a different period:</b> Select this option if you only want to process a subset of data in the raw data directory. Leave it blank to process all available raw data."));
    subsetCheckBox->setStyleSheet(QStringLiteral("QCheckBox {margin-left: 13px}"));

    dateRangeDetectButton = new QPushButton(tr("Detect Dataset Dates"));
    dateRangeDetectButton->setProperty("mdButton", true);
    dateRangeDetectButton->setMinimumWidth(dateRangeDetectButton->sizeHint().width());
    dateRangeDetectButton->setMaximumWidth(dateRangeDetectButton->sizeHint().width());
    dateRangeDetectButton->setToolTip(tr("<b>Detect Dataset Dates:</b> Click this button to ask EddyPro to retrieve the starting and ending date of the raw dataset contained in the <i>Raw data directory</i>. You can override this automatic setting by using the <i>Select a different period</i> option."));

    crossWindCheckBox = new QCheckBox(tr("Cross wind correction of sonic temperature applied by the anemometer firmware"));
    crossWindCheckBox->setToolTip(tr("<b>Cross-wind correction for sonic temperature:</b> Check this box if the crosswind correction is applied internally by the anemometer firmware before outputting sonic temperature. Be aware that some anemometers do apply the correction internally, others not, and others provide it as an option.<br />"
                                     "Users of Gill WindMaster and WindMaster Pro: the crosswind correction is not applied internally in anemometer units of type 1352, while it is available in the firmware of later types 1561 and 1590."));
    crossWindCheckBox->setProperty("paddedCheckbox", true);

    northLabel = new QLabel(tr("North reference :"));
    northLabel->setToolTip(tr("<b>North reference:</b> Indicate whether you want the outputs to be referenced to magnetic or geographic north. If you choose geographic north, EddyPro can retrieve the Magnetic Declination at your site from NOAA (U.S. National Oceanic and Atmospheric Administration) online resources (http://www.ngdc.noaa.gov/geomag-web/calculators/calculateDeclination). You can also enter the magnetic declination manually. EddyPro assumes that north is assessed at the site using the compass, so that everything you provide to the software is with respect to local geographic north. If, instead, your measurements are taken with respect to due north, than just select <i>Use magnetic North</i> or enter a declination of zero degrees."));

    useMagneticNRadio = new QRadioButton;
    useMagneticNRadio->setText(tr("Use magnetic North"));
    useGeographicNRadio = new QRadioButton;
    useGeographicNRadio->setText(tr("Use geographic North"));

    auto northRadioGroup = new QButtonGroup(this);
    northRadioGroup->addButton(useMagneticNRadio, 0);
    northRadioGroup->addButton(useGeographicNRadio, 1);

    declinationLabel = new ClickLabel(tr("Magnetic declination :"));
    declinationEdit = new QLineEdit;
    declinationEdit->setText(tr("000%1 00' E").arg(Defs::DEGREE));
    QString dec_pattern = tr("(?:(0\\d\\d)%1\\s([0-5]\\d)'\\s(E|W))|").arg(Defs::DEGREE);
            dec_pattern += tr("(?:(1[0-7]\\d)%1\\s([0-5]\\d)'\\s(E|W))|").arg(Defs::DEGREE);
            dec_pattern += tr("(?:(180)%1\\s(00)'\\s(E|W))").arg(Defs::DEGREE);
    QRegExp decRx(dec_pattern);
    auto decValidator = new QRegExpValidator(decRx, declinationEdit);
    declinationEdit->setValidator(decValidator);
    declinationEdit->setInputMask(tr("000%1 00' >A;x").arg(Defs::DEGREE));
    declinationEdit->setAlignment(Qt::AlignRight);
    declinationEdit->setMinimumWidth(110);
    declinationEdit->setMaximumWidth(125);

    declinationDateLabel = new ClickLabel(this);
    declinationDateLabel->setText(tr("On :"));
    declinationDateLabel->setToolTip(tr("<b> Date :</b> Date used to retrieve the magnetic declination from the NOAA website."));
    declinationDateEdit = new QDateEdit;
    declinationDateEdit->setToolTip(declinationDateLabel->toolTip());
    declinationDateEdit->setCalendarPopup(true);
    declinationDateEdit->setMinimumWidth(70);
    declinationDateEdit->setMaximumWidth(70);
    WidgetUtils::customizeCalendar(declinationDateEdit->calendarWidget());
    // NOTE: manage NOAA website API limitation, where current last day available is 2019-12-31
    // compare http://www.ngdc.noaa.gov/geomag-web/#declination
    declinationDateEdit->setMaximumDate(QDate(2019, 12, 31));

    decChangingLabel = new QLabel;
    decChangingLabel->setTextFormat(Qt::PlainText);
    decChangingLabel->setObjectName(QStringLiteral("citeLabel"));

    declinationFetchButton = new QPushButton(tr("Fetch from NOAA"));
    declinationFetchButton->setProperty("mdButton", true);
    declinationFetchButton->setMinimumWidth(declinationFetchButton->sizeHint().width());
    declinationFetchButton->setMaximumWidth(declinationFetchButton->sizeHint().width());

    createQuestionMark();

    auto fileGroupTitle = new QLabel(tr("Files Info"));
    fileGroupTitle->setProperty("groupTitle", true);

    auto filesInfoLayout = new QGridLayout;
    filesInfoLayout->addWidget(datapathLabel, 1, 0, Qt::AlignRight);
    filesInfoLayout->addWidget(datapathBrowse, 1, 2, 1, 3);
    filesInfoLayout->addWidget(filesFound, 2, 4, 1, 1, Qt::AlignRight);
    filesInfoLayout->addWidget(findFileProgressWidget, 2, 4, 1, 1, Qt::AlignCenter);
    filesInfoLayout->addWidget(recursionCheckBox, 2, 2, 1, 2);
    filesInfoLayout->addWidget(subsetCheckBox, 3, 3, 1, 2, Qt::AlignLeft);
    filesInfoLayout->addWidget(dateRangeDetectButton, 3, 2, 1, 1, Qt::AlignLeft);
    filesInfoLayout->addWidget(progressWidget_2, 3, 4, 1, 1, Qt::AlignLeft);
    filesInfoLayout->addWidget(startDateLabel, 4, 0, Qt::AlignRight);
    filesInfoLayout->addLayout(startDateContainer, 4, 2, 1, 2);
    filesInfoLayout->addWidget(lockedIcon, 4, 1, 2, 1, Qt::AlignCenter);
    filesInfoLayout->addWidget(endDateLabel, 5, 0, Qt::AlignRight);
    filesInfoLayout->addLayout(endDateContainer, 5, 2, 1, 2);

    filesInfoLayout->addWidget(filePrototypeLabel, 6, 0, Qt::AlignRight);
    filesInfoLayout->addWidget(questionMark_4, 6, 1);
    filesInfoLayout->addWidget(filePrototypeEdit, 6, 2, 1, 3);
    filesInfoLayout->addWidget(outpathLabel, 7, 0, Qt::AlignRight);
    filesInfoLayout->addWidget(outpathBrowse, 7, 2, 1, 3);
    filesInfoLayout->addWidget(idLabel, 8, 0, Qt::AlignRight);
    filesInfoLayout->addWidget(idEdit, 8, 2, 1, 2);
    filesInfoLayout->addWidget(previousDatapathLabel, 9, 0, Qt::AlignRight);
    filesInfoLayout->addWidget(questionMark_2, 9, 1, Qt::AlignLeft);
    filesInfoLayout->addWidget(previousDatapathBrowse, 9, 2, 1, 3);

    filesInfoLayout->addWidget(maxLackLabel, 1, 5, Qt::AlignRight);
    filesInfoLayout->addWidget(maxLackSpin, 1, 7, 1, 1);
    filesInfoLayout->addWidget(avgIntervalLabel, 2, 5, Qt::AlignRight);
    filesInfoLayout->addWidget(avgIntervalSpin, 2, 7);
    filesInfoLayout->addWidget(northLabel, 4, 5, Qt::AlignRight);
    filesInfoLayout->addWidget(useMagneticNRadio, 4, 7, 1, 2);
    filesInfoLayout->addWidget(useGeographicNRadio, 5, 7, 1, 2);
    filesInfoLayout->addWidget(declinationFetchButton, 5, 9, 1, 2);
    filesInfoLayout->addWidget(declinationLabel, 6, 5, Qt::AlignRight);
    filesInfoLayout->addWidget(declinationEdit, 6, 7);
    filesInfoLayout->addWidget(declinationDateLabel, 6, 8, Qt::AlignRight);
    filesInfoLayout->addWidget(declinationDateEdit, 6, 9);
    filesInfoLayout->addWidget(questionMark_5, 6, 10, Qt::AlignLeft);
    filesInfoLayout->addWidget(decChangingLabel, 7, 7, 1, -1);
    filesInfoLayout->addWidget(progressWidget_3, 8, 7);
    filesInfoLayout->setRowStretch(10, 1);
    filesInfoLayout->setRowMinimumHeight(1, 21);
    filesInfoLayout->setRowMinimumHeight(2, 21);
    filesInfoLayout->setRowMinimumHeight(3, 21);
    filesInfoLayout->setRowMinimumHeight(4, 21);
    filesInfoLayout->setRowMinimumHeight(5, 21);
    filesInfoLayout->setRowMinimumHeight(6, 21);
    filesInfoLayout->setRowMinimumHeight(7, 21);
    filesInfoLayout->setRowMinimumHeight(8, 21);
    filesInfoLayout->setRowMinimumHeight(9, 21);
    filesInfoLayout->setColumnMinimumWidth(1, 10);
    filesInfoLayout->setColumnMinimumWidth(5, 150);
    filesInfoLayout->setColumnStretch(0, 1);
    filesInfoLayout->setColumnStretch(1, 0);
    filesInfoLayout->setColumnStretch(2, 0);
    filesInfoLayout->setColumnStretch(3, 0);
    filesInfoLayout->setColumnStretch(4, 1);
    filesInfoLayout->setColumnStretch(5, 1);
    filesInfoLayout->setColumnStretch(6, 0);
    filesInfoLayout->setColumnStretch(7, 1);
    filesInfoLayout->setColumnStretch(8, 0);
    filesInfoLayout->setColumnStretch(9, 1);
    filesInfoLayout->setColumnStretch(10, 0);
    filesInfoLayout->setVerticalSpacing(3);
    filesInfoLayout->setContentsMargins(0, 0, 50, 15);

    anemRefLabel = new ClickLabel(tr("Master Anemometer :"), this);
    anemRefLabel->setToolTip(tr("<b>Master anemometer:</b> Select the sonic anemometer from which wind and sonic temperature data should be used for calculating fluxes."));
    anemRefCombo = new QComboBox;
    anemRefCombo->setToolTip(anemRefLabel->toolTip());

    anemFlagLabel = new ClickLabel(tr("Anemometer Diagnostics:"), this);
    anemFlagLabel->setToolTip(tr("<b>anemometer Diagnostics:</b> Select ..."));
    anemFlagCombo = new QComboBox;
    anemFlagCombo->setToolTip(anemFlagLabel->toolTip());

    tsRefLabel = new ClickLabel(tr("Fast temperature reading (alternative to sonic temp) :"), this);
    tsRefLabel->setToolTip(tr("<b>Fast temperature reading:</b> If raw files contain valid readings of air temperature collected at high frequency (e.g. by a thermocouple), you can use any of them in place of sonic temperature. In this case, corrections specific to sonic temperature (cross-wind correction, humidity correction), will not be applied."));
    tsRefCombo = new QComboBox;
    tsRefCombo->setToolTip(tsRefLabel->toolTip());

    co2RefLabel = new ClickLabel(tr("%1 :").arg(Defs::CO2_STRING), this);
    co2RefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    co2RefCombo = new QComboBox;
    co2RefCombo->setToolTip(co2RefLabel->toolTip());

    h2oRefLabel = new ClickLabel(tr("%1 :").arg(Defs::H2O_STRING), this);
    h2oRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    h2oRefCombo = new QComboBox;
    h2oRefCombo->setToolTip(h2oRefLabel->toolTip());

    ch4RefLabel = new ClickLabel(tr("%1 :").arg(Defs::CH4_STRING), this);
    ch4RefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    ch4RefCombo = new QComboBox;
    ch4RefCombo->setToolTip(ch4RefLabel->toolTip());

    fourthGasRefLabel = new ClickLabel(tr("%1 trace gas (passive scalar) :").arg(Defs::GAS4_STRING), this);
    fourthGasRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    fourthGasRefCombo = new QComboBox;
    fourthGasRefCombo->setToolTip(fourthGasRefLabel->toolTip());

    gasMwLabel = new ClickLabel(tr("Molecular weight :"), this);
    gasMw = new QDoubleSpinBox;
    gasMw->setDecimals(4);
    gasMw->setRange(0.0, 1000.0);
    gasMw->setSingleStep(1.0);
    gasMw->setAccelerated(true);
    gasMw->setSuffix(QStringLiteral(" [g/mol]"));
    gasMw->setMinimumWidth(130);

    gasDiffLabel = new ClickLabel(tr("Molecular diffusivity in air :"), this);
    gasDiff = new QDoubleSpinBox;
    gasDiff->setDecimals(5);
    gasDiff->setRange(0.0, 1.0);
    gasDiff->setSingleStep(0.1);
    gasDiff->setAccelerated(true);
    gasDiff->setSuffix(QStringLiteral(" [%1]").arg(Defs::CM2_S_STRING));
    gasDiff->setMinimumWidth(130);

    moreButton = new QPushButton;
    moreButton->setObjectName(QStringLiteral("foldButton"));
    moreButton->setCheckable(true);
    moreButton->setChecked(false);
    moreButton->setAutoDefault(false);

    auto extensionLayout = new QGridLayout;
    extensionLayout->setContentsMargins(0, 0, 0, 0);
    extensionLayout->addWidget(gasMwLabel, 0, 0, Qt::AlignRight);
    extensionLayout->addWidget(gasMw, 0, 1);
    extensionLayout->addWidget(gasDiffLabel, 1, 0, Qt::AlignRight);
    extensionLayout->addWidget(gasDiff, 1, 1);
    gasExtension = new QWidget;
    gasExtension->setLayout(extensionLayout);
    gasExtension->hide();

    intTcRefLabel = new ClickLabel(tr("Average Cell Temperature :"), this);
    intTcRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    intTcRefCombo = new QComboBox;
    intTcRefCombo->setToolTip(intTcRefLabel->toolTip());

    intT1RefLabel = new ClickLabel(tr("Cell Temperature In :"), this);
    intT1RefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    intT1RefCombo = new QComboBox;
    intT1RefCombo->setToolTip(intT1RefLabel->toolTip());

    intT2RefLabel = new ClickLabel(tr("Cell Temperature Out :"), this);
    intT2RefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    intT2RefCombo = new QComboBox;
    intT2RefCombo->setToolTip(intT2RefLabel->toolTip());

    intPRefLabel = new ClickLabel(tr("Cell Pressure :"), this);
    intPRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    intPRefCombo = new QComboBox;
    intPRefCombo->setToolTip(intPRefLabel->toolTip());

    airTRefLabel = new ClickLabel(tr("Ambient Temperature :"), this);
    airTRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    airTRefCombo = new QComboBox;
    airTRefCombo->setToolTip(airTRefLabel->toolTip());

    airPRefLabel = new ClickLabel(tr("Ambient Pressure :"), this);
    airPRefLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    airPRefCombo = new QComboBox;
    airPRefCombo->setToolTip(airPRefLabel->toolTip());

    rhLabel = new ClickLabel(tr("Ambient Relative Humidity :"), this);
    rhLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    rhCombo = new QComboBox;
    rhCombo->setToolTip(rhLabel->toolTip());

    rgLabel = new ClickLabel(tr("Global Radiation :"), this);
    rgLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    rgCombo = new QComboBox;
    rgCombo->setToolTip(rgLabel->toolTip());

    lwinLabel = new ClickLabel(tr("Longwave Incoming Radiation :"), this);
    lwinLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    lwinCombo = new QComboBox;
    lwinCombo->setToolTip(lwinLabel->toolTip());

    ppfdLabel = new ClickLabel(tr("Photosynthetically Active Radiation (PAR, PPFD) :"), this);
    ppfdLabel->setToolTip(tr("Select the variables to be used for calculating fluxes, among those available."));
    ppfdCombo = new QComboBox;
    ppfdCombo->setToolTip(ppfdLabel->toolTip());

    diag7500Label = new ClickLabel(tr("LI-7500/A/RS Diagnostics :"), this);
    diag7500Label->setToolTip(tr("Select the variables to be used for diagnostics of this gas analyzer."));
    diag7500Combo = new QComboBox;
    diag7500Combo->setToolTip(diag7500Label->toolTip());

    diag7200Label = new ClickLabel(tr("LI-7200/RS Diagnostics :"), this);
    diag7200Label->setToolTip(tr("Select the variables to be used for diagnostics of this gas analyzer."));
    diag7200Combo = new QComboBox;
    diag7200Combo->setToolTip(diag7200Label->toolTip());

    diag7700Label = new ClickLabel(tr("LI-7700 Diagnostics :"), this);
    diag7700Label->setToolTip(tr("Select the variables to be used for diagnostics of this gas analyzer."));
    diag7700Combo = new QComboBox;
    diag7700Combo->setToolTip(diag7700Label->toolTip());

    flag1Label = new ClickLabel(tr("Flag 1 :"), this);
    flag1Label->setObjectName(QStringLiteral("flag1Label"));
    flag1Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag1Label->setProperty("optionalField", true);
    flag1VarCombo = new QComboBox;
    flag1VarCombo->setObjectName(QStringLiteral("flag1Combo"));
    flag1VarCombo->setMaxVisibleItems(20);
    flag1VarCombo->setToolTip(flag1Label->toolTip());
    flag1UnitLabel = new QLabel;
    flag1UnitLabel->setObjectName(QStringLiteral("flag1UnitLabel"));
    flag1UnitLabel->setProperty("flagLabel", true);
    flag1ThresholdSpin = new QDoubleSpinBox;
    flag1ThresholdSpin->setDecimals(10);
    flag1ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag1ThresholdSpin->setSingleStep(1.0);
    flag1ThresholdSpin->setAccelerated(true);
    flag1PolicyCombo = new QComboBox;
    flag1PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag1PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag2Label = new ClickLabel(tr("Flag 2 :"), this);
    flag2Label->setObjectName(QStringLiteral("flag2Label"));
    flag2Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag2Label->setProperty("optionalField", true);
    flag2VarCombo = new QComboBox;
    flag2VarCombo->setObjectName(QStringLiteral("flag2Combo"));
    flag2VarCombo->setMaxVisibleItems(20);
    flag2VarCombo->setToolTip(flag2Label->toolTip());
    flag2UnitLabel = new QLabel;
    flag2UnitLabel->setObjectName(QStringLiteral("flag2UnitLabel"));
    flag2UnitLabel->setProperty("flagLabel", true);
    flag2ThresholdSpin = new QDoubleSpinBox;
    flag2ThresholdSpin->setDecimals(10);
    flag2ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag2ThresholdSpin->setSingleStep(1.0);
    flag2ThresholdSpin->setAccelerated(true);
    flag2PolicyCombo = new QComboBox;
    flag2PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag2PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag3Label = new ClickLabel(tr("Flag 3 :"), this);
    flag3Label->setObjectName(QStringLiteral("flag3Label"));
    flag3Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag3Label->setProperty("optionalField", true);
    flag3VarCombo = new QComboBox;
    flag3VarCombo->setObjectName(QStringLiteral("flag3Combo"));
    flag3VarCombo->setMaxVisibleItems(20);
    flag3VarCombo->setToolTip(flag3Label->toolTip());
    flag3UnitLabel = new QLabel;
    flag3UnitLabel->setObjectName(QStringLiteral("flag3UnitLabel"));
    flag3UnitLabel->setProperty("flagLabel", true);
    flag3ThresholdSpin = new QDoubleSpinBox;
    flag3ThresholdSpin->setDecimals(10);
    flag3ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag3ThresholdSpin->setSingleStep(1.0);
    flag3ThresholdSpin->setAccelerated(true);
    flag3PolicyCombo = new QComboBox;
    flag3PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag3PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag4Label = new ClickLabel(tr("Flag 4 :"), this);
    flag4Label->setObjectName(QStringLiteral("flag4Label"));
    flag4Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag4Label->setProperty("optionalField", true);
    flag4VarCombo = new QComboBox;
    flag4VarCombo->setObjectName(QStringLiteral("flag4Combo"));
    flag4VarCombo->setMaxVisibleItems(20);
    flag4VarCombo->setToolTip(flag4Label->toolTip());
    flag4UnitLabel = new QLabel;
    flag4UnitLabel->setObjectName(QStringLiteral("flag4UnitLabel"));
    flag4UnitLabel->setProperty("flagLabel", true);
    flag4ThresholdSpin = new QDoubleSpinBox;
    flag4ThresholdSpin->setDecimals(10);
    flag4ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag4ThresholdSpin->setSingleStep(1.0);
    flag4ThresholdSpin->setAccelerated(true);
    flag4PolicyCombo = new QComboBox;
    flag4PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag4PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag5Label = new ClickLabel(tr("Flag 5 :"), this);
    flag5Label->setObjectName(QStringLiteral("flag5Label"));
    flag5Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag5Label->setProperty("optionalField", true);
    flag5VarCombo = new QComboBox;
    flag5VarCombo->setObjectName(QStringLiteral("flag5Combo"));
    flag5VarCombo->setMaxVisibleItems(20);
    flag5VarCombo->setToolTip(flag5Label->toolTip());
    flag5UnitLabel = new QLabel;
    flag5UnitLabel->setObjectName(QStringLiteral("flag5UnitLabel"));
    flag5UnitLabel->setProperty("flagLabel", true);
    flag5ThresholdSpin = new QDoubleSpinBox;
    flag5ThresholdSpin->setDecimals(10);
    flag5ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag5ThresholdSpin->setSingleStep(1.0);
    flag5ThresholdSpin->setAccelerated(true);
    flag5PolicyCombo = new QComboBox;
    flag5PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag5PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag6Label = new ClickLabel(tr("Flag 6 :"), this);
    flag6Label->setObjectName(QStringLiteral("flag6Label"));
    flag6Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag6Label->setProperty("optionalField", true);
    flag6VarCombo = new QComboBox;
    flag6VarCombo->setObjectName(QStringLiteral("flag6Combo"));
    flag6VarCombo->setMaxVisibleItems(20);
    flag6VarCombo->setToolTip(flag6Label->toolTip());
    flag6UnitLabel = new QLabel;
    flag6UnitLabel->setObjectName(QStringLiteral("flag6UnitLabel"));
    flag6UnitLabel->setProperty("flagLabel", true);
    flag6ThresholdSpin = new QDoubleSpinBox;
    flag6ThresholdSpin->setDecimals(10);
    flag6ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag6ThresholdSpin->setSingleStep(1.0);
    flag6ThresholdSpin->setAccelerated(true);
    flag6PolicyCombo = new QComboBox;
    flag6PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag6PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag7Label = new ClickLabel(tr("Flag 7 :"), this);
    flag7Label->setObjectName(QStringLiteral("flag7Label"));
    flag7Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag7Label->setProperty("optionalField", true);
    flag7VarCombo = new QComboBox;
    flag7VarCombo->setObjectName(QStringLiteral("flag7Combo"));
    flag7VarCombo->setMaxVisibleItems(20);
    flag7VarCombo->setToolTip(flag7Label->toolTip());
    flag7UnitLabel = new QLabel;
    flag7UnitLabel->setObjectName(QStringLiteral("flag7UnitLabel"));
    flag7UnitLabel->setProperty("flagLabel", true);
    flag7ThresholdSpin = new QDoubleSpinBox;
    flag7ThresholdSpin->setDecimals(10);
    flag7ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag7ThresholdSpin->setSingleStep(1.0);
    flag7ThresholdSpin->setAccelerated(true);
    flag7PolicyCombo = new QComboBox;
    flag7PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag7PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag8Label = new ClickLabel(tr("Flag 8 :"), this);
    flag8Label->setObjectName(QStringLiteral("flag8Label"));
    flag8Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag8Label->setProperty("optionalField", true);
    flag8VarCombo = new QComboBox;
    flag8VarCombo->setMaxVisibleItems(20);
    flag8VarCombo->setToolTip(flag8Label->toolTip());
    flag8VarCombo->setObjectName(QStringLiteral("flag8Combo"));
    flag8UnitLabel = new QLabel;
    flag8UnitLabel->setObjectName(QStringLiteral("flag8UnitLabel"));
    flag8UnitLabel->setProperty("flagLabel", true);
    flag8ThresholdSpin = new QDoubleSpinBox;
    flag8ThresholdSpin->setDecimals(10);
    flag8ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag8ThresholdSpin->setSingleStep(1.0);
    flag8ThresholdSpin->setAccelerated(true);
    flag8PolicyCombo = new QComboBox;
    flag8PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag8PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag9Label = new ClickLabel(tr("Flag 9 :"), this);
    flag9Label->setObjectName(QStringLiteral("flag9Label"));
    flag9Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag9Label->setProperty("optionalField", true);
    flag9Label->setToolTip(flag9Label->toolTip());
    flag9VarCombo = new QComboBox;
    flag9VarCombo->setObjectName(QStringLiteral("flag9Combo"));
    flag9VarCombo->setToolTip(flag9Label->toolTip());
    flag9VarCombo->setMaxVisibleItems(20);
    flag9UnitLabel = new QLabel;
    flag9UnitLabel->setObjectName(QStringLiteral("flag9UnitLabel"));
    flag9UnitLabel->setProperty("flagLabel", true);
    flag9ThresholdSpin = new QDoubleSpinBox;
    flag9ThresholdSpin->setDecimals(10);
    flag9ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag9ThresholdSpin->setSingleStep(1.0);
    flag9ThresholdSpin->setAccelerated(true);
    flag9PolicyCombo = new QComboBox;
    flag9PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag9PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    flag10Label = new ClickLabel(tr("Flag 10 :"), this);
    flag10Label->setObjectName(QStringLiteral("flag10Label"));
    flag10Label->setToolTip(tr("<b>Flags:</b> Each column of the raw data file that was not tagged as <i>to be ignored</i> can be used as a mask to filter out individual high frequency records. Up to ten flags can be specified. Note that an entire record (that is, all variables measured at a certain time instant, one line of raw data) is eliminated any time a flag variable does not comply with its quality criterion."));
    flag10Label->setProperty("optionalField", true);
    flag10Label->setToolTip(flag10Label->toolTip());
    flag10VarCombo = new QComboBox;
    flag10VarCombo->setObjectName(QStringLiteral("flag10Combo"));
    flag10VarCombo->setMaxVisibleItems(20);
    flag10VarCombo->setToolTip(flag10VarCombo->toolTip());
    flag10UnitLabel = new QLabel;
    flag10UnitLabel->setObjectName(QStringLiteral("flag10UnitLabel"));
    flag10UnitLabel->setProperty("flagLabel", true);
    flag10ThresholdSpin = new QDoubleSpinBox;
    flag10ThresholdSpin->setDecimals(10);
    flag10ThresholdSpin->setRange(-9999999.0, 9999999.0);
    flag10ThresholdSpin->setSingleStep(1.0);
    flag10ThresholdSpin->setAccelerated(true);
    flag10PolicyCombo = new QComboBox;
    flag10PolicyCombo->addItem(FLAG_POLICY_STRING_0);
    flag10PolicyCombo->addItem(FLAG_POLICY_STRING_1);

    auto varTitle_1 = new QLabel(tr("Gas measurements (eddy data, used for covariances and fluxes)"));
    varTitle_1->setProperty("groupLabel", true);
    auto varTitle_2 = new QLabel(tr("Cell measurements (closed-path eddy data, used for covariances and fluxes)"));
    varTitle_2->setProperty("groupLabel", true);
    auto varTitle_3 = new QLabel(tr("Ambient measurements (eddy or biomet data, used for flux correction and calculation of other parameters)"));
    varTitle_3->setProperty("groupLabel", true);
    auto varTitle_4 = new QLabel(tr("Diagnostic measurements (eddy data, for quality screening and assurance)"));
    varTitle_4->setProperty("groupLabel", true);

    auto varLayout = new QGridLayout;
    varLayout->addWidget(varTitle_1, 0, 1);
    varLayout->addWidget(co2RefLabel, 1, 0, Qt::AlignRight);
    varLayout->addWidget(co2RefCombo, 1, 1);
    varLayout->addWidget(h2oRefLabel, 2, 0, Qt::AlignRight);
    varLayout->addWidget(h2oRefCombo, 2, 1);
    varLayout->addWidget(ch4RefLabel, 3, 0, Qt::AlignRight);
    varLayout->addWidget(ch4RefCombo, 3, 1);
    varLayout->addWidget(fourthGasRefLabel, 4, 0, Qt::AlignRight);
    varLayout->addWidget(fourthGasRefCombo, 4, 1);
    varLayout->addWidget(moreButton, 4, 2, Qt::AlignLeft);
    varLayout->addWidget(gasExtension, 5, 1, Qt::AlignRight);
    varLayout->addWidget(varTitle_2, 6, 1);
    varLayout->addWidget(intTcRefLabel, 7, 0, Qt::AlignRight);
    varLayout->addWidget(intTcRefCombo, 7, 1);
    varLayout->addWidget(intT1RefLabel, 8, 0, Qt::AlignRight);
    varLayout->addWidget(intT1RefCombo, 8, 1);
    varLayout->addWidget(intT2RefLabel, 9, 0, Qt::AlignRight);
    varLayout->addWidget(intT2RefCombo, 9, 1);
    varLayout->addWidget(intPRefLabel, 10, 0, Qt::AlignRight);
    varLayout->addWidget(intPRefCombo, 10, 1);
    varLayout->addWidget(varTitle_3, 11, 1);
    varLayout->addWidget(airTRefLabel, 12, 0, Qt::AlignRight);
    varLayout->addWidget(airTRefCombo, 12, 1);
    varLayout->addWidget(airPRefLabel, 13, 0, Qt::AlignRight);
    varLayout->addWidget(airPRefCombo, 13, 1);
    varLayout->addWidget(rhLabel, 14, 0, Qt::AlignRight);
    varLayout->addWidget(rhCombo, 14, 1);
    varLayout->addWidget(rgLabel, 15, 0, Qt::AlignRight);
    varLayout->addWidget(rgCombo, 15, 1);
    varLayout->addWidget(lwinLabel, 16, 0, Qt::AlignRight);
    varLayout->addWidget(lwinCombo, 16, 1);
    varLayout->addWidget(ppfdLabel, 17, 0, Qt::AlignRight);
    varLayout->addWidget(ppfdCombo, 17, 1);
    varLayout->addWidget(varTitle_4, 18, 1);
    varLayout->addWidget(diag7500Label, 19, 0, Qt::AlignRight);
    varLayout->addWidget(diag7500Combo, 19, 1);
    varLayout->addWidget(diag7200Label, 20, 0, Qt::AlignRight);
    varLayout->addWidget(diag7200Combo, 20, 1);
    varLayout->addWidget(diag7700Label, 21, 0, Qt::AlignRight);
    varLayout->addWidget(diag7700Combo, 21, 1);
    varLayout->setContentsMargins(15, 0, 0, 0);
    varLayout->setRowStretch(22, 1);
    varLayout->setColumnStretch(0, 1);
    varLayout->setColumnStretch(1, 2);
    varLayout->setColumnStretch(2, 1);

    auto descLabel = new QLabel(tr("Optionally, each variable in raw data files can be "
                      "used as a mask, to filter out individual "
                      "raw records that do not conform quality "
                      "criteria. Select the variable to be used "
                      "as a flag, and define the quality "
                      "criterion by entering a threshold value "
                      "for that variable and whether to discard "
                      "records if they are above or below the thresholds.<br>"
                      "Note: If you describe more than one flag, EddyPro will "
                      "eliminate all records flagged by at least one test. "
                      "Note also that currently the same variable cannot "
                      "be used in two different flag definitions. "
                      "The result of such an operation is unpredictable; "
                      "most likely, only the latest flag definition with "
                      "the same variable will have an effect."));
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(QStringLiteral("QLabel {margin-left: 15px;}"));

    auto flagLabel = new QLabel(tr("Flags"));
    flagLabel->setProperty("groupLabel", true);
    auto unitLabel = new QLabel(tr("Unit"));
    unitLabel->setProperty("groupLabel", true);
    auto thresholdLabel = new QLabel(tr("Threshold"));
    thresholdLabel->setProperty("groupLabel", true);
    auto policyLabel = new QLabel(tr("Policy: Discard if"));
    policyLabel->setProperty("groupLabel", true);

    auto descLayout = new QHBoxLayout;
    descLayout->addWidget(questionMark_3, 0);
    descLayout->addWidget(descLabel, 1);

    auto flagLayout = new QGridLayout;
    flagLayout->addLayout(descLayout, 0, 1, 1, 4);
    flagLayout->addWidget(flagLabel, 1, 1);
    flagLayout->addWidget(unitLabel, 1, 3);
    flagLayout->addWidget(thresholdLabel, 1, 2);
    flagLayout->addWidget(policyLabel, 1, 4);
    flagLayout->addWidget(flag1Label, 2, 0, Qt::AlignRight);
    flagLayout->addWidget(flag1VarCombo, 2, 1);
    flagLayout->addWidget(flag1ThresholdSpin, 2, 2);
    flagLayout->addWidget(flag1UnitLabel, 2, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag1PolicyCombo, 2, 4);
    flagLayout->addWidget(flag2Label, 3, 0, Qt::AlignRight);
    flagLayout->addWidget(flag2VarCombo, 3, 1);
    flagLayout->addWidget(flag2ThresholdSpin, 3, 2);
    flagLayout->addWidget(flag2UnitLabel, 3, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag2PolicyCombo, 3, 4);
    flagLayout->addWidget(flag3Label, 4, 0, Qt::AlignRight);
    flagLayout->addWidget(flag3VarCombo, 4, 1);
    flagLayout->addWidget(flag3ThresholdSpin, 4, 2);
    flagLayout->addWidget(flag3UnitLabel, 4, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag3PolicyCombo, 4, 4);
    flagLayout->addWidget(flag4Label, 5, 0, Qt::AlignRight);
    flagLayout->addWidget(flag4VarCombo, 5, 1);
    flagLayout->addWidget(flag4ThresholdSpin, 5, 2);
    flagLayout->addWidget(flag4UnitLabel, 5, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag4PolicyCombo, 5, 4);
    flagLayout->addWidget(flag5Label, 6, 0, Qt::AlignRight);
    flagLayout->addWidget(flag5VarCombo, 6, 1);
    flagLayout->addWidget(flag5ThresholdSpin, 6, 2);
    flagLayout->addWidget(flag5UnitLabel, 6, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag5PolicyCombo, 6, 4);
    flagLayout->addWidget(flag6Label, 7, 0, Qt::AlignRight);
    flagLayout->addWidget(flag6VarCombo, 7, 1);
    flagLayout->addWidget(flag6ThresholdSpin, 7, 2);
    flagLayout->addWidget(flag6UnitLabel, 7, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag6PolicyCombo, 7, 4);
    flagLayout->addWidget(flag7Label, 8, 0, Qt::AlignRight);
    flagLayout->addWidget(flag7VarCombo, 8, 1);
    flagLayout->addWidget(flag7ThresholdSpin, 8, 2);
    flagLayout->addWidget(flag7UnitLabel, 8, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag7PolicyCombo, 8, 4);
    flagLayout->addWidget(flag8Label, 9, 0, Qt::AlignRight);
    flagLayout->addWidget(flag8VarCombo, 9, 1);
    flagLayout->addWidget(flag8ThresholdSpin, 9, 2);
    flagLayout->addWidget(flag8UnitLabel, 9, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag8PolicyCombo, 9, 4);
    flagLayout->addWidget(flag9Label, 10, 0, Qt::AlignRight);
    flagLayout->addWidget(flag9VarCombo, 10, 1);
    flagLayout->addWidget(flag9ThresholdSpin, 10, 2);
    flagLayout->addWidget(flag9UnitLabel, 10, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag9PolicyCombo, 10, 4);
    flagLayout->addWidget(flag10Label, 11, 0, Qt::AlignRight);
    flagLayout->addWidget(flag10VarCombo, 11, 1);
    flagLayout->addWidget(flag10ThresholdSpin, 11, 2);
    flagLayout->addWidget(flag10UnitLabel, 11, 3, Qt::AlignCenter);
    flagLayout->addWidget(flag10PolicyCombo, 11, 4);
    flagLayout->setVerticalSpacing(3);
    flagLayout->setContentsMargins(15, 0, 0, 0);
    flagLayout->setRowStretch(12, 1);
    flagLayout->setColumnMinimumWidth(3, 100);
    flagLayout->setColumnStretch(0, 1);
    flagLayout->setColumnStretch(1, 2);
    flagLayout->setColumnStretch(2, 1);
    flagLayout->setColumnStretch(3, 0);
    flagLayout->setColumnStretch(4, 1);
    flagLayout->setColumnStretch(5, 1);

    auto varList = new QGroupBox;
    varList->setObjectName(QStringLiteral("simpleGroupBox2"));
    varList->setFlat(true);
    varList->setLayout(varLayout);

    auto flagList = new QGroupBox;
    flagList->setObjectName(QStringLiteral("simpleGroupBox2"));
    flagList->setFlat(true);
    flagList->setLayout(flagLayout);

    auto varTab = new QTabWidget;
    varTab->addTab(varList, tr("Variables"));
    varTab->addTab(flagList, tr("Flags"));

    auto referenceGroupTitle = new QLabel(tr("Select Items for Flux Computation"));
    referenceGroupTitle->setProperty("groupTitle", true);

    auto varContainerLayout = new QGridLayout;
    varContainerLayout->addWidget(anemRefLabel, 0, 0, Qt::AlignRight);
    varContainerLayout->addWidget(anemRefCombo, 0, 1);
    varContainerLayout->addWidget(crossWindCheckBox, 1, 1);
    varContainerLayout->addWidget(anemFlagLabel, 2, 0, Qt::AlignRight);
    varContainerLayout->addWidget(anemFlagCombo, 2, 1);
    varContainerLayout->addWidget(tsRefLabel, 3, 0, Qt::AlignRight);
    varContainerLayout->addWidget(tsRefCombo, 3, 1);
    varContainerLayout->addWidget(varTab, 4, 0, 1, -1);
    varContainerLayout->setColumnStretch(0, 1);
    varContainerLayout->setColumnStretch(1, 2);
    varContainerLayout->setColumnStretch(2, 1);

    auto varAreaLayout = new QVBoxLayout;
    varAreaLayout->addWidget(referenceGroupTitle);
    varAreaLayout->addWidget(WidgetUtils::getContainerScrollArea(this, varContainerLayout));

    auto varArea = new QWidget;
    varArea->setLayout(varAreaLayout);

    auto splitter = new Splitter(Qt::Vertical, this);
    splitter->addWidget(WidgetUtils::getContainerScrollArea(this, filesInfoLayout));
    splitter->addWidget(varArea);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->handle(1)->setToolTip(tr("Handle the separator."));
    splitter->setContentsMargins(15, 5, 15, 0);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(smartfluxBar_);
    mainLayout->addWidget(fileGroupTitle);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    connect(ecProject_, &EcProject::ecProjectNew,
            this, &BasicSettingsPage::reset);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &BasicSettingsPage::refresh);

    connect(datapathLabel, &ClickLabel::clicked,
            this, [=](){ datapathBrowse->focusAndSelect(); });
    connect(datapathBrowse, &DirBrowseWidget::clearRequested,
            this, &BasicSettingsPage::clearDataSelection);
    connect(datapathBrowse, &DirBrowseWidget::pathChanged,
            this, &BasicSettingsPage::updateDataPath);
    connect(datapathBrowse, &DirBrowseWidget::pathSelected,
            this, &BasicSettingsPage::datapathSelected);

    connect(recursionCheckBox, &QCheckBox::toggled,
            this, &BasicSettingsPage::updateRecursion);

    connect(filePrototypeLabel, &ClickLabel::clicked,
            filePrototypeEdit, &FileFormatWidget::focusAndSelect);
    connect(filePrototypeEdit, &FileFormatWidget::pathChanged,
            this, &BasicSettingsPage::updateFilePrototype);
    connect(filePrototypeEdit->button(), &QPushButton::clicked,
            this, &BasicSettingsPage::showSetPrototype);
    connect(filePrototypeEdit, &FileFormatWidget::clearRequested,
            this, &BasicSettingsPage::clearFilePrototype);

    connect(outpathLabel, &ClickLabel::clicked,
            this, [=](){ outpathBrowse->focusAndSelect(); });
    connect(outpathBrowse, &DirBrowseWidget::pathSelected,
            this, &BasicSettingsPage::outpathBrowseSelected);
    connect(outpathBrowse, &DirBrowseWidget::pathChanged,
            this, &BasicSettingsPage::updateOutPath);

    connect(previousDatapathLabel, &ClickLabel::clicked,
            this, [=](){ previousDatapathBrowse->focusAndSelect(); });
    connect(previousDatapathBrowse, &DirBrowseWidget::pathChanged,
            this, &BasicSettingsPage::updatePreviousDataPath);
    connect(previousDatapathBrowse, &DirBrowseWidget::pathSelected,
            this, &BasicSettingsPage::previousDatapathSelected);

    connect(idLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onIdLabelClicked);
    connect(idEdit, &CustomClearLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setGeneralId(s); });

    connect(avgIntervalLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onAvgLenLabelClicked);
    connect(avgIntervalSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateAvrgLen(int)));

    connect(maxLackLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onMaxLackLabelClicked);
    connect(maxLackSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateMaxLack(int)));

    connect(subsetCheckBox, &QCheckBox::toggled,
            this, &BasicSettingsPage::updateSubsetSelection);
//    connect(subsetCheckBox, SIGNAL(toggled(bool)),
//            dateRangeDetectButton, SLOT(setDisabled(bool)));

    connect(dateRangeDetectButton, &QPushButton::clicked,
            this, &BasicSettingsPage::dateRangeDetect);

    connect(startDateLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onStartDateLabelClicked);
    connect(startDateEdit, &QDateEdit::dateChanged,
            this, &BasicSettingsPage::updateStartDate);

    connect(endDateLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onEndDateLabelClicked);
    connect(endDateEdit, &QDateEdit::dateChanged,
            this, &BasicSettingsPage::updateEndDate);
    connect(endDateEdit, &QDateEdit::dateChanged,
            this, &BasicSettingsPage::alignDeclinationDate);

    connect(startTimeEdit, &QTimeEdit::timeChanged,
            this, &BasicSettingsPage::updateStartTime);
    connect(endTimeEdit, &QTimeEdit::timeChanged,
            this, &BasicSettingsPage::updateEndTime);

    connect(anemRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickAnemRefLabel);
    connect(anemRefCombo, SIGNAL(activated(QString)),
            this, SLOT(updateAnemRefCombo(QString)));

    connect(anemFlagLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickAnemFlagLabel);
    connect(anemFlagCombo, SIGNAL(activated(int)),
            this, SLOT(updateAnemFlagCombo(int)));

    connect(co2RefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickCo2RefLabel);
    connect(co2RefCombo, SIGNAL(activated(int)),
            this, SLOT(updateCo2RefCombo(int)));

    connect(h2oRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickH2oRefLabel);
    connect(h2oRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateH2oRefCombo(int)));

    connect(ch4RefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickCh4RefLabel);
    connect(ch4RefCombo, SIGNAL(activated(int)),
            this, SLOT(updateCh4RefCombo(int)));

    connect(gasMw, SIGNAL(valueChanged(double)),
            this, SLOT(updateGasMw(double)));
    connect(gasDiff, SIGNAL(valueChanged(double)),
            this, SLOT(updateGasDiff(double)));

    connect(fourthGasRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickFourthGasRefLabel);
    connect(fourthGasRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateFourthGasRefCombo(int)));
    connect(fourthGasRefCombo, SIGNAL(activated(QString)),
            this, SLOT(updateFourthGasSettings(QString)));

    connect(intTcRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickIntTcRefLabel);
    connect(intTcRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateIntTcRefCombo(int)));

    connect(intT1RefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickIntT1RefLabel);
    connect(intT1RefCombo, SIGNAL(activated(int)),
            this, SLOT(updateIntT1RefCombo(int)));

    connect(intT2RefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickIntT2RefLabel);
    connect(intT2RefCombo, SIGNAL(activated(int)),
            this, SLOT(updateIntT2RefCombo(int)));

    connect(intPRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickIntPRefLabel);
    connect(intPRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateIntPRefCombo(int)));

    connect(airTRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickAirTRefLabel);
    connect(airTRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateAirTRefCombo(int)));

    connect(airPRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickAirPRefLabel);
    connect(airPRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateAirPRefCombo(int)));

    connect(rhLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickRhLabel);
    connect(rhCombo, SIGNAL(activated(int)),
            this, SLOT(updateRhCombo(int)));

    connect(rgLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickRgLabel);
    connect(rgCombo, SIGNAL(activated(int)),
            this, SLOT(updateRgCombo(int)));

    connect(lwinLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickLwinLabel);
    connect(lwinCombo, SIGNAL(activated(int)),
            this, SLOT(updateLwinCombo(int)));

    connect(ppfdLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickPpfdLabel);
    connect(ppfdCombo, SIGNAL(activated(int)),
            this, SLOT(updatePpfdCombo(int)));

    connect(diag7500Label, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickDiag7500Label);
    connect(diag7500Combo, SIGNAL(activated(int)),
            this, SLOT(updateDiag7500Combo(int)));

    connect(diag7200Label, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickDiag7200Label);
    connect(diag7200Combo, SIGNAL(activated(int)),
            this, SLOT(updateDiag7200Combo(int)));

    connect(diag7700Label, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickDiag7700Label);
    connect(diag7700Combo, SIGNAL(activated(int)),
            this, SLOT(updateDiag7700Combo(int)));

    connect(tsRefLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickTsRefLabel);
    connect(tsRefCombo, SIGNAL(activated(int)),
            this, SLOT(updateTsRefCombo(int)));

    connect(crossWindCheckBox, &QCheckBox::toggled,
            this, &BasicSettingsPage::updateCrossWind);

    connect(flag1VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag1Combo(int)));
    connect(flag1ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag1Threshold(double)));
    connect(flag1PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag1Policy(int)));

    connect(flag2VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag2Combo(int)));
    connect(flag2ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag2Threshold(double)));
    connect(flag2PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag2Policy(int)));

    connect(flag3VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag3Combo(int)));
    connect(flag3ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag3Threshold(double)));
    connect(flag3PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag3Policy(int)));

    connect(flag4VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag4Combo(int)));
    connect(flag4ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag4Threshold(double)));
    connect(flag4PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag4Policy(int)));

    connect(flag5VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag5Combo(int)));
    connect(flag5ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag5Threshold(double)));
    connect(flag5PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag5Policy(int)));

    connect(flag6VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag6Combo(int)));
    connect(flag6ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag6Threshold(double)));
    connect(flag6PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag6Policy(int)));

    connect(flag7VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag7Combo(int)));
    connect(flag7ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag7Threshold(double)));
    connect(flag7PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag7Policy(int)));

    connect(flag8VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag8Combo(int)));
    connect(flag8ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag8Threshold(double)));
    connect(flag8PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag8Policy(int)));

    connect(flag9VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag9Combo(int)));
    connect(flag9ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag9Threshold(double)));
    connect(flag9PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag9Policy(int)));

    connect(flag10VarCombo, SIGNAL(activated(int)),
            this, SLOT(updateFlag10Combo(int)));
    connect(flag10ThresholdSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateFlag10Threshold(double)));
    connect(flag10PolicyCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFlag10Policy(int)));

    connect(moreButton, &QPushButton::toggled,
            gasExtension, &QWidget::setVisible);

    connect(northRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(northRadioClicked(int)));
    connect(northRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateUseGeoNorth(int)));

    connect(declinationLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onClickDeclinationLabel);
    connect(declinationEdit, &QLineEdit::textChanged,
            this, &BasicSettingsPage::updateMagDec);

    connect(declinationDateLabel, &ClickLabel::clicked,
            this, &BasicSettingsPage::onDeclinationDateLabelClicked);
    connect(declinationDateEdit, &QDateEdit::dateChanged,
            this, &BasicSettingsPage::updateDeclinationDate);

    connect(declinationFetchButton, &QPushButton::clicked,
            this, &BasicSettingsPage::fetchMagneticDeclination);

    foreach (QComboBox *combo,
             QList<QComboBox *>() << flag1VarCombo
                                  << flag2VarCombo
                                  << flag3VarCombo
                                  << flag4VarCombo
                                  << flag5VarCombo
                                  << flag6VarCombo
                                  << flag7VarCombo
                                  << flag8VarCombo
                                  << flag9VarCombo
                                  << flag10VarCombo)
    {
        connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(updateFlagUnit(int)));
    }

    foreach (ClickLabel *label,
             QList<ClickLabel *>() << flag1Label
                                   << flag2Label
                                   << flag3Label
                                   << flag4Label
                                   << flag5Label
                                   << flag6Label
                                   << flag7Label
                                   << flag8Label
                                   << flag9Label
                                   << flag10Label)
    {
        connect(label, &ClickLabel::clicked,
                this, &BasicSettingsPage::onClickFlagLabel);
    }

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));

    // other inits
    QTimer::singleShot(0, this, SLOT(reset()));
}

BasicSettingsPage::~BasicSettingsPage()
{
    DEBUG_FUNC_NAME
    if (httpReply_)
    {
        qDebug() << "deleting httpReply_ ...";
        delete httpReply_;
    }
    if (httpManager_)
    {
        qDebug() << "deleting httpManager_ ...";
        delete httpManager_;
    }
}

void BasicSettingsPage::datapathSelected(const QString& dir_path)
{
    DEBUG_FUNC_NAME

    // do nothing if dir is equal to the existing now
//    if (QDir::fromNativeSeparators(dir_path) == ecProject_->screenDataPath()) { return; }

    // warning dialog
    if (handleVariableReset() == QMessageBox::Cancel) { return; }

    qDebug() << currentRawDataList_;
    currentRawDataList_.clear();
    qDebug() << currentRawDataList_;

    FileUtils::cleanSmfDirRecursively(configState_->general.env);

    datapathBrowse->setPath(dir_path);

    QDir dataDir(dir_path);
    auto canonicalDataDir = dataDir.canonicalPath();
    configState_->window.last_data_path = canonicalDataDir;
    GlobalSettings::updateLastDatapath(canonicalDataDir);

    updateMetadataRead(true);
    qDebug() << currentRawDataList_;

    if (!currentRawDataList_.isEmpty()
        || ecProject_->generalFileType() != Defs::RawFileType::GHG)
    {
        setPrototype();
    }
}

void BasicSettingsPage::showSetPrototype()
{
    bool dialogOn = true;
    setPrototype(dialogOn);
}

void BasicSettingsPage::setPrototype(bool showDialog)
{
    DEBUG_FUNC_NAME

    qDebug() << "showDialog:" << showDialog;
    // GHG case
    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        // test if the raw data folder contains different file prototypes

        // get the ghg suffixes in case of raw data path selection
        // or empty raw data list
        if (currentRawDataList_.isEmpty())
        {
            updateFilesFound(ecProject_->screenRecurse());
        }

        suffixList_ = getAvailableGhgSuffixes();
        qDebug() << "suffixList_" << suffixList_;

        // case 1: suffixes all identical
        // then define the prototype using the std GHG timestamp
        if (suffixList_.size() == 1)
        {
            qDebug() << "GHG case 1";
            updateFilePrototypeEdit(Defs::GHG_TIMESTAMP_FORMAT + suffixList_.first());

            if (showDialog)
            {
                askRawFilenamePrototype();
            }
            updateFilesFound(ecProject_->screenRecurse());
            return;
        }
        // case 2: at least 2 different suffixes or no files found
        else
        {
            qDebug() << "GHG case 2";
            askRawFilenamePrototype();
            updateFilesFound(ecProject_->screenRecurse());
            return;
        }
    }

    // non GHG cases
    askRawFilenamePrototype();
    updateFilesFound(ecProject_->screenRecurse());
}

QStringList BasicSettingsPage::getAvailableGhgSuffixes()
{
    DEBUG_FUNC_NAME
    // progressWidget_4->startAnimation();
    QFuture<QStringList> future = QtConcurrent::run(&FileUtils::getGhgFileSuffixList, currentRawDataList_);
    while (!future.isFinished())
    {
        QCoreApplication::processEvents();
    }
    // progressWidget_4->stopAnimation();
    return future.result();
}

void BasicSettingsPage::previousDatapathSelected(const QString& dir_path)
{
    DEBUG_FUNC_NAME

    previousDatapathBrowse->setPath(dir_path);

    QDir dataDir(dir_path);
    auto canonicalDataDir = dataDir.canonicalPath();
    configState_->window.last_data_path = canonicalDataDir;
    GlobalSettings::updateLastDatapath(canonicalDataDir);
}

// search and open at least one zip file, extract and read the metadata files
// inside it. with that information populate the group of combobox
// with correct values and update the processing project
void BasicSettingsPage::captureEmbeddedMetadata(EmbeddedFileFlags type)
{
    DEBUG_FUNC_NAME

    QString ghgFormat = QStringLiteral("*.") + Defs::GHG_NATIVE_DATA_FILE_EXT;
    QString mdFormat = QStringLiteral("*.") + Defs::METADATA_FILE_EXT;
    QString biometMdFormat = QStringLiteral("*%1.%2")
                            .arg(Defs::DEFAULT_BIOMET_SUFFIX)
                            .arg(Defs::METADATA_FILE_EXT);

    findFileProgressWidget->startAnimation();
    currentRawDataList_ = FileUtils::getFiles(datapathBrowse->path(), ghgFormat, ecProject_->screenRecurse());
    findFileProgressWidget->stopAnimation();

    auto filesCount = currentRawDataList_.count();
    updateFilesFoundLabel(filesCount);
    updateProjectFilesFound(filesCount);

    QString mdFile;
    QString biometMdFile;
    bool hasMd = false;
    bool hasBiometMd = false;
    foreach (const QString& zipFile, currentRawDataList_)
    {
        qDebug() << "zipFile" << zipFile;

        if (!hasMd && (type & rawEmbeddedFile))
        {
            hasMd = FileUtils::zipContainsFiletype(zipFile, mdFormat);
            qDebug() << "hasMd" << hasMd;
            if (hasMd)
            {
                mdFile = zipFile;
            }
        }

        if (!hasBiometMd && (type & biometEmbeddedFile))
        {
            hasBiometMd = FileUtils::zipContainsFiletype(zipFile, biometMdFormat);
            qDebug() << "hasBiometMd" << hasBiometMd;
            if (hasBiometMd)
            {
                biometMdFile = zipFile;
            }
        }

        if ((type & rawEmbeddedFile) && (type & biometEmbeddedFile))
        {
            if (hasMd && hasBiometMd)
            {
                break;
            }
        }
        else if ((type & rawEmbeddedFile) && !(type & biometEmbeddedFile))
        {
            if (hasMd)
            {
                break;
            }
        }
        else if (!(type & rawEmbeddedFile) && (type & biometEmbeddedFile))
        {
            if (hasBiometMd)
            {
                break;
            }
        }
    }

    // lastEmbeddedMdFileRead_ is a caching variable
    lastEmbeddedMdFileRead_ = mdFile;
    qDebug() << "lastEmbeddedMdFileRead_ 2" << lastEmbeddedMdFileRead_;

    QString homePath;
    homePath = configState_->general.env;

    // dir from where we recover extracted metadata file
    QString smfDir = homePath + QLatin1Char('/') + Defs::SMF_FILE_DIR;
    QDir mdDir(smfDir);

    if (type & rawEmbeddedFile)
    {
        if (mdFile.isEmpty())
        {
            if (this->isVisible())
            {
                WidgetUtils::warning(QApplication::activeWindow(),
                                     tr("Raw Data Missing"),
                                     tr("The selected directory doesn't "
                                        "contain any valid LI-COR GHG data."));
                clearFilesFound();
                return;
            }
            else
            {
                // NOTE: discard silently
                return;
            }
        }
        else
        {
            FileUtils::zipExtract(mdFile, smfDir);

            QStringList mdFilters;
            mdFilters << QStringLiteral("*.") + Defs::METADATA_FILE_EXT;
            mdDir.setNameFilters(mdFilters);
            mdDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            mdDir.setSorting(QDir::Name | QDir::DirsLast);

            QStringList mdFileList = mdDir.entryList();

            if (!mdFileList.isEmpty())
            {
                foreach (const QString& str, mdFileList)
                {
                    // raw embedded metadata
                    if (!str.contains(Defs::DEFAULT_BIOMET_SUFFIX))
                    {
                        if (type & rawEmbeddedFile)
                        {
                            mdFile = smfDir + QLatin1Char('/') + str;
                            qDebug() << "mdFile" << mdFile;

                            // skip reading in case of identical metadata file
//                          if (!dlProject_->matchProjectFile(mdFile))
//                          {
//                              readEmbeddedMetadata(mdFile);
//                          }

                            readEmbeddedMetadata(mdFile);
                        }
                    }
                }
            }
            else
            {
                // NOTE: discard silently
            }
        }
    }

    if (type & biometEmbeddedFile)
    {
        if (biometMdFile.isEmpty())
        {
            // prevent the message in case of smartflux cfg
            if (this->isVisible() && !configState_->project.smartfluxMode)
            {
                WidgetUtils::warning(QApplication::activeWindow(),
                                     tr("Biomet Data Missing"),
                                     tr("The selected directory doesn't "
                                        "contain any valid LI-COR GHG biomet "
                                        "data."));
                // NOTE: to avoid because unexpected
                // clearFilesFound();
                return;
            }
            else
            {
                // NOTE: discard silently
                return;
            }
        }
        else
        {
            FileUtils::zipExtract(biometMdFile, smfDir);

            QStringList mdFilters;
            mdFilters << QStringLiteral("*.") + Defs::METADATA_FILE_EXT;
            mdDir.setNameFilters(mdFilters);
            mdDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            mdDir.setSorting(QDir::Name | QDir::DirsLast);
            QStringList mdFileList = mdDir.entryList();

            if (!mdFileList.isEmpty())
            {
                foreach (const QString& str, mdFileList)
                {
                    if (str.contains(Defs::DEFAULT_BIOMET_SUFFIX))
                    {
                        if (type & biometEmbeddedFile)
                        {
                            if (!mdFileList.filter(Defs::DEFAULT_BIOMET_SUFFIX).isEmpty())
                            {
                                biometMdFile = smfDir + QLatin1Char('/') + str;
                                qDebug() << "biometMdFile" << biometMdFile;

                                readBiomEmbMetadata(biometMdFile);
                            }
                        }
                    }
                }
            }
            else
            {
                // NOTE: discard silently
            }
        }
    }
}

void BasicSettingsPage::readEmbeddedMetadata(const QString& mdFile)
{
    DEBUG_FUNC_NAME

    bool modified = false;
    if (dlProject_->loadProject(mdFile, false, &modified))
    {
        parseMetadataProject(true);
    }
    else
    {
        WidgetUtils::warning(QApplication::activeWindow(),
                             tr("Metadata Error"),
                             tr("Error reading raw data "
                                "metadata information!"));
        emit updateMetadataReadResult(false);
    }
}

void BasicSettingsPage::readAlternativeMetadata(const QString& mdFile, bool firstReading)
{
    DEBUG_FUNC_NAME

    bool modified = false;
    if (dlProject_->loadProject(mdFile, true, &modified, firstReading))
    {
        parseMetadataProject(false);
    }
    else
    {
        WidgetUtils::warning(QApplication::activeWindow(),
                             tr("Metadata Error"),
                             tr("Error reading raw data "
                                "metadata information!"));
        emit updateMetadataReadResult(false);
    }
}

void BasicSettingsPage::readBiomEmbMetadata(const QString& mdFile)
{
    DEBUG_FUNC_NAME

    // clear the list if necessary
    while (!biomList_.isEmpty())
    {
        biomList_.clear();
    }

    BiomMetadataReader reader(&biomList_);

    if (reader.readEmbMetadata(mdFile))
    {
        parseBiomMetadata();
    }
    else
    {
        WidgetUtils::warning(QApplication::activeWindow(),
                             tr("Metadata Error"),
                             tr("Error reading biomet "
                                "metadata information!"));
        emit updateMetadataReadResult(false);
    }
}

bool BasicSettingsPage::readBiomAltMetadata(const QString& mdFile)
{
    DEBUG_FUNC_NAME

    // clear the list if necessary
    while (!biomList_.isEmpty())
    {
        biomList_.clear();
    }

    BiomMetadataReader reader(&biomList_);

    if (reader.readAltMetadata(mdFile))
    {
        if (!biomList_.isEmpty())
        {
            parseBiomMetadata();
            return true;
        }
        else
        {
            // NOTE: notice to put somewhere else or to silently ignore
//            WidgetUtils::warning(QApplication::activeWindow(),
//                                 tr("Metadata Error"),
//                                 tr("No biomet "
//                                    "information!"));
//            emit updateMetadataReadResult(false);
            return false;
        }
    }
    else
    {
        WidgetUtils::warning(QApplication::activeWindow(),
                             tr("Metadata Error"),
                             tr("Error reading biomet "
                                "information!"));
        return false;
    }
}

void BasicSettingsPage::parseMetadataProject(bool isEmbedded)
{
    DEBUG_FUNC_NAME

    AnemDescList *adl = dlProject_->anems();
    VariableDescList *vdl = dlProject_->variables();
    qDebug() << "AnemDescList TOT #" << adl->count();
    qDebug() << "VariableDescList TOT #" << vdl->count();

    clearVarsCombo();
    clearFlagVars();
    clearFlagUnits();

    QList<QString> anemList;

    // parse anemometers
    int k = 0;
    foreach (const AnemDesc& anem, *adl)
    {
        ++k;
        qDebug() << "k" << k << "###===============================###";
        qDebug() << "anem" << anem.manufacturer() << anem.model();
        qDebug() << "isGoodAnemometer" << AnemDesc::isGoodAnemometer(anem);
        qDebug() << "hasGoodWindComponents()" << anem.hasGoodWindComponents();
        qDebug() << "hasGoodTemp()" << anem.hasGoodTemp();

        const QString anemUiModel = anem.model();
        const QString anemDataModel = DlProject::toIniAnemModel(anemUiModel);
        const QString fullUiAnemModel = anemUiModel + tr(" [Anemometer ") + QString::number(k) + QStringLiteral("]");
        const QString fullDataAnemModel = anemDataModel + QStringLiteral("_") + QString::number(k);

        anemList << fullDataAnemModel;

        qDebug() << "anemUiModel" << anemUiModel;
        qDebug() << "anemDataModel" << anemDataModel;
        qDebug() << "fullUiAnemModel" << fullUiAnemModel;
        qDebug() << "fullDataAnemModel" << fullDataAnemModel;
        qDebug() << "ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();

        if (isEmbedded)
        {
            qDebug() << "embedded md";
            if (AnemDesc::isGoodAnemometer(anem))
            {
                qDebug() << "isGoodAnemometer";

                anemRefLabel->setEnabled(true);
                anemRefCombo->setEnabled(true);
                crossWindCheckBox->setEnabled(true);
                anemRefCombo->addItem(fullUiAnemModel, fullDataAnemModel);
            }
        }
        else
        {
            qDebug() << "alternative md";
            qDebug() << "0 ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();

            if (AnemDesc::isGoodAnemometer(anem)
                && anem.hasGoodWindComponents())
            {
                qDebug() << "isGoodAnemometer";
                qDebug() << "hasGoodWindComponents";

                anemRefLabel->setEnabled(true);
                anemRefCombo->setEnabled(true);
                crossWindCheckBox->setEnabled(true);
                qDebug() << "00 ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();
                qDebug() << "00 test fullDataAnemModel" << fullDataAnemModel;
                qDebug() << "1 ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();
                qDebug() << "1 test fullDataAnemModel" << fullDataAnemModel;
                qDebug() << "2 ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();
                anemRefCombo->addItem(fullUiAnemModel, fullDataAnemModel);
                qDebug() << "3 ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();
            }
        }

        // test
        qDebug() << "test anemUiModel" << anemUiModel;
        qDebug() << "test anemDataModel" << anemDataModel;
        qDebug() << "test fullUiAnemModel" << fullUiAnemModel;
        qDebug() << "test fullDataAnemModel" << fullDataAnemModel;
        qDebug() << "test ecProject_->generalColMasterSonic()" << ecProject_->generalColMasterSonic();
        qDebug() << "anemRefCombo->count()" << anemRefCombo->count();
        for (int l = 0; l < anemRefCombo->count(); ++l)
        {
            qDebug() << "listing anem combo, item:" << anemRefCombo->itemText(l);
        }
    }

    qDebug() << "foreach 1";

    if (!anemList.isEmpty())
    {
        // pick the anemometer:
        // if the last master sonic is still available, just pick it
        // else, pick the first and change it accordingly
        if (anemList.contains(ecProject_->generalColMasterSonic()))
        {
            anemRefCombo->setCurrentIndex(anemList.indexOf(ecProject_->generalColMasterSonic()));
        }
        else
        {
            anemRefCombo->setCurrentIndex(0);
            ecProject_->setGeneralColMasterSonic(anemList.first());
        }
    }

    // parse described variables
    k = 0;
    foreach (const VariableDesc& var, *vdl)
    {
        ++k;
        const QString varName = var.variable();
        const QString instrType = var.instrument();
        QString measureType = var.measureType();

        const QString ignoreFlag = var.ignore();
        const QString numericFlag = var.numeric();

        // 1.3 condition
        bool isCustomLabel = (varName != VariableDesc::getVARIABLE_VAR_STRING_0())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_1())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_2())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_3())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_4())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_5())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_6())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_7())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_8())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_9())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_10())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_11())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_12())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_13())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_15())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_19())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_20())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_21())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_22())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_23())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_24())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_25())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_26())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_27())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_28())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_29())
                             && (varName != VariableDesc::getVARIABLE_VAR_STRING_30());

        if (ignoreFlag == QLatin1String("no")
            && numericFlag == QLatin1String("yes"))
        {
            qDebug() << "k" << k << "###===============================###";
            qDebug() << "varName" << varName;
            qDebug() << "isGoodWindComponent" << VariableDesc::isGoodWindComponent(var);
            qDebug() << "isGoodGas" << VariableDesc::isGoodGas(var, isCustomLabel);
            qDebug() << "isGoodTemperature" << VariableDesc::isGoodTemperature(var);
            qDebug() << "isGoodPressure" << VariableDesc::isGoodPressure(var);
            QString varString;

            // 1.1 condition
            if (!instrType.isEmpty())
            {
                // 1.2 condition
                if (!measureType.isEmpty())
                {
                    if (measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_3())
                    {
                        measureType.clear();
                    }
                    else
                    {
                        measureType.append(QLatin1Char(' '));
                    }
                }

                if (instrType != tr("Other"))
                {
                    QStringList instrList = instrType.split(QLatin1Char(':'));
                    QString instrModel = instrList.at(1).trimmed();
                    QString instrStrNumber = instrList.at(0);
                    int instrNumber = instrStrNumber.mid(5).toInt();

                    QString instrTypeStr;
                    if (instrList.at(0).split(QLatin1Char(' ')).at(0) == tr("Sonic"))
                    {
                        instrTypeStr = tr("Anemometer ");
                    }
                    else if (instrList.at(0).split(QLatin1Char(' ')).at(0) == tr("Irga"))
                    {
                        instrTypeStr = tr("Gas analyzer ");
                    }
                    else
                    {
                        instrTypeStr = QLatin1Char(' ');
                    }

                    varString = varName
                                + QLatin1Char(' ')
                                + measureType
                                + tr("from ")
                                + instrModel
                                + QStringLiteral(" [")
                                + instrTypeStr
                                + QString::number(instrNumber)
                                + QStringLiteral("]");
                }
                else
                {
                    varString = varName
                                + QLatin1Char(' ')
                                + measureType
                                + tr("from other instruments");

                }
            }
            else
            {
                varString = varName
                        + tr(" from raw data files: Column # ")
                        + QString::number(k);
            }

            qDebug() << "============varString===========" << varString;

            // gas, custom labels and cell measures section
            if (varName == VariableDesc::getVARIABLE_VAR_STRING_5()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_6()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_7()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_8()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_19()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_20()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_21()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_22()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_23()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_24()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_9()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_10()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_11()
                || varName == VariableDesc::getVARIABLE_VAR_STRING_15()
                || isCustomLabel)
            {
                if (!instrType.isEmpty())
                {
                    qDebug() << "SECTION gas, custom labels and cell measures";

                    bool isOtherMeasureType = measureType.trimmed().isEmpty();
                    qDebug() << "isOtherMeasureType" << isOtherMeasureType;

                    // CO2
                    // 1.2 and 1.3 conditions
                    if (varName == VariableDesc::getVARIABLE_VAR_STRING_5()
                            && VariableDesc::isGoodGas(var, isCustomLabel))
                    {
                        qDebug() << "INSERT varName" << varName;
                        co2RefLabel->setEnabled(true);
                        co2RefCombo->setEnabled(true);
                        co2RefCombo->addItem(varString, k);
                    }

                    // H2O
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_6()
                             && VariableDesc::isGoodGas(var, isCustomLabel))
                    {
                        qDebug() << "INSERT varName" << varName;
                        h2oRefLabel->setEnabled(true);
                        h2oRefCombo->setEnabled(true);
                        h2oRefCombo->addItem(varString, k);
                    }

                    // CH4
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_7()
                             && VariableDesc::isGoodGas(var, isCustomLabel))
                    {
                        qDebug() << "INSERT varName" << varName;
                        ch4RefLabel->setEnabled(true);
                        ch4RefCombo->setEnabled(true);
                        ch4RefCombo->addItem(varString, k);
                    }

                    // 4th gas o custom gas
                    else if (VariableDesc::isGoodGas(var, isCustomLabel))
                    {
                        qDebug() << "INSERT varName" << varName;
                        qDebug() << "INSERT varString" << varString;
                        fourthGasRefLabel->setEnabled(true);
                        fourthGasRefCombo->setEnabled(true);
                        fourthGasRefCombo->addItem(varString, k);

                        gasMwLabel->setEnabled(true);
                        gasDiffLabel->setEnabled(true);
                        gasExtension->setVisible(true);
                        moreButton->setChecked(true);
                        updateGeometry();

                        updateFourthGasSettings(fourthGasRefCombo->currentText());
                    }

                    // cell temp in
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_9()
                        && VariableDesc::isGoodTemperature(var))
                    {
                        qDebug() << "INSERT varName" << varName;
                        intT1RefLabel->setEnabled(true);
                        intT1RefCombo->setEnabled(true);
                        intT1RefCombo->addItem(varString, k);
                    }

                    // cell temp out
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_10()
                        && VariableDesc::isGoodTemperature(var))
                    {
                        qDebug() << "INSERT varName" << varName;
                        intT2RefLabel->setEnabled(true);
                        intT2RefCombo->setEnabled(true);
                        intT2RefCombo->addItem(varString, k);
                    }

                    // cell press 1
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_11()
                        && VariableDesc::isGoodPressure(var))
                    {
                        qDebug() << "INSERT varName" << varName;
                        intPRefLabel->setEnabled(true);
                        intPRefCombo->setEnabled(true);
                        intPRefCombo->addItem(varString, k);
                        qDebug() << "addItem" << intPRefCombo->count();
                    }

                    // avg cell temp
                    else if (varName == VariableDesc::getVARIABLE_VAR_STRING_15()
                        && VariableDesc::isGoodTemperature(var))
                    {
                        qDebug() << "INSERT varName" << varName;
                        intTcRefLabel->setEnabled(true);
                        intTcRefCombo->setEnabled(true);
                        intTcRefCombo->addItem(varString, k);
                        qDebug() << "addItem" << intTcRefCombo->count();
                    } // if
                } // if
            } // if

            // ambient temperatures and diagnostics
            else if (varName == VariableDesc::getVARIABLE_VAR_STRING_12()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_13()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_25()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_26()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_27()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_28()
                     || varName == VariableDesc::getVARIABLE_VAR_STRING_30())
            {
                qDebug() << "SECTION ambient temperatures and diagnostics";

                // ambient temp
                if (varName == VariableDesc::getVARIABLE_VAR_STRING_12()
                    && VariableDesc::isGoodTemperature(var))
                {
                    qDebug() << "INSERT varName" << varName;
                    airTRefLabel->setEnabled(true);
                    airTRefCombo->setEnabled(true);
                    airTRefCombo->addItem(varString, k);
                }
                // fast temp
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_28()
                    && VariableDesc::isGoodTemperature(var, VariableDesc::AnalogType::FAST))
                {
                    qDebug() << "INSERT varName" << varName;
                    tsRefLabel->setEnabled(true);
                    tsRefCombo->setEnabled(true);
                    tsRefCombo->addItem(varString, k);
                }
                // ambient press
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_13()
                    && VariableDesc::isGoodPressure(var))
                {
                    qDebug() << "INSERT varName" << varName;
                    airPRefLabel->setEnabled(true);
                    airPRefCombo->setEnabled(true);
                    airPRefCombo->addItem(varString, k);
                }
                // 7500A diagnostics
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_25()
                    && instrType != QLatin1String("Other"))
                {
                    qDebug() << "INSERT varName" << varName;
                    qDebug() << "varString" << varString;
                    diag7500Label->setEnabled(true);
                    diag7500Combo->setEnabled(true);
                    diag7500Combo->addItem(varString, k);
                }
                // 7200 diagnostics
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_26()
                    && instrType != QLatin1String("Other"))
                {
                    qDebug() << "INSERT varName" << varName;
                    qDebug() << "varString" << varString;
                    diag7200Label->setEnabled(true);
                    diag7200Combo->setEnabled(true);
                    diag7200Combo->addItem(varString, k);
                }
                // 7700 diagnostics
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_27()
                    && instrType != tr("Other"))
                {
                    qDebug() << "INSERT varName" << varName;
                    qDebug() << "varString" << varString;
                    diag7700Label->setEnabled(true);
                    diag7700Combo->setEnabled(true);
                    diag7700Combo->addItem(varString, k);
                }
                // Anem diagnostics
                else if (varName == VariableDesc::getVARIABLE_VAR_STRING_30()
                         && instrType != tr("Other"))
                {
                    qDebug() << "INSERT varName" << varName;
                    qDebug() << "varString" << varString;
                    anemFlagLabel->setEnabled(true);
                    anemFlagCombo->setEnabled(true);
                    anemFlagCombo->addItem(varString, k);
                }
            } // else if

            qDebug() << "start SECTION flags";
            // flags
            if (!varName.isEmpty())
            {
                foreach (QComboBox *combo, QList<QComboBox *>()
                         << flag1VarCombo
                         << flag2VarCombo
                         << flag3VarCombo
                         << flag4VarCombo
                         << flag5VarCombo
                         << flag6VarCombo
                         << flag7VarCombo
                         << flag8VarCombo
                         << flag9VarCombo
                         << flag10VarCombo)
                {
                    combo->setEnabled(true);
                    combo->addItem(varString, k);
                    qDebug() << varString << k;
                }

                foreach (QWidget *w, QWidgetList()
                         << flag1Label
                         << flag2Label
                         << flag3Label
                         << flag4Label
                         << flag5Label
                         << flag6Label
                         << flag7Label
                         << flag8Label
                         << flag9Label
                         << flag10Label
                         << flag1ThresholdSpin
                         << flag2ThresholdSpin
                         << flag3ThresholdSpin
                         << flag4ThresholdSpin
                         << flag5ThresholdSpin
                         << flag6ThresholdSpin
                         << flag7ThresholdSpin
                         << flag8ThresholdSpin
                         << flag9ThresholdSpin
                         << flag10ThresholdSpin
                         << flag1UnitLabel
                         << flag2UnitLabel
                         << flag3UnitLabel
                         << flag4UnitLabel
                         << flag5UnitLabel
                         << flag6UnitLabel
                         << flag7UnitLabel
                         << flag8UnitLabel
                         << flag9UnitLabel
                         << flag10UnitLabel
                         << flag1PolicyCombo
                         << flag2PolicyCombo
                         << flag3PolicyCombo
                         << flag4PolicyCombo
                         << flag5PolicyCombo
                         << flag6PolicyCombo
                         << flag7PolicyCombo
                         << flag8PolicyCombo
                         << flag9PolicyCombo
                         << flag10PolicyCombo)
                {
                    w->setEnabled(true);
                }
            }
            qDebug() << "end SECTION flags";

        } // if (ignore == no && numeric == yes)
    } // foreach

    qDebug() << "foreach 2";

    addNoneStr_1();
    filterVariables();
    emit updateMetadataReadResult(true);
}

void BasicSettingsPage::parseBiomMetadata()
{
    clearBiometCombo();

    qDebug() << "biomList_.count:" << biomList_.count();

    DEBUG_FUNC_NAME
    foreach (const BiomItem& bi, biomList_)
    {
        qDebug() << biomList_.count() << bi.type_ << bi.id_ << bi.col_ + 1000;
        QString varString = bi.type_
                            + tr("' from biomet files: ")
                            + tr("Column # ")
                            + QString::number(bi.col_);

        if (bi.type_.contains(BiomMetadataReader::getVAR_TA()))
        {
            varString.prepend(tr("Ambient Temperature '"));
            airTRefLabel->setEnabled(true);
            airTRefCombo->setEnabled(true);
            airTRefCombo->addItem(varString, bi.col_ + 1000);
        }
        else if (bi.type_.contains(BiomMetadataReader::getVAR_PA()))
        {
            varString.prepend(tr("Ambient Pressure '"));
            airPRefLabel->setEnabled(true);
            airPRefCombo->setEnabled(true);
            airPRefCombo->addItem(varString, bi.col_ + 1000);
        }
        else if (bi.type_.contains(BiomMetadataReader::getVAR_RH()))
        {
            varString.prepend(tr("Ambient Relative Humidity '"));
            rhLabel->setEnabled(true);
            rhCombo->setEnabled(true);
            rhCombo->addItem(varString, bi.col_);
        }
        else if (bi.type_.contains(BiomMetadataReader::getVAR_RG()))
        {
            varString.prepend(tr("Global Radiation '"));
            rgLabel->setEnabled(true);
            rgCombo->setEnabled(true);
            rgCombo->addItem(varString, bi.col_);
        }
        else if (bi.type_.contains(BiomMetadataReader::getVAR_LWIN()))
        {
            varString.prepend(tr("Longwave Incoming Radiation '"));
            lwinLabel->setEnabled(true);
            lwinCombo->setEnabled(true);
            lwinCombo->addItem(varString, bi.col_);
        }
        else if (bi.type_.contains(BiomMetadataReader::getVAR_PPFD()))
        {
            varString.prepend(tr("Photosynthetically Active Radiation '"));
            ppfdLabel->setEnabled(true);
            ppfdCombo->setEnabled(true);
            ppfdCombo->addItem(varString, bi.col_);
        }
    }

    addNoneStr_2();
    emit updateMetadataReadResult(true);
}

/// \fn void BasicSettingsPage::addNoneToVariables()
/// \brief Add 'None' item to variables combobox
void BasicSettingsPage::addNoneStr_1()
{
    DEBUG_FUNC_NAME

    foreach (QComboBox *combo, QList<QComboBox *>()
             << anemFlagCombo
             << tsRefCombo
             << co2RefCombo
             << h2oRefCombo
             << ch4RefCombo
             << fourthGasRefCombo
             << intTcRefCombo
             << intT1RefCombo
             << intT2RefCombo
             << intPRefCombo
             << diag7500Combo
             << diag7200Combo
             << diag7700Combo
             << flag1VarCombo
             << flag2VarCombo
             << flag3VarCombo
             << flag4VarCombo
             << flag5VarCombo
             << flag6VarCombo
             << flag7VarCombo
             << flag8VarCombo
             << flag9VarCombo
             << flag10VarCombo)
    {
        if (combo->findData(0) < 0)
            combo->addItem(tr("None"), 0);
    }

    if (ecProject_->generalUseBiomet() == 0)
    {
        addNoneStr_2();
    }
}
void BasicSettingsPage::addNoneStr_2()
{
    DEBUG_FUNC_NAME

    foreach (QWidget *w, QWidgetList()
             << airTRefCombo
             << airPRefCombo
             << rhCombo
             << rgCombo
             << lwinCombo
             << ppfdCombo)
    {
        QComboBox *combo = static_cast<QComboBox*>(w);
        if (combo->findData(0) < 0)
            combo->addItem(tr("None"), 0);
    }
}

void BasicSettingsPage::clearBiometCombo()
{
    DEBUG_FUNC_NAME
    foreach (QComboBox *combo, QList<QComboBox *>()
             << rhCombo
             << rgCombo
             << lwinCombo
             << ppfdCombo)
    {
        combo->clear();
        combo->setEnabled(false);
    }

    QString biometStr = QStringLiteral("biomet");
    QString noneStr = tr("None");

    qDebug() << "airTRefCombo->count()" <<airTRefCombo->count();
    for (int n = airTRefCombo->count() - 1; n >= 0; --n)
    {
        qDebug() << "n" << n << airTRefCombo->itemText(n);
        if (airTRefCombo->itemText(n).contains(biometStr))
            airTRefCombo->removeItem(n);
    }

    if (airTRefCombo->count() == 1
        && airTRefCombo->currentText().contains(noneStr))
    {
        airTRefCombo->setEnabled(false);
    }

    qDebug() << "airPRefCombo->count()" <<airPRefCombo->count();
    for (int n = airPRefCombo->count() - 1; n >= 0; --n)
    {
        qDebug() << "n" << n << airPRefCombo->itemText(n);
        if (airPRefCombo->itemText(n).contains(biometStr))
            airPRefCombo->removeItem(n);
    }

    if (airPRefCombo->count() == 1
        && airPRefCombo->currentText().contains(noneStr))
    {
        airPRefCombo->setEnabled(false);
    }
}

void BasicSettingsPage::clearVarsCombo()
{
    DEBUG_FUNC_NAME

    foreach (QLabel *label, QList<QLabel *>()
             << anemRefLabel
             << anemFlagLabel
             << co2RefLabel
             << h2oRefLabel
             << ch4RefLabel
             << fourthGasRefLabel
             << gasMwLabel
             << gasDiffLabel
             << intTcRefLabel
             << intT1RefLabel
             << intT2RefLabel
             << intPRefLabel
             << airTRefLabel
             << airPRefLabel
             << rhLabel
             << rgLabel
             << lwinLabel
             << ppfdLabel
             << diag7500Label
             << diag7200Label
             << diag7700Label
             << tsRefLabel)
    {
        label->setEnabled(false);
    }

    foreach (QComboBox *combo, QList<QComboBox *>()
             << anemRefCombo
             << anemFlagCombo
             << co2RefCombo
             << h2oRefCombo
             << ch4RefCombo
             << fourthGasRefCombo
             << intTcRefCombo
             << intT1RefCombo
             << intT2RefCombo
             << intPRefCombo
             << airTRefCombo
             << airPRefCombo
             << rhCombo
             << rgCombo
             << lwinCombo
             << ppfdCombo
             << diag7500Combo
             << diag7200Combo
             << diag7700Combo
             << tsRefCombo)
    {
        combo->clear();
        combo->setEnabled(false);
    }

    gasMw->setEnabled(false);
    gasDiff->setEnabled(false);
}

void BasicSettingsPage::clearFlagVars()
{
    DEBUG_FUNC_NAME

    foreach (QLabel *label, QList<QLabel *>()
             << flag1Label
             << flag2Label
             << flag3Label
             << flag4Label
             << flag5Label
             << flag6Label
             << flag7Label
             << flag8Label
             << flag9Label
             << flag10Label)
    {
        label->setEnabled(false);
    }

    foreach (QComboBox *combo, QList<QComboBox *>()
             << flag1VarCombo
             << flag2VarCombo
             << flag3VarCombo
             << flag4VarCombo
             << flag5VarCombo
             << flag6VarCombo
             << flag7VarCombo
             << flag8VarCombo
             << flag9VarCombo
             << flag10VarCombo)
    {
        combo->clear();
        combo->setEnabled(false);
    }
}

void BasicSettingsPage::clearFlagUnits()
{
    DEBUG_FUNC_NAME

    foreach (QLabel *label, QList<QLabel *>()
             << flag1UnitLabel
             << flag2UnitLabel
             << flag3UnitLabel
             << flag4UnitLabel
             << flag5UnitLabel
             << flag6UnitLabel
             << flag7UnitLabel
             << flag8UnitLabel
             << flag9UnitLabel
             << flag10UnitLabel)
    {
        label->clear();
    }
}

void BasicSettingsPage::clearFlagThresholdsAndPolicies()
{
    DEBUG_FUNC_NAME

    foreach (QDoubleSpinBox *spin, QList<QDoubleSpinBox *>()
             << flag1ThresholdSpin
             << flag2ThresholdSpin
             << flag3ThresholdSpin
             << flag4ThresholdSpin
             << flag5ThresholdSpin
             << flag6ThresholdSpin
             << flag7ThresholdSpin
             << flag8ThresholdSpin
             << flag9ThresholdSpin
             << flag10ThresholdSpin)
    {
        spin->setEnabled(false);
        spin->setValue(-9999.0);
    }

    foreach (QLabel *label, QList<QLabel *>()
             << flag1UnitLabel
             << flag2UnitLabel
             << flag3UnitLabel
             << flag4UnitLabel
             << flag5UnitLabel
             << flag6UnitLabel
             << flag7UnitLabel
             << flag8UnitLabel
             << flag9UnitLabel
             << flag10UnitLabel)
    {
        label->setEnabled(false);
    }

    foreach (QComboBox *combo, QList<QComboBox *>()
             << flag1PolicyCombo
             << flag2PolicyCombo
             << flag3PolicyCombo
             << flag4PolicyCombo
             << flag5PolicyCombo
             << flag6PolicyCombo
             << flag7PolicyCombo
             << flag8PolicyCombo
             << flag9PolicyCombo
             << flag10PolicyCombo)
    {
        combo->setEnabled(false);
        combo->setCurrentIndex(1);
    }
}

/// \fn void BasicSettingsPage::filterVariables()
/// \brief Show only molar density gas variables if 7500/A/RS, 7700
/// For 7200/RS order will be: molar density, mixing ratio, mole fraction
void BasicSettingsPage::filterVariables()
{
    DEBUG_FUNC_NAME
    const auto li6262Str = QStringLiteral("LI-6262");
    const auto li7000Str = QStringLiteral("LI-7000");
    const auto li7200Str = QStringLiteral("LI-7200");
    const auto li7200RSStr = QStringLiteral("LI-7200RS");
    const auto li7500Str = QStringLiteral("LI-7500");
    const auto li7500AStr = QStringLiteral("LI-7500A");
    const auto li7500RSStr = QStringLiteral("LI-7500RS");
    const auto li7700Str = QStringLiteral("LI-7700");
//    const auto densityStr = QStringLiteral("density");
    const auto fractionStr = QStringLiteral("fraction");
    const auto ratioStr = QStringLiteral("ratio");
    const auto noneStr = tr("None");
    const auto genericStr = tr("Generic");
    const auto openPathStr1 = QStringLiteral("open");
    const auto openPathStr2 = QStringLiteral("OP");
    const auto anemStr = QStringLiteral("Anemometer");

    // filter vars (always possible)
    for (int i = 0; i < co2RefCombo->count(); ++i)
    {
        qDebug() << "FILTER co2RefCombo" << i << co2RefCombo->itemText(i);
        if (!co2RefCombo->itemText(i).contains(noneStr))
        {
            // open path
            if (co2RefCombo->itemText(i).contains(li7500Str)
                || co2RefCombo->itemText(i).contains(li7500AStr)
                || co2RefCombo->itemText(i).contains(li7500RSStr)
                || co2RefCombo->itemText(i).contains(li7700Str)
                || co2RefCombo->itemText(i).contains(openPathStr1)
                || co2RefCombo->itemText(i).contains(openPathStr2))
            {
                // filter if not molar density
                if (co2RefCombo->itemText(i).contains(fractionStr)
                    || co2RefCombo->itemText(i).contains(ratioStr))
                {
                    co2RefCombo->removeItem(i);
                }
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < h2oRefCombo->count(); ++i)
    {
        qDebug() << "FILTER h2oRefCombo:" << i << h2oRefCombo->itemText(i);
        if (!h2oRefCombo->itemText(i).contains(noneStr))
        {
            if (h2oRefCombo->itemText(i).contains(li7500Str)
                || h2oRefCombo->itemText(i).contains(li7500AStr)
                || h2oRefCombo->itemText(i).contains(li7500RSStr)
                || h2oRefCombo->itemText(i).contains(li7700Str)
                || h2oRefCombo->itemText(i).contains(openPathStr1)
                || h2oRefCombo->itemText(i).contains(openPathStr2))
            {
                if (h2oRefCombo->itemText(i).contains(fractionStr)
                    || h2oRefCombo->itemText(i).contains(ratioStr))
                {
                    h2oRefCombo->removeItem(i);
                }
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < ch4RefCombo->count(); ++i)
    {
        qDebug() << "FILTER ch4RefCombo" << i << ch4RefCombo->itemText(i);
        if (!ch4RefCombo->itemText(i).contains(noneStr))
        {
            if (ch4RefCombo->itemText(i).contains(li7700Str)
                || ch4RefCombo->itemText(i).contains(openPathStr1)
                || ch4RefCombo->itemText(i).contains(openPathStr2))
            {
                if (ch4RefCombo->itemText(i).contains(fractionStr)
                    || ch4RefCombo->itemText(i).contains(ratioStr))
                {
                    ch4RefCombo->removeItem(i);
                }
            }
            else if (!ch4RefCombo->itemText(i).contains(genericStr))
            {
                ch4RefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    qDebug() << "fourthGasRefCombo->count()" << fourthGasRefCombo->count();
    for (int i = 0; i < fourthGasRefCombo->count(); ++i)
    {
        qDebug() << "FILTER fourthGasRefCombo" << i << fourthGasRefCombo->itemText(i);
        if (!fourthGasRefCombo->itemText(i).contains(noneStr))
        {
            if (fourthGasRefCombo->itemText(i).contains(li6262Str)
                || fourthGasRefCombo->itemText(i).contains(li7000Str)
                || fourthGasRefCombo->itemText(i).contains(li7200Str)
                || fourthGasRefCombo->itemText(i).contains(li7200RSStr)
                || fourthGasRefCombo->itemText(i).contains(li7500Str)
                || fourthGasRefCombo->itemText(i).contains(li7500AStr)
                || fourthGasRefCombo->itemText(i).contains(li7500RSStr)
                || fourthGasRefCombo->itemText(i).contains(li7700Str))
            {
                fourthGasRefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < intT1RefCombo->count(); ++i)
    {
        qDebug() << "intT1RefCombo" << i << intT1RefCombo->itemText(i);
        if (!intT1RefCombo->itemText(i).contains(noneStr))
        {
            if (intT1RefCombo->itemText(i).contains(li6262Str)
                || intT1RefCombo->itemText(i).contains(li7000Str)
                || intT1RefCombo->itemText(i).contains(li7500Str)
                || intT1RefCombo->itemText(i).contains(li7500AStr)
                || intT1RefCombo->itemText(i).contains(li7500RSStr)
                || intT1RefCombo->itemText(i).contains(li7700Str))
            {
                intT1RefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < intT2RefCombo->count(); ++i)
    {
        qDebug() << "intT2RefCombo" << i << intT2RefCombo->itemText(i);
        if (!intT2RefCombo->itemText(i).contains(noneStr))
        {
            if (intT2RefCombo->itemText(i).contains(li6262Str)
                || intT2RefCombo->itemText(i).contains(li7000Str)
                || intT2RefCombo->itemText(i).contains(li7500Str)
                || intT2RefCombo->itemText(i).contains(li7500AStr)
                || intT2RefCombo->itemText(i).contains(li7500RSStr)
                || intT2RefCombo->itemText(i).contains(li7700Str))
            {
                intT2RefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < intTcRefCombo->count(); ++i)
    {
        qDebug() << "intTcRefCombo" << i << intTcRefCombo->itemText(i);
        if (!intTcRefCombo->itemText(i).contains(noneStr))
        {
            if (intTcRefCombo->itemText(i).contains(li7500Str)
                || intTcRefCombo->itemText(i).contains(li7500AStr)
                || intTcRefCombo->itemText(i).contains(li7500RSStr)
                || intTcRefCombo->itemText(i).contains(li7700Str))
            {
                intTcRefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < intPRefCombo->count(); ++i)
    {
        qDebug() << "intPRefCombo" << i << intPRefCombo->itemText(i);
        if (!intPRefCombo->itemText(i).contains(noneStr))
        {
            if (intPRefCombo->itemText(i).contains(li7500Str)
                || intPRefCombo->itemText(i).contains(li7500AStr)
                || intPRefCombo->itemText(i).contains(li7500RSStr)
                || intPRefCombo->itemText(i).contains(li7700Str))
            {
                intPRefCombo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < diag7500Combo->count(); ++i)
    {
        qDebug() << "i" << i;
        qDebug() << "diag7500Combo->itemText(i)" << diag7500Combo->itemText(i);
        if (!diag7500Combo->itemText(i).contains(noneStr))
        {
            qDebug() << "diag7500Combo" << i << diag7500Combo->itemText(i);
            if (!diag7500Combo->itemText(i).contains(li7500Str)
                && !diag7500Combo->itemText(i).contains(li7500AStr)
                && !diag7500Combo->itemText(i).contains(li7500RSStr))
            {
                diag7500Combo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < diag7200Combo->count(); ++i)
    {
        if (!diag7200Combo->itemText(i).contains(noneStr))
        {
            qDebug() << "diag7200Combo" << i << diag7200Combo->itemText(i);
            if (!diag7200Combo->itemText(i).contains(li7200Str)
                && !diag7200Combo->itemText(i).contains(li7200RSStr))
            {
                diag7200Combo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < diag7700Combo->count(); ++i)
    {
        if (!diag7700Combo->itemText(i).contains(noneStr))
        {
            qDebug() << "diag7700Combo" << i << diag7700Combo->itemText(i);
            if (!diag7700Combo->itemText(i).contains(li7700Str))
            {
                diag7700Combo->removeItem(i);
            }
        }
    }

    // filter vars (always possible)
    for (int i = 0; i < anemFlagCombo->count(); ++i)
    {
        if (!anemFlagCombo->itemText(i).contains(noneStr))
        {
            qDebug() << "anemFlagCombo" << i << anemFlagCombo->itemText(i);
            if (!anemFlagCombo->itemText(i).contains(anemStr))
            {
                anemFlagCombo->removeItem(i);
            }
        }
    }
}

void BasicSettingsPage::preselectDensityVariables(QComboBox* combo)
{
    DEBUG_FUNC_NAME
    const auto densityStr = QStringLiteral("density");
    const auto ratioStr = QStringLiteral("ratio");

    qDebug() << "combo->count()" << combo->count();
    // select the first density var or the first item if not present
    for (int i = 0; i < combo->count(); ++i)
    {
        qDebug() << "combo->itemText(i)" << combo->itemText(i);

        if (combo->itemText(i).contains(ratioStr))
        {
            qDebug() << "i" << i;

            combo->setCurrentIndex(i);
            break;
        }
        else if (combo->itemText(i).contains(densityStr))
        {
            qDebug() << "i" << i;
            combo->setCurrentIndex(i);
        }
        else
        {
            qDebug() << "i" << i;
            combo->setCurrentIndex(0);
        }
    }
}

void BasicSettingsPage::preselect7700Variables(QComboBox* combo)
{
    DEBUG_FUNC_NAME
    const auto li7700Str = QStringLiteral("LI-7700");

    // preselect 7700 air temp var if present (not always possible)
    for (auto i = 0; i < combo->count(); ++i)
    {
        qDebug() << "combo" << i << combo->itemText(i);
        if (combo->itemText(i).contains(li7700Str))
        {
            combo->setCurrentIndex(i);
            break;
        }
        else
        {
            combo->setCurrentIndex(0);
        }
    }
}

// NOTE: not used
void BasicSettingsPage::preselectVariables()
{
    DEBUG_FUNC_NAME

    // set none as initial value on the flags (not always possible)
    foreach (QComboBox *combo, QList<QComboBox *>()
             << flag1VarCombo
             << flag2VarCombo
             << flag3VarCombo
             << flag4VarCombo
             << flag5VarCombo
             << flag6VarCombo
             << flag7VarCombo
             << flag8VarCombo
             << flag9VarCombo
             << flag10VarCombo)
    {
        combo->setCurrentIndex(combo->findData(0));
    }
}

void BasicSettingsPage::outpathBrowseSelected(const QString& dir_path)
{
    outpathBrowse->setPath(dir_path);

    QDir outDir(dir_path);
    auto canonicalOutDir = outDir.canonicalPath();
    configState_->window.last_data_path = canonicalOutDir;
    GlobalSettings::updateLastDatapath(canonicalOutDir);
}

void BasicSettingsPage::updateDataPath(const QString& dp)
{
    DEBUG_FUNC_NAME
    ecProject_->setScreenDataPath(QDir::cleanPath(dp));
}

void BasicSettingsPage::updateOutPath(const QString& dp)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralOutPath(QDir::cleanPath(dp));
}

void BasicSettingsPage::updatePreviousDataPath(const QString& dp)
{
    ecProject_->setSpectraExDir(QDir::cleanPath(dp));
}

void BasicSettingsPage::onAvgLenLabelClicked()
{
    avgIntervalSpin->setFocus();
    avgIntervalSpin->selectAll();
}

void BasicSettingsPage::updateRecursion(bool b)
{
    DEBUG_FUNC_NAME
    ecProject_->setScreenRecurse(b);

    QTimer::singleShot(0, this, SLOT(runUpdateFilesFound()));
}

void BasicSettingsPage::updateSubsetSelection(bool b)
{
    ecProject_->setGeneralSubset(b);

    foreach (QWidget *w, QWidgetList()
             << startDateLabel
             << startDateEdit
             << startTimeEdit
             << lockedIcon
             << endDateLabel
             << endDateEdit
             << endTimeEdit)
    {
        w->setEnabled(b);
    }
    if (b)
    {
        alignDeclinationDate(endDateEdit->date());
    }
}

void BasicSettingsPage::updateAvrgLen(int n)
{
    ecProject_->setScreenAvrgLen(n);
}

void BasicSettingsPage::reset()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    recursionCheckBox->setChecked(ecProject_->defaultSettings.screenGeneral.recurse);
    datapathBrowse->clear();
    clearFilesFound();

    filePrototypeEdit->clear();
    outpathBrowse->clear();
    idEdit->clear();
    previousDatapathBrowse->clear();

    maxLackSpin->setValue(ecProject_->defaultSettings.screenSetting.max_lack);
    avgIntervalSpin->setValue(ecProject_->defaultSettings.screenSetting.avrg_len);
    useMagneticNRadio->setChecked(!ecProject_->defaultSettings.screenGeneral.use_geo_north);

    subsetCheckBox->setChecked(ecProject_->defaultSettings.projectGeneral.subset);
    startDateLabel->setEnabled(false);
    startDateEdit->setEnabled(false);
    startTimeEdit->setEnabled(false);
    endDateLabel->setEnabled(false);
    endDateEdit->setEnabled(false);
    endTimeEdit->setEnabled(false);

    startDateEdit->setDate(QDate(2000, 1, 1));
    startTimeEdit->setTime(QTime(0, 0));
    endDateEdit->setDate(QDate::currentDate());
    endTimeEdit->setTime(QTime::currentTime());
    forceEndDatePolicy();
    forceEndTimePolicy();

    if (httpReply_)
    {
        httpReply_->abort();
    }

    declinationLabel->setEnabled(false);
    declinationEdit->setEnabled(false);
    declinationEdit->setText(strDeclination(0.0));
    declinationDateLabel->setEnabled(false);
    declinationDateEdit->setDate(QDate::fromString(ecProject_->generalEndDate(), Qt::ISODate));
    declinationDateEdit->setEnabled(false);
    declinationFetchButton->setEnabled(false);
    decChangingLabel->clear();

    crossWindCheckBox->setEnabled(false);
    crossWindCheckBox->setChecked(!ecProject_->defaultSettings.screenSetting.cross_wind);
    moreButton->setChecked(false);

    clearVarsCombo();
    clearFlagVars();
    clearFlagUnits();
    clearFlagThresholdsAndPolicies();

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void BasicSettingsPage::refresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    recursionCheckBox->setChecked(ecProject_->screenRecurse());
    clearFilesFound();

    if (FileUtils::existsPath(ecProject_->screenDataPath()))
    {
        datapathBrowse->setPath(ecProject_->screenDataPath());
//        updateFilesFound(ecProject_->screenRecurse());
    }
    else
    {
        datapathBrowse->clear();
        clearFilesFound();
        updateDataPath(QString());
    }

    filePrototypeEdit->setText(ecProject_->generalFilePrototype());

    // NOTE: we should leave non existing path, in this case,
    // but sometime it's unsafe because the engine can't create
    // subdirectory everywhere
    if (FileUtils::existsPath(ecProject_->generalOutPath()))
    {
        outpathBrowse->setPath(ecProject_->generalOutPath());
    }
    else
    {
        outpathBrowse->clear();
        updateOutPath(QString());
    }

    if (idEdit->text() != ecProject_->generalId())
    {
        idEdit->setText(ecProject_->generalId());
    }

    if (FileUtils::existsPath(ecProject_->spectraExDir()))
    {
        previousDatapathBrowse->setPath(ecProject_->spectraExDir());
    }
    else
    {
        previousDatapathBrowse->clear();
        updatePreviousDataPath(QString());
    }

    maxLackSpin->setValue(ecProject_->screenMaxLack());
    avgIntervalSpin->setValue(ecProject_->screenAvrgLen());

    subsetCheckBox->setChecked(ecProject_->generalSubset());
    startDateLabel->setEnabled(subsetCheckBox->isChecked());
    startDateEdit->setEnabled(subsetCheckBox->isChecked());
    startTimeEdit->setEnabled(subsetCheckBox->isChecked());
    endDateLabel->setEnabled(subsetCheckBox->isChecked());
    endDateEdit->setEnabled(subsetCheckBox->isChecked());
    endTimeEdit->setEnabled(subsetCheckBox->isChecked());

    startDateEdit->setDate(QDate::fromString(ecProject_->generalStartDate(), Qt::ISODate));
    startTimeEdit->setTime(QTime::fromString(ecProject_->generalStartTime(), QStringLiteral("hh:mm")));
    endDateEdit->setDate(QDate::fromString(ecProject_->generalEndDate(), Qt::ISODate));
    endTimeEdit->setTime(QTime::fromString(ecProject_->generalEndTime(), QStringLiteral("hh:mm")));

    crossWindCheckBox->setChecked(!ecProject_->screenCrossWind());

    if (ecProject_->screenUseGeoNorth())
    {
        useGeographicNRadio->setChecked(true);

        declinationLabel->setEnabled(true);
        declinationLabel->setToolTip(tr("<b>Magnetic Declination:</b> Based upon the latitude and longitudinal coordinates entered, EddyPro determines the magnetic declination from the U.S. NOAA (National Oceanic and Atmospheric Organization) internet resources (U.S. National Geophysical Data Center)."));
        declinationEdit->setEnabled(true);
        declinationDateLabel->setEnabled(true);
        declinationDateEdit->setEnabled(true);
        declinationDateEdit->setDate(QDate::fromString(ecProject_->screenDecDate(), Qt::ISODate));
        declinationFetchButton->setEnabled(true);
    }
    else
    {
        useMagneticNRadio->setChecked(true);

        declinationLabel->setEnabled(false);
        declinationEdit->setEnabled(false);
        declinationDateLabel->setEnabled(false);
        declinationDateEdit->setEnabled(false);

        // NOTE: manage NOAA website API limitation, where current last day available is 2019-12-31
        // compare http://www.ngdc.noaa.gov/geomag-web/#declination
        if (ecProject_->generalSubset())
        {
            if (endDateEdit->date().year() <= 2019)
            {
                declinationDateEdit->setDate(endDateEdit->date());
            }
            else
            {
                declinationDateEdit->setDate(QDate(2019, 12, 31));
            }
        }
        else
        {
            declinationDateEdit->setDate(QDate::fromString(ecProject_->screenDecDate(), Qt::ISODate));
        }
        declinationFetchButton->setEnabled(false);
    }
    declinationEdit->setText(strDeclination(ecProject_->screenMagDec()));

    int currData = ecProject_->screenFlag1Col();
    int currItemIndex = flag1VarCombo->findData(currData);
    int noneIndex = flag1VarCombo->findData(0);
    qDebug() << "refresh Flag1Col";
    qDebug() << "currData" << currData;
    qDebug() << "currItemIndex" << currItemIndex;
    qDebug() << "noneIndex" << noneIndex;

    flag1ThresholdSpin->setValue(ecProject_->screenFlag1Threshold());
    flag1PolicyCombo->setCurrentIndex(ecProject_->screenFlag1Upper());
    flag2ThresholdSpin->setValue(ecProject_->screenFlag2Threshold());
    flag2PolicyCombo->setCurrentIndex(ecProject_->screenFlag2Upper());
    flag3ThresholdSpin->setValue(ecProject_->screenFlag3Threshold());
    flag3PolicyCombo->setCurrentIndex(ecProject_->screenFlag3Upper());
    flag4ThresholdSpin->setValue(ecProject_->screenFlag4Threshold());
    flag4PolicyCombo->setCurrentIndex(ecProject_->screenFlag4Upper());
    flag5ThresholdSpin->setValue(ecProject_->screenFlag5Threshold());
    flag5PolicyCombo->setCurrentIndex(ecProject_->screenFlag5Upper());
    flag6ThresholdSpin->setValue(ecProject_->screenFlag6Threshold());
    flag6PolicyCombo->setCurrentIndex(ecProject_->screenFlag6Upper());
    flag7ThresholdSpin->setValue(ecProject_->screenFlag7Threshold());
    flag7PolicyCombo->setCurrentIndex(ecProject_->screenFlag7Upper());
    flag8ThresholdSpin->setValue(ecProject_->screenFlag8Threshold());
    flag8PolicyCombo->setCurrentIndex(ecProject_->screenFlag8Upper());
    flag9ThresholdSpin->setValue(ecProject_->screenFlag9Threshold());
    flag9PolicyCombo->setCurrentIndex(ecProject_->screenFlag9Upper());
    flag10ThresholdSpin->setValue(ecProject_->screenFlag10Threshold());
    flag10PolicyCombo->setCurrentIndex(ecProject_->screenFlag10Upper());

    qDebug() << "ep value screenFlag1Threshold" << ecProject_->screenFlag1Threshold();
    qDebug() << "ep value Flag1Upper" << ecProject_->screenFlag1Upper();

    qDebug() << "ep value screenFlag2Threshold" << ecProject_->screenFlag2Threshold();
    qDebug() << "ep value Flag2Upper" << ecProject_->screenFlag2Upper();

    moreButton->setChecked(false);
    updateFourthGasSettings(fourthGasRefCombo->currentText());
    gasMw->setValue(ecProject_->generalGasMw());
    gasDiff->setValue(ecProject_->generalGasDiff());

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);

    qDebug() << "endtime:" << endTimeEdit->text();
}

void BasicSettingsPage::partialRefresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    subsetCheckBox->setChecked(ecProject_->generalSubset());
    startDateLabel->setEnabled(subsetCheckBox->isChecked());
    startDateEdit->setEnabled(subsetCheckBox->isChecked());
    startTimeEdit->setEnabled(subsetCheckBox->isChecked());
    endDateLabel->setEnabled(subsetCheckBox->isChecked());
    endDateEdit->setEnabled(subsetCheckBox->isChecked());
    endTimeEdit->setEnabled(subsetCheckBox->isChecked());

    startDateEdit->setDate(QDate::fromString(ecProject_->generalStartDate(), Qt::ISODate));
    startTimeEdit->setTime(QTime::fromString(ecProject_->generalStartTime(), QStringLiteral("hh:mm")));
    endDateEdit->setDate(QDate::fromString(ecProject_->generalEndDate(), Qt::ISODate));
    endTimeEdit->setTime(QTime::fromString(ecProject_->generalEndTime(), QStringLiteral("hh:mm")));

    if (ecProject_->screenUseGeoNorth())
    {
        useGeographicNRadio->setChecked(true);

        declinationLabel->setEnabled(true);
        declinationLabel->setToolTip(tr("<b>Magnetic Declination:</b> Based upon the latitude and longitudinal coordinates entered, EddyPro determines the magnetic declination from the U.S. NOAA (National Oceanic and Atmospheric Organization) internet resources (U.S. National Geophysical Data Center)."));
        declinationEdit->setEnabled(true);
        declinationDateLabel->setEnabled(true);
        declinationDateEdit->setEnabled(true);
        declinationDateEdit->setDate(QDate::fromString(ecProject_->screenDecDate(), Qt::ISODate));
        declinationFetchButton->setEnabled(true);
    }
    else
    {
        useMagneticNRadio->setChecked(true);

        declinationLabel->setEnabled(false);
        declinationEdit->setEnabled(false);
        declinationDateLabel->setEnabled(false);
        declinationDateEdit->setEnabled(false);

        // NOTE: manage NOAA website API limitation, where current last day available is 2019-12-31
        // compare http://www.ngdc.noaa.gov/geomag-web/#declination
        if (ecProject_->generalSubset())
        {
            if (endDateEdit->date().year() <= 2019)
            {
                declinationDateEdit->setDate(endDateEdit->date());
            }
            else
            {
                declinationDateEdit->setDate(QDate(2019, 12, 31));
            }
        }
        else
        {
            declinationDateEdit->setDate(QDate::fromString(ecProject_->screenDecDate(), Qt::ISODate));
        }
        declinationFetchButton->setEnabled(false);
    }
    declinationEdit->setText(strDeclination(ecProject_->screenMagDec()));

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void BasicSettingsPage::onStartDateLabelClicked()
{
    DEBUG_FUNC_NAME
    startDateEdit->setFocus();
    WidgetUtils::showCalendarOf(startDateEdit);
}

void BasicSettingsPage::onEndDateLabelClicked()
{
    DEBUG_FUNC_NAME
    endDateEdit->setFocus();
    WidgetUtils::showCalendarOf(endDateEdit);
}

void BasicSettingsPage::updateStartDate(const QDate& d)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralStartDate(d.toString(Qt::ISODate));
    forceEndDatePolicy();
}

void BasicSettingsPage::updateStartTime(const QTime& t)
{
    ecProject_->setGeneralStartTime(t.toString(QStringLiteral("hh:mm")));
    forceEndTimePolicy();
}

void BasicSettingsPage::updateEndDate(const QDate& d)
{
    ecProject_->setGeneralEndDate(d.toString(Qt::ISODate));
}

void BasicSettingsPage::updateEndTime(const QTime& t)
{
    ecProject_->setGeneralEndTime(t.toString(QStringLiteral("hh:mm")));
}

void BasicSettingsPage::onMaxLackLabelClicked()
{
    maxLackSpin->setFocus(Qt::ShortcutFocusReason);
    maxLackSpin->selectAll();
}

void BasicSettingsPage::updateMaxLack(int n)
{
    ecProject_->setScreenMaxLack(n);
}

// NOTE: it's correct only up to 9 anem (?), to extend
void BasicSettingsPage::updateAnemRefCombo(const QString& s)
{
    DEBUG_FUNC_NAME
    if (!s.isEmpty())
    {
        QString modelStr = s.section(QLatin1Char('['), 0, 0).trimmed();
        ecProject_->setGeneralColMasterSonic(DlProject::toIniAnemModel(modelStr)
                                      + QStringLiteral("_")
                                      + s.mid(s.indexOf(QLatin1Char(']')) - 1, 1));
    }

    qDebug() << "ecProject_->generalColMasterSonic()"
             << ecProject_->generalColMasterSonic();
}

void BasicSettingsPage::updateAnemFlagCombo(int i)
{
    DEBUG_FUNC_NAME
            ecProject_->setGeneralColDiagAnem(anemFlagCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateCo2RefCombo(int i)
{
    DEBUG_FUNC_NAME
    qDebug() << "i" << i << co2RefCombo->itemData(i).toInt();
    ecProject_->setGeneralColCo2(co2RefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateH2oRefCombo(int i)
{
    ecProject_->setGeneralColH2o(h2oRefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateCh4RefCombo(int i)
{
    ecProject_->setGeneralColCh4(ch4RefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFourthGasRefCombo(int i)
{
    ecProject_->setGeneralColGas4(fourthGasRefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFourthGasSettings(const QString& s)
{
    DEBUG_FUNC_NAME

    const auto N2OStr = QLatin1Char('N') + Defs::SUBTWO + QLatin1Char('O');
    const auto COStr = QStringLiteral("CO");
    const auto SO2Str = QStringLiteral("SO") + Defs::SUBTWO;
    const auto O3Str = QLatin1Char('O') + Defs::SUBTHREE;
    const auto NH3Str = QStringLiteral("NH") + Defs::SUBTHREE;
    const auto NOStr = QStringLiteral("NO");
    const auto NO2Str = QStringLiteral("NO") + Defs::SUBTWO;

    QString gasStr(s.split(QLatin1Char(' ')).at(0));

    qDebug() << "gasStr" << gasStr << Defs::N2O_STRING;

    if (gasStr == N2OStr)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(44.01);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1436);
    }
    else if (gasStr == COStr)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(28.01);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1807);
    }
    else if (gasStr == SO2Str)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(64.06);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1089);
    }
    else if (gasStr == O3Str)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(48.00);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1444);
    }
    else if (gasStr == NH3Str)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(17.03);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1908);
    }
    else if (gasStr == NOStr)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(30.00);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1988);
    }
    else if (gasStr == NO2Str)
    {
        gasMw->setEnabled(false);
        gasMw->setValue(46.00);
        gasDiff->setEnabled(false);
        gasDiff->setValue(0.1361);
    }
    else
    {
        gasMw->setEnabled(true);
        gasDiff->setEnabled(true);
    }
}

void BasicSettingsPage::updateIntTcRefCombo(int i)
{
    ecProject_->setGeneralColIntTc(intTcRefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateIntT1RefCombo(int i)
{
    ecProject_->setGeneralColIntT1(intT1RefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateIntT2RefCombo(int i)
{
    ecProject_->setGeneralColIntT2(intT2RefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateIntPRefCombo(int i)
{
    ecProject_->setGeneralColIntP(intPRefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateAirTRefCombo(int i)
{
    DEBUG_FUNC_NAME

    int colNum = airTRefCombo->itemData(i).toInt();
    qDebug() << "index" << i;
    qDebug() << "data" << colNum;

    // raw data
    if (colNum > 0 && colNum < 1000)
    {
        ecProject_->setGeneralColAirT(colNum);
        ecProject_->setBiomParamColAirT(1000);
    }
    // biomet data
    else if (colNum > 1000)
    {
        ecProject_->setGeneralColAirT(0);
        ecProject_->setBiomParamColAirT(colNum);
    }
    // none (0)
    else
    {
        ecProject_->setGeneralColAirT(0);
        ecProject_->setBiomParamColAirT(1000);
    }
}

void BasicSettingsPage::updateAirPRefCombo(int i)
{
    int colNum = airPRefCombo->itemData(i).toInt();

    // raw data
    if (colNum > 0 && colNum < 1000)
    {
        ecProject_->setGeneralColAirP(colNum);
        ecProject_->setBiomParamColAirP(1000);
    }
    // biomet data
    else if (colNum > 1000)
    {
        ecProject_->setGeneralColAirP(0);
        ecProject_->setBiomParamColAirP(colNum);
    }
    // none ()
    else
    {
        ecProject_->setGeneralColAirP(0);
        ecProject_->setBiomParamColAirP(1000);
    }
}

void BasicSettingsPage::updateRhCombo(int i)
{
    int colNum = rhCombo->itemData(i).toInt();
    qDebug() << "colNum" << colNum;
    ecProject_->setBiomParamColRh(colNum);
}

void BasicSettingsPage::updateRgCombo(int i)
{
    ecProject_->setBiomParamColRg(rgCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateLwinCombo(int i)
{
    ecProject_->setBiomParamColLwin(lwinCombo->itemData(i).toInt());
}

void BasicSettingsPage::updatePpfdCombo(int i)
{
    ecProject_->setBiomParamColPpfd(ppfdCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateDiag7500Combo(int i)
{
    ecProject_->setGeneralColDiag75(diag7500Combo->itemData(i).toInt());
}

void BasicSettingsPage::updateDiag7200Combo(int i)
{
    ecProject_->setGeneralColDiag72(diag7200Combo->itemData(i).toInt());
}

void BasicSettingsPage::updateDiag7700Combo(int i)
{
    ecProject_->setGeneralColDiag77(diag7700Combo->itemData(i).toInt());
}

void BasicSettingsPage::updateTsRefCombo(int i)
{
    ecProject_->setGeneralColTs(tsRefCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlagUnit(int i)
{
//    DEBUG_FUNC_NAME

    QComboBox* senderCombo = qobject_cast<QComboBox *>(sender());

    QString comboName = senderCombo->objectName();
    QString flagName = comboName.left(comboName.indexOf(QLatin1String("Combo")));
    QString flagUnitLabelName = flagName + QStringLiteral("UnitLabel");

    QLabel *unitLabel = this->findChild<QLabel *>(flagUnitLabelName);

    if (senderCombo->currentText() == tr("None")
        || senderCombo->currentText().isEmpty())
    {
        unitLabel->clear();
    }
    else
    {
        // check if valid index position in the list (i.e., 0 <= i < size())
        int k = senderCombo->itemData(i).toInt() - 1;
        if (k >= 0 && k < dlProject_->variables()->size())
        {
            const VariableDesc var = dlProject_->variables()->at(k);
            unitLabel->setText(getFlagUnit(var));
        }
    }
}

QString BasicSettingsPage::getFlagUnit(const VariableDesc& varStr)
{
    DEBUG_FUNC_NAME

    QString var = varStr.variable();
    QString measureType = varStr.measureType();
    QString conversionType = varStr.conversionType();
    QString inputUnitType = varStr.inputUnit();
    QString outputUnitType = varStr.outputUnit();

    if (var == VariableDesc::getVARIABLE_VAR_STRING_0()
        || var == VariableDesc::getVARIABLE_VAR_STRING_1()
        || var == VariableDesc::getVARIABLE_VAR_STRING_2()
        || var == VariableDesc::getVARIABLE_VAR_STRING_4())
    {
        return QStringLiteral("[m/s]");
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_3()
        || var == VariableDesc::getVARIABLE_VAR_STRING_9()
        || var == VariableDesc::getVARIABLE_VAR_STRING_10()
        || var == VariableDesc::getVARIABLE_VAR_STRING_12()
        || var == VariableDesc::getVARIABLE_VAR_STRING_15()
        || var == VariableDesc::getVARIABLE_VAR_STRING_28())
    {
        return QStringLiteral("[K]");
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_11()
        || var == VariableDesc::getVARIABLE_VAR_STRING_13())
    {
        return QStringLiteral("[Pa]");
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_5()
             || var == VariableDesc::getVARIABLE_VAR_STRING_7()
             || var == VariableDesc::getVARIABLE_VAR_STRING_8()
             || var == VariableDesc::getVARIABLE_VAR_STRING_19()
             || var == VariableDesc::getVARIABLE_VAR_STRING_20()
             || var == VariableDesc::getVARIABLE_VAR_STRING_21()
             || var == VariableDesc::getVARIABLE_VAR_STRING_22()
             || var == VariableDesc::getVARIABLE_VAR_STRING_23()
             || var == VariableDesc::getVARIABLE_VAR_STRING_24())
    {
        if (measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0())
        {
            return QStringLiteral("[%1]").arg(Defs::MMOL_M3_STRING);
        }
        else if (measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1()
                 || measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2())
        {
            return QStringLiteral("[%1]").arg(Defs::UMOL_MOL_STRING);
        }
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_6())
    {
        if (measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_0())
        {
            return QStringLiteral("[%1]").arg(Defs::MMOL_M3_STRING);
        }
        else if (measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_1()
                 || measureType == VariableDesc::getVARIABLE_MEASURE_TYPE_STRING_2())
        {
            return QStringLiteral("[%1]").arg(Defs::MMOL_MOL_STRING);
        }
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_29())
    {
        return QStringLiteral("[%1]").arg(Defs::M3_S_STRING);
    }
    else if (var == VariableDesc::getVARIABLE_VAR_STRING_25()
             || var == VariableDesc::getVARIABLE_VAR_STRING_26()
             || var == VariableDesc::getVARIABLE_VAR_STRING_27()
             || var == VariableDesc::getVARIABLE_VAR_STRING_30())
    {
        return QStringLiteral("[-]");
    }
    else
    {
        if (conversionType.isEmpty()
            || conversionType == VariableDesc::getVARIABLE_CONVERSION_TYPE_STRING_2())
        {
            if (!inputUnitType.isEmpty())
                return QStringLiteral("[%1]").arg(inputUnitType);
        }
        else
        {
            if (!outputUnitType.isEmpty())
                return QStringLiteral("[%1]").arg(outputUnitType);
        }
    }
    return QStringLiteral("[-]");
}

void BasicSettingsPage::updateFlag1Combo(int i)
{
    DEBUG_FUNC_NAME
    ecProject_->setScreenFlag1Col(flag1VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag2Combo(int i)
{
    ecProject_->setScreenFlag2Col(flag2VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag3Combo(int i)
{
    ecProject_->setScreenFlag3Col(flag3VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag4Combo(int i)
{
    ecProject_->setScreenFlag4Col(flag4VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag5Combo(int i)
{
    ecProject_->setScreenFlag5Col(flag5VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag6Combo(int i)
{
    ecProject_->setScreenFlag6Col(flag6VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag7Combo(int i)
{
    ecProject_->setScreenFlag7Col(flag7VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag8Combo(int i)
{
    ecProject_->setScreenFlag8Col(flag8VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag9Combo(int i)
{
    ecProject_->setScreenFlag9Col(flag9VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::updateFlag10Combo(int i)
{
    ecProject_->setScreenFlag10Col(flag10VarCombo->itemData(i).toInt());
}

void BasicSettingsPage::onClickAnemRefLabel()
{
    anemRefCombo->setFocus();
    anemRefCombo->showPopup();
}

void BasicSettingsPage::onClickAnemFlagLabel()
{
    anemFlagCombo->setFocus();
    anemFlagCombo->showPopup();
}

void BasicSettingsPage::onClickCo2RefLabel()
{
    co2RefCombo->setFocus();
    co2RefCombo->showPopup();
}

void BasicSettingsPage::onClickH2oRefLabel()
{
    h2oRefCombo->setFocus();
    h2oRefCombo->showPopup();
}

void BasicSettingsPage::onClickCh4RefLabel()
{
    ch4RefCombo->setFocus();
    ch4RefCombo->showPopup();
}

void BasicSettingsPage::onClickFourthGasRefLabel()
{
    fourthGasRefCombo->setFocus();
    fourthGasRefCombo->showPopup();
}

void BasicSettingsPage::onClickIntTcRefLabel()
{
    intTcRefCombo->setFocus();
    intTcRefCombo->showPopup();
}

void BasicSettingsPage::onClickIntT1RefLabel()
{
    intT1RefCombo->setFocus();
    intT1RefCombo->showPopup();
}

void BasicSettingsPage::onClickIntT2RefLabel()
{
    intT2RefCombo->setFocus();
    intT2RefCombo->showPopup();
}

void BasicSettingsPage::onClickIntPRefLabel()
{
    intPRefCombo->setFocus();
    intPRefCombo->showPopup();
}

void BasicSettingsPage::onClickDiag7500Label()
{
    diag7500Combo->setFocus();
    diag7500Combo->showPopup();
}

void BasicSettingsPage::onClickDiag7200Label()
{
    diag7200Combo->setFocus();
    diag7200Combo->showPopup();
}

void BasicSettingsPage::onClickDiag7700Label()
{
    diag7700Combo->setFocus();
    diag7700Combo->showPopup();
}

void BasicSettingsPage::onClickTsRefLabel()
{
    tsRefCombo->setFocus();
    tsRefCombo->showPopup();
}

void BasicSettingsPage::onClickAirTRefLabel()
{
    airTRefCombo->setFocus();
    airTRefCombo->showPopup();
}

void BasicSettingsPage::onClickAirPRefLabel()
{
    airPRefCombo->setFocus();
    airPRefCombo->showPopup();
}

void BasicSettingsPage::onClickRhLabel()
{
    rhCombo->setFocus();
    rhCombo->showPopup();
}

void BasicSettingsPage::onClickRgLabel()
{
    rgCombo->setFocus();
    rgCombo->showPopup();
}

void BasicSettingsPage::onClickLwinLabel()
{
    lwinCombo->setFocus();
    lwinCombo->showPopup();
}

void BasicSettingsPage::onClickPpfdLabel()
{
    ppfdCombo->setFocus();
    ppfdCombo->showPopup();
}

void BasicSettingsPage::createQuestionMark()
{
    questionMark_2 = new QPushButton;
    questionMark_3 = new QPushButton;
    questionMark_4 = new QPushButton;
    questionMark_5 = new QPushButton;

    foreach (QPushButton *btn, QList<QPushButton *>() << questionMark_2
                                                      << questionMark_3
                                                      << questionMark_4
                                                      << questionMark_5)
    {
        btn->setObjectName(QStringLiteral("questionMarkImg"));
    }

    connect(questionMark_2, &QPushButton::clicked,
            this, &BasicSettingsPage::onlineHelpTrigger_2);
    connect(questionMark_3, &QPushButton::clicked,
            this, &BasicSettingsPage::onlineHelpTrigger_3);
    connect(questionMark_4, &QPushButton::clicked,
            this, &BasicSettingsPage::onlineHelpTrigger_4);
    connect(questionMark_5, &QPushButton::clicked,
            this, &BasicSettingsPage::onlineHelpTrigger_5);
}

void BasicSettingsPage::onlineHelpTrigger_2()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Using_Prev_Results.html")));
}

void BasicSettingsPage::onlineHelpTrigger_3()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Flags.html")));
}

void BasicSettingsPage::onlineHelpTrigger_4()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Raw_File_Name_Format.html")));
}

void BasicSettingsPage::onlineHelpTrigger_5()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Declination.html")));
}

void BasicSettingsPage::updateCrossWind(bool b)
{
    DEBUG_FUNC_NAME
    qDebug() << "b" << b;
    qDebug() << "ecProject_->screenCrossWind()" << ecProject_->screenCrossWind();
    ecProject_->setScreenCrossWind(!b);
}

void BasicSettingsPage::updateMetadataRead(bool firstReading)
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    // alternative metadata
    if (ecProject_->generalUseAltMdFile())
    {
        QString mdFile(ecProject_->generalMdFilepath());
        if (!mdFile.isEmpty())
        {
            if (QFile::exists(mdFile))
            {
                qDebug() << "updateMetadataRead";
                updateFilesFound(ecProject_->screenRecurse());
                readAlternativeMetadata(mdFile, firstReading);
                reloadSelectedItems_1();
            }
            else
            {
                ecProject_->setGeneralMdFilepath(QString());
                clearFilesFound();
            }
        }
    }
    // embedded metadata
    else
    {
        if (!datapathBrowse->path().isEmpty()
            && ecProject_->generalFileType() == Defs::RawFileType::GHG)
        {
            // re-capture metadata if dataDir exists, otherwise discard silently
            QDir dataDir(datapathBrowse->path());
            if (dataDir.exists())
            {
                captureEmbeddedMetadata(rawEmbeddedFile);
                reloadSelectedItems_1();
            }
            else
            {
                datapathBrowse->clear();
                clearFilesFound();
            }
        }
    }

    qDebug() << "ecProject_->generalUseBiomet()" << ecProject_->generalUseBiomet();

    // biomet vars
    switch (ecProject_->generalUseBiomet())
    {
        case 0:
            break;
        case 1:
            qDebug()<< "CASE 11111111111111111111111";
            if (!datapathBrowse->path().isEmpty()
                && ecProject_->generalFileType() == Defs::RawFileType::GHG)
            {
                // re-capture metadata if dataDir exists, otherwise discard silently
                QDir dataDir(datapathBrowse->path());
                if (dataDir.exists())
                {
                    captureEmbeddedMetadata(biometEmbeddedFile);
                    reloadSelectedItems_2();
                }
            }
            break;
        case 2:
        {
            qDebug()<< "CASE 2222222222222222222222222222";
            QString biomDataFile = ecProject_->generalBiomFile();
            if (!biomDataFile.isEmpty())
            {
                if (QFile::exists(biomDataFile))
                {
                    if (readBiomAltMetadata(biomDataFile))
                    {
                        reloadSelectedItems_2();
                    }
                }
            }
            break;
        }
        case 3:
        {
            qDebug()<< "CASE 333333333333333333333333333333";
            QStringList biomFileList = FileUtils::getFiles(ecProject_->generalBiomDir(),
                                                      QStringLiteral("*.") + ecProject_->generalBiomExt(),
                                                      ecProject_->generalBiomRecurse());

            qDebug() << "case 3" << ecProject_->generalBiomDir()
                     << ecProject_->generalBiomExt()
                     << ecProject_->generalBiomRecurse()
                     << biomFileList;
            foreach (const QString& file, biomFileList)
            {
                qDebug() << "file" << file;
                if (readBiomAltMetadata(file))
                {
                    reloadSelectedItems_2();
                    break;
                }
                else
                {
                    continue;
                }
            }
            break;
        }
        default:
            break;
    }

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void BasicSettingsPage::reloadSelectedItems_1()
{
    DEBUG_FUNC_NAME

    addNoneStr_1();

    QString currDataStr = ecProject_->generalColMasterSonic();
    qDebug() << "currDataStr" << currDataStr;
    int currItemIndex = anemRefCombo->findData(currDataStr);
    qDebug() << "currItemIndex" << currItemIndex;
    QString selectedDataStr = WidgetUtils::currentComboItemData(anemRefCombo).toString();
    qDebug() << "selectedDataStr" << selectedDataStr;
    int selectedItemIndex = anemRefCombo->findData(selectedDataStr);
    qDebug() << "selectedItemIndex" << selectedItemIndex;

    if (currDataStr.isEmpty() && !selectedDataStr.isEmpty())
    {
        anemRefCombo->setCurrentIndex(selectedItemIndex);
        ecProject_->setGeneralColMasterSonic(selectedDataStr);
    }
    else if (!currDataStr.isEmpty() && currItemIndex != -1)
    {
        anemRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColMasterSonic(currDataStr);
    }
    else
    {
        anemRefCombo->setCurrentIndex(0);
        ecProject_->setGeneralColMasterSonic(anemRefCombo->itemData(0).toString());
    }
//
    int currData = ecProject_->generalColTs();
    currItemIndex = tsRefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        tsRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColTs(currData);
    }
    else
    {
        // preselect something better
        preselect7700Variables(tsRefCombo);
        ecProject_->setGeneralColTs(WidgetUtils::currentComboItemData(tsRefCombo).toInt());
    }
    qDebug() << "tsRefCombo" << ecProject_->generalColTs() << currItemIndex;
//
    int noneIndex = co2RefCombo->findData(0);
    currData = ecProject_->generalColCo2();
    currItemIndex = co2RefCombo->findData(currData);
    qDebug() << "co2";
    qDebug() << "currData" << currData;
    qDebug() << "currItemIndex" << currItemIndex;
    qDebug() << "noneIndex" << noneIndex;
    if (currItemIndex >= 0)
    {
        co2RefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColCo2(currData);
    }
    else
    {
        // preselect something better
        preselectDensityVariables(co2RefCombo);
        ecProject_->setGeneralColCo2(WidgetUtils::currentComboItemData(co2RefCombo).toInt());
    }
//
    qDebug() << "h2o";
    qDebug() << "currData" << currData;
    qDebug() << "currItemIndex" << currItemIndex;
    qDebug() << "noneIndex" << h2oRefCombo->findData(-1);
    currData = ecProject_->generalColH2o();
    currItemIndex = h2oRefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        h2oRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColH2o(currData);
    }
    else
    {
        preselectDensityVariables(h2oRefCombo);
        ecProject_->setGeneralColH2o(WidgetUtils::currentComboItemData(h2oRefCombo).toInt());
    }
//
    currData = ecProject_->generalColCh4();
    currItemIndex = ch4RefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        ch4RefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColCh4(currData);
    }
    else
    {
        preselectDensityVariables(ch4RefCombo);
        ecProject_->setGeneralColCh4(WidgetUtils::currentComboItemData(ch4RefCombo).toInt());
    }
//
    currData = ecProject_->generalColGas4();
    currItemIndex = fourthGasRefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        fourthGasRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColGas4(currData);
    }
    else
    {
        preselectDensityVariables(fourthGasRefCombo);
        ecProject_->setGeneralColGas4(WidgetUtils::currentComboItemData(fourthGasRefCombo).toInt());
    }

    gasMw->setValue(ecProject_->generalGasMw());
    gasDiff->setValue(ecProject_->generalGasDiff());
//
    currData = ecProject_->generalColIntTc();
    currItemIndex = intTcRefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        intTcRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColIntTc(currData);
    }
    else
    {
        intTcRefCombo->setCurrentIndex(0);
        ecProject_->setGeneralColIntTc(intTcRefCombo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColIntT1();
    currItemIndex = intT1RefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        intT1RefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColIntT1(currData);
    }
    else
    {
        intT1RefCombo->setCurrentIndex(0);
        ecProject_->setGeneralColIntT1(intT1RefCombo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColIntT2();
    currItemIndex = intT2RefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        intT2RefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColIntT2(currData);
    }
    else
    {
        intT2RefCombo->setCurrentIndex(0);
        ecProject_->setGeneralColIntT2(intT2RefCombo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColIntP();
    currItemIndex = intPRefCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        intPRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColIntP(currData);
    }
    else
    {
        intPRefCombo->setCurrentIndex(0);
        ecProject_->setGeneralColIntP(intPRefCombo->itemData(0).toInt());
    }
//
    if (ecProject_->generalUseBiomet() == 0)
    {
        currData = ecProject_->generalColAirT();
        currItemIndex = airTRefCombo->findData(currData);
        qDebug() << "airT";
        qDebug() << "currData" << currData;
        qDebug() << "currItemIndex" << currItemIndex;
        if (currItemIndex >= 0)
        {
            airTRefCombo->setCurrentIndex(currItemIndex);
            ecProject_->setGeneralColAirT(currData);
        }
        else
        {
            // select something better
            preselect7700Variables(airTRefCombo);
            updateAirTRefCombo(airTRefCombo->currentIndex());
        }
    //
        currData = ecProject_->generalColAirP();
        currItemIndex = airPRefCombo->findData(currData);
        if (currItemIndex >= 0)
        {
            airPRefCombo->setCurrentIndex(currItemIndex);
            ecProject_->setGeneralColAirP(currData);
        }
        else
        {
            // select something better
            preselect7700Variables(airPRefCombo);
            updateAirPRefCombo(airPRefCombo->currentIndex());
        }
    }
//
    currData = ecProject_->generalColDiag75();
    currItemIndex = diag7500Combo->findData(currData);
    if (currItemIndex >= 0)
    {
        diag7500Combo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColDiag75(currData);
    }
    else
    {
        diag7500Combo->setCurrentIndex(0);
        ecProject_->setGeneralColDiag75(diag7500Combo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColDiag72();
    currItemIndex = diag7200Combo->findData(currData);
    if (currItemIndex >= 0)
    {
        diag7200Combo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColDiag72(currData);
    }
    else
    {
        diag7200Combo->setCurrentIndex(0);
        ecProject_->setGeneralColDiag72(diag7200Combo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColDiag77();
    currItemIndex = diag7700Combo->findData(currData);
    if (currItemIndex >= 0)
    {
        diag7700Combo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColDiag77(currData);
    }
    else
    {
        diag7700Combo->setCurrentIndex(0);
        ecProject_->setGeneralColDiag77(diag7700Combo->itemData(0).toInt());
    }
//
    currData = ecProject_->generalColDiagAnem();
    currItemIndex = anemFlagCombo->findData(currData);
    if (currItemIndex >= 0)
    {
        anemFlagCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColDiagAnem(currData);
    }
    else
    {
        anemFlagCombo->setCurrentIndex(0);
        ecProject_->setGeneralColDiagAnem(anemFlagCombo->itemData(0).toInt());
    }
    //
    currData = ecProject_->screenFlag1Col();
    currItemIndex = flag1VarCombo->findData(currData);
    noneIndex = flag1VarCombo->findData(0);
    qDebug() << "reloadSelectedItems Flag1Col";
    qDebug() << "currData" << currData;
    qDebug() << "currItemIndex" << currItemIndex;
    qDebug() << "noneIndex" << noneIndex;
    if (currItemIndex >= 0)
    {
        flag1VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag1Col(currData);
    }
    else
    {
        flag1VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag1Col(0);
    }
//
    currData = ecProject_->screenFlag2Col();
    currItemIndex = flag2VarCombo->findData(currData);
    noneIndex = flag2VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag2VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag2Col(currData);
    }
    else
    {
        flag2VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag2Col(0);
    }
//
    currData = ecProject_->screenFlag3Col();
    currItemIndex = flag3VarCombo->findData(currData);
    noneIndex = flag3VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag3VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag3Col(currData);
    }
    else
    {
        flag3VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag3Col(0);
    }
//
    currData = ecProject_->screenFlag4Col();
    currItemIndex = flag4VarCombo->findData(currData);
    noneIndex = flag4VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag4VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag4Col(currData);
    }
    else
    {
        flag4VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag4Col(0);
    }
//
    currData = ecProject_->screenFlag5Col();
    currItemIndex = flag5VarCombo->findData(currData);
    noneIndex = flag5VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag5VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag5Col(currData);
    }
    else
    {
        flag5VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag5Col(0);
    }
//
    currData = ecProject_->screenFlag6Col();
    currItemIndex = flag6VarCombo->findData(currData);
    noneIndex = flag6VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag6VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag6Col(currData);
    }
    else
    {
        flag6VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag6Col(0);
    }
//
    currData = ecProject_->screenFlag7Col();
    currItemIndex = flag7VarCombo->findData(currData);
    noneIndex = flag7VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag7VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag7Col(currData);
    }
    else
    {
        flag7VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag7Col(0);
    }
//
    currData = ecProject_->screenFlag8Col();
    currItemIndex = flag8VarCombo->findData(currData);
    noneIndex = flag8VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag8VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag8Col(currData);
    }
    else
    {
        flag8VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag8Col(0);
    }
//
    currData = ecProject_->screenFlag9Col();
    currItemIndex = flag9VarCombo->findData(currData);
    noneIndex = flag9VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag9VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag9Col(currData);
    }
    else
    {
        flag9VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag9Col(0);
    }
//
    currData = ecProject_->screenFlag10Col();
    currItemIndex = flag10VarCombo->findData(currData);
    noneIndex = flag10VarCombo->findData(0);
    if (currItemIndex >= 0)
    {
        flag10VarCombo->setCurrentIndex(currItemIndex);
        ecProject_->setScreenFlag10Col(currData);
    }
    else
    {
        flag10VarCombo->setCurrentIndex(noneIndex);
        ecProject_->setScreenFlag10Col(0);
    }

    flag1ThresholdSpin->setValue(ecProject_->screenFlag1Threshold());
    flag1PolicyCombo->setCurrentIndex(ecProject_->screenFlag1Upper());
    flag2ThresholdSpin->setValue(ecProject_->screenFlag2Threshold());
    flag2PolicyCombo->setCurrentIndex(ecProject_->screenFlag2Upper());
    flag3ThresholdSpin->setValue(ecProject_->screenFlag3Threshold());
    flag3PolicyCombo->setCurrentIndex(ecProject_->screenFlag3Upper());
    flag4ThresholdSpin->setValue(ecProject_->screenFlag4Threshold());
    flag4PolicyCombo->setCurrentIndex(ecProject_->screenFlag4Upper());
    flag5ThresholdSpin->setValue(ecProject_->screenFlag5Threshold());
    flag5PolicyCombo->setCurrentIndex(ecProject_->screenFlag5Upper());
    flag6ThresholdSpin->setValue(ecProject_->screenFlag6Threshold());
    flag6PolicyCombo->setCurrentIndex(ecProject_->screenFlag6Upper());
    flag7ThresholdSpin->setValue(ecProject_->screenFlag7Threshold());
    flag7PolicyCombo->setCurrentIndex(ecProject_->screenFlag7Upper());
    flag8ThresholdSpin->setValue(ecProject_->screenFlag8Threshold());
    flag8PolicyCombo->setCurrentIndex(ecProject_->screenFlag8Upper());
    flag9ThresholdSpin->setValue(ecProject_->screenFlag9Threshold());
    flag9PolicyCombo->setCurrentIndex(ecProject_->screenFlag9Upper());
    flag10ThresholdSpin->setValue(ecProject_->screenFlag10Threshold());
    flag10PolicyCombo->setCurrentIndex(ecProject_->screenFlag10Upper());
}

void BasicSettingsPage::reloadSelectedItems_2()
{
    DEBUG_FUNC_NAME

    addNoneStr_1();

    int currData = ecProject_->generalColAirT();
    int currItemIndex = airTRefCombo->findData(currData);
    int currData2 = ecProject_->biomParamColAirT();
    int currItemIndex2 = airTRefCombo->findData(currData2);
    int noneIndex = airTRefCombo->findData(0);

    qDebug() << "airt currData" << currData;
    qDebug() << "airt currItemIndex" << currItemIndex;
    qDebug() << "airt currData2" << currData2;
    qDebug() << "airt currItemIndex2" << currItemIndex2;

    if (currData > 0 && currItemIndex != -1)
    {
        airTRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColAirT(currData);
        ecProject_->setBiomParamColAirT(1000);
    }
    else if (currData2 > 1000 && currItemIndex2 != -1)
    {
        airTRefCombo->setCurrentIndex(currItemIndex2);
        ecProject_->setGeneralColAirT(0);
        ecProject_->setBiomParamColAirT(currData2);
    }
    else if (currData == 0 && currData2 == 1000)
    {
        // select none
        airTRefCombo->setCurrentIndex(noneIndex);
        ecProject_->setGeneralColAirT(0);
        ecProject_->setBiomParamColAirT(1000);
    }
    else
    {
        // select something better
        preselect7700Variables(airTRefCombo);
        updateAirTRefCombo(airTRefCombo->currentIndex());
    }

    qDebug() << "airt currData" << currData;
    qDebug() << "airt currItemIndex" << currItemIndex;
    qDebug() << "airt currData2" << currData2;
    qDebug() << "airt currItemIndex2" << currItemIndex2;
//
    currData = ecProject_->generalColAirP();
    currItemIndex = airPRefCombo->findData(currData);
    currData2 = ecProject_->biomParamColAirP();
    currItemIndex2 = airPRefCombo->findData(currData2);
    noneIndex = airPRefCombo->findData(0);

    if (currData > 0)
    {
        airPRefCombo->setCurrentIndex(currItemIndex);
        ecProject_->setGeneralColAirP(currData);
        ecProject_->setBiomParamColAirP(1000);
    }
    else if (currData2 > 1000)
    {
        airPRefCombo->setCurrentIndex(currItemIndex2);
        ecProject_->setGeneralColAirP(0);
        ecProject_->setBiomParamColAirP(currData2);
    }
    else if (currData == 0 && currData2 == 1000)
    {
        // select none
        airPRefCombo->setCurrentIndex(noneIndex);
        ecProject_->setGeneralColAirP(0);
        ecProject_->setBiomParamColAirP(1000);
    }
    else
    {
        // select something better
        preselect7700Variables(airPRefCombo);
        updateAirPRefCombo(airPRefCombo->currentIndex());
    }
//
    currData = ecProject_->biomParamColRh();
    currItemIndex = rhCombo->findData(currData);
    int firstIndexData = rhCombo->itemData(0).toInt();
    qDebug() << "rh currData" << currData;
    qDebug() << "rh currItemIndex" << currItemIndex;
    if (currItemIndex >= 0)
    {
        rhCombo->setCurrentIndex(currItemIndex);
        ecProject_->setBiomParamColRh(currData);
    }
    else
    {
        rhCombo->setCurrentIndex(0);
        ecProject_->setBiomParamColRh(firstIndexData);
    }
//
    currData = ecProject_->biomParamColRg();
    currItemIndex = rgCombo->findData(currData);
    firstIndexData = rgCombo->itemData(0).toInt();
    if (currItemIndex >= 0)
    {
        rgCombo->setCurrentIndex(currItemIndex);
        ecProject_->setBiomParamColRg(currData);
    }
    else
    {
        rgCombo->setCurrentIndex(0);
        ecProject_->setBiomParamColRg(firstIndexData);
    }
//
    currData = ecProject_->biomParamColLwin();
    currItemIndex = lwinCombo->findData(currData);
    firstIndexData = lwinCombo->itemData(0).toInt();
    if (currItemIndex >= 0)
    {
        lwinCombo->setCurrentIndex(currItemIndex);
        ecProject_->setBiomParamColLwin(currData);
    }
    else
    {
        lwinCombo->setCurrentIndex(0);
        ecProject_->setBiomParamColLwin(firstIndexData);
    }
//
    currData = ecProject_->biomParamColPpfd();
    currItemIndex = ppfdCombo->findData(currData);
    firstIndexData = ppfdCombo->itemData(0).toInt();
    if (currItemIndex >= 0)
    {
        ppfdCombo->setCurrentIndex(currItemIndex);
        ecProject_->setBiomParamColPpfd(currData);
    }
    else
    {
        ppfdCombo->setCurrentIndex(0);
        ecProject_->setBiomParamColPpfd(firstIndexData);
    }
}

void BasicSettingsPage::onIdLabelClicked()
{
    idEdit->setFocus();
    idEdit->selectAll();
}

void BasicSettingsPage::updateFlag1Threshold(double n)
{
    ecProject_->setScreenFlag1Threshold(n);
}

void BasicSettingsPage::updateFlag2Threshold(double n)
{
    ecProject_->setScreenFlag2Threshold(n);
}

void BasicSettingsPage::updateFlag3Threshold(double n)
{
    ecProject_->setScreenFlag3Threshold(n);
}

void BasicSettingsPage::updateFlag4Threshold(double n)
{
    ecProject_->setScreenFlag4Threshold(n);
}

void BasicSettingsPage::updateFlag5Threshold(double n)
{
    ecProject_->setScreenFlag5Threshold(n);
}

void BasicSettingsPage::updateFlag6Threshold(double n)
{
    ecProject_->setScreenFlag6Threshold(n);
}

void BasicSettingsPage::updateFlag7Threshold(double n)
{
    ecProject_->setScreenFlag7Threshold(n);
}

void BasicSettingsPage::updateFlag8Threshold(double n)
{
    ecProject_->setScreenFlag8Threshold(n);
}

void BasicSettingsPage::updateFlag9Threshold(double n)
{
    ecProject_->setScreenFlag9Threshold(n);
}

void BasicSettingsPage::updateFlag10Threshold(double n)
{
    ecProject_->setScreenFlag10Threshold(n);
}

void BasicSettingsPage::updateFlag1Policy(int n)
{
    ecProject_->setScreenFlag1Policy(n);
}

void BasicSettingsPage::updateFlag2Policy(int n)
{
    ecProject_->setScreenFlag2Policy(n);
}

void BasicSettingsPage::updateFlag3Policy(int n)
{
    ecProject_->setScreenFlag3Policy(n);
}

void BasicSettingsPage::updateFlag4Policy(int n)
{
    ecProject_->setScreenFlag4Policy(n);
}

void BasicSettingsPage::updateFlag5Policy(int n)
{
    ecProject_->setScreenFlag5Policy(n);
}

void BasicSettingsPage::updateFlag6Policy(int n)
{
    ecProject_->setScreenFlag6Policy(n);
}

void BasicSettingsPage::updateFlag7Policy(int n)
{
    ecProject_->setScreenFlag7Policy(n);
}

void BasicSettingsPage::updateFlag8Policy(int n)
{
    ecProject_->setScreenFlag8Policy(n);
}

void BasicSettingsPage::updateFlag9Policy(int n)
{
    ecProject_->setScreenFlag9Policy(n);
}

void BasicSettingsPage::updateFlag10Policy(int n)
{
    ecProject_->setScreenFlag10Policy(n);
}

// NOTE: not used
void BasicSettingsPage::triggerGasProperties()
{
    DEBUG_FUNC_NAME

    bool gasExtHidden = gasExtension->isHidden();

    gasExtension->setVisible(gasExtHidden);
    moreButton->setChecked(gasExtHidden);
    updateGeometry();
}

void BasicSettingsPage::onClickFlagLabel()
{
    QLabel* labelSender = qobject_cast<QLabel *>(sender());

    QString labelName = labelSender->objectName();
    QString flagName = labelName.left(labelName.indexOf(QLatin1String("Label")));
    QString flagComboObjectName = flagName + QStringLiteral("Combo");

    QComboBox *flagCombo = this->findChild<QComboBox *>(flagComboObjectName);

    flagCombo->setFocus();
    flagCombo->showPopup();
}

void BasicSettingsPage::updateGasMw(double value)
{
    ecProject_->setGeneralColGasMw(value);
}

void BasicSettingsPage::updateGasDiff(double value)
{
    ecProject_->setGeneralColGasDiff(value);
}

// enforce (start date&time) <= (end date&time)
void BasicSettingsPage::forceEndDatePolicy()
{
    DEBUG_FUNC_NAME

    endDateEdit->setMinimumDate(startDateEdit->date());
}

// enforce (start date&time) <= (end date&time)
void BasicSettingsPage::forceEndTimePolicy()
{
    DEBUG_FUNC_NAME

    qDebug() << "start - end, dates:" << startDateEdit->date() << endDateEdit->date();

    if (startDateEdit->date() == endDateEdit->date())
    {
        endTimeEdit->setMinimumTime(startTimeEdit->time());
    }
    else
    {
        endTimeEdit->clearMinimumTime();
    }
}

///
/// \brief BasicSettingsPage::updateProjectFilesFound
/// \param fileNumber
///
void BasicSettingsPage::updateProjectFilesFound(int fileNumber)
{
    ecProject_->setGeneralFilesFound(fileNumber);
}

void BasicSettingsPage::clearFilePrototype()
{
    updateFilePrototype(QString());
    clearFilesFound();
}

// called by programmatic changes on filePrototypeEdit
void BasicSettingsPage::updateFilePrototype(const QString& pattern)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralFilePrototype(pattern);
}

// called by:
// 1. captureEmbeddedMetadata(), ok
// 2. clearFilesFound(), ok
// 3. updateFilesFound(), ok
void BasicSettingsPage::updateFilesFoundLabel(int fileNumber)
{
    DEBUG_FUNC_NAME

    qDebug() << "fileNumber" << fileNumber;

    if (fileNumber == 0)
    {
        filesFound->clear();
    }
    else if (fileNumber == 1)
    {
        filesFound->setText(tr("1 file found"));
    }
    else
    {
        filesFound->setText(tr("%1 files found").arg(fileNumber));
    }
}

// called by:
// 1. captureEmbeddedMetadata() in case of no files found, ok
// 2. reset(), ok
// 3. updateMetadaRead, ok
// 4. refresh(), ?
void BasicSettingsPage::clearFilesFound()
{
    updateFilesFoundLabel(0);
    updateProjectFilesFound(0);
}

// called by updateRecursion() when recursion checkbox is toggled
void BasicSettingsPage::runUpdateFilesFound()
{
    DEBUG_FUNC_NAME
    updateFilesFound(ecProject_->screenRecurse());
}

// transform prototype p to a regular expression pattern
QString BasicSettingsPage::prototypeToRegExp(const QString& p)
{
    auto pattern = p;

    pattern.replace(QLatin1String("."), QLatin1String("[.]"));  // dot
    pattern.replace(QLatin1String("?"), QLatin1String("."));    // single char
    pattern.replace(QLatin1String("yyyy"), QLatin1String("(19[89][0-9]|20[0-9][0-9]|2100)")); // year 4 digits
    pattern.replace(QLatin1String("yy"), QLatin1String("([0-9][0-9])"));         // year 2 digits
    pattern.replace(QLatin1String("mm"), QLatin1String("(0[1-9]|1[012])"));    // month 2 digits
    pattern.replace(QLatin1String("ddd"), QLatin1String("(00[1-9]|0[1-9][0-9]|[12][0-9][0-9]|3[0-5][0-9]|36[0-6])"));      // day of year 3 digits
    pattern.replace(QLatin1String("dd"), QLatin1String("(0[1-9]|[1-2][0-9]|3[01])")); // day 2 digits
    pattern.replace(QLatin1String("HH"), QLatin1String("([01][0-9]|2[0-4])")); // hours 2 digits
    pattern.replace(QLatin1String("MM"), QLatin1String("([0-5][0-9])"));       // minutes 2 digits

    return pattern;
}

QStringList BasicSettingsPage::filterRawDataWithPrototype(const QString& p)
{
    auto rePattern = prototypeToRegExp(p);
    qDebug() << p << "rePattern" << rePattern;

    QRegularExpression re;
    re.setPattern(rePattern);
    qDebug() << "re valid? " << re.isValid();

    if (re.isValid())
    {
        currentFilteredRawDataList_ = currentRawDataList_;

        foreach (const auto &filename, currentFilteredRawDataList_)
        {
            qDebug() << "filename" << filename;
            if (!filename.contains(re))
            {
                qDebug() << "filtering";
                currentFilteredRawDataList_.removeAll(filename);
            }
        }
    }

    return currentFilteredRawDataList_;
}

// called:
// 1. at the end of setPrototype() ok
// 2. from runUpdateFilesFound() ok
// 3. from updateMetadataRead() ok
void BasicSettingsPage::updateFilesFound(bool recursionToggled)
{
    DEBUG_FUNC_NAME

    if (datapathBrowse->path().isEmpty())
    {
        return;
    }

    auto fileCount = 0;

    findFileProgressWidget->startAnimation();

    // first pass, filter by extension on the file system
    if (filePrototypeEdit->text().isEmpty())
    {
        if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
        {
            QString extension = QStringLiteral("*.") + Defs::GHG_NATIVE_DATA_FILE_EXT;
            currentRawDataList_ = FileUtils::getFiles(datapathBrowse->path(), extension, recursionToggled);
        }
    }
    else
    {
        int extensionIndex = ecProject_->generalFilePrototype().lastIndexOf(QLatin1String(".")) + 1;
        QString extension = QStringLiteral("*.") + ecProject_->generalFilePrototype().mid(extensionIndex);
        currentRawDataList_ = FileUtils::getFiles(datapathBrowse->path(), extension, recursionToggled);
    }

    // second pass, filter the file list with a regexp
    if (filePrototypeEdit->text().isEmpty())
    {
        currentFilteredRawDataList_ = currentRawDataList_;
    }
    else
    {
        currentFilteredRawDataList_ = filterRawDataWithPrototype(filePrototypeEdit->text());
    }

    findFileProgressWidget->stopAnimation();

    fileCount = currentFilteredRawDataList_.count();
    qDebug() << "fileCount" << fileCount;

    updateFilesFoundLabel(fileCount);
    updateProjectFilesFound(fileCount);
}

// called by:
// 1. RawFilenameDialog::updateFileFormatRequest() signal, ok
// 2. setPrototype(), ok
void BasicSettingsPage::updateFilePrototypeEdit(const QString& f)
{
    DEBUG_FUNC_NAME
    filePrototypeEdit->setText(f);

    updateFilesFound(ecProject_->screenRecurse());
}

void BasicSettingsPage::askRawFilenamePrototype()
{
    DEBUG_FUNC_NAME
    if (!rawFilenameDialog)
    {
        qDebug() << "create dialog";
        rawFilenameDialog = new RawFilenameDialog(this,
                                                  ecProject_,
                                                  &suffixList_,
                                                  &currentFilteredRawDataList_);
        rawFilenameDialog->setObjectName(QStringLiteral("RawFilenameDialog"));
        connect(rawFilenameDialog, &RawFilenameDialog::updateFileFormatRequest,
                this, &BasicSettingsPage::updateFilePrototypeEdit);
    }

    rawFilenameDialog->refresh();

    rawFilenameDialog->show();
    rawFilenameDialog->raise();
    rawFilenameDialog->activateWindow();
}

void BasicSettingsPage::fetchMagneticDeclination()
{
    DEBUG_FUNC_NAME

    progressWidget_3->startAnimation();
    httpManager_ = new QNetworkAccessManager(this);

    auto noaaServiceUrl = QUrl(QStringLiteral("http://www.ngdc.noaa.gov/geomag-web/calculators/calculateDeclination"));

    QNetworkRequest postRequest;
    postRequest.setUrl(noaaServiceUrl);
    postRequest.setRawHeader("User-Agent", "MyOwnBrowser 1.0");
    postRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                          QStringLiteral("application/x-www-form-urlencoded; charset=utf-8"));

    auto decLat = dlProject_->siteLatitude();
//    QString minLatStr = QString::number(qAbs(decLat), 'd', 6);
//    QString minLatHemisphere = QStringLiteral("N");
//    if (decLat < 0)
//        minLatHemisphere = QStringLiteral("S");
    auto decLon = dlProject_->siteLongitude();
//    QString minLonStr = QString::number(qAbs(decLon), 'd', 6);
//    QString minLonHemisphere = QStringLiteral("E");
//    if (decLon < 0)
//        minLonHemisphere = QStringLiteral("W");

//    qDebug() << decLat << minLatStr << minLatHemisphere;
//    qDebug() << decLon << minLonStr << minLonHemisphere;

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("lat1"), QString::number(decLat, 'd', 6));
    q.addQueryItem(QStringLiteral("lon1"), QString::number(decLon, 'd', 6));
    q.addQueryItem(QStringLiteral("model"), QStringLiteral("IGRF"));
    q.addQueryItem(QStringLiteral("startYear"), QString::number(declinationDateEdit->date().year()));
    q.addQueryItem(QStringLiteral("startMonth"), QString::number(declinationDateEdit->date().month()));
    q.addQueryItem(QStringLiteral("startDay"), QString::number(declinationDateEdit->date().day()));
    q.addQueryItem(QStringLiteral("resultFormat"), QStringLiteral("csv"));

    httpReply_ = httpManager_->post(postRequest, q.query(QUrl::FullyEncoded).toUtf8());

    connect(httpManager_, &QNetworkAccessManager::finished,
            this, &BasicSettingsPage::replyFinished);

    connect(httpReply_, &QNetworkReply::readyRead,
            this, &BasicSettingsPage::bufferHttpReply);
}

void BasicSettingsPage::replyFinished(QNetworkReply* reply)
{
    DEBUG_FUNC_NAME

    // if no error
    progressWidget_3->stopAnimation();
    if (!reply->error()) { return; }

    // handle the error
    qDebug() << reply->error();
    qDebug() << reply->errorString();

    // show only if not aborted by the user
    if (reply->error() != QNetworkReply::OperationCanceledError)
    {
        noNoaaConnectionMsg();
    }
}

void BasicSettingsPage::bufferHttpReply()
{
    DEBUG_FUNC_NAME

    if (httpReply_->error() == QNetworkReply::NoError)
    {
        QByteArray data = httpReply_->readAll();
        qDebug() << "data" << data;

        httpBuffer_.append(data);
        QByteArray line(httpBuffer_);
        QByteArrayList lineList(line.split('\n'));
        qDebug() << "lineList.size" << lineList.size();

        // newlines found
        if (lineList.at(0) != httpBuffer_)
        {
            for (int i = 0; i < lineList.size(); ++i)
            {
                qDebug() << "i" << i;
                if (parseHttpReply(lineList.at(i)))
                {
                   continue;
                }
                else
                {
                    // manage NOAA server errors
                    noNoaaDownloadMsg();
                    progressWidget_3->stopAnimation();
                    return;
                }
            }

            if (lineList.last().endsWith('\n'))
                httpBuffer_.resize(0);
            else
                httpBuffer_ = lineList.last();
        }
    }
    else
    {
        // handle the error
        qDebug() << httpReply_->errorString();
    }
}

bool BasicSettingsPage::parseHttpReply(const QByteArray& data)
{
    QByteArray cleanLine;

    cleanLine.append(data.simplified());
    qDebug() << "cleanLine" << cleanLine;

    if (cleanLine.startsWith("#") || cleanLine.startsWith("\n"))
    {
        // skip comments or empty row
        return true;
    }
    QByteArrayList columnList(cleanLine.split(','));

    qDebug() << "columnList" << columnList;
    // web-server error handling
    if (columnList.size() > 1)
    {
        // declination in decimal degrees
        declination_ = QVariant(columnList.at(4)).toString();

        QString decStr = strDeclination(declination_.toDouble());
        qDebug() << "declination_" << declination_
                 << "declination_.toDouble()" << declination_.toDouble()
                 << "decStr" << decStr;
        declinationEdit->setText(decStr);

        // variation of declination in decimal degrees / year
        double variation = QVariant(columnList.at(5)).toDouble();
        // variation of declination in decimal minutes / year
        QString variationDecValue = strDeclination(variation);
        qDebug() << "variation value" << columnList.at(5)
                 << "variationDecValue" << variationDecValue;

        QString variationStr = tr("Changing by %1 per year").arg(variationDecValue);
        qDebug() << "variationStr" << variationStr;

        decChangingLabel->setText(variationStr);

        progressWidget_3->stopAnimation();
    }
    return true;
}

void BasicSettingsPage::northRadioClicked(int b)
{
    declinationLabel->setEnabled(b);
    declinationLabel->setToolTip(tr("<b>Magnetic Declination:</b> Based upon the latitude and longitudinal coordinates entered, EddyPro determines the magnetic declination from the U.S. NOAA (National Oceanic and Atmospheric Organization) internet resources (U.S. National Geophysical Data Center)."));

    declinationEdit->setEnabled(b);
    declinationDateLabel->setEnabled(b);
    declinationDateEdit->setEnabled(b);
    declinationFetchButton->setEnabled(b);

    if (b)
    {
        declinationDateEdit->setDate(endDateEdit->date());
    }

    // block fetching
    if (b == 0 && httpReply_)
    {
        qDebug() << "httpReply_";
        httpReply_->abort();
    }
}

void BasicSettingsPage::updateUseGeoNorth(int b)
{
    ecProject_->setScreenUseGeoNorth(b);
}

void BasicSettingsPage::updateMagDec(const QString& dec)
{
    QString dec_str = QString::number(numDeclination(dec), 'd', 6);
    ecProject_->setScreenMagDec(dec_str.toDouble());
}

// get declination in signed decimal degrees from degree, minutes (dddmm.sss) string
double BasicSettingsPage::numDeclination(const QString &text)
{
    double dec = 0.0;

    QString dec_pattern = tr("(?:(0\\d\\d)%1\\s([0-5]\\d)'\\s(E|W))|").arg(Defs::DEGREE);
            dec_pattern += tr("(?:(1[0-7]\\d)%1\\s([0-5]\\d)'\\s(E|W))|").arg(Defs::DEGREE);
            dec_pattern += tr("(?:(180)%1\\s(00)'\\s(E|W))").arg(Defs::DEGREE);
    QRegExp decRx(dec_pattern);
    bool res = decRx.exactMatch(text);
    if (res)
    {
        bool ok;
        if (!decRx.cap(1).isEmpty())
        {
            // first case: pattern from cap(1) to cap(3)
            dec = decRx.cap(1).toDouble(&ok);
            dec += decRx.cap(2).toDouble(&ok) / 60.0;
        }
        else if (!decRx.cap(4).isEmpty())
        {
            // second case: pattern from cap(4) to cap(6)
            dec = decRx.cap(4).toDouble(&ok);
            dec += decRx.cap(5).toDouble(&ok) / 60.0;
        }
        else
        {
            // third case: pattern from cap(7) to cap(9)
            dec = 180.0;
        }
        // negative coordinates case
        if ((decRx.cap(3) == QLatin1String("W"))||(decRx.cap(6) == QLatin1String("W"))||(decRx.cap(9) == QLatin1String("W")))
        {
            dec = 0.0 - dec;
        }
    }
    return dec;
}

void BasicSettingsPage::onClickDeclinationLabel()
{
    declinationEdit->setFocus();
    declinationEdit->selectAll();
}

// get declination in degree, minutes (dddmm.sss) string
// from signed decimal degrees
QString BasicSettingsPage::strDeclination(double dec)
{
    QString dms = QString();

    double degrees;
    modf(dec, &degrees);

    double rest = dec - degrees;
    double min_rest = rest * 60.0;

    double min_d;
    modf(min_rest, &min_d);

    int minutes = static_cast<int>(min_d);

    QString degrees_str;
    QTextStream d(&degrees_str);
    d << qSetFieldWidth(3) << qSetPadChar(QLatin1Char('0')) << right << qAbs( degrees );

    QString minutes_str;
    QTextStream m(&minutes_str);
    m << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << right << qAbs( minutes );

    dms.append(degrees_str);
    dms.append(tr("%1 ").arg(Defs::DEGREE));
    dms.append(minutes_str);
    dms.append(QLatin1String("' "));

    if (dec >= 0)
    {
        dms.append(QLatin1String("E"));
    }
    else
    {
        dms.append(QLatin1String("W"));
    }
    return dms;
}

// get variation of declination in decimal minutes / year, i.e. in mmss.sss
// string from signed decimal degrees
// NOTE: not used
QString BasicSettingsPage::strVariation(double dec)
{
    QString dms = QString();

    double degrees;
    modf(dec, &degrees);

    double rest = dec - degrees;
    double min_rest = rest * 60.0;

    double min_d;
    modf(min_rest, &min_d);

    int minutes = static_cast<int>(min_d);

    QString degrees_str;
    QTextStream d(&degrees_str);
    d << qSetFieldWidth(3) << qSetPadChar(QLatin1Char('0')) << right << qAbs( degrees );

    QString minutes_str;
    QTextStream m(&minutes_str);
    m << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << right << qAbs( minutes );

    dms.append(degrees_str);
    dms.append(tr("%1 ").arg(Defs::DEGREE));
    dms.append(minutes_str);
    dms.append(QLatin1String("' "));

    if (dec >= 0)
    {
        dms.append(QLatin1String("E"));
    }
    else
    {
        dms.append(QLatin1String("W"));
    }
    return dms;
}

void BasicSettingsPage::onDeclinationDateLabelClicked()
{
    DEBUG_FUNC_NAME
    declinationDateEdit->setFocus();
    WidgetUtils::showCalendarOf(declinationDateEdit);
}

void BasicSettingsPage::updateDeclinationDate(const QDate &d)
{
    ecProject_->setScreenDecDate(d.toString(Qt::ISODate));
}

void BasicSettingsPage::alignDeclinationDate(const QDate& d)
{
    DEBUG_FUNC_NAME

    if (ecProject_->generalSubset()
        && ecProject_->screenUseGeoNorth())
    {
        auto currentDeclinationDate = declinationDateEdit->date();
        if (currentDeclinationDate != d)
        {
            declinationDateEdit->setDate(d);
            if (ecProject_->screenMagDec() != 0.0)
            {
                fetchMagneticDeclination();
                emit saveSilentlyRequest();
            }
        }
    }
}

void BasicSettingsPage::clearDataSelection()
{
    int ret_code = acceptVariableReset();
    if (ret_code != QMessageBox::Ok) { return; }

    datapathBrowse->clear();
    clearFilesFound();
    subsetCheckBox->setChecked(false);
}

int BasicSettingsPage::handleVariableReset()
{
    DEBUG_FUNC_NAME

    qDebug() << ecProject_->screenDataPath();
    qDebug() << ecProject_->generalColMasterSonic();
    qDebug() << ecProject_->generalColCo2();

    // if not new project
    if (!ecProject_->screenDataPath().isEmpty()
        && !ecProject_->generalColMasterSonic().isEmpty()
        && ecProject_->generalColCo2() != -1)
    {
        qDebug() << "not new project";
        return acceptVariableReset();
    }
    return QMessageBox::Yes;
}

int BasicSettingsPage::acceptVariableReset()
{
    DEBUG_FUNC_NAME

    // keep this value to go through in case of not showing dialog
    int res = QMessageBox::Ok;

    if (ecProject_->generalUseAltMdFile()) return res;

    bool showDialog = GlobalSettings::getAppPersistentSettings(
                            Defs::CONFGROUP_WINDOW,
                            Defs::CONF_WIN_BASIC_SETTINGS_CLEARING_MSG,
                            true).toBool();

    if (showDialog)
    {
        qDebug() << "create dialog";
        InfoMessage runDialog(QDialogButtonBox::Ok
                              | QDialogButtonBox::Cancel, this);
        runDialog.setTitle(tr("Variable and Flag Reset"));
        runDialog.setIcon(QPixmap(QStringLiteral(":/icons/msg-question")));
        runDialog.setType(InfoMessage::Type::SELECTION_CLEANING);
        runDialog.setDoNotShowAgainVisible(true);
        runDialog.setMessage(tr("When changing the raw data directory, "
                                "all current 'Variables' will be replaced "
                                "by new 'Variables' from the new metadata "
                                "inside the GHG files. "
                                "In addition all 'Flags' variables and their "
                                "settings under 'Select Items for Flux "
                                "Computation' will be reset to defaults. "
                                "\n\n"));
        runDialog.refresh();

        res = runDialog.exec();
        qDebug() << "button res" << res;
    }

    if (res == QMessageBox::Ok)
    {
        clearSelectedItems();
        FileUtils::cleanSmfDirRecursively(configState_->general.env);
    }
    return res;
}

void BasicSettingsPage::clearSelectedItems()
{
    crossWindCheckBox->setEnabled(false);
    crossWindCheckBox->setChecked(true);
    clearVarsCombo();
    clearFlagVars();
    clearFlagUnits();
    clearFlagThresholdsAndPolicies();

    ecProject_->setGeneralColMasterSonic(QString());
    ecProject_->setGeneralColCo2(-1);
    ecProject_->setGeneralColH2o(-1);
    ecProject_->setGeneralColCh4(-1);
    ecProject_->setGeneralColGas4(-1);
    ecProject_->setGeneralColIntTc(-1);
    ecProject_->setGeneralColIntT1(-1);
    ecProject_->setGeneralColIntT2(-1);
    ecProject_->setGeneralColIntP(-1);
    ecProject_->setGeneralColAirT(-1);
    ecProject_->setGeneralColAirP(-1);

    ecProject_->setBiomParamColAirT(999);
    ecProject_->setBiomParamColAirP(999);
    ecProject_->setBiomParamColRh(-1);
    ecProject_->setBiomParamColRg(-1);
    ecProject_->setBiomParamColLwin(-1);
    ecProject_->setBiomParamColPpfd(-1);

    ecProject_->setGeneralColGasMw(-1.0);
    ecProject_->setGeneralColGasDiff(-1.0);
    ecProject_->setGeneralColTs(-1);
    ecProject_->setGeneralColDiag72(-1);
    ecProject_->setGeneralColDiag75(-1);
    ecProject_->setGeneralColDiag77(-1);

    ecProject_->setScreenFlag1Col(-1);
    ecProject_->setScreenFlag2Col(-1);
    ecProject_->setScreenFlag3Col(-1);
    ecProject_->setScreenFlag4Col(-1);
    ecProject_->setScreenFlag5Col(-1);
    ecProject_->setScreenFlag6Col(-1);
    ecProject_->setScreenFlag7Col(-1);
    ecProject_->setScreenFlag8Col(-1);
    ecProject_->setScreenFlag9Col(-1);
    ecProject_->setScreenFlag10Col(-1);
}

void BasicSettingsPage::dateRangeDetect()
{
    DEBUG_FUNC_NAME
    qDebug() << "currentRawDataList_.isEmpty():" << currentRawDataList_.isEmpty();

    if (!currentRawDataList_.isEmpty())
    {
        progressWidget_2->startAnimation();

        QPair<QDateTime, QDateTime> dates;

        QFuture<QPair<QDateTime, QDateTime>> future = QtConcurrent::run(&FileUtils::getDateRangeFromFileList, currentRawDataList_, ecProject_->generalFilePrototype());
        while (!future.isFinished())
        {
            QCoreApplication::processEvents();
        }
        dates = future.result();

        progressWidget_2->stopAnimation();

        startDateEdit->setDate(dates.first.date());
        startTimeEdit->setTime(dates.first.time());

        // correct the start/end date accounting for file duration
        if (dlProject_->timestampEnd() == 0)
        {
            dates.second = dates.second.addSecs(dlProject_->fileDuration() * 60);
        }
        else
        {
            dates.first = dates.first.addSecs(-dlProject_->fileDuration() * 60);
        }

        endDateEdit->setDate(dates.second.date());
        endTimeEdit->setTime(dates.second.time());

        emit setDateRangeRequest(dates);
    }
}

void BasicSettingsPage::updateSmartfluxBar()
{
    DEBUG_FUNC_NAME
    qDebug() << configState_->project.smartfluxMode;
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
    setSmartfluxUI(configState_->project.smartfluxMode);
}

void BasicSettingsPage::setSmartfluxUI(bool on)
{
    QWidgetList widgets;
    widgets << avgIntervalLabel
         << avgIntervalSpin
         << previousDatapathLabel
         << outpathLabel
         << outpathBrowse
         << idLabel
         << idEdit
         << previousDatapathBrowse
         << anemRefLabel
         << anemRefCombo
         << anemFlagLabel
         << anemFlagCombo
         << recursionCheckBox
         << subsetCheckBox
         << dateRangeDetectButton;

    foreach (QWidget *w, widgets)
    {
        if (on)
        {
            oldEnabled.push_back(w->isEnabled());
            w->setDisabled(on);
        }
        else
        {
            w->setEnabled(oldEnabled.at(static_cast<unsigned long>(widgets.indexOf(w))));
        }
    }

    if (on)
    {
        recursionCheckBox->setChecked(false);
        subsetCheckBox->setChecked(false);

        outpathBrowse->clear();
        previousDatapathBrowse->clear();

        // set the output id to a fixed string
        idEdit->setText(QStringLiteral("adv"));

        avgIntervalSpin->setValue(30);
    }
}

void BasicSettingsPage::noNoaaConnectionMsg()
{
    WidgetUtils::warning(this,
                         tr("NOAA Connection Problem"),
                         tr("<p>No connection available or connection "
                            "error updating the magnetic declination.</p>"));
}

void BasicSettingsPage::noNoaaDownloadMsg()
{
    bool showDialog
        = GlobalSettings::getAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                                   Defs::CONF_WIN_NOAA_WEBSITE_MSG,
                                                   true).toBool();
    if (!showDialog) { return; }

    // info message
    InfoMessage noaaDialog(QDialogButtonBox::Ok, nullptr);
    noaaDialog.setTitle(tr("NOAA Download Problem"));
    noaaDialog.setType(InfoMessage::Type::NOAA_WEBSITE);
    noaaDialog.setMessage(tr("<p>Server not responding or service not "
                             "available updating the magnetic "
                             "declination.</p>"));
    noaaDialog.refresh();
    noaaDialog.exec();
}
