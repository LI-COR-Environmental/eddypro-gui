/***************************************************************************
  advspectraloptions.cpp
  -------------------
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

#include "advspectraloptions.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTimer>
#include <QTimeEdit>
#include <QUrl>

#include "ancillaryfiletest.h"
#include "clicklabel.h"
#include "customcombomodel.h"
#include "customclearlineedit.h"
#include "dbghelper.h"
#include "dirbrowsewidget.h"
#include "dlproject.h"
#include "ecproject.h"
#include "fileutils.h"
#include "filebrowsewidget.h"
#include "globalsettings.h"
#include "widget_utils.h"

AdvSpectralOptions::AdvSpectralOptions(QWidget *parent,
                                       DlProject *dlProject,
                                       EcProject *ecProject,
                                       ConfigState *config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config)
{
    settingsGroupTitle_1 = new QLabel(tr("Spectra and Cospetra Calculation"));
    settingsGroupTitle_1->setProperty("groupTitle2", true);
    settingsGroupTitle_1->setStyleSheet(
            QStringLiteral("QLabel { margin: 2px 0px 3px -2px; padding: 0px; }"));

    // spectra and cospectra section
    binnedSpectraNonExistingRadio = new QRadioButton(tr("Binned (co)spectra files not available"));
    binnedSpectraNonExistingRadio->setToolTip(tr("<b>Binned (co)spectra files not available:</b> Select this option if you did not yet obtain <i>Binned spectra and cospectra files</i> for the current dataset in a previous run of EddyPro. Note that such binned (co)spectra files do not need to correspond exactly to the current dataset, rather they need to be representative of it. Binned (co)spectra files are used by certain spectral corrections procedures to quantify spectral attenuations, thus they must have been collected in conditions comparable to those of the current dataset (e.g., same EC system and similar canopy heights, measurement height, instrument spatial separations, etc.). At least one month of spectra files is needed for a robust spectral attenuation assessment. If you select this option, the option <i>All binned spectra and cospectra</i> in the Output Files page will be automatically selected."));
    binnedSpectraNonExistingRadio->setStyleSheet(QStringLiteral("QRadioButton { margin-left: 5px; }"));

    binnedSpectraExistingRadio = new QRadioButton(tr("Binned (co)spectra files available for this dataset :"));
    binnedSpectraExistingRadio->setToolTip(tr("<b>Binned (co)spectra files available:</b> Select this option if you already obtained <i>Binned spectra and cospectra files</i> for the current dataset in a previous run of EddyPro. Note that the binned (co)spectra files do not need to correspond exactly to the current dataset, rather they need to be representative of it. Binned (co)spectra are used here for quantification of spectral attenuations, thus they must have been collected in conditions comparable to those of the current dataset (e.g., the same EC system and similar canopy heights, measurement height, instrument spatial separations). At least one month of spectra files is needed for a robust spectral attenuation assessment. If you select this option, the option <i>All binned spectra and cospectra</i> in the Output Files page will be automatically deselected and activated."));
    binnedSpectraExistingRadio->setStyleSheet(QStringLiteral("QRadioButton { margin-right: 0px; }"));

    binnedSpectraDirBrowse = new DirBrowseWidget;
    binnedSpectraDirBrowse->setToolTip(tr("<b>Browse:</b> Specify the folder that contains the binned (co)spectra files."));
    binnedSpectraDirBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());
    binnedSpectraDirBrowse->setDialogTitle(tr("Select the Binned (Co)Spectra Files Directory"));

    binnedSpectraRadioGroup = new QButtonGroup(this);
    binnedSpectraRadioGroup->addButton(binnedSpectraNonExistingRadio, 0);
    binnedSpectraRadioGroup->addButton(binnedSpectraExistingRadio, 1);

    subsetCheckBox = new QCheckBox;
    subsetCheckBox->setText(tr("Select a different period"));
    subsetCheckBox->setToolTip(tr("<b>Select a different period:</b> Select the starting and ending date of the period you want the use (co)spectra from. If you selected the option <i>Binned (co)spectra files not available</i>, then this subperiod must overlap, at least partially, with that covered by available raw data or with the subperiod selected in the Basic Settings page, if one was selected. If you selected the option Binned (co)spectra files available for this dataset, then this subperiod must overlap, at least partially, with that covered by available (co)spectra files."));
    subsetCheckBox->setStyleSheet(QStringLiteral("QCheckBox {margin-left: 40px;}"));

    lockedIcon = new QLabel;
    auto pixmap_2x = QPixmap(QStringLiteral(":/icons/vlink-locked"));
#if defined(Q_OS_MAC)
    pixmap_2x.setDevicePixelRatio(2.0);
#endif
    lockedIcon->setPixmap(pixmap_2x);

    startDateLabel = new ClickLabel;
    startDateLabel->setText(tr("Start :"));
    startDateLabel->setToolTip(tr("<b>Start:</b> Beginning of the period to use (co)spectra from. If (co)spectra will be used in spectral corrections, we recommend using a time period that is as long as possible. However, make sure that the instrument setup (sampling line, instrument separations) did not undergo any major change during the selected time period."));
    startDateEdit = new QDateEdit;
    startDateEdit->setToolTip(startDateLabel->toolTip());
    startDateEdit->setCalendarPopup(true);
    WidgetUtils::customizeCalendar(startDateEdit->calendarWidget());

    startTimeEdit = new QTimeEdit;
    startTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    startTimeEdit->setAccelerated(true);

    endDateLabel = new ClickLabel;
    endDateLabel->setText(tr("End :"));
    endDateLabel->setToolTip(tr("<b>End:</b> End of the period to use (co)spectra from. If (co)spectra will be used in spectral corrections, we recommend using a time period that is as long as possible. However, make sure that the instrument setup (sampling line, instrument separations) did not undergo any major change during the selected time period."));
    endDateEdit = new QDateEdit;
    endDateEdit->setToolTip(endDateLabel->toolTip());
    endDateEdit->setCalendarPopup(true);
    WidgetUtils::customizeCalendar(endDateEdit->calendarWidget());

    endTimeEdit = new QTimeEdit;
    endTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    endTimeEdit->setAccelerated(true);

    auto dateTimeContainer = new QGridLayout;
    dateTimeContainer->addWidget(startDateEdit, 0, 1);
    dateTimeContainer->addWidget(startTimeEdit, 0, 2);
    dateTimeContainer->addWidget(lockedIcon, 0, 0, 2, 1);
    dateTimeContainer->addWidget(endDateEdit, 1, 1);
    dateTimeContainer->addWidget(endTimeEdit, 1, 2);
    dateTimeContainer->setColumnStretch(1, 1);
    dateTimeContainer->setColumnStretch(2, 1);
    dateTimeContainer->setColumnStretch(3, 2);
    dateTimeContainer->setContentsMargins(0, 0, 0, 0);

    // FFT section
    filterLabel = new ClickLabel(tr("Tapering window :"));
    filterLabel->setToolTip(tr("<b>Tapering window:</b> Select the shape of the window used to taper the time series before the Fast Fourier Transform. The tapering procedure is a sample-wise multiplication in the time domain between the time series and the window, performed to reduce the discontinuities of the time series at the boundaries and avoid spectral energy overestimation. Kaimal & Kristensen (1991) suggested the Hamming window."));
    filterCombo = new QComboBox;
    filterCombo->setToolTip(filterLabel->toolTip());
    filterCombo->addItem(tr("Squared (no window)"));
    filterCombo->addItem(tr("Bartlett"));
    filterCombo->addItem(tr("Welch"));
    filterCombo->addItem(tr("Hamming"));
    filterCombo->addItem(tr("Hann"));

    nBinsLabel = new ClickLabel(tr("Frequency bins for (co)spectra reduction :"));
    nBinsLabel->setToolTip(tr("<b>Frequency bins for spectra and cospectra reduction:</b> Select the number of exponentially-spaced frequency bins to reduce spectra and cospectra. All spectral samples falling in a given bin are averaged, so that smoother curves result, greatly reduced in length. In EddyPro binned (co)spectra are used for in-situ spectral assessments and for calculation of ensemble averaged (co)spectra."));
    nBinsSpin = new QSpinBox;
    nBinsSpin->setToolTip(nBinsLabel->toolTip());
    nBinsSpin->setRange(10, 3000);
    nBinsSpin->setSingleStep(10);
    nBinsSpin->setAccelerated(true);
    nBinsSpin->setValue(50);

    fftCheckBox = new QCheckBox(tr("Use power-of-two samples to speed up the FFT"));
    fftCheckBox->setToolTip(tr("<b>Use power-of-two samples to speed up the FFT: </b>Check this box to instruct EddyPro to use a number of samples equal to the power-of-two closest to the currently available samples, for calculating spectra. This option greatly speeds up the FFT procedure and is therefore recommended."));
    fftCheckBox->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    spin31Label = new QLabel(tr("%1").arg(Defs::CO2_STRING));
    spin31Label->setProperty("blueLabel", true);
    spin32Label = new QLabel(tr("%2").arg(Defs::H2O_STRING));
    spin32Label->setProperty("blueLabel", true);
    spin33Label = new QLabel(tr("%3").arg(Defs::CH4_STRING));
    spin33Label->setProperty("blueLabel", true);
    spin34Label = new QLabel(tr("%4 Gas").arg(Defs::GAS4_STRING));
    spin34Label->setProperty("blueLabel", true);

    spin30Label = new ClickLabel(tr("Lowest noise frequency :"));
    spin30Label->setToolTip(tr("<b>Lowest noise frequency:</b> High-frequency noise (blue noise) can compromise the spectral assessment by modifying the shape of spectra. EddyPro has an option to eliminate such noise. Set the minimum frequency at which you expect the noise to start being relevant. EddyPro will linearly (in a log-log sense) interpolate the high frequency portion of the spectra and subtract it from the spectra before calculating transfer functions. Set 0 Hz to instruct EddyPro to not perform noise elimination. In this case the string <i>Do not remove noise</i> will appear in this field."));
    spin31 = new QDoubleSpinBox;
    spin31->setRange(0.0, 50.0);
    spin31->setSingleStep(0.1);
    spin31->setDecimals(4);
    spin31->setSuffix(QStringLiteral(" [Hz]"));
    spin31->setAccelerated(true);
    spin31->setSpecialValueText(tr("Do not remove noise"));
    spin32 = new QDoubleSpinBox;
    spin32->setRange(0.0, 50.0);
    spin32->setSingleStep(0.1);
    spin32->setDecimals(4);
    spin32->setSuffix(QStringLiteral(" [Hz]"));
    spin32->setAccelerated(true);
    spin32->setSpecialValueText(tr("Do not remove noise"));
    spin33 = new QDoubleSpinBox;
    spin33->setRange(0.0, 50.0);
    spin33->setSingleStep(0.1);
    spin33->setDecimals(4);
    spin33->setSuffix(QStringLiteral(" [Hz]"));
    spin33->setAccelerated(true);
    spin33->setSpecialValueText(tr("Do not remove noise"));
    spin34 = new QDoubleSpinBox;
    spin34->setRange(0.0, 50.0);
    spin34->setSingleStep(0.1);
    spin34->setDecimals(4);
    spin34->setSuffix(QStringLiteral(" [Hz]"));
    spin34->setAccelerated(true);
    spin34->setSpecialValueText(tr("Do not remove noise"));

    // QA/QC section
    auto spectraQATitle = new QLabel(tr("Spectra and Cospectra QA/QC"));
    spectraQATitle->setProperty("groupTitle2", true);

    minUnstableLabel = new ClickLabel(tr("Minimum, unstable :"));
    minUnstableLabel->setToolTip(tr("<b>Minimum, unstable flux:</b> when fluxes are below these minima, corresponding cospectra are excluded from calculation of ensemble averaged cospectra in unstable stratifications, and corresponding spectra are excluded from calculation ensemble averaged spectra. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));

    minStableLabel = new ClickLabel(tr("Minimum, stable :"));
    minStableLabel->setToolTip(tr("<b>Minimum, stable flux:</b> when fluxes are below these minima, corresponding cospectra are excluded from calculation of ensemble averaged cospectra in stable stratifications. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));

    maxLabel = new ClickLabel(tr("Maximum :"));
    maxLabel->setToolTip(tr("<b>Maximum :</b> when fluxes are above these maxima, corresponding (co)spectra are excluded from any ensemble averaging procedure. Maxima are meant to exclude spikes or periods characterized by abnormal fluxes. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));

    frictionVelocityLabel = new ClickLabel(tr("Friction velocity"));
    latentHeatLabel = new ClickLabel(tr("Latent heat flux"));
    sensibleHeatLabel = new ClickLabel(tr("Sensible heat flux"));
    qcCo2Label = new ClickLabel(tr("%1 flux").arg(Defs::CO2_STRING));
    qcCh4Label = new ClickLabel(tr("%1 flux").arg(Defs::CH4_STRING));
    qcGas4Label = new ClickLabel(tr("%1 gas flux").arg(Defs::GAS4_STRING));

    qcMinUnstableUstarSpin = new QDoubleSpinBox;
    qcMinUnstableUstarSpin->setRange(0.0, 5.0);
    qcMinUnstableUstarSpin->setSingleStep(0.05);
    qcMinUnstableUstarSpin->setDecimals(4);
    qcMinUnstableUstarSpin->setSuffix(QStringLiteral(" [m/s]"));
    qcMinUnstableUstarSpin->setAccelerated(true);

    qcMinUnstableHSpin = new QDoubleSpinBox;
    qcMinUnstableHSpin->setRange(0.0, 10000.0);
    qcMinUnstableHSpin->setSingleStep(10.0);
    qcMinUnstableHSpin->setDecimals(4);
    qcMinUnstableHSpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMinUnstableHSpin->setAccelerated(true);

    qcMinUnstableLESpin = new QDoubleSpinBox;
    qcMinUnstableLESpin->setRange(0.0, 10000.0);
    qcMinUnstableLESpin->setSingleStep(10.0);
    qcMinUnstableLESpin->setDecimals(4);
    qcMinUnstableLESpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMinUnstableLESpin->setAccelerated(true);

    qcMinUnstableCo2Spin = new QDoubleSpinBox;
    qcMinUnstableCo2Spin->setRange(0.0, 5000.0);
    qcMinUnstableCo2Spin->setSingleStep(1.0);
    qcMinUnstableCo2Spin->setDecimals(4);
    qcMinUnstableCo2Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinUnstableCo2Spin->setAccelerated(true);

    qcMinUnstableCh4Spin = new QDoubleSpinBox;
    qcMinUnstableCh4Spin->setRange(0.0, 5000.0);
    qcMinUnstableCh4Spin->setSingleStep(0.1);
    qcMinUnstableCh4Spin->setDecimals(4);
    qcMinUnstableCh4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinUnstableCh4Spin->setAccelerated(true);

    qcMinUnstableGas4Spin = new QDoubleSpinBox;
    qcMinUnstableGas4Spin->setRange(0.0, 5000.0);
    qcMinUnstableGas4Spin->setSingleStep(0.1);
    qcMinUnstableGas4Spin->setDecimals(4);
    qcMinUnstableGas4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinUnstableGas4Spin->setAccelerated(true);

    qcMinStableUstarSpin = new QDoubleSpinBox;
    qcMinStableUstarSpin->setRange(0.0, 5.0);
    qcMinStableUstarSpin->setSingleStep(0.05);
    qcMinStableUstarSpin->setDecimals(4);
    qcMinStableUstarSpin->setSuffix(QStringLiteral(" [m/s]"));
    qcMinStableUstarSpin->setAccelerated(true);

    qcMinStableHSpin = new QDoubleSpinBox;
    qcMinStableHSpin->setRange(0.0, 10000.0);
    qcMinStableHSpin->setSingleStep(10.0);
    qcMinStableHSpin->setDecimals(4);
    qcMinStableHSpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMinStableHSpin->setAccelerated(true);

    qcMinStableLESpin = new QDoubleSpinBox;
    qcMinStableLESpin->setRange(0.0, 10000.0);
    qcMinStableLESpin->setSingleStep(10.0);
    qcMinStableLESpin->setDecimals(4);
    qcMinStableLESpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMinStableLESpin->setAccelerated(true);

    qcMinStableCo2Spin = new QDoubleSpinBox;
    qcMinStableCo2Spin->setRange(0.0, 5000.0);
    qcMinStableCo2Spin->setSingleStep(1.0);
    qcMinStableCo2Spin->setDecimals(4);
    qcMinStableCo2Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinStableCo2Spin->setAccelerated(true);

    qcMinStableCh4Spin = new QDoubleSpinBox;
    qcMinStableCh4Spin->setRange(0.0, 5000.0);
    qcMinStableCh4Spin->setSingleStep(0.1);
    qcMinStableCh4Spin->setDecimals(4);
    qcMinStableCh4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinStableCh4Spin->setAccelerated(true);

    qcMinStableGas4Spin = new QDoubleSpinBox;
    qcMinStableGas4Spin->setRange(0.0, 5000.0);
    qcMinStableGas4Spin->setSingleStep(0.1);
    qcMinStableGas4Spin->setDecimals(4);
    qcMinStableGas4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMinStableGas4Spin->setAccelerated(true);

    qcMaxUstarSpin = new QDoubleSpinBox;
    qcMaxUstarSpin->setRange(0.0, 5.0);
    qcMaxUstarSpin->setSingleStep(0.1);
    qcMaxUstarSpin->setDecimals(4);
    qcMaxUstarSpin->setSuffix(QStringLiteral(" [m/s]"));
    qcMaxUstarSpin->setAccelerated(true);

    qcMaxHSpin = new QDoubleSpinBox;
    qcMaxHSpin->setRange(0.0, 10000.0);
    qcMaxHSpin->setSingleStep(100.0);
    qcMaxHSpin->setDecimals(4);
    qcMaxHSpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMaxHSpin->setAccelerated(true);

    qcMaxLESpin = new QDoubleSpinBox;
    qcMaxLESpin->setRange(0.0, 10000.0);
    qcMaxLESpin->setSingleStep(100.0);
    qcMaxLESpin->setDecimals(4);
    qcMaxLESpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    qcMaxLESpin->setAccelerated(true);

    qcMaxCo2Spin = new QDoubleSpinBox;
    qcMaxCo2Spin->setRange(0.0, 5000.0);
    qcMaxCo2Spin->setSingleStep(10.0);
    qcMaxCo2Spin->setDecimals(4);
    qcMaxCo2Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMaxCo2Spin->setAccelerated(true);

    qcMaxCh4Spin = new QDoubleSpinBox;
    qcMaxCh4Spin->setRange(0.0, 5000.0);
    qcMaxCh4Spin->setSingleStep(10.0);
    qcMaxCh4Spin->setDecimals(4);
    qcMaxCh4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMaxCh4Spin->setAccelerated(true);

    qcMaxGas4Spin = new QDoubleSpinBox;
    qcMaxGas4Spin->setRange(0.0, 5000.0);
    qcMaxGas4Spin->setSingleStep(10.0);
    qcMaxGas4Spin->setDecimals(4);
    qcMaxGas4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    qcMaxGas4Spin->setAccelerated(true);

    vmFlagsCheckBox = new QCheckBox(tr("Filter (co)spectra according "
                                       "to Vickers and Mahrt (1997) "
                                       "test results  "));
    vmFlagsCheckBox->setToolTip(tr("<b>Filter (co)spectra according"
                                   "to Vickers and Mahrt (1997) "
                                   "test results:</b> check this option to exclude from ensemble averages (co)spectra for periods, during which the corresponding time series were flagged by the statistical tests found in the Statistical Analysis page. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));
    vmFlagsCheckBox->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    // filter cospectra mauder section
    auto filterCospectraMauderTitle
        = new QLabel(tr("Filter (co)spectra "
                        "according to micrometeorological quality test results "
                        "(Mauder and Foken, 2004)"));
    filterCospectraMauderTitle->setProperty("groupLabel", true);

    lowQualityCheckBox = new QCheckBox(tr("Low data quality (flag value = 2)"));
    lowQualityCheckBox->setToolTip(tr("<b>Low data quality:</b> "
                                      "check this option to exclude from ensemble averages (co)spectra for periods, during which the corresponding fluxes where flagged for low quality by Foken’s micrometeorological tests. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));
    lowQualityCheckBox->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    moderateQualityCheckBox = new QCheckBox(tr("Moderate data quality "
                                               "(flag value = 1)"));
    moderateQualityCheckBox->setToolTip(tr("<b>Moderate data quality:</b> "
                                           "check this option to exclude from ensemble averages (co)spectra for periods, during which the corresponding fluxes where flagged for moderate quality by Foken’s micrometeorological tests. Note that choosing these options, forces the pair option ‘low data quality’ to be automatically selected. For more details, click on the question mark at the right side of the title of this section <i>Spectra and cospectra QA/QC</i>."));
    moderateQualityCheckBox->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    // Spectral corrections title
    auto spectralCorrectionTitle = new QLabel(tr("Spectral Correction Options"));
    spectralCorrectionTitle->setProperty("groupTitle2", true);

    // low freq section
    lowFreqTitle = new QLabel(tr("Low frequency range"));
    lowFreqTitle->setProperty("groupLabel", true);

    lfMethodCheck = new QCheckBox(tr("Analytic correction of high-pass filtering effects (Moncrieff et al. 2004)   "));
    lfMethodCheck->setToolTip(tr("<b>Analytic correction of high-pass filtering effects:</b> Check this option to apply a low frequency spectral correction, to compensate flux losses due to finite averaging length and detrending. The method is adapted to the selected fluctuation computation procedure and its time constant as applicable."));
    lfMethodCheck->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    // high freq section
    highFreqTitle = new QLabel(tr("High frequency range"));
    highFreqTitle->setProperty("groupLabel", true);

    hfMethodCheck = new QCheckBox(tr("Correction of low-pass filtering effects"));
    hfMethodCheck->setToolTip(tr("<b>Correction of low-pass filtering effects:</b> Check this option to apply a high frequency spectral correction, to compensate flux losses due to finite sensors separation, signal attenuation, path averaging, time response, etc. Select the most appropriate method according to your EC setup."));
    hfMethodCheck->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    hfMethLabel = new ClickLabel(tr("Method :"));
    hfMethCombo = new QComboBox;
    hfMethCombo->setModel(new CustomComboModel(hfMethCombo));
    hfMethCombo->addItem(tr("Moncrieff et al. (1997) - Fully analytic"));
    hfMethCombo->addItem(tr("Massmann (2000, 2001) - Fully analytic"));
    hfMethCombo->addItem(tr("Horst (1997) - Analytic with in situ parameterization"));
    hfMethCombo->addItem(tr("Ibrom et al. (2007) - In situ/analytic"));
    hfMethCombo->addItem(tr("Fratini et al. (2012) - In situ/analytic"));
    hfMethCombo->setItemData(0, tr("<b>Moncrieff et al. (1997):</b> This method models all major sources of flux attenuation by means of a mathematical formulation. The use of this method is suggested for open path EC systems or for closed path systems if the sampling line is short and heated. This method may seriously underestimate the attenuation (and hence the correction) - notably for water vapor - when the sampling line is long and/or not heated, because of the dependency of attenuation of H<sub>2</sub>O on relative humidity."), Qt::ToolTipRole);
    hfMethCombo->setItemData(1, tr("<b>Massmann (2000, 2001):</b> This method provides a simple analytical expression for the spectral correction factors. The use of this method is suggested for open path EC systems or for closed path systems if the sampling line is short and heated. This method may seriously underestimate the attenuation (and hence the correction) for water vapor, when the sampling line is long and/or not heated, because of the dependency of attenuation of %2 on relative humidity. For closed path systems, this method is only applicable for %1, %2, %3, %4 and %5 fluxes.").arg(Defs::CO2_STRING).arg(Defs::H2O_STRING).arg(Defs::CH4_STRING).arg(Defs::N2O_STRING).arg(Defs::O3_STRING), Qt::ToolTipRole);
    hfMethCombo->setItemData(3, tr("<b>Horst (1997):</b> Correction method based on an analytical formulation of the spectral correction factor that requires an in-situ assessment of the system's cut-off frequency. Provide the information below to specify how to perform such assessment."), Qt::ToolTipRole);
    hfMethCombo->setItemData(4, tr("<b>Ibrom et al. (2007):</b> Correction method based on an analytical formulation of the spectra correction factors, that requires an in-situ assessment of the system's cut-off frequencies, separately for each instrument and gas, and as a function of relative humidity for water vapor. Provide the settings in the <i>Assessment of high-frequency attenuation</i> to specify how to perform the assessment. This method is recommended in most cases, notably for closed-path systems placed high over rough canopies."), Qt::ToolTipRole);
    hfMethCombo->setItemData(5, tr("<b>Fratini et al. (2012):</b> Correction method based on the combination of a direct approach (similar to Hollinger et al., 2009) and the analytical formulation of Ibrom et al., 2007. It requires an in-situ assessment of the system's cut-off frequencies, separately for each instrument and gas, and as a function of relative humidity for water vapor. It also requires full length cospectra of measured sensible heat. This method is recommendable in most cases, notably for closed-path systems placed low over smooth surfaces."), Qt::ToolTipRole);

    horstCheck = new QCheckBox(tr("Correction for instruments separation"));
    horstCheck->setToolTip(tr("<b>Correction for instrument separation:</b> Check this option and select the corresponding method to add an extra correction term to that calculated with the method by Ibrom et al. (2007). This accounts for any separation between the inlet of the sampling line (closed path instruments) or the center of the open path instrument and the center of the anemometer."));
    horstCheck->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));
    horstMethodLabel = new ClickLabel(tr("Method :"));
    horstCombo = new QComboBox;
    horstCombo->addItem(tr("Horst and Lenschow (2009), along-wind, crosswind and vertical"));
    horstCombo->addItem(tr("Horst and Lenschow (2009), only crosswind and vertical"));
    horstCombo->setItemData(0, tr("<b>Horst and Lenschow (2009), along-wind, crosswind and vertical:</b> Select this option to account for sensor separations in any direction. Note that correcting for along-wind separations may result in overcorrection, if any time lag compensation method was also selected."), Qt::ToolTipRole);
    horstCombo->setItemData(1, tr("<b>Horst and Lenschow (2009), only crosswind and vertical:</b> Select this option to account for sensor separations only in the crosswind and vertical directions. Recommended when a time lag compensation method is selected."), Qt::ToolTipRole);

    spectraExistingRadio = new QRadioButton(tr("Spectral assessment file available for this dataset :"));
    spectraExistingRadio->setToolTip(tr("<b>Spectral assessment file available:</b> If you have a spectral assessment file from a previous run, and it applies to the current dataset, you can use the same file to by providing the path to the file named \"eddypro_spectral_assessment_ID.txt\". This file includes the results of the assessment. It can be used to shorten program execution time and assure full comparability between previous and current results."));

    spectraNonExistingRadio = new QRadioButton(tr("Spectral assessment file not available"));
    spectraNonExistingRadio->setToolTip(tr("<b>Spectral assessment file not available:</b> Choose this option and provide the following information if you need to calculate cut-off frequencies for your system. The assessment will be performed as an intermediate step, after all binned (co)spectra for the current dataset are calculated and before calculating and correcting fluxes."));

    spectraFileBrowse = new FileBrowseWidget;
    spectraFileBrowse->setToolTip(tr("<b>Load:</b> Load an existing spectral assessment file"));
    spectraFileBrowse->setDialogTitle(tr("Select the Spectral Assessment File"));
    spectraFileBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());
    spectraFileBrowse->setDialogFilter(tr("All Files (*.*)"));

    spectraRadioGroup = new QButtonGroup(this);
    spectraRadioGroup->addButton(spectraExistingRadio, 0);
    spectraRadioGroup->addButton(spectraNonExistingRadio, 1);


    freqAttenuationTitle = new QLabel(tr("Assessment of high-frequency attenuation"));
    freqAttenuationTitle->setProperty("groupLabel", true);

    spin11Label = new QLabel(tr("%1").arg(Defs::CO2_STRING));
    spin11Label->setProperty("blueLabel", true);
    spin12Label = new QLabel(tr("%2").arg(Defs::H2O_STRING));
    spin12Label->setProperty("blueLabel", true);
    spin13Label = new QLabel(tr("%3").arg(Defs::CH4_STRING));
    spin13Label->setProperty("blueLabel", true);
    spin14Label = new QLabel(tr("%4 Gas").arg(Defs::GAS4_STRING));
    spin14Label->setProperty("blueLabel", true);

    minMaxFreqLabel = new QLabel(tr("Frequency range for fitting in-situ "
                                            "transfer functions (based on "
                                            "temperature and concentrations spectra)"));
    minMaxFreqLabel->setProperty("blueLabel", true);

    noiseFreqLabel = new QLabel(tr("Removal of high frequency noise"));
    noiseFreqLabel->setProperty("blueLabel", true);

    spin10Label = new ClickLabel(tr("Lowest frequency :"));
    spin10Label->setToolTip(tr("<b>Lowest frequency:</b> The assessment of the system transfer function implies the frequency-wise ratio of gas concentration to temperature spectra (temperature considered as proxy for un-attenuated atmospheric scalar spectra). This ratio must be taken in the frequency range where the system filtering is expected to occur. At lower frequencies, slow-paced atmospheric and source/sink dynamics may imply a breakdown of the similarity assumption. Default values can be good in most occasions, but the lower frequency should be adapted based mostly on the averaging interval."));

    spin20Label = new ClickLabel(tr("Highest frequency :"));
    spin20Label->setToolTip(tr("<b>Highest frequency:</b> The assessment of the system transfer function implies the frequency-wise ratio of gas concentration to temperature spectra (temperature being considered as a proxy for un-attenuated atmospheric scalar spectra). This ratio must be taken in the frequency range where the system filtering is expected to occur. At higher frequencies, noise and aliasing may corrupt the procedure. Default values can be good in most occasions, but the higher frequency should be adapted based on acquisition frequency and instrument performance."));

    spin11 = new QDoubleSpinBox;
    spin11->setRange(0.0, 50.0);
    spin11->setSingleStep(0.1);
    spin11->setDecimals(4);
    spin11->setSuffix(QStringLiteral(" [Hz]"));
    spin11->setAccelerated(true);
    spin12 = new QDoubleSpinBox;
    spin12->setRange(0.0, 50.0);
    spin12->setSingleStep(0.1);
    spin12->setDecimals(4);
    spin12->setSuffix(QStringLiteral(" [Hz]"));
    spin12->setAccelerated(true);
    spin13 = new QDoubleSpinBox;
    spin13->setRange(0.0, 50.0);
    spin13->setSingleStep(0.1);
    spin13->setDecimals(4);
    spin13->setSuffix(QStringLiteral(" [Hz]"));
    spin13->setAccelerated(true);
    spin14 = new QDoubleSpinBox;
    spin14->setRange(0.0, 50.0);
    spin14->setSingleStep(0.1);
    spin14->setDecimals(4);
    spin14->setSuffix(QStringLiteral(" [Hz]"));
    spin14->setAccelerated(true);
    spin21 = new QDoubleSpinBox;
    spin21->setRange(0.0, 50.0);
    spin21->setSingleStep(0.1);
    spin21->setDecimals(4);
    spin21->setSuffix(QStringLiteral(" [Hz]"));
    spin21->setAccelerated(true);
    spin22 = new QDoubleSpinBox;
    spin22->setRange(0.0, 50.0);
    spin22->setSingleStep(0.1);
    spin22->setDecimals(4);
    spin22->setSuffix(QStringLiteral(" [Hz]"));
    spin22->setAccelerated(true);
    spin23 = new QDoubleSpinBox;
    spin23->setRange(0.0, 50.0);
    spin23->setSingleStep(0.1);
    spin23->setDecimals(4);
    spin23->setSuffix(QStringLiteral(" [Hz]"));
    spin23->setAccelerated(true);
    spin24 = new QDoubleSpinBox;
    spin24->setRange(0.0, 50.0);
    spin24->setSingleStep(0.1);
    spin24->setDecimals(4);
    spin24->setSuffix(QStringLiteral(" [Hz]"));
    spin24->setAccelerated(true);

    minSmplLabel = new ClickLabel(tr("Minimum number of (co)spectra for valid averages :"));
    minSmplLabel->setToolTip(tr("<b>Minimum number of spectra for valid averages:</b> Select the minimum number of spectra that should be found in each class, for the corresponding ensemble average to be valid. Currently classes are defined only for H<sub>2</sub>O with respect to ambient relative humidity: 9 classes are defined between RH = 5% and RH = 95%. We expect to add classes also for passive gases, related to time periods. Entering a number that is too high may imply that, for certain classes, average spectra cannot be calculated. A number that is too small may result in poor characterization of average spectra. The higher this number, the longer the time period needed."));
    minSmplSpin = new QSpinBox;
    minSmplSpin->setRange(1, 1000);
    minSmplSpin->setSingleStep(1);
    minSmplSpin->setAccelerated(true);
    minSmplSpin->setToolTip(minSmplLabel->toolTip());

    // fratini section
    fratiniTitle = new QLabel(tr("Fratini et al. (2012) method settings"));
    fratiniTitle->setProperty("groupLabel", true);

    fullSpectraExistingRadio = new QRadioButton(tr("Full w/Ts cospectra files available for this dataset :"));
    fullSpectraExistingRadio->setToolTip(tr("<b>Full w/Ts cospectra files available:</b> Select this option if you already obtained <i>Full cospectra of w/T<sub>s</sub></i> for the current dataset (from a previous run of EddyPro). Note that the cospectra files need to correspond exactly to the current dataset. Full cospectra of w/T<sub>s</sub> (sensible heat) are used for definition of the spectral correction factor for each flux with the method by Fratini et al. (2012). If you select this option, the option <i>Full length cospectra w/T<sub>s</sub></i> in the Output Files page will be automatically deselected and activated."));

    fullSpectraNonExistingRadio = new QRadioButton(tr("Full w/Ts cospectra files not available"));
    fullSpectraNonExistingRadio->setToolTip(tr("<b>Full w/T<sub>s</sub> cospectra files not available:</b> Select this option if you do not have <i>Full cospectra of w/T<sub>s</sub></i> for the current dataset (from a previous run of EddyPro). Note that existing cospectra files need to correspond exactly to the current dataset. Full cospectra of w/T<sub>s</sub> (sensible heat) are used for definition of the spectral correction factor for each flux with the method by Fratini et al. (2012). If you select this option, the option <i>Full length cospectra w/T<sub>s</sub></i> in the Output Files page will be automatically selected and deactivated."));

    fullSpectraDirBrowse = new DirBrowseWidget;
    fullSpectraDirBrowse->setToolTip(tr("<b>Browse:</b> Specify the folder that contains the full w/T<sub>s</sub> cospectra files."));
    fullSpectraDirBrowse->setDialogTitle(tr("Select the Full Spectra Files Directory"));
    fullSpectraDirBrowse->setDialogWorkingDir(WidgetUtils::getSearchPathHint());

    fullSpectraRadioGroup = new QButtonGroup(this);
    fullSpectraRadioGroup->addButton(fullSpectraNonExistingRadio, 0);
    fullSpectraRadioGroup->addButton(fullSpectraExistingRadio, 1);

    addSonicCheck = new QCheckBox(tr("Include anemometer losses for path averaging and time response"));
    addSonicCheck->setToolTip(tr("<b>Include anemometer losses for path averaging and time response:</b> Select this option to instruct EddyPro to correct sensible heat cospectra for those losses, before using them as a model to calculate correction factors according to Fratini et al. (2012)."));
    addSonicCheck->setStyleSheet(QStringLiteral("QCheckBox { margin-left: 40px; }"));

    // horizontal rules
    auto hrLabel_0 = new QLabel;
    hrLabel_0->setObjectName(QStringLiteral("hrLabel"));
    auto hrLabel_1 = new QLabel;
    hrLabel_1->setObjectName(QStringLiteral("hrLabel"));
    auto hrLabel_2 = new QLabel;
    hrLabel_2->setObjectName(QStringLiteral("hrLabel"));
    hrLabel_2->setStyleSheet(QStringLiteral("QLabel {margin: 0px;}"));
    auto hrLabel_3 = new QLabel;
    hrLabel_3->setObjectName(QStringLiteral("hrLabel"));
    hrLabel_3->setStyleSheet(QStringLiteral("QLabel {margin: 0px;}"));

    // question marks
    createQuestionMarks();
    auto spectraQaQcLabel = new QHBoxLayout;
    spectraQaQcLabel->addWidget(spectraQATitle);
    spectraQaQcLabel->addWidget(questionMark_1, 0, Qt::AlignRight | Qt::AlignVCenter);
    spectraQaQcLabel->addStretch();
    auto settingsGroup1Label = new QHBoxLayout;
    settingsGroup1Label->addWidget(settingsGroupTitle_1);
    settingsGroup1Label->addWidget(questionMark_11, 0, Qt::AlignRight | Qt::AlignVCenter);
    settingsGroup1Label->addStretch();
    auto lowFreqLabel = new QHBoxLayout;
    lowFreqLabel->addWidget(lowFreqTitle);
    lowFreqLabel->addWidget(questionMark_22, 0, Qt::AlignLeft | Qt::AlignVCenter);
    lowFreqLabel->addStretch();
    auto highFreqLabel = new QHBoxLayout;
    highFreqLabel->addWidget(highFreqTitle);
    highFreqLabel->addWidget(questionMark_33, 0, Qt::AlignLeft | Qt::AlignVCenter);
    highFreqLabel->addStretch();
    auto assessmentHighFreqLabel= new QHBoxLayout;
    assessmentHighFreqLabel->addWidget(freqAttenuationTitle);
    assessmentHighFreqLabel->addWidget(questionMark_44, 0, Qt::AlignLeft | Qt::AlignVCenter);
    assessmentHighFreqLabel->addStretch();

    auto settingsLayout = new QGridLayout;
    settingsLayout->addLayout(settingsGroup1Label, 0, 0, 1, -1);
    settingsLayout->addWidget(binnedSpectraNonExistingRadio, 1, 0);
    settingsLayout->addWidget(binnedSpectraExistingRadio, 2, 0, Qt::AlignRight);
    settingsLayout->addWidget(binnedSpectraDirBrowse, 2, 1, 1, 4);

    settingsLayout->addWidget(subsetCheckBox, 3, 0);
    settingsLayout->addWidget(startDateLabel, 3, 0, Qt::AlignRight);
    settingsLayout->addWidget(endDateLabel, 4, 0, Qt::AlignRight);
    settingsLayout->addLayout(dateTimeContainer, 3, 1, 2, 2);

    settingsLayout->addWidget(filterLabel, 6, 0, Qt::AlignRight);
    settingsLayout->addWidget(filterCombo, 6, 1);
    settingsLayout->addWidget(nBinsLabel, 7, 0, Qt::AlignRight);
    settingsLayout->addWidget(nBinsSpin, 7, 1);
    settingsLayout->addWidget(fftCheckBox, 8, 0, 1, 2);

    settingsLayout->addWidget(spin31Label, 9, 1);
    settingsLayout->addWidget(spin32Label, 9, 2);
    settingsLayout->addWidget(spin33Label, 9, 3);
    settingsLayout->addWidget(spin34Label, 9, 4);
    settingsLayout->addWidget(hrLabel_3, 10, 1, 1, 4);

    settingsLayout->addWidget(noiseFreqLabel, 11, 1, 1, 2);
    settingsLayout->addWidget(spin30Label, 12, 0, Qt::AlignRight);
    settingsLayout->addWidget(spin31, 12, 1);
    settingsLayout->addWidget(spin32, 12, 2);
    settingsLayout->addWidget(spin33, 12, 3);
    settingsLayout->addWidget(spin34, 12, 4);
    settingsLayout->addWidget(hrLabel_1 , 13, 0, 1, -1);

    settingsLayout->addLayout(spectraQaQcLabel, 14, 0, 1, -1);
    settingsLayout->addWidget(frictionVelocityLabel, 15, 1);
    settingsLayout->addWidget(sensibleHeatLabel, 15, 2);
    settingsLayout->addWidget(latentHeatLabel, 15, 3);
    settingsLayout->addWidget(qcCo2Label, 15, 4);
    settingsLayout->addWidget(qcCh4Label, 15, 5);
    settingsLayout->addWidget(qcGas4Label, 15, 6);

    settingsLayout->addWidget(minUnstableLabel, 16, 0, Qt::AlignRight);
    settingsLayout->addWidget(qcMinUnstableUstarSpin, 16, 1);
    settingsLayout->addWidget(qcMinUnstableHSpin, 16, 2);
    settingsLayout->addWidget(qcMinUnstableLESpin, 16, 3);
    settingsLayout->addWidget(qcMinUnstableCo2Spin, 16, 4);
    settingsLayout->addWidget(qcMinUnstableCh4Spin, 16, 5);
    settingsLayout->addWidget(qcMinUnstableGas4Spin, 16, 6);
    settingsLayout->addWidget(minStableLabel, 17, 0, Qt::AlignRight);
    settingsLayout->addWidget(qcMinStableUstarSpin, 17, 1);
    settingsLayout->addWidget(qcMinStableHSpin, 17, 2);
    settingsLayout->addWidget(qcMinStableLESpin, 17, 3);
    settingsLayout->addWidget(qcMinStableCo2Spin, 17, 4);
    settingsLayout->addWidget(qcMinStableCh4Spin, 17, 5);
    settingsLayout->addWidget(qcMinStableGas4Spin, 17, 6);
    settingsLayout->addWidget(maxLabel, 18, 0, Qt::AlignRight);
    settingsLayout->addWidget(qcMaxUstarSpin, 18, 1);
    settingsLayout->addWidget(qcMaxHSpin, 18, 2);
    settingsLayout->addWidget(qcMaxLESpin, 18, 3);
    settingsLayout->addWidget(qcMaxCo2Spin, 18, 4);
    settingsLayout->addWidget(qcMaxCh4Spin, 18, 5);
    settingsLayout->addWidget(qcMaxGas4Spin, 18, 6);
    settingsLayout->addWidget(minSmplLabel, 19, 0, Qt::AlignRight);
    settingsLayout->addWidget(minSmplSpin, 19, 1);
    settingsLayout->addWidget(vmFlagsCheckBox, 20, 0, 1, 2);

    settingsLayout->addWidget(filterCospectraMauderTitle, 21, 0, 1, -1);
    settingsLayout->addWidget(lowQualityCheckBox, 22, 0, 1, -1);
    settingsLayout->addWidget(moderateQualityCheckBox, 23, 0, 1, 2);
    settingsLayout->addWidget(hrLabel_0, 24, 0, 1, -1);

    settingsLayout->addWidget(spectralCorrectionTitle, 25, 0);
    settingsLayout->addLayout(lowFreqLabel, 26, 0);
    settingsLayout->addWidget(lfMethodCheck, 27, 0, 1, 2);

    settingsLayout->addLayout(highFreqLabel, 28, 0);
    settingsLayout->addWidget(hfMethodCheck, 29, 0, 1, 2);
    settingsLayout->addWidget(hfMethLabel, 29, 1, Qt::AlignRight);
    settingsLayout->addWidget(hfMethCombo, 29, 2, 1, 3);
    settingsLayout->addWidget(horstCheck, 30, 0, 1, 2);
    settingsLayout->addWidget(horstMethodLabel, 30, 1, Qt::AlignRight);
    settingsLayout->addWidget(horstCombo, 30, 2, 1, 3);
    settingsLayout->addWidget(spectraExistingRadio, 31, 0, 1, 2);
    settingsLayout->addWidget(spectraFileBrowse, 31, 1, 1, 4);
    settingsLayout->addWidget(spectraNonExistingRadio, 32, 0, 1, 2);

    settingsLayout->addLayout(assessmentHighFreqLabel, 33, 0, 1, -1);
    settingsLayout->addWidget(spin11Label, 34, 1);
    settingsLayout->addWidget(spin12Label, 34, 2);
    settingsLayout->addWidget(spin13Label, 34, 3);
    settingsLayout->addWidget(spin14Label, 34, 4);
    settingsLayout->addWidget(hrLabel_2, 35, 1, 1, 4);
    settingsLayout->addWidget(minMaxFreqLabel, 36, 1, 1, -1);
    settingsLayout->addWidget(spin10Label, 37, 0, Qt::AlignRight);
    settingsLayout->addWidget(spin11, 37, 1);
    settingsLayout->addWidget(spin12, 37, 2);
    settingsLayout->addWidget(spin13, 37, 3);
    settingsLayout->addWidget(spin14, 37, 4);
    settingsLayout->addWidget(spin20Label, 38, 0, Qt::AlignRight);
    settingsLayout->addWidget(spin21, 38, 1);
    settingsLayout->addWidget(spin22, 38, 2);
    settingsLayout->addWidget(spin23, 38, 3);
    settingsLayout->addWidget(spin24, 38, 4);

    settingsLayout->addWidget(fratiniTitle, 39, 0, 1, -1);
    settingsLayout->addWidget(fullSpectraNonExistingRadio, 40, 0, 1, 2);
    settingsLayout->addWidget(fullSpectraExistingRadio, 41, 0, 1, 2);
    settingsLayout->addWidget(fullSpectraDirBrowse, 41, 1, 1, 4);
    settingsLayout->addWidget(addSonicCheck, 42, 0, 1, -1);
    settingsLayout->setColumnStretch(7, 1);

    auto settingsGroupLayout = new QHBoxLayout;
    settingsGroupLayout
            ->addWidget(WidgetUtils::getContainerScrollArea(this,
                                                            settingsLayout));

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(settingsGroupLayout);
    mainLayout->setContentsMargins(15, 15, 15, 10);
    setLayout(mainLayout);

    connect(binnedSpectraRadioGroup,
            static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int radioButton){ ecProject_->setGeneralBinSpectraAvail(radioButton); });
    connect(binnedSpectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(binnedSpectraRadioClicked(int)));
    connect(binnedSpectraDirBrowse, &DirBrowseWidget::pathChanged,
            this, &AdvSpectralOptions::updateBinnedSpectraFile);
    connect(binnedSpectraDirBrowse, &DirBrowseWidget::pathSelected,
            this, &AdvSpectralOptions::binnedSpectraDirSelected);
    connect(subsetCheckBox, &QCheckBox::toggled, [=](bool toggle)
            { ecProject_->setSpectraSubset(toggle); } );
    connect(subsetCheckBox, &QCheckBox::toggled,
            this, &AdvSpectralOptions::onSubsetCheckboxToggled);
    connect(startDateLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onStartDateLabelClicked);
    connect(startDateEdit, &QDateEdit::dateChanged,
            this, &AdvSpectralOptions::updateStartDate);
    connect(startTimeEdit, &QTimeEdit::timeChanged,
            this, &AdvSpectralOptions::updateStartTime);
    connect(endDateLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onEndDateLabelClicked);
    connect(endDateEdit, &QDateEdit::dateChanged,
            this, &AdvSpectralOptions::updateEndDate);
    connect(endTimeEdit, &QTimeEdit::timeChanged,
            this, &AdvSpectralOptions::updateEndTime);

    connect(filterLabel, &ClickLabel::clicked, [=]()
            { if (filterCombo->isEnabled()) filterCombo->showPopup(); });
    connect(filterCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFilter(int)));
    connect(nBinsLabel, &ClickLabel::clicked, [=]()
            { nBinsSpin->setFocus(); nBinsSpin->selectAll(); });
    connect(nBinsSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateNBins(int)));
    connect(fftCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setScreenlPowerOfTwo(checked); });

    connect(qcMinUnstableUstarSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableUstar(d); });
    connect(qcMinUnstableHSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableH(d); });
    connect(qcMinUnstableLESpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableLE(d); });
    connect(qcMinUnstableCo2Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableCo2(d); });
    connect(qcMinUnstableCh4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableCh4(d); });
    connect(qcMinUnstableGas4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinUnstableGas4(d); });

    connect(qcMinStableUstarSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableUstar(d); });
    connect(qcMinStableHSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableH(d); });
    connect(qcMinStableLESpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableLE(d); });
    connect(qcMinStableCo2Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableCo2(d); });
    connect(qcMinStableCh4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableCh4(d); });
    connect(qcMinStableGas4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMinStableGas4(d); });

    connect(qcMaxUstarSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxUstar(d); });
    connect(qcMaxHSpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxH(d); });
    connect(qcMaxLESpin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxLE(d); });
    connect(qcMaxCo2Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxCo2(d); });
    connect(qcMaxCh4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxCh4(d); });
    connect(qcMaxGas4Spin,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double d){ ecProject_->setSpectraMaxGas4(d); });

    connect(vmFlagsCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setSpectraUseVmFlags(checked); });
    connect(lowQualityCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setSpectraUseFokenLow(checked); });
    connect(moderateQualityCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setSpectraUseFokenMid(checked); });

    connect(lfMethodCheck, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setGeneralLfMethod(checked); });

    connect(hfMethodCheck, &QCheckBox::toggled,
            this, &AdvSpectralOptions::updateHfMethod_1);
    connect(hfMethodCheck, &QCheckBox::toggled,
            hfMethLabel, &ClickLabel::setEnabled);
    connect(hfMethodCheck, &QCheckBox::toggled,
            hfMethCombo, &QComboBox::setEnabled);
    connect(hfMethLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onClickHfMethLabel);
    connect(hfMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateHfMethod_2(int)));
    connect(horstCheck, &QCheckBox::toggled,
            horstMethodLabel, &ClickLabel::setEnabled);
    connect(horstCheck, &QCheckBox::toggled,
            horstCombo, &QComboBox::setEnabled);
    connect(horstMethodLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onClickHorstLabel);
    connect(horstCheck, &QCheckBox::toggled,
            this, &AdvSpectralOptions::updateHorst_1);
    connect(horstCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateHorst_2(int)));
    connect(spectraRadioGroup,
            static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            [=](int radioButton){ ecProject_->setSpectraMode(radioButton); });
    connect(spectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(spectraRadioClicked(int)));
    connect(spectraFileBrowse, &FileBrowseWidget::pathChanged,
            this, &AdvSpectralOptions::updateSpectraFile);
    connect(spectraFileBrowse, &FileBrowseWidget::pathSelected,
            this, &AdvSpectralOptions::testSelectedSpectraFile);
    connect(minSmplLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onMinSmplLabelClicked);
    connect(minSmplSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateMinSmpl(int)));
    connect(minUnstableLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onMinCo2LabelClicked);
    connect(minStableLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onMinCh4LabelClicked);
    connect(maxLabel, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onMinGas4LabelClicked);
    connect(spin10Label, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onSpin10LabelClicked);
    connect(spin11, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminCo2(double)));
    connect(spin12, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminH2o(double)));
    connect(spin13, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminCh4(double)));
    connect(spin14, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminGas4(double)));
    connect(spin20Label, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onSpin20LabelClicked);
    connect(spin21, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxCo2(double)));
    connect(spin22, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxH2o(double)));
    connect(spin23, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxCh4(double)));
    connect(spin24, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxGas4(double)));
    connect(spin30Label, &ClickLabel::clicked,
            this, &AdvSpectralOptions::onSpin30LabelClicked);
    connect(spin31, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnCo2(double)));
    connect(spin32, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnH2o(double)));
    connect(spin33, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnCh4(double)));
    connect(spin34, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnGas4(double)));

    connect(fullSpectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fullSpectraRadioClicked(int)));
    connect(fullSpectraDirBrowse, &DirBrowseWidget::pathChanged,
            this, &AdvSpectralOptions::updateFullSpectraFile);
    connect(fullSpectraDirBrowse, &DirBrowseWidget::pathSelected,
            this, &AdvSpectralOptions::fullSpectraDirSelected);
    connect(addSonicCheck, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setSpectraAddSonic(checked); });

    connect(ecProject_, &EcProject::ecProjectNew,
            this, &AdvSpectralOptions::reset);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &AdvSpectralOptions::refresh);

    foreach (QComboBox *combo,
             QList<QComboBox *>() << hfMethCombo
                                  << horstCombo)
    {
        connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(updateTooltip(int)));
    }

    // fix layout alignment
    auto max_spin_width = spin33->width();
    foreach(QWidget* w,
            QWidgetList() << spin34
                          << spin24
                          << qcMinUnstableCo2Spin
                          << qcMinUnstableCh4Spin
                          << qcMinUnstableGas4Spin
                          << qcMinStableCo2Spin
                          << qcMinStableCh4Spin
                          << qcMinStableGas4Spin
                          << qcMaxCo2Spin
                          << qcMaxCh4Spin
                          << qcMaxGas4Spin)
    {
        w->setMaximumWidth(max_spin_width);
    }

    QTimer::singleShot(0, this, SLOT(reset()));
}

AdvSpectralOptions::~AdvSpectralOptions()
{
    DEBUG_FUNC_NAME
}

void AdvSpectralOptions::setSmartfluxUI()
{
    bool on = configState_->project.smartfluxMode;

    if (on)
    {
        spectraNonExistingRadioOldEnabled = spectraNonExistingRadio->isEnabled();
        spectraNonExistingRadio->setDisabled(on);
        hfMethCombo->setItemData(4, QStringLiteral("disabled"), Qt::UserRole);
    }
    else
    {
        spectraNonExistingRadio->setEnabled(spectraNonExistingRadioOldEnabled);
        hfMethCombo->setItemData(4, QStringLiteral("enabled"), Qt::UserRole);
    }

    // block project modified() signal
//    bool oldmod;
//    if (!on)
//    {
//        // save the modified flag to prevent side effects of setting widgets
//        oldmod = ecProject_->modified();
//        ecProject_->blockSignals(true);
//    }

    if (on)
    {
        spectraRadioGroup->button(0)->click();
    }

//    // restore project modified() signal
//    if (!on)
//    {
//        // restore modified flag
//        ecProject_->setModified(oldmod);
//        ecProject_->blockSignals(false);
//    }
}

void AdvSpectralOptions::reset()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    vmFlagsCheckBox->setChecked(ecProject_->defaultSettings.spectraSettings.use_vm_flags);
    lowQualityCheckBox->setChecked(ecProject_->defaultSettings.spectraSettings.use_foken_low);
    moderateQualityCheckBox->setChecked(ecProject_->defaultSettings.spectraSettings.use_foken_mid);

    filterCombo->setCurrentIndex(ecProject_->defaultSettings.screenSetting.tap_win);
    filterCombo->setEnabled(true);
    nBinsSpin->setValue(ecProject_->defaultSettings.screenSetting.nbins);
    nBinsSpin->setEnabled(true);
    fftCheckBox->setChecked(ecProject_->defaultSettings.screenSetting.power_of_two);
    fftCheckBox->setEnabled(true);

    spin31->setValue(ecProject_->defaultSettings.spectraSettings.sa_hfn_co2_fmin);
    spin32->setValue(ecProject_->defaultSettings.spectraSettings.sa_hfn_h2o_fmin);
    spin33->setValue(ecProject_->defaultSettings.spectraSettings.sa_hfn_ch4_fmin);
    spin34->setValue(ecProject_->defaultSettings.spectraSettings.sa_hfn_gas4_fmin);

    spectraExistingRadio->setEnabled(false);
    spectraExistingRadio->setChecked(!ecProject_->defaultSettings.spectraSettings.sa_mode);
    spectraFileBrowse->setEnabled(false);
    spectraFileBrowse->clear();
    spectraNonExistingRadio->setEnabled(false);
    spectraNonExistingRadio->setChecked(ecProject_->defaultSettings.spectraSettings.sa_mode);

    binnedSpectraExistingRadio->setChecked(ecProject_->defaultSettings.projectGeneral.bin_sp_avail);
    binnedSpectraDirBrowse->setEnabled(false);
    binnedSpectraNonExistingRadio->setChecked(!ecProject_->defaultSettings.projectGeneral.bin_sp_avail);
    binnedSpectraDirBrowse->clear();

    lfMethodCheck->setChecked(ecProject_->defaultSettings.projectGeneral.lf_meth);
    hfMethodCheck->setChecked(ecProject_->defaultSettings.projectGeneral.hf_meth);
    WidgetUtils::resetComboToItem(hfMethCombo, 0);
    horstMethodLabel->setEnabled(false);
    horstCheck->setEnabled(false);
    horstCheck->setChecked(false);
    WidgetUtils::resetComboToItem(horstCombo, 1);
    horstCombo->setEnabled(false);

    subsetCheckBox->setChecked(ecProject_->defaultSettings.spectraSettings.subset);
    startDateLabel->setEnabled(false);
    startDateEdit->setEnabled(false);
    startTimeEdit->setEnabled(false);
    lockedIcon->setEnabled(false);
    endDateLabel->setEnabled(false);
    endDateEdit->setEnabled(false);
    endTimeEdit->setEnabled(false);

    startDateEdit->setDate(QDate::fromString(ecProject_->generalStartDate(), Qt::ISODate));
    startTimeEdit->setTime(QTime::fromString(ecProject_->generalStartTime(), QStringLiteral("hh:mm")));
    endDateEdit->setDate(QDate::fromString(ecProject_->generalEndDate(), Qt::ISODate));
    endTimeEdit->setTime(QTime::fromString(ecProject_->generalEndTime(), QStringLiteral("hh:mm")));
    forceEndDatePolicy();
    forceEndTimePolicy();

    qcMinUnstableUstarSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_ustar);
    qcMinUnstableHSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_h);
    qcMinUnstableLESpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_le);
    qcMinUnstableCo2Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_co2);
    qcMinUnstableCh4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_ch4);
    qcMinUnstableGas4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_un_gas4);
    qcMinStableUstarSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_ustar);
    qcMinStableHSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_h);
    qcMinStableLESpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_le);
    qcMinStableCo2Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_co2);
    qcMinStableCh4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_ch4);
    qcMinStableGas4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_st_gas4);
    qcMaxUstarSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_ustar);
    qcMaxHSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_h);
    qcMaxLESpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_le);
    qcMaxCo2Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_co2);
    qcMaxCh4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_ch4);
    qcMaxGas4Spin->setValue(ecProject_->defaultSettings.spectraSettings.sa_max_gas4);

    minSmplSpin->setValue(ecProject_->defaultSettings.spectraSettings.sa_min_smpl);

    spin11Label->setEnabled(false);
    spin12Label->setEnabled(false);
    spin13Label->setEnabled(false);
    spin14Label->setEnabled(false);
    spin10Label->setEnabled(false);
    spin20Label->setEnabled(false);
    minMaxFreqLabel->setEnabled(false);
    noiseFreqLabel->setEnabled(false);

    spin11->setEnabled(false);
    spin12->setEnabled(false);
    spin13->setEnabled(false);
    spin14->setEnabled(false);
    spin21->setEnabled(false);
    spin22->setEnabled(false);
    spin23->setEnabled(false);
    spin24->setEnabled(false);

    spin11->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmin_co2);
    spin12->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmin_h2o);
    spin13->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmin_ch4);
    spin14->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmin_gas4);
    spin21->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmax_co2);
    spin22->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmax_h2o);
    spin23->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmax_ch4);
    spin24->setValue(ecProject_->defaultSettings.spectraSettings.sa_fmax_gas4);

    fullSpectraExistingRadio->setEnabled(false);
    fullSpectraExistingRadio->setChecked(ecProject_->defaultSettings.projectGeneral.full_sp_avail);
    fullSpectraNonExistingRadio->setEnabled(false);
    fullSpectraNonExistingRadio->setChecked(!ecProject_->defaultSettings.projectGeneral.full_sp_avail);
    fullSpectraDirBrowse->setEnabled(false);
    fullSpectraDirBrowse->clear();

    addSonicCheck->setChecked(ecProject_->defaultSettings.spectraSettings.add_sonic_lptf);
    addSonicCheck->setEnabled(false);

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);

    emit updateOutputsRequest(0);
}

void AdvSpectralOptions::partialRefresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    subsetCheckBox->setChecked(ecProject_->spectraSubset());
    if (ecProject_->spectraSubset())
    {
        startDateEdit->setDate(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
        startTimeEdit->setTime(QTime::fromString(ecProject_->spectraStartTime(), QStringLiteral("hh:mm")));
        endDateEdit->setDate(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
        endTimeEdit->setTime(QTime::fromString(ecProject_->spectraEndTime(), QStringLiteral("hh:mm")));
    }
    else
    {
        startDateEdit->setDate(QDate::fromString(ecProject_->generalStartDate(), Qt::ISODate));
        startTimeEdit->setTime(QTime::fromString(ecProject_->generalStartTime(), QStringLiteral("hh:mm")));
        endDateEdit->setDate(QDate::fromString(ecProject_->generalEndDate(), Qt::ISODate));
        endTimeEdit->setTime(QTime::fromString(ecProject_->generalEndTime(), QStringLiteral("hh:mm")));
    }

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void AdvSpectralOptions::refresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    vmFlagsCheckBox->setChecked(ecProject_->spectraUseVmFlags());
    lowQualityCheckBox->setChecked(ecProject_->spectraUseFokenLow());
    moderateQualityCheckBox->setChecked(ecProject_->spectraUseFokenMid());

    lfMethodCheck->setChecked(ecProject_->generalLfMethod());
    hfMethodCheck->setChecked(ecProject_->generalHfMethod());

    int hfMethod = ecProject_->generalHfMethod();
    switch(hfMethod)
    {
    case 0:
    case 1: // moncrieff
        hfMethCombo->setCurrentIndex(0);
        break;
    case 2: // horst
        hfMethCombo->setCurrentIndex(2);
        break;
    case 3: // ibrom
        hfMethCombo->setCurrentIndex(3);
        break;
    case 4: // fratini
        hfMethCombo->setCurrentIndex(4);
        break;
    case 5: // massmann
        hfMethCombo->setCurrentIndex(1);
        break;
    }

    spectraExistingRadio->setEnabled(hfMethodCheck->isChecked()
                                     && isHorstIbromFratini());
    spectraNonExistingRadio->setEnabled(hfMethodCheck->isChecked()
                                        && isHorstIbromFratini());
    spectraExistingRadio->setChecked(!ecProject_->spectraMode());
    spectraNonExistingRadio->setChecked(ecProject_->spectraMode());

    spectraFileBrowse->setPath(QDir::toNativeSeparators(ecProject_->spectraFile()));
    spectraFileBrowse->setEnabled(spectraExistingRadio->isEnabled()
                                  && spectraExistingRadio->isChecked());

    subsetCheckBox->setChecked(ecProject_->spectraSubset());

    binnedSpectraNonExistingRadio->setChecked(!ecProject_->generalBinSpectraAvail());
    binnedSpectraExistingRadio->setChecked(ecProject_->generalBinSpectraAvail());
    binnedSpectraDirBrowse->setPath(ecProject_->spectraBinSpectra());
    binnedSpectraDirBrowse->setEnabled(binnedSpectraExistingRadio->isChecked());

    startDateLabel->setEnabled(subsetCheckBox->isEnabled() && subsetCheckBox->isChecked());
    startDateEdit->setEnabled(startDateLabel->isEnabled());
    startTimeEdit->setEnabled(startDateLabel->isEnabled());
    lockedIcon->setEnabled(startDateLabel->isEnabled());
    endDateLabel->setEnabled(startDateLabel->isEnabled());
    endDateEdit->setEnabled(startDateLabel->isEnabled());
    endTimeEdit->setEnabled(startDateLabel->isEnabled());

    startDateEdit->setDate(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
    startTimeEdit->setTime(QTime::fromString(ecProject_->spectraStartTime(), QStringLiteral("hh:mm")));
    endDateEdit->setDate(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
    endTimeEdit->setTime(QTime::fromString(ecProject_->spectraEndTime(), QStringLiteral("hh:mm")));

    filterCombo->setCurrentIndex(ecProject_->screenTapWin());
    filterCombo->setEnabled(!ecProject_->generalBinSpectraAvail());
    nBinsSpin->setValue(ecProject_->screenNBins());
    nBinsSpin->setEnabled(!ecProject_->generalBinSpectraAvail());
    fftCheckBox->setChecked(ecProject_->screenPowerOfTwo());
    fftCheckBox->setEnabled(!ecProject_->generalBinSpectraAvail());

    horstCheck->setEnabled(isIbrom() || isFratini());
    if (ecProject_->spectraHorst() > 0)
    {
        horstCombo->setCurrentIndex(ecProject_->spectraHorst() - 1);
    }
    else
    {
        horstCombo->setCurrentIndex(1);
    }
    horstCheck->setChecked(ecProject_->spectraHorst());
    horstMethodLabel->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());
    horstCombo->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());

    auto toEnable = isHorstIbromFratini()
                    && spectraNonExistingRadio->isEnabled()
                    && spectraNonExistingRadio->isChecked();

    minSmplSpin->setValue(ecProject_->spectraMinSmpl());

    qcMinUnstableUstarSpin->setValue(ecProject_->spectraMinUnstableUstar());
    qcMinUnstableHSpin->setValue(ecProject_->spectraMinUnstableH());
    qcMinUnstableLESpin->setValue(ecProject_->spectraMinUnstableLE());
    qcMinUnstableCo2Spin->setValue(ecProject_->spectraMinUnstableCo2());
    qcMinUnstableCh4Spin->setValue(ecProject_->spectraMinUnstableCh4());
    qcMinUnstableGas4Spin->setValue(ecProject_->spectraMinUnstableGas4());
    qcMinStableUstarSpin->setValue(ecProject_->spectraMinStableUstar());
    qcMinStableHSpin->setValue(ecProject_->spectraMinStableH());
    qcMinStableLESpin->setValue(ecProject_->spectraMinStableLE());
    qcMinStableCo2Spin->setValue(ecProject_->spectraMinStableCo2());
    qcMinStableCh4Spin->setValue(ecProject_->spectraMinStableCh4());
    qcMinStableGas4Spin->setValue(ecProject_->spectraMinStableGas4());
    qcMaxUstarSpin->setValue(ecProject_->spectraMaxUstar());
    qcMaxHSpin->setValue(ecProject_->spectraMaxH());
    qcMaxLESpin->setValue(ecProject_->spectraMaxLE());
    qcMaxCo2Spin->setValue(ecProject_->spectraMaxCo2());
    qcMaxCh4Spin->setValue(ecProject_->spectraMaxCh4());
    qcMaxGas4Spin->setValue(ecProject_->spectraMaxGas4());

    spin11Label->setEnabled(toEnable);
    spin12Label->setEnabled(toEnable);
    spin13Label->setEnabled(toEnable);
    spin14Label->setEnabled(toEnable);
    spin10Label->setEnabled(toEnable);
    spin20Label->setEnabled(toEnable);
    minMaxFreqLabel->setEnabled(toEnable);
    noiseFreqLabel->setEnabled(toEnable);

    spin11->setValue(ecProject_->spectraFminCo2());
    spin11->setEnabled(toEnable);

    spin12->setValue(ecProject_->spectraFminH2o());
    spin12->setEnabled(toEnable);

    spin13->setValue(ecProject_->spectraFminCh4());
    spin13->setEnabled(toEnable);

    spin14->setValue(ecProject_->spectraFminGas4());
    spin14->setEnabled(toEnable);

    spin21->setValue(ecProject_->spectraFmaxCo2());
    spin21->setEnabled(toEnable);

    spin22->setValue(ecProject_->spectraFmaxH2o());
    spin22->setEnabled(toEnable);

    spin23->setValue(ecProject_->spectraFmaxCh4());
    spin23->setEnabled(toEnable);

    spin24->setValue(ecProject_->spectraFmaxGas4());
    spin24->setEnabled(toEnable);

    spin31->setValue(ecProject_->spectraHfnCo2());
    spin32->setValue(ecProject_->spectraHfnH2o());
    spin33->setValue(ecProject_->spectraHfnCh4());
    spin34->setValue(ecProject_->spectraHfnGas4());

    auto toEnableFratini = isFratini();

    fullSpectraExistingRadio->setEnabled(toEnableFratini);
    fullSpectraExistingRadio->setChecked(ecProject_->generalFullSpectraAvail());
    fullSpectraNonExistingRadio->setEnabled(toEnableFratini);
    fullSpectraNonExistingRadio->setChecked(!ecProject_->generalFullSpectraAvail());

    fullSpectraDirBrowse->setPath(ecProject_->spectraFullSpectra());
    fullSpectraDirBrowse->setEnabled(toEnableFratini
                                     && fullSpectraExistingRadio->isChecked());

    addSonicCheck->setChecked(ecProject_->spectraAddSonic());
    addSonicCheck->setEnabled(toEnableFratini);

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);

    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::updateSpectraFile(const QString &fp)
{
    ecProject_->setSpectraFile(QDir::cleanPath(fp));
}

void AdvSpectralOptions::updateBinnedSpectraFile(const QString &fp)
{
    DEBUG_FUNC_NAME
    ecProject_->setSpectraBinSpectra(QDir::cleanPath(fp));
}

void AdvSpectralOptions::updateFullSpectraFile(const QString &fp)
{
    ecProject_->setSpectraFullSpectra(QDir::cleanPath(fp));
}

void AdvSpectralOptions::testSelectedSpectraFile(const QString& fp)
{
    QFileInfo paramFilePath(fp);
    QString canonicalParamFile = paramFilePath.canonicalFilePath();

    AncillaryFileTest test_dialog(AncillaryFileTest::FileType::Spectra, this);
    test_dialog.refresh(canonicalParamFile);

    auto test_result = test_dialog.makeTest();
    qDebug() << "test_result" << test_result;

    auto dialog_result = true;

    // blocking behavior if test fails
    if (!test_result)
    {
        dialog_result = test_dialog.exec();
    }

    if (dialog_result)
    {
        spectraFileBrowse->setPath(fp);

        QString lastPath = paramFilePath.canonicalPath();
        configState_->window.last_data_path = lastPath;
        GlobalSettings::updateLastDatapath(lastPath);
    }
    else
    {
        spectraFileBrowse->clear();
    }
}

void AdvSpectralOptions::binnedSpectraDirSelected(const QString& dir_path)
{
    DEBUG_FUNC_NAME

    binnedSpectraDirBrowse->setPath(dir_path);

    QDir dataDir(dir_path);
    QString canonicalDataDir = dataDir.canonicalPath();
    configState_->window.last_data_path = canonicalDataDir;
    GlobalSettings::updateLastDatapath(canonicalDataDir);
}

void AdvSpectralOptions::fullSpectraDirSelected(const QString& dir_path)
{
    DEBUG_FUNC_NAME

    fullSpectraDirBrowse->setPath(dir_path);

    QDir dataDir(dir_path);
    QString canonicalDataDir = dataDir.canonicalPath();
    configState_->window.last_data_path = canonicalDataDir;
    GlobalSettings::updateLastDatapath(canonicalDataDir);
}

void AdvSpectralOptions::spectraRadioClicked(int radioButton)
{
    // existing spectral assessment file
    if (radioButton == 0)
    {
        spin11Label->setEnabled(false);
        spin12Label->setEnabled(false);
        spin13Label->setEnabled(false);
        spin14Label->setEnabled(false);
        spin10Label->setEnabled(false);
        spin20Label->setEnabled(false);
        minMaxFreqLabel->setEnabled(false);
        noiseFreqLabel->setEnabled(false);
        spin11->setEnabled(false);
        spin12->setEnabled(false);
        spin13->setEnabled(false);
        spin14->setEnabled(false);
        spin21->setEnabled(false);
        spin22->setEnabled(false);
        spin23->setEnabled(false);
        spin24->setEnabled(false);
    }
    // non existing spectral assessment file
    else
    {
        auto toEnable = hfMethodCheck->isChecked() && isHorstIbromFratini();

        spin11Label->setEnabled(toEnable);
        spin12Label->setEnabled(toEnable);
        spin13Label->setEnabled(toEnable);
        spin14Label->setEnabled(toEnable);
        spin10Label->setEnabled(toEnable);
        spin20Label->setEnabled(toEnable);
        minMaxFreqLabel->setEnabled(toEnable);
        noiseFreqLabel->setEnabled(toEnable);
        spin11->setEnabled(toEnable);
        spin12->setEnabled(toEnable);
        spin13->setEnabled(toEnable);
        spin14->setEnabled(toEnable);
        spin21->setEnabled(toEnable);
        spin22->setEnabled(toEnable);
        spin23->setEnabled(toEnable);
        spin24->setEnabled(toEnable);
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::binnedSpectraRadioClicked(int radioButton)
{
    if (radioButton == 0)
    {
        binnedSpectraDirBrowse->setEnabled(false);
        filterCombo->setEnabled(true);
        nBinsSpin->setEnabled(true);
        fftCheckBox->setEnabled(true);
    }
    else
    {
        binnedSpectraDirBrowse->setEnabled(true);
        filterCombo->setEnabled(false);
        nBinsSpin->setEnabled(false);
        fftCheckBox->setEnabled(false);
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::fullSpectraRadioClicked(int radioButton)
{
    if (radioButton == 0)
    {
        ecProject_->setGeneralFullSpectraAvail(0);
        fullSpectraDirBrowse->setEnabled(false);
    }
    else
    {
        ecProject_->setGeneralFullSpectraAvail(1);
        fullSpectraDirBrowse->setEnabled(true);
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::onStartDateLabelClicked()
{
    DEBUG_FUNC_NAME
    startDateEdit->setFocus();
    WidgetUtils::showCalendarOf(startDateEdit);
}

void AdvSpectralOptions::onEndDateLabelClicked()
{
    DEBUG_FUNC_NAME
    endDateEdit->setFocus();
    WidgetUtils::showCalendarOf(endDateEdit);
}

void AdvSpectralOptions::updateStartDate(const QDate &d)
{
    ecProject_->setSpectraStartDate(d.toString(Qt::ISODate));
    forceEndDatePolicy();
}

void AdvSpectralOptions::updateStartTime(const QTime& t)
{
    ecProject_->setSpectraStartTime(t.toString(QStringLiteral("hh:mm")));
    forceEndTimePolicy();
}

void AdvSpectralOptions::updateEndDate(const QDate &d)
{
    ecProject_->setSpectraEndDate(d.toString(Qt::ISODate));
}

void AdvSpectralOptions::updateEndTime(const QTime& t)
{
    ecProject_->setSpectraEndTime(t.toString(QStringLiteral("hh:mm")));
}

void AdvSpectralOptions::onClickHfMethLabel()
{
    if (hfMethCombo->isEnabled())
    {
        hfMethCombo->showPopup();
    }
}

void AdvSpectralOptions::setHfMethod(int hfMethComboIndex)
{
    DEBUG_FUNC_NAME

    switch (hfMethComboIndex)
    {
    case 0: // moncrieff
        ecProject_->setGeneralHfMethod(1);
        break;
    case 1: // massmann
        ecProject_->setGeneralHfMethod(5);
        break;
    case 2: // horst
        ecProject_->setGeneralHfMethod(2);
        break;
    case 3: // ibrom
        ecProject_->setGeneralHfMethod(3);
        break;
    case 4: // fratini
        ecProject_->setGeneralHfMethod(4);
        break;
    }
    qDebug() << "hfMethComboIndex" << hfMethComboIndex;
    qDebug() << "ecProject_->generalHfMethod()" << ecProject_->generalHfMethod();
}

void AdvSpectralOptions::updateHfMethod_1(bool b)
{
    bool smartfluxOn = configState_->project.smartfluxMode;

    if (b)
    {
        setHfMethod(hfMethCombo->currentIndex());

        spectraExistingRadio->setEnabled(isHorstIbromFratini());
        spectraNonExistingRadio->setEnabled(isHorstIbromFratini()
                                            && !smartfluxOn);
        spectraFileBrowse->setEnabled(isHorstIbromFratini()
                                      && spectraExistingRadio->isChecked());
        auto toEnable = isHorstIbromFratini() && spectraNonExistingRadio->isChecked();

        horstCheck->setEnabled(isIbrom() || isFratini());
        horstMethodLabel->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());
        horstCombo->setEnabled(horstMethodLabel->isEnabled());

        spin11Label->setEnabled(toEnable);
        spin12Label->setEnabled(toEnable);
        spin13Label->setEnabled(toEnable);
        spin14Label->setEnabled(toEnable);
        spin10Label->setEnabled(toEnable);
        spin20Label->setEnabled(toEnable);
        minMaxFreqLabel->setEnabled(toEnable);
        noiseFreqLabel->setEnabled(toEnable);
        spin11->setEnabled(toEnable);
        spin12->setEnabled(toEnable);
        spin13->setEnabled(toEnable);
        spin14->setEnabled(toEnable);
        spin21->setEnabled(toEnable);
        spin22->setEnabled(toEnable);
        spin23->setEnabled(toEnable);
        spin24->setEnabled(toEnable);

        auto toEnableFratini = isFratini();
        fullSpectraNonExistingRadio->setEnabled(toEnableFratini);
        fullSpectraExistingRadio->setEnabled(toEnableFratini);
        fullSpectraNonExistingRadio->setEnabled(toEnableFratini);
        fullSpectraDirBrowse->setEnabled(toEnableFratini
                                         && fullSpectraExistingRadio->isChecked());
        addSonicCheck->setEnabled(toEnableFratini);

        emit updateOutputsRequest(hfMethCombo->currentIndex());
    }
    else
    {
        ecProject_->setGeneralHfMethod(0);

        horstMethodLabel->setEnabled(false);
        horstCheck->setEnabled(false);
        horstCombo->setEnabled(false);
        spectraExistingRadio->setEnabled(false);
        spectraNonExistingRadio->setEnabled(false);
        spectraFileBrowse->setEnabled(false);
        spin11Label->setEnabled(false);
        spin12Label->setEnabled(false);
        spin13Label->setEnabled(false);
        spin14Label->setEnabled(false);
        spin10Label->setEnabled(false);
        spin20Label->setEnabled(false);
        minMaxFreqLabel->setEnabled(false);
        noiseFreqLabel->setEnabled(false);
        spin11->setEnabled(false);
        spin12->setEnabled(false);
        spin13->setEnabled(false);
        spin14->setEnabled(false);
        spin21->setEnabled(false);
        spin22->setEnabled(false);
        spin23->setEnabled(false);
        spin24->setEnabled(false);

        fullSpectraExistingRadio->setEnabled(false);
        fullSpectraNonExistingRadio->setEnabled(false);
        fullSpectraDirBrowse->setEnabled(false);
        addSonicCheck->setEnabled(false);

        emit updateOutputsRequest(0);
    }
}

// update project properties and fluxes rotation choices
void AdvSpectralOptions::updateHfMethod_2(int n)
{
    bool smartfluxOn = configState_->project.smartfluxMode;

    setHfMethod(n);

    spectraExistingRadio->setEnabled(n > 1);
    spectraNonExistingRadio->setEnabled((n > 1) && !smartfluxOn);
    spectraFileBrowse->setEnabled(n > 1 && spectraExistingRadio->isChecked());

    auto toEnable = isHorstIbromFratini() && spectraNonExistingRadio->isChecked();

    horstCheck->setEnabled(isIbrom() || isFratini());
    horstMethodLabel->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());
    horstCombo->setEnabled(horstMethodLabel->isEnabled());

    spin11Label->setEnabled(toEnable);
    spin12Label->setEnabled(toEnable);
    spin13Label->setEnabled(toEnable);
    spin14Label->setEnabled(toEnable);
    spin10Label->setEnabled(toEnable);
    spin20Label->setEnabled(toEnable);
    minMaxFreqLabel->setEnabled(toEnable);
    noiseFreqLabel->setEnabled(toEnable);
    spin11->setEnabled(toEnable);
    spin12->setEnabled(toEnable);
    spin13->setEnabled(toEnable);
    spin14->setEnabled(toEnable);
    spin21->setEnabled(toEnable);
    spin22->setEnabled(toEnable);
    spin23->setEnabled(toEnable);
    spin24->setEnabled(toEnable);

    // fratini only
    auto toEnableFratini = (n == 4);

    fullSpectraExistingRadio->setEnabled(toEnableFratini);
    fullSpectraNonExistingRadio->setEnabled(toEnableFratini);
    fullSpectraDirBrowse->setEnabled(toEnableFratini
                                     && fullSpectraExistingRadio->isChecked());
    addSonicCheck->setEnabled(toEnableFratini);

    emit updateOutputsRequest(n);
}

bool AdvSpectralOptions::isHorstIbromFratini()
{
    return (ecProject_->generalHfMethod() == 2)
            || (ecProject_->generalHfMethod() == 3)
            || (ecProject_->generalHfMethod() == 4);
}

bool AdvSpectralOptions::isIbrom()
{
    return (ecProject_->generalHfMethod() == 3);
}

bool AdvSpectralOptions::isFratini()
{
    return (ecProject_->generalHfMethod() == 4);
}

void AdvSpectralOptions::onMinSmplLabelClicked()
{
    minSmplSpin->setFocus();
    minSmplSpin->selectAll();
}

void AdvSpectralOptions::updateMinSmpl(int n)
{
    ecProject_->setSpectraMinSmpl(n);
}

void AdvSpectralOptions::onClickHorstLabel()
{
    if (horstCombo->isEnabled())
    {
        horstCombo->showPopup();
    }
}

void AdvSpectralOptions::updateHorst_1(bool b)
{
    DEBUG_FUNC_NAME
    if (b)
    {
        ecProject_->setSpectraHorst(horstCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setSpectraHorst(0);
    }
}

void AdvSpectralOptions::updateHorst_2(int n)
{
    DEBUG_FUNC_NAME
    qDebug() << "n" << n;
    ecProject_->setSpectraHorst(n + 1);
}

void AdvSpectralOptions::onMinCo2LabelClicked()
{
    qcMinStableCo2Spin->setFocus();
    qcMinStableCo2Spin->selectAll();
}

void AdvSpectralOptions::onMinCh4LabelClicked()
{
    qcMinStableCh4Spin->setFocus();
    qcMinStableCh4Spin->selectAll();
}

void AdvSpectralOptions::onMinGas4LabelClicked()
{
    qcMinStableGas4Spin->setFocus();
    qcMinStableGas4Spin->selectAll();
}

void AdvSpectralOptions::onSpin10LabelClicked()
{
    spin11->setFocus();
    spin11->selectAll();
}

void AdvSpectralOptions::onSpin20LabelClicked()
{
    spin21->setFocus();
    spin21->selectAll();
}

void AdvSpectralOptions::onSpin30LabelClicked()
{
    spin31->setFocus();
    spin31->selectAll();
}

void AdvSpectralOptions::updateFminCo2(double d)
{
    ecProject_->setSpectraFminCo2(d);

    // min/max constraint
    if (d >= spin21->value())
    {
        spin21->setValue(d + 0.0001);
    }
}

void AdvSpectralOptions::updateFminH2o(double d)
{
    ecProject_->setSpectraFminH2o(d);

    // min/max constraint
    if (d >= spin22->value())
    {
        spin22->setValue(d + 0.0001);
    }
}

void AdvSpectralOptions::updateFminCh4(double d)
{
    ecProject_->setSpectraFminCh4(d);

    // min/max constraint
    if (d >= spin23->value())
    {
        spin23->setValue(d + 0.0001);
    }
}

void AdvSpectralOptions::updateFminGas4(double d)
{
    ecProject_->setSpectraFminGas4(d);

    // min/max constraint
    if (d >= spin24->value())
    {
        spin24->setValue(d + 0.0001);
    }
}

void AdvSpectralOptions::updateFmaxCo2(double d)
{
    ecProject_->setSpectraFmaxCo2(d);

    // min/max constraint
    if (d <= spin11->value())
    {
        spin11->setValue(d - 0.0001);
    }
}

void AdvSpectralOptions::updateFmaxH2o(double d)
{
    ecProject_->setSpectraFmaxH2o(d);

    // min/max constraint
    if (d <= spin12->value())
    {
        spin12->setValue(d - 0.0001);
    }
}

void AdvSpectralOptions::updateFmaxCh4(double d)
{
    ecProject_->setSpectraFmaxCh4(d);

    // min/max constraint
    if (d <= spin13->value())
    {
        spin13->setValue(d - 0.0001);
    }
}

void AdvSpectralOptions::updateFmaxGas4(double d)
{
    ecProject_->setSpectraFmaxGas4(d);

    // min/max constraint
    if (d <= spin14->value())
    {
        spin14->setValue(d - 0.0001);
    }
}

void AdvSpectralOptions::updateHfnCo2(double d)
{
    ecProject_->setSpectraHfnCo2(d);
}

void AdvSpectralOptions::updateHfnH2o(double d)
{
    ecProject_->setSpectraHfnH2o(d);
}

void AdvSpectralOptions::updateHfnCh4(double d)
{
    ecProject_->setSpectraHfnCh4(d);
}

void AdvSpectralOptions::updateHfnGas4(double d)
{
    ecProject_->setSpectraHfnGas4(d);
}

// enforce (start date&time) <= (end date&time)
void AdvSpectralOptions::forceEndDatePolicy()
{
    DEBUG_FUNC_NAME

    endDateEdit->setMinimumDate(startDateEdit->date());
}

// enforce (start date&time) <= (end date&time)
void AdvSpectralOptions::forceEndTimePolicy()
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

void AdvSpectralOptions::createQuestionMarks()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_11 = new QPushButton;
    questionMark_11->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_22 = new QPushButton;
    questionMark_22->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_33 = new QPushButton;
    questionMark_33->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_44 = new QPushButton;
    questionMark_44->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_55 = new QPushButton;
    questionMark_55->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_11);
    connect(questionMark_11, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_1);
    connect(questionMark_22, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_2);
    connect(questionMark_33, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_3);
    connect(questionMark_44, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_4);
    connect(questionMark_55, &QPushButton::clicked,
            this, &AdvSpectralOptions::onlineHelpTrigger_5);
}

void AdvSpectralOptions::onlineHelpTrigger_11()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/Ensemble_Averages.html")));
}

void AdvSpectralOptions::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/Calculating_Spectral_Correction_Factors.html")));
}

void AdvSpectralOptions::onlineHelpTrigger_2()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/High-pass_Filtering.html")));
}

void AdvSpectralOptions::onlineHelpTrigger_3()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/Low-pass_Filtering.html")));
}

void AdvSpectralOptions::onlineHelpTrigger_4()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/Spectral_Corrections.html")));
}

void AdvSpectralOptions::onlineHelpTrigger_5()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro6/Content/Calculating_Spectra_Cospectra_and_Ogives.html")));
}

void AdvSpectralOptions::updateTooltip(int i)
{
    QComboBox* senderCombo = qobject_cast<QComboBox *>(sender());

    WidgetUtils::updateComboItemTooltip(senderCombo, i);
}

void AdvSpectralOptions::onSubsetCheckboxToggled(bool b)
{
    foreach (QWidget *w,
             QWidgetList()
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
}

double AdvSpectralOptions::getLowestFrequencyValue()
{
    DEBUG_FUNC_NAME

    int avrgLen = ecProject_->screenAvrgLen();
    int fileDuration = dlProject_->fileDuration();
    double lowestValue = 1.0;

    if (avrgLen > 0)
    {
        lowestValue = 1.0 / (avrgLen * 60);
    }
    else
    {
        if (fileDuration)
        {
            lowestValue = 1.0 / (fileDuration * 60);
        }
    }
    return lowestValue;
}

double AdvSpectralOptions::getHighestFrequencyValue()
{
    return (dlProject_->acquisitionFrequency() / 5.0);
}

void AdvSpectralOptions::updateFilter(int n)
{
    ecProject_->setScreenTapWin(n);
}

void AdvSpectralOptions::updateNBins(int n)
{
    ecProject_->setScreenNBins(n);
}
