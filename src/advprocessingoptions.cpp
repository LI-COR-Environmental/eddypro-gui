/***************************************************************************
  advprocessingoptions.cpp
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

#include "advprocessingoptions.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>
#include <QDesktopServices>

#include <fstream>
#include <iostream>

#include "clicklabel.h"
#include "configstate.h"
#include "customresetlineedit.h"
#include "dlproject.h"
#include "ecproject.h"
#include "fileutils.h"
#include "infomessage.h"
#include "planarfitsettingsdialog.h"
#include "richtextcheckbox.h"
#include "timelagsettingsdialog.h"
#include "stringutils.h"
#include "widget_utils.h"

AdvProcessingOptions::AdvProcessingOptions(QWidget *parent,
                                           DlProject *dlProject,
                                           EcProject *ecProject,
                                           ConfigState *config) : QWidget(parent),
                                                                  dlProject_(dlProject),
                                                                  ecProject_(ecProject),
                                                                  configState_(config)
{
    createQuestionMark();

    auto rawProcessingTitle = new QLabel(tr("Raw data processing"));
    rawProcessingTitle->setProperty("groupLabel", true);

    windOffsetLabel = new QLabel(tr("Wind speed measurement offsets "));
    windOffsetLabel->setMinimumWidth(windOffsetLabel->sizeHint().width());
    windOffsetLabel->setMargin(11);
    windOffsetLabel->setToolTip(tr("<b>Wind speed measurement offsets:</b> Wind measurements by a sonic anemometer may be biased by systematic deviations, which need to be eliminated (e.g., for a proper assessment of tilt angles). You may get these offsets from the calibration certificate of your anemometer, but you could also assess it easily, by recording the 3 wind components from the anemometer enclosed in a box with still air (zero-wind test). Any long-term systematic deviation from zero of a wind component is a good estimation of this bias."));
    uLabel = new ClickLabel(tr("U :"));
    uLabel->setToolTip(windOffsetLabel->toolTip());
    uOffsetSpin = new QDoubleSpinBox;
    uOffsetSpin->setRange(-10.0, 10.0);
    uOffsetSpin->setSingleStep(0.1);
    uOffsetSpin->setDecimals(3);
    uOffsetSpin->setValue(0.0);
    uOffsetSpin->setAccelerated(true);
    uOffsetSpin->setSuffix(tr("  [m/s]", "Velocity"));
#if defined(Q_OS_WIN)
    uOffsetSpin->setMinimumWidth(uOffsetSpin->sizeHint().width() * 1.3);
#elif defined(Q_OS_MACOS)
    uOffsetSpin->setMinimumWidth(102);
#endif
    uOffsetSpin->setToolTip(windOffsetLabel->toolTip());

    vLabel = new ClickLabel(tr("V :"));
    vLabel->setToolTip(windOffsetLabel->toolTip());
    vOffsetSpin = new QDoubleSpinBox;
    vOffsetSpin->setRange(-10.0, 10.0);
    vOffsetSpin->setSingleStep(0.1);
    vOffsetSpin->setDecimals(3);
    vOffsetSpin->setValue(0.0);
    vOffsetSpin->setAccelerated(true);
    vOffsetSpin->setSuffix(tr("  [m/s]", "Velocity"));
#if defined(Q_OS_WIN)
    vOffsetSpin->setMinimumWidth(vOffsetSpin->sizeHint().width() * 1.3);
#elif defined(Q_OS_MACOS)
    vOffsetSpin->setMinimumWidth(102);
#endif
    vOffsetSpin->setToolTip(windOffsetLabel->toolTip());

    wLabel = new ClickLabel(tr("W :"));
    wLabel->setToolTip(windOffsetLabel->toolTip());
    wOffsetSpin = new QDoubleSpinBox;
    wOffsetSpin->setRange(-10.0, 10.0);
    wOffsetSpin->setSingleStep(0.1);
    wOffsetSpin->setDecimals(3);
    wOffsetSpin->setValue(0.0);
    wOffsetSpin->setAccelerated(true);
    wOffsetSpin->setSuffix(tr("  [m/s]", "Velocity"));
#if defined(Q_OS_WIN)
    wOffsetSpin->setMinimumWidth(wOffsetSpin->sizeHint().width() * 1.3);
#elif defined(Q_OS_MACOS)
    wOffsetSpin->setMinimumWidth(102);
#endif
    wOffsetSpin->setToolTip(windOffsetLabel->toolTip());

    auto windComponentLayout = new QHBoxLayout;
    windComponentLayout->addWidget(uLabel, 0, Qt::AlignRight);
    windComponentLayout->addWidget(uOffsetSpin, 1);
    windComponentLayout->addStretch(1);
    windComponentLayout->addWidget(vLabel, 0, Qt::AlignRight);
    windComponentLayout->addWidget(vOffsetSpin, 1);
    windComponentLayout->addStretch(1);
    windComponentLayout->addWidget(wLabel, 0, Qt::AlignRight);
    windComponentLayout->addWidget(wOffsetSpin, 1);
    windComponentLayout->addStretch(1);

    wBoostCheckBox = new RichTextCheckBox;
    wBoostCheckBox->setText(tr("Fix 'w boost' bug (Gill WindMaster and WindMaster Pro only)"));
    wBoostCheckBox->setToolTip(tr("<b>Fix 'w boost' bug:</b> Gill WindMaster and WindMaster Pro produced between 2006 and 2015 and identified by a firmware version of the form 2329.x.y with x &lt; 700, are affected by a bug such that the vertical wind speed is underestimated. Check this option to have EddyPro fix the bug. For more details, please visit <a href=\"http://gillinstruments.com/data/manuals/KN1509_WindMaster_WBug_info.pdf\">Gill's Technical Key Note</a>"));
    wBoostCheckBox->setQuestionMark(QStringLiteral("https://www.licor.com/env/help/eddypro/topics/w-boost-correction.html"));

    aoaCheckBox = new RichTextCheckBox;
    aoaCheckBox->setToolTip(tr("<b>Angle-of-attack correction:</b> Applies only to vertical mount Gill sonic anemometers with the same geometry of the R3 (e.g., R2, WindMaster, WindMaster Pro). This correction is meant to compensate the effects of flow distortion induced by the anemometer frame on the turbulent flow field. We recommend applying this correction whenever an R3-shaped anemometer was used."));
    aoaCheckBox->setText(tr("Angle-of-attack correction for wind components (Gill's only)"));
    aoaCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/angle-of-attack-correction.html"));

    aoaMethLabel = new ClickLabel(tr("Method :"));
    aoaMethLabel->setEnabled(false);
    aoaMethCombo = new QComboBox;
    aoaMethCombo->addItem(tr("Select automatically"), -1);
    aoaMethCombo->addItem(tr("Field calibration (Nakai and Shimoyama, 2012)"), 1);
    aoaMethCombo->addItem(tr("Wind tunnel calibration (Nakai et al., 2006)"), 2);
    aoaMethCombo->setItemData(0, tr("<b>Select automatically:</b> Select this option to allow EddyPro to choose the most appropriate angle of attack correction method based on the anemometer model and - in the case of the WindMaster<sup>%1</sup> or WindMaster Pro - its firmware version.").arg(Defs::TRADEMARK_SYMBOL), Qt::ToolTipRole);
    aoaMethCombo->setItemData(1, tr("<b>Field calibration:</b> Select this option to apply the angle-of-attack correction according to the method described in the referenced paper, which makes use of a field calibration instead of the wind tunnel calibration."), Qt::ToolTipRole);
    aoaMethCombo->setItemData(2, tr("<b>Wind tunnel calibration:</b> Select this option to apply the angle-of-attack correction according to the method described in the referenced paper, which makes use of a wind tunnel calibration instead of the field calibration."), Qt::ToolTipRole);
    aoaMethCombo->setEnabled(false);

    rotCheckBox = new RichTextCheckBox;
    rotCheckBox->setToolTip(tr("<b>Axis rotation for tilt correction:</b> Select the appropriate method for compensating anemometer tilt with respect to local streamlines. Uncheck the box to <i>not perform</i> any rotation (not recommnended). If your site has a complex or sloping topography, a planar-fit method is advisable. Click on the <b><i>Planar Fit Settings...</i></b> to configure the procedure."));
    rotCheckBox->setText(tr("Axis rotations for tilt correction"));
    rotCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/anemometer-tilt-correction.html"));

    rotMethLabel = new ClickLabel(tr("Rotation method :"));
    rotMethCombo = new QComboBox;
    rotMethCombo->addItem(tr("Double rotation"));
    rotMethCombo->addItem(tr("Triple rotation"));
    rotMethCombo->addItem(tr("Planar fit (Wilczak et al. 2001)"));
    rotMethCombo->addItem(tr("Planar fit with no velocity bias (van Dijk et al. 2004)"));
    rotMethCombo->setItemData(0, tr("<b>Double rotation:</b> Aligns the x-axis of the anemometer to the current mean streamlines, nullifying the vertical and cross-wind components. This is suggested in cases of flat topography and homogeneous canopies and in all conditions in which it is safe to assume that local wind streamlines are horizontal, parallel to the underlying surface."), Qt::ToolTipRole);
    rotMethCombo->setItemData(1, tr("<b>Triple rotation:</b> Double rotations plus a third rotation that nullifies the cross-stream stress. Not suitable in situations where the cross-stream stress is not expected to vanish, e.g., over water surfaces. Not recommended in general. Provided primarily for backwards compatibility."), Qt::ToolTipRole);
    rotMethCombo->setItemData(2, tr("<b>Planar fit:</b> Aligns the anemometer coordinate system to local streamlines assessed on a long time period (e.g., 2 weeks or more). Can be performed sector-wise, meaning that different rotation angles are calculated for different wind sectors. Suggested for complex topographies and canopy structures, if canopy heights don\'t change too quickly. Click on the <b><i>Planar Fit Settings...</i></b> to configure the procedure."), Qt::ToolTipRole);
    rotMethCombo->setItemData(3, tr("<b>Planar fit with no velocity bias:</b> Aligns the anemometer coordinate system to local streamlines assessed on a long time period (e.g., 2 weeks or more), but unlike the classic <i>Planar fit</i>, it assumes that any bias in the measurement of vertical wind is preliminarily compensated, and forces the fitting plane to pass through the origin (that is, such that if average <i>u</i> and <i>v</i> are zero, also average <i>w</i> is zero), thus its coefficient <i>b0</i> is zero. Can be performed sector-wise, meaning that different rotation angles are calculated for different wind sectors. It is recommended for complex topographies and canopy structures, if canopy heights don\'t change too quickly. Click on the <b><i>Planar Fit Settings...</i></b> to configure the procedure."), Qt::ToolTipRole);
    rotMethCombo->setEnabled(false);

    pfSettingsButton = new QPushButton(tr("Planar Fit Settings..."));
    pfSettingsButton->setProperty("mdButton", true);
    pfSettingsButton->setMaximumWidth(pfSettingsButton->sizeHint().width());

    detrendLabel = new QLabel(tr("Turbulent fluctuations"));
    detrendLabel->setMargin(11);
    detrendLabel->setToolTip(tr("<b>Turbulent fluctuations:</b> Select the method to extract turbulence fluctuations out of the time series."));
    detrendMethLabel = new ClickLabel(tr("Detrend method :"));
    detrendCombo = new QComboBox;
    detrendCombo->addItem(tr("Block average"));
    detrendCombo->addItem(tr("Linear detrending"));
    detrendCombo->addItem(tr("Running mean"));
    detrendCombo->addItem(tr("Exponential running mean"));
    detrendCombo->setItemData(0, tr("<b>Block average:</b> Simply removes the mean value from the time series, calculated over the flux averaging interval. Obeys Reynolds decomposition rule (the mean value of fluctuations is identically zero). Among all methods available, block average retains the largest portion of low frequency content, including genuine turbulent fluctuations and possible non-turbulence related trends."), Qt::ToolTipRole);
    detrendCombo->setItemData(1, tr("<b>Linear detrending:</b> Calculates fluctuations as the deviations from a linear trend. The linear trend can be evaluated on a time basis different from the flux averaging interval. Specify this time basis using the <b><i>Time constant</i></b> entry. For classic linear detrending, with the trend evaluated on the whole flux averaging interval, set <b><i>Time constant = 0</i></b>, which will be automatically converted into the text <i>Same as Flux averaging interval</i>."), Qt::ToolTipRole);
    detrendCombo->setItemData(2, tr("<b>Running mean:</b> High-pass, finite impulse response filter. The current mean is determined by the previous <i>N</i> data points, where <i>N</i> depends on the <i>time constant</i>. The smaller the time constant, the more low-frequency content is eliminated from the time series."), Qt::ToolTipRole);
    detrendCombo->setItemData(3, tr("<b>Exponential running mean:</b> High-pass, infinite impulse response filter. Similar to the simple running mean, but weighted in such a way that distant samples have an exponentially decreasing weight in the current mean, never reaching zero. The smaller the time constant, the more low-frequency content is eliminated from the time series."), Qt::ToolTipRole);

    timeConstantLabel = new ClickLabel(tr("Time constant :"));
    timeConstantLabel->setEnabled(false);
    timeConstantLabel->setToolTip(tr("<b>Time constant:</b> Applies to the linear detrending, running mean and exponential running mean methods. In general, the higher the time constant, the more low-frequency content is retained in the turbulent fluctuations. Note that for the linear detrending the unit is minutes, while for the running means it is seconds."));
    timeConstantSpin = new QDoubleSpinBox(this);
    timeConstantSpin->setToolTip(timeConstantLabel->toolTip());
    timeConstantSpin->setRange(0.0, 5000.0);
    timeConstantSpin->setValue(0.0);
    timeConstantSpin->setDecimals(1);
    timeConstantSpin->setAccelerated(true);
    timeConstantSpin->setSingleStep(10.0);
    timeConstantSpin->setSuffix(tr("  [s]", "Second"));
    timeConstantSpin->setEnabled(false);
    timeConstantSpin->setSpecialValueText(tr("Same as Flux averaging interval"));
    timeConstantSpin->setMaximumWidth(timeConstantSpin->sizeHint().width());

    timeLagCheckBox = new RichTextCheckBox;
    timeLagCheckBox->setToolTip(tr("<b>Time lags compensation:</b> Select the method to compensate time lags between anemometric measurements and any other high frequency measurements included in the raw files. Time lags arise due mainly to sensors physical distances and to the passage of air into sampling lines. Uncheck this box to instruct EddyPro not to compensate time lags (not recommended)."));
    timeLagCheckBox->setText(tr("Time lags compensation"));
    timeLagCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/time-lag-detect-correct.html"));

    timeLagMethodLabel = new ClickLabel(tr("Time lag detection method :"));
    timeLagMethodCombo = new QComboBox;
    timeLagMethodCombo->addItem(tr("Constant"));
    timeLagMethodCombo->addItem(tr("Covariance maximization with default"));
    timeLagMethodCombo->addItem(tr("Covariance maximization"));
    timeLagMethodCombo->addItem(tr("Automatic time lag optimization"));
    timeLagMethodCombo->setItemData(0, tr("<b>Constant:</b> EddyPro will apply constant time lags for all flux averaging intervals, using the <b><i>Nominal time lag</i></b> stored inside the GHG files or in the <b><i>Alternative metadata file</i></b> (for files other than GHG). While it can speed up the computation, this method is not recommended for physically displaced sensors or closed/enclosed path gas analysers. It can be used for closed/enclosed analysers if flow rate in the sampling line is strictly controlled and the sampling tube is actively heated to keep relative humidity low and constant."), Qt::ToolTipRole);
    timeLagMethodCombo->setItemData(1, tr("<b>Covariance maximization with default:</b> Similar to the <i>Covariance maximization</i>, this calculates the most likely time lag based on the circular correlation procedure. However, if a maximum of the covariance is not attained within the window (but at one of its ends), the time lag is set to the <b><i>Nominal time lag</i></b> value stored inside the GHG files or in the <b><i>Alternative metadata file</i></b> (for files other than GHG), for each variable. Recommended in most situations."), Qt::ToolTipRole);
    timeLagMethodCombo->setItemData(2, tr("<b>Covariance maximization:</b> Calculates the most likely time lag within a plausible window, based on the circular correlation procedure. The window is defined by the <i>Minimum time lags</i> and <i>Maximum time lags</i> stored inside the GHG files or in the <i>Alternative metadata file</i> (for files other than GHG), for each variable."), Qt::ToolTipRole);
    timeLagMethodCombo->setItemData(3, tr("<b>Automatic time lag optimization:</b> Select this option and configure it clicking on the <b><i>Time Lag Optimization Settings...</i></b> to instruct EddyPro to perform a statistical optimization of time lags. It will calculate nominal time lags and plausibility windows and apply them in the raw data processing step. For water vapor, the assessment is performed as a function of relative humidity."), Qt::ToolTipRole);
    timeLagMethodCombo->setEnabled(false);

    tlSettingsButton = new QPushButton(tr("Time Lag Optimization Settings..."));
    tlSettingsButton->setProperty("mdButton", true);
    tlSettingsButton->setMaximumWidth(tlSettingsButton->sizeHint().width());

    qcCheckBox = new RichTextCheckBox;
    qcCheckBox->setToolTip(tr("<b>Quality check:</b> Select the quality flagging policy. Flux quality flags are obtained from the combination of two partial flags that result from the application of the steady-state and the developed turbulence tests. Select the flag combination policy."));
    qcCheckBox->setText(tr("Quality check"));
    qcCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/flux-quality-flags.html"));

    qcLabel = new ClickLabel(tr("Flagging policy :"));
    qcMethodCombo = new QComboBox;
    qcMethodCombo->setToolTip(tr(R"(<b>Mauder and Foken 2004:</b> Policy described in the documentation of the TK2 Eddy Covariance software that also constituted the standard of the CarboEurope IP project and is widely adopted. "0" means high quality fluxes, "1" means fluxes are ok for budget analysis, "2" fluxes should be discarded from the result dataset.)"));
    qcMethodCombo->addItem(tr("Mauder and Foken (2004) (0-1-2 system)"));
    qcMethodCombo->addItem(tr("Foken (2003) (1 to 9 system)"));
    qcMethodCombo->addItem(tr("Goeckede et al. (2004) (1 to 5 system)"));
    qcMethodCombo->setItemData(0, tr(R"(<b>Mauder and Foken 2004:</b> Policy described in the documentation of the TK2 Eddy Covariance software that also constituted the standard of the CarboEurope IP project and is widely adopted. "0" means high quality fluxes, "1" means fluxes are ok for budget analysis, "2" fluxes should be discarded from the result dataset.)"), Qt::ToolTipRole);
    qcMethodCombo->setItemData(1, tr(R"(<b>Foken 2003:</b> A system based on 9 quality grades. "1" is best, "9" is worst. The system of Mauder and Foken (2004) and of Goeckede et al. (2006) are based on a rearrangement of these system.)"), Qt::ToolTipRole);
    qcMethodCombo->setItemData(2, tr(R"(<b>Goeckede et al., 2004:</b> A system based on 5 quality grades. "1" is best, "5" is worst.)"), Qt::ToolTipRole);

    fpCheckBox = new RichTextCheckBox;
    fpCheckBox->setToolTip(tr("<b>Footprint estimation:</b> Select whether to calculate flux footprint estimations and which method should be used. Flux crosswind-integrated footprints are provided as distances from the tower contributing for 10%, 30%, 50%, 70% and 90% to measured fluxes. Also, the location of the peak contribution is given."));
    fpCheckBox->setText(tr("Footprint estimation"));
    fpCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/estimating-flux-footprint.html"));

    fpLabel = new ClickLabel(tr("Footprint method :"));
    fpMethodCombo = new QComboBox;
    fpMethodCombo->setToolTip(tr("<b>Kljun et al. (2004):</b> A cross-wind integrated parameterization of footprint estimations obtained with a 3D Lagrangian model by means of a scaling procedure."));
    fpMethodCombo->addItem(tr("Kljun et al. (2004)"));
    fpMethodCombo->addItem(tr("Kormann and Meixner (2001)"));
    fpMethodCombo->addItem(tr("Hsieh et al. (2000)"));
    fpMethodCombo->setItemData(0, tr("<b>Kljun et al. (2004):</b> A cross-wind integrated parameterization of footprint estimations obtained with a 3D Lagrangian model by means of a scaling procedure."), Qt::ToolTipRole);
    fpMethodCombo->setItemData(1, tr("<b>Kormann and Meixner (2001):</b> A cross-wind integrated model based on the solution of the two dimensional advection-diffusion equation given by van Ulden (1978) and others for power-law profiles in wind velocity and eddy diffusivity."), Qt::ToolTipRole);
    fpMethodCombo->setItemData(2, tr("<b>Hsien et al. (2000):</b> A cross-wind integrated model based based on the former model of Gash (1986) and on simulations with a Lagrangian stochastic model."), Qt::ToolTipRole);

    wplCheckBox = new RichTextCheckBox;
    wplCheckBox->setToolTip(tr("<b>Compensate density fluctuations:</b> This is the so-called WPL correction (Webb et al., 1980). Choose whether to apply the compensation of density fluctuations to raw gas concentrations available as molar densities or mole fractions (moles gas per mole of wet air). The correction does not apply if raw concentrations are available as mixing ratios (mole gas per mole dry air)."));
    wplCheckBox->setText(tr("Compensate density fluctuations (WPL terms)"));
    wplCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/converting-to-mixing-ratio.html"));

    // burba correction
    burbaCorrCheckBox = new RichTextCheckBox;
    burbaCorrCheckBox->setToolTip(tr("<b>Add instrument sensible heat components, only for LI-7500:</b> Only applies to the LI-7500. It takes into account air density fluctuations due to temperature fluctuations induced by heat exchange processes at the instrument surfaces, as from Burba et al. (2008)."));
    burbaCorrCheckBox->setText(tr("Add instrument sensible heat components, only for LI-7500 "));
    burbaCorrCheckBox->setQuestionMark(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/calculate-offseason-uptake-correction.html"));

    burbaTypeLabel = new ClickLabel;
    burbaTypeLabel->setText(tr("Surface temperature estimation :"));
    burbaSimpleRadio = new QRadioButton(tr("Simple linear regressions"), this);
    burbaSimpleRadio->setToolTip(tr("<b>Simple linear regressions:</b> Instrument surface temperatures are estimated based on air temperature, using linear regressions as from Burba et al. 2008, eqs. 3-8. Default regression parameters are from Table 3 in the same paper. If you have experimental data for your LI-7500 unit, you may customize those values. Otherwise we suggest using default values."));
    burbaMultiRadio = new QRadioButton(tr("Multiple regressions"), this);
    burbaMultiRadio->setToolTip(tr("<b>Multiple regressions:</b> Instrument surface temperatures are estimated based on air temperature, global radiation, long-wave radiation and wind speed, as from Burba et al. 2008, Table 2. Default regression parameters are from the same table. If you have experimental data for your LI-7500 unit, you may customize those values. Otherwise we suggest using default values."));

    burbaRadioGroup = new QButtonGroup(this);
    burbaRadioGroup->addButton(burbaSimpleRadio, 0);
    burbaRadioGroup->addButton(burbaMultiRadio, 1);

    setDefaultsButton = new QPushButton(tr("Restore Default Values"));
    setDefaultsButton->setProperty("mdButton", true);
    setDefaultsButton->setMaximumWidth(setDefaultsButton->sizeHint().width());
    setDefaultsButton->setToolTip(tr("<b>Restore Default Values</b>: Resets the surface heating correction to the default values of Burba et al. (2008)."));

    auto defaultLabel = new QLabel(tr("Default values as from Burba et al. (2008)"));
    defaultLabel->setObjectName(QStringLiteral("citeLabel"));

    auto defaultContainerLayout = new QHBoxLayout;
    defaultContainerLayout->addWidget(setDefaultsButton);
    defaultContainerLayout->addWidget(defaultLabel);
    defaultContainerLayout->addStretch();

    auto defaultContainer = new QWidget;
    defaultContainer->setLayout(defaultContainerLayout);

    burbaSimpleDay = new QWidget;
    burbaSimpleNight = new QWidget;
    burbaMultiDay = new QWidget;
    burbaMultiNight = new QWidget;

    createBurbaParamItems();

    burbaSimpleTab = new QTabWidget;
    burbaSimpleTab->addTab(burbaSimpleDay, tr("Day time"));
    burbaSimpleTab->addTab(burbaSimpleNight, tr("Night time"));

    burbaMultiTab = new QTabWidget;
    burbaMultiTab->addTab(burbaMultiDay, tr("Day time"));
    burbaMultiTab->addTab(burbaMultiNight, tr("Night time"));

    burbaParamWidget = new QStackedWidget;
    burbaParamWidget->addWidget(burbaSimpleTab);
    burbaParamWidget->addWidget(burbaMultiTab);
    burbaParamWidget->setCurrentIndex(0);
    //
    auto wplTitle = new QLabel(tr("Compensation of density fluctuations"));
    wplTitle->setProperty("groupLabel", true);

    auto qcTitle = new QLabel(tr("Other options"));
    qcTitle->setProperty("groupLabel", true);

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));
    auto hrLabel_2 = new QLabel;
    hrLabel_2->setObjectName(QStringLiteral("hrLabel"));

    auto qBox_1 = new QHBoxLayout;
    qBox_1->addWidget(windOffsetLabel);
    qBox_1->addWidget(questionMark_1);
    qBox_1->addStretch();

    auto qBox_2 = new QHBoxLayout;
    qBox_2->addWidget(detrendLabel);
    qBox_2->addWidget(questionMark_2);
    qBox_2->addStretch();

    //
    auto toviLogo = new QPushButton;
    toviLogo->setObjectName(QStringLiteral("toviLogoImg"));
    connect(toviLogo, &QPushButton::clicked,
            this, &AdvProcessingOptions::openToviHomepage);

    auto toviAdsText = new QLabel(tr("For more advanced footprint tools and visualization, "
                                     "try <a href=\"https://tovi.io/?utm_source=EddyPro%20Software&utm_medium=Tovi%20Ads&utm_campaign=EP_Tovi_ads\">Tovi</a>"));
    toviAdsText->setProperty("toviAds", true);
    toviAdsText->setOpenExternalLinks(true);

    auto toviBox = new QHBoxLayout;
    toviBox->addWidget(toviLogo);
    toviBox->addWidget(toviAdsText);
    toviBox->addStretch();

    //
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(rawProcessingTitle, 0, 0);
    settingsLayout->addLayout(qBox_1, 1, 0, 1, 2);
    settingsLayout->addLayout(windComponentLayout, 1, 2, 1, 1);
    settingsLayout->addWidget(wBoostCheckBox, 2, 0);
    settingsLayout->addWidget(aoaCheckBox, 3, 0);
    settingsLayout->addWidget(aoaMethLabel, 3, 1, Qt::AlignRight);
    settingsLayout->addWidget(aoaMethCombo, 3, 2);
    settingsLayout->addWidget(rotCheckBox, 4, 0);
    settingsLayout->addWidget(rotMethLabel, 4, 1, Qt::AlignRight);
    settingsLayout->addWidget(rotMethCombo, 4, 2);
    settingsLayout->addWidget(pfSettingsButton, 4, 3);
    settingsLayout->addLayout(qBox_2, 5, 0);
    settingsLayout->addWidget(detrendMethLabel, 5, 1, Qt::AlignRight);
    settingsLayout->addWidget(detrendCombo, 5, 2);
    settingsLayout->addWidget(timeConstantLabel, 6, 1, Qt::AlignRight);
    settingsLayout->addWidget(timeConstantSpin, 6, 2);
    settingsLayout->addWidget(timeLagCheckBox, 7, 0);
    settingsLayout->addWidget(timeLagMethodLabel, 7, 1, Qt::AlignRight);
    settingsLayout->addWidget(timeLagMethodCombo, 7, 2);
    settingsLayout->addWidget(tlSettingsButton, 7, 3);
    settingsLayout->addWidget(hrLabel, 8, 0, 1, 4);
    settingsLayout->addWidget(wplTitle, 9, 0);
    settingsLayout->addWidget(wplCheckBox, 10, 0);
    settingsLayout->addWidget(burbaCorrCheckBox, 11, 0);
    settingsLayout->addWidget(burbaTypeLabel, 12, 0, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(burbaSimpleRadio, 12, 1);
    settingsLayout->addWidget(burbaMultiRadio, 13, 1);
    settingsLayout->addWidget(burbaParamWidget, 14, 0, 1, 4);
    settingsLayout->addWidget(defaultContainer, 15, 0, 1, 4);
    settingsLayout->addWidget(hrLabel_2, 16, 0, 1, 4);

    settingsLayout->addWidget(qcTitle, 22, 0);
    settingsLayout->addWidget(qcCheckBox, 23, 0);
    settingsLayout->addWidget(qcLabel, 23, 1, Qt::AlignRight);
    settingsLayout->addWidget(qcMethodCombo, 23, 2);
    settingsLayout->addWidget(fpCheckBox, 24, 0);
    settingsLayout->addWidget(fpLabel, 24, 1, Qt::AlignRight);
    settingsLayout->addWidget(fpMethodCombo, 24, 2);
    settingsLayout->addLayout(toviBox, 25, 0, 1, -1);
    settingsLayout->setRowStretch(26, 1);
    settingsLayout->setColumnStretch(4, 1);

    //    auto overallFrame = new QWidget;
    //    overallFrame->setProperty("scrollContainerWidget", true);
    //    overallFrame->setLayout(settingsLayout);

    //    auto scrollArea = new QScrollArea;
    //    scrollArea->setWidget(overallFrame);
    //    scrollArea->setWidgetResizable(true);

    //    auto settingsGroupLayout = new QHBoxLayout;
    //    settingsGroupLayout->addWidget(scrollArea);

    auto settingsGroupLayout = new QHBoxLayout;
    settingsGroupLayout->addWidget(WidgetUtils::getContainerScrollArea(this, settingsLayout));

    auto settingsGroupTitle = new QLabel(tr("Raw Processing Options"));
    settingsGroupTitle->setProperty("groupTitle2", true);

    auto qBox_11 = new QHBoxLayout;
    qBox_11->addWidget(settingsGroupTitle, 0, Qt::AlignRight | Qt::AlignBottom);
    qBox_11->addWidget(questionMark_3);
    qBox_11->addStretch();

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(qBox_11);
    mainLayout->addLayout(settingsGroupLayout);
    mainLayout->setContentsMargins(15, 15, 15, 10);
    setLayout(mainLayout);

    connect(uLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onULabelClicked);
    connect(vLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onVLabelClicked);
    connect(wLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onWLabelClicked);

    connect(uOffsetSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateUOffset(double)));
    connect(vOffsetSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateVOffset(double)));
    connect(wOffsetSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateWOffset(double)));

    connect(wBoostCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateWBoost);
    connect(aoaCheckBox, &RichTextCheckBox::toggled,
            aoaMethLabel, &ClickLabel::setEnabled);
    connect(aoaCheckBox, &RichTextCheckBox::toggled,
            aoaMethCombo, &QComboBox::setEnabled);
    connect(aoaMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickAoaMethLabel);
    connect(aoaCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateAoaMethod_1);
    connect(aoaMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateAoaMethod_2(int)));

    connect(rotCheckBox, &RichTextCheckBox::toggled,
            rotMethLabel, &ClickLabel::setEnabled);
    connect(rotCheckBox, &RichTextCheckBox::toggled,
            rotMethCombo, &QComboBox::setEnabled);
    connect(rotMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickRotMethLabel);
    connect(rotCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateRotMethod_1);
    connect(rotMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateRotMethod_2(int)));
    connect(rotCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updatePfSettingsButton);

    connect(pfSettingsButton, &QPushButton::clicked,
            this, &AdvProcessingOptions::showPfSettingsDialog);
    connect(tlSettingsButton, &QPushButton::clicked,
            this, &AdvProcessingOptions::showTlSettingsDialog);

    connect(detrendMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickDetrendLabel);
    connect(timeConstantLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickTimeConstantLabel);
    connect(detrendCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onClickDetrendCombo(int)));
    connect(detrendCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateDetrendMeth(int)));

    connect(timeConstantSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateTimeConst(double)));

    connect(timeLagCheckBox, &RichTextCheckBox::toggled,
            timeLagMethodLabel, &ClickLabel::setEnabled);
    connect(timeLagCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateTlSettingsButton);
    connect(timeLagCheckBox, &RichTextCheckBox::toggled,
            timeLagMethodCombo, &QComboBox::setEnabled);
    connect(timeLagMethodLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickTimeLagMethLabel);
    connect(timeLagCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateTlagMeth_1);
    connect(timeLagMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTlagMeth_2(int)));

    connect(qcCheckBox, &RichTextCheckBox::toggled,
            qcLabel, &ClickLabel::setEnabled);
    connect(qcCheckBox, &RichTextCheckBox::toggled,
            qcMethodCombo, &QComboBox::setEnabled);
    connect(qcLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickQcMethodLabel);
    connect(qcCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateQcMeth_1);
    connect(qcMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateQcMeth_2(int)));

    connect(fpCheckBox, &RichTextCheckBox::toggled,
            fpLabel, &ClickLabel::setEnabled);
    connect(fpCheckBox, &RichTextCheckBox::toggled,
            fpMethodCombo, &QComboBox::setEnabled);
    connect(fpLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickFpMethodLabel);
    connect(fpCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateFpMeth_1);
    connect(fpMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFpMeth_2(int)));

    connect(wplCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateWplMeth_1);
    connect(wplCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::updateBurbaGroup);
    connect(wplCheckBox, &RichTextCheckBox::toggled,
            [=](bool b) { burbaCorrCheckBox->setEnabled(b); });
    connect(burbaCorrCheckBox, &RichTextCheckBox::toggled, [=](bool checked) { ecProject_->setScreenBuCorr(checked); });
    connect(burbaCorrCheckBox, &RichTextCheckBox::toggled,
            this, &AdvProcessingOptions::enableBurbaCorrectionArea);
    connect(burbaRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateBurbaType_2(int)));
    connect(setDefaultsButton, &QPushButton::clicked,
            this, &AdvProcessingOptions::on_setDefaultsButton_clicked);

    connect(ecProject_, &EcProject::ecProjectNew,
            this, &AdvProcessingOptions::reset);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &AdvProcessingOptions::refresh);

    auto combo_list = QWidgetList() << aoaMethCombo
                                    << rotMethCombo
                                    << detrendCombo
                                    << timeLagMethodCombo
                                    << qcMethodCombo
                                    << fpMethodCombo;
    for (auto widget : combo_list)
    {
        auto combo = dynamic_cast<QComboBox *>(widget);
        connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(updateTooltip(int)));
    }

    createPfSettingsDialog();
    createTlSettingsDialog();
    QTimer::singleShot(0, this, SLOT(reset()));
}

AdvProcessingOptions::~AdvProcessingOptions()
{
    delete pfDialog_;
    delete tlDialog_;
}

void AdvProcessingOptions::updateUOffset(double d)
{
    ecProject_->setScreenUOffset(d);
}

void AdvProcessingOptions::updateVOffset(double d)
{
    ecProject_->setScreenVOffset(d);
}

void AdvProcessingOptions::updateWOffset(double d)
{
    ecProject_->setScreenWOffset(d);
}

// update project properties
void AdvProcessingOptions::updateAoaMethod_1(bool b)
{
    if (b)
    {
        auto value = aoaMethCombo->itemData(aoaMethCombo->currentIndex());
        ecProject_->setScreenFlowDistortion(value.toInt());
    }
    else
    {
        ecProject_->setScreenFlowDistortion(0);
    }
}

// update project properties
void AdvProcessingOptions::updateAoaMethod_2(int n)
{
    auto value = aoaMethCombo->itemData(n);
    ecProject_->setScreenFlowDistortion(value.toInt());
}

// update project properties
void AdvProcessingOptions::updateRotMethod_1(bool b)
{
    if (b)
    {
        ecProject_->setScreenRotMethod(rotMethCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setScreenRotMethod(0);
    }
}

// update project properties and fluxes rotation choices
void AdvProcessingOptions::updateRotMethod_2(int n)
{
    ecProject_->setScreenRotMethod(n + 1);

    // planar fit
    pfSettingsButton->setEnabled((n == 2) || (n == 3));
}

void AdvProcessingOptions::updatePfSettingsButton(bool b)
{
    int n = rotMethCombo->currentIndex();
    if (b)
        pfSettingsButton->setEnabled((n == 2) || (n == 3));
    else
        pfSettingsButton->setEnabled(false);
}

void AdvProcessingOptions::updateTlSettingsButton(bool b)
{
    int n = timeLagMethodCombo->currentIndex();
    if (b)
        tlSettingsButton->setEnabled(n == 3);
    else
        tlSettingsButton->setEnabled(false);
}

void AdvProcessingOptions::updateDetrendMeth(int l)
{
    ecProject_->setScreenDetrendMeth(l);
    previousDetrendMethod_ = static_cast<DetrendMethod>(l);
}

void AdvProcessingOptions::updateTimeConst(double l)
{
    // write [min] on the GUI but [sec] in the file
    if (detrendCombo->currentIndex() == 1)
    {
        Q_ASSERT_X(detrendCombo->currentIndex() == 1,
                   "detrending",
                   "save linear time constant in sec");
        ecProject_->setScreenTimeConst(l * 60.0);
    }
    // write in [sec]
    else
    {
        ecProject_->setScreenTimeConst(l);
    }
}

void AdvProcessingOptions::updateTlagMeth_1(bool b)
{
    if (b)
    {
        ecProject_->setScreenTlagMeth(timeLagMethodCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setScreenTlagMeth(0);
    }
}

void AdvProcessingOptions::updateTlagMeth_2(int n)
{
    ecProject_->setScreenTlagMeth(n + 1);

    // timelag optimization button
    tlSettingsButton->setEnabled(n == 3);
}

void AdvProcessingOptions::onClickDetrendCombo(int newDetrendMethod)
{
    auto currDetrendMethod = static_cast<DetrendMethod>(newDetrendMethod);

    if (previousDetrendMethod_ == DetrendMethod::LinearDetrending)
    {
        if (timeConstantSpin->value() == 0.0)
            timeConstantSpin->setValue(250.0);
        else
            timeConstantSpin->setValue(timeConstantSpin->value() * 60.0);
    }
    else if (currDetrendMethod == DetrendMethod::LinearDetrending)
    {
        timeConstantSpin->setValue(timeConstantSpin->value() / 60.0);
    }

    if (currDetrendMethod == DetrendMethod::BlockAverage)
    {
        timeConstantSpin->setSingleStep(10.0);
        timeConstantSpin->setSuffix(tr("  [s]", "Second"));
        timeConstantLabel->setEnabled(false);
        timeConstantSpin->setEnabled(false);
    }
    else if (currDetrendMethod == DetrendMethod::LinearDetrending)
    {
        if (qFuzzyCompare(timeConstantSpin->value(), 4.2))
        {
            timeConstantSpin->setValue(0.0);
        }
        timeConstantSpin->setSingleStep(1.0);
        timeConstantSpin->setSuffix(tr("  [min]", "Minute"));
        timeConstantLabel->setEnabled(true);
        timeConstantSpin->setEnabled(true);
        timeConstantSpin->setFocus();
        timeConstantSpin->selectAll();
    }
    else
    {
        timeConstantSpin->setSingleStep(10.0);
        timeConstantSpin->setSuffix(tr("  [s]", "Second"));
        timeConstantLabel->setEnabled(true);
        timeConstantSpin->setEnabled(true);
        timeConstantSpin->setFocus();
        timeConstantSpin->selectAll();
    }

    // possibly write the new correct value in minutes or seconds
    updateTimeConst(timeConstantSpin->value());
}

void AdvProcessingOptions::onClickDetrendLabel()
{
    detrendCombo->setFocus();
    detrendCombo->showPopup();
}

void AdvProcessingOptions::onClickTimeConstantLabel()
{
    if (timeConstantSpin->isEnabled())
    {
        timeConstantSpin->setFocus();
        timeConstantSpin->selectAll();
    }
}

void AdvProcessingOptions::updateWBoost(bool b)
{
    ecProject_->setScreenWBoost(b);
}

void AdvProcessingOptions::onClickAoaMethLabel()
{
    if (aoaMethCombo->isEnabled())
    {
        aoaMethCombo->showPopup();
    }
}

void AdvProcessingOptions::onClickRotMethLabel()
{
    if (rotMethCombo->isEnabled())
    {
        rotMethCombo->showPopup();
    }
}

void AdvProcessingOptions::onClickTimeLagMethLabel()
{
    if (timeLagMethodCombo->isEnabled())
    {
        timeLagMethodCombo->showPopup();
    }
}

void AdvProcessingOptions::onULabelClicked()
{
    uOffsetSpin->setFocus();
    uOffsetSpin->selectAll();
}

void AdvProcessingOptions::onVLabelClicked()
{
    vOffsetSpin->setFocus();
    vOffsetSpin->selectAll();
}

void AdvProcessingOptions::onWLabelClicked()
{
    wOffsetSpin->setFocus();
    wOffsetSpin->selectAll();
}

void AdvProcessingOptions::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    uOffsetSpin->setValue(ecProject_->defaultSettings.screenSetting.u_offset);
    vOffsetSpin->setValue(ecProject_->defaultSettings.screenSetting.v_offset);
    wOffsetSpin->setValue(ecProject_->defaultSettings.screenSetting.w_offset);

    wBoostCheckBox->setChecked(ecProject_->defaultSettings.screenSetting.gill_wm_wboost);
    aoaCheckBox->setChecked(ecProject_->defaultSettings.screenSetting.flow_distortion);
    aoaMethCombo->setCurrentIndex(0);

    rotCheckBox->setChecked(true);
    rotMethCombo->setCurrentIndex(0);
    pfSettingsButton->setEnabled(false);

    detrendCombo->setCurrentIndex(ecProject_->defaultSettings.screenSetting.detrend_meth);

    timeConstantSpin->setValue(ecProject_->defaultSettings.screenSetting.timeconst);

    timeLagCheckBox->setChecked(true);
    timeLagMethodCombo->setCurrentIndex(1);
    tlSettingsButton->setEnabled(false);

    pfDialog_->reset();
    tlDialog_->reset();

    qcLabel->setEnabled(true);
    qcCheckBox->setChecked(true);
    qcMethodCombo->setCurrentIndex(0);

    fpLabel->setEnabled(true);
    fpCheckBox->setChecked(true);
    fpMethodCombo->setCurrentIndex(0);

    wplCheckBox->setChecked(ecProject_->defaultSettings.projectGeneral.wpl_meth);

    setBurbaDefaultValues();

    burbaCorrCheckBox->setEnabled(true);
    burbaCorrCheckBox->setChecked(false);

    enableBurbaCorrectionArea(false);

    burbaSimpleRadio->setChecked(true);
    burbaParamWidget->setCurrentIndex(0);
    burbaSimpleTab->setCurrentIndex(0);
    burbaMultiTab->setCurrentIndex(0);

    WidgetUtils::updateComboItemTooltip(aoaMethCombo, 0);
    WidgetUtils::updateComboItemTooltip(rotMethCombo, 0);
    WidgetUtils::updateComboItemTooltip(detrendCombo, 0);
    WidgetUtils::updateComboItemTooltip(timeLagMethodCombo, 1);
    WidgetUtils::updateComboItemTooltip(qcMethodCombo, 0);
    WidgetUtils::updateComboItemTooltip(fpMethodCombo, 0);

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void AdvProcessingOptions::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    uOffsetSpin->setValue(ecProject_->screenUOffset());
    vOffsetSpin->setValue(ecProject_->screenVOffset());
    wOffsetSpin->setValue(ecProject_->screenWOffset());

    wBoostCheckBox->setChecked(ecProject_->screenWBoost());

    auto aoaCorrection = ecProject_->screenFlowDistortion();
    aoaCheckBox->setChecked(aoaCorrection);
    if (aoaCorrection)
    {
        switch (aoaCorrection)
        {
        // nakai 2012
        case 1:
            aoaMethCombo->setCurrentIndex(1);
            break;
        // nakai 2006
        case 2:
            aoaMethCombo->setCurrentIndex(2);
            break;
        // automatic
        case -1:
            aoaMethCombo->setCurrentIndex(0);
            break;
        }
    }
    else
    {
        aoaMethCombo->setCurrentIndex(0);
    }

    rotCheckBox->setChecked(ecProject_->screenRotMethod());
    if (ecProject_->screenRotMethod())
    {
        rotMethCombo->setCurrentIndex(ecProject_->screenRotMethod() - 1);
    }
    else
    {
        rotMethCombo->setCurrentIndex(0);
    }

    pfSettingsButton->setEnabled((ecProject_->screenRotMethod() == 3) || (ecProject_->screenRotMethod() == 4));

    timeConstantSpin->setValue(ecProject_->screenTimeConst());
    detrendCombo->setCurrentIndex(ecProject_->screenDetrendMeth());

    timeLagCheckBox->setChecked(ecProject_->screenTlagMeth());
    if (ecProject_->screenTlagMeth())
    {
        timeLagMethodCombo->setCurrentIndex(ecProject_->screenTlagMeth() - 1);
    }
    else
    {
        timeLagMethodCombo->setCurrentIndex(0);
    }
    tlSettingsButton->setEnabled(ecProject_->screenTlagMeth() == 4);

    qcCheckBox->setChecked(ecProject_->generalQcfMeth());
    if (ecProject_->generalQcfMeth())
    {
        qcMethodCombo->setCurrentIndex(ecProject_->generalQcfMeth() - 1);
    }
    else
    {
        qcMethodCombo->setCurrentIndex(0);
    }

    fpCheckBox->setChecked(ecProject_->generalFpMeth());
    if (ecProject_->generalFpMeth())
    {
        fpMethodCombo->setCurrentIndex(ecProject_->generalFpMeth() - 1);
    }
    else
    {
        fpMethodCombo->setCurrentIndex(0);
    }

    wplCheckBox->setChecked(ecProject_->generalWplMeth());

    burbaCorrCheckBox->setChecked(ecProject_->screenBuCorr());
    burbaCorrCheckBox->setEnabled(ecProject_->generalWplMeth());

    burbaRadioGroup->buttons().at(ecProject_->screenBuMulti())->setChecked(true);

    burbaParamWidget->setCurrentIndex(ecProject_->screenBuMulti());
    burbaSimpleTab->setCurrentIndex(0);
    burbaMultiTab->setCurrentIndex(0);

    burbaTypeLabel->setEnabled(wplCheckBox->isChecked() && burbaCorrCheckBox->isChecked());
    burbaSimpleRadio->setEnabled(wplCheckBox->isChecked() && burbaCorrCheckBox->isChecked());
    burbaParamWidget->setEnabled(wplCheckBox->isChecked() && burbaCorrCheckBox->isChecked());
    burbaMultiTab->setEnabled(wplCheckBox->isChecked() && burbaCorrCheckBox->isChecked());
    setDefaultsButton->setEnabled(wplCheckBox->isChecked() && burbaCorrCheckBox->isChecked());

    lDayBotGain->setText(QString::number(ecProject_->screenLDayBotGain(), 'f', 3));
    lDayBotOffset->setText(QString::number(ecProject_->screenLDayBotOffset(), 'f', 2));
    lDayTopGain->setText(QString::number(ecProject_->screenLDayTopGain(), 'f', 3));
    lDayTopOffset->setText(QString::number(ecProject_->screenLDayTopOffset(), 'f', 2));
    lDaySparGain->setText(QString::number(ecProject_->screenLDaySparGain(), 'f', 3));
    lDaySparOffset->setText(QString::number(ecProject_->screenLDaySparOffset(), 'f', 2));
    lNightBotGain->setText(QString::number(ecProject_->screenLNightBotGain(), 'f', 3));
    lNightBotOffset->setText(QString::number(ecProject_->screenLNightBotOffset(), 'f', 2));
    lNightTopGain->setText(QString::number(ecProject_->screenLNightTopGain(), 'f', 3));
    lNightTopOffset->setText(QString::number(ecProject_->screenLNightTopOffset(), 'f', 2));
    lNightSparGain->setText(QString::number(ecProject_->screenLNightSparGain(), 'f', 3));
    lNightSparOffset->setText(QString::number(ecProject_->screenLNightSparOffset(), 'f', 2));
    mDayBot1->setText(QString::number(ecProject_->screenMDayBot1(), 'f', 1));
    mDayBot2->setText(QString::number(ecProject_->screenMDayBot2(), 'f', 4));
    mDayBot3->setText(QString::number(ecProject_->screenMDayBot3(), 'f', 4));
    mDayBot4->setText(QString::number(ecProject_->screenMDayBot4(), 'f', 3));
    mDayTop1->setText(QString::number(ecProject_->screenMDayTop1(), 'f', 1));
    mDayTop2->setText(QString::number(ecProject_->screenMDayTop2(), 'f', 4));
    mDayTop3->setText(QString::number(ecProject_->screenMDayTop3(), 'f', 4));
    mDayTop4->setText(QString::number(ecProject_->screenMDayTop4(), 'f', 3));
    mDaySpar1->setText(QString::number(ecProject_->screenMDaySpar1(), 'f', 1));
    mDaySpar2->setText(QString::number(ecProject_->screenMDaySpar2(), 'f', 4));
    mDaySpar3->setText(QString::number(ecProject_->screenMDaySpar3(), 'f', 4));
    mDaySpar4->setText(QString::number(ecProject_->screenMDaySpar4(), 'f', 3));
    mNightBot1->setText(QString::number(ecProject_->screenMNightBot1(), 'f', 1));
    mNightBot2->setText(QString::number(ecProject_->screenMNightBot2(), 'f', 4));
    mNightBot3->setText(QString::number(ecProject_->screenMNightBot3(), 'f', 4));
    mNightBot4->setText(QString::number(ecProject_->screenMNightBot4(), 'f', 3));
    mNightTop1->setText(QString::number(ecProject_->screenMNightTop1(), 'f', 1));
    mNightTop2->setText(QString::number(ecProject_->screenMNightTop2(), 'f', 4));
    mNightTop3->setText(QString::number(ecProject_->screenMNightTop3(), 'f', 4));
    mNightTop4->setText(QString::number(ecProject_->screenMNightTop4(), 'f', 3));
    mNightSpar1->setText(QString::number(ecProject_->screenMNightSpar1(), 'f', 1));
    mNightSpar2->setText(QString::number(ecProject_->screenMNightSpar2(), 'f', 4));
    mNightSpar3->setText(QString::number(ecProject_->screenMNightSpar3(), 'f', 4));
    mNightSpar4->setText(QString::number(ecProject_->screenMNightSpar4(), 'f', 3));

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void AdvProcessingOptions::createPfSettingsDialog()
{
    if (!pfDialog_)
    {
        pfDialog_ = new PlanarFitSettingsDialog(this, ecProject_, configState_);
    }
}

void AdvProcessingOptions::showPfSettingsDialog()
{
    pfDialog_->refresh();
    pfDialog_->show();
    pfDialog_->raise();
    pfDialog_->activateWindow();
}

void AdvProcessingOptions::createTlSettingsDialog()
{
    if (!tlDialog_)
    {
        tlDialog_ = new TimeLagSettingsDialog(this, ecProject_, configState_);
    }
}

void AdvProcessingOptions::showTlSettingsDialog()
{
    tlDialog_->refresh();
    tlDialog_->show();
    tlDialog_->raise();
    tlDialog_->activateWindow();
}

void AdvProcessingOptions::onClickQcMethodLabel()
{
    if (qcMethodCombo->isEnabled())
    {
        qcMethodCombo->showPopup();
    }
}

void AdvProcessingOptions::updateQcMeth_1(bool b)
{
    if (b)
    {
        ecProject_->setGeneralQcMeth(qcMethodCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setGeneralQcMeth(0);
    }
}

void AdvProcessingOptions::updateQcMeth_2(int n)
{
    ecProject_->setGeneralQcMeth(n + 1);
}

void AdvProcessingOptions::onClickFpMethodLabel()
{
    if (fpMethodCombo->isEnabled())
    {
        fpMethodCombo->showPopup();
    }
}

void AdvProcessingOptions::updateFpMeth_1(bool b)
{
    if (b)
    {
        ecProject_->setGeneralFpMeth(fpMethodCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setGeneralFpMeth(0);
    }
}

void AdvProcessingOptions::updateFpMeth_2(int n)
{
    ecProject_->setGeneralFpMeth(n + 1);
}

void AdvProcessingOptions::createBurbaParamItems()
{
    auto simpleDayGrid = new QGridLayout;
    auto simpleNightGrid = new QGridLayout;
    auto multiDayGrid = new QGridLayout;
    auto multiNightGrid = new QGridLayout;

    simpleDayGrid->addWidget(new QLabel(tr("Bottom :"), this), 0, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>bot</sub> = "), this), 0, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 0, 3, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr("Top :"), this), 1, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>top</sub> = "), this), 1, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 1, 3, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr("Spar :"), this), 2, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>spar</sub> = "), this), 2, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 2, 3, 1, 1);

    simpleNightGrid->addWidget(new QLabel(tr("Bottom :"), this), 0, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>bot</sub> = "), this), 0, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 0, 3, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr("Top :"), this), 1, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>top</sub> = "), this), 1, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 1, 3, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr("Spar :"), this), 2, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>spar</sub> = "), this), 2, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 2, 3, 1, 1);

    // matching floating point number with no exponents
    QString floatingPointRegexp = QStringLiteral("[-+]?[0-9]*\\.?[0-9]+");
    // TODO: use a QDoubleValidator with also range specs

    lDayBotGain = new CustomResetLineEdit;
    lDayBotGain->setMaxLength(10);
    lDayBotGain->setRegExp(floatingPointRegexp);
    lDayBotOffset = new CustomResetLineEdit;
    lDayBotOffset->setMaxLength(10);
    lDayBotOffset->setRegExp(floatingPointRegexp);
    lDayTopGain = new CustomResetLineEdit;
    lDayTopGain->setMaxLength(10);
    lDayTopGain->setRegExp(floatingPointRegexp);
    lDayTopOffset = new CustomResetLineEdit;
    lDayTopOffset->setMaxLength(10);
    lDayTopOffset->setRegExp(floatingPointRegexp);
    lDaySparGain = new CustomResetLineEdit;
    lDaySparGain->setMaxLength(10);
    lDaySparGain->setRegExp(floatingPointRegexp);
    lDaySparOffset = new CustomResetLineEdit;
    lDaySparOffset->setMaxLength(10);
    lDaySparOffset->setRegExp(floatingPointRegexp);

    simpleDayGrid->addWidget(lDayBotGain, 0, 2, 1, 1);
    simpleDayGrid->addWidget(lDayBotOffset, 0, 4, 1, 1);
    simpleDayGrid->addWidget(lDayTopGain, 1, 2, 1, 1);
    simpleDayGrid->addWidget(lDayTopOffset, 1, 4, 1, 1);
    simpleDayGrid->addWidget(lDaySparGain, 2, 2, 1, 1);
    simpleDayGrid->addWidget(lDaySparOffset, 2, 4, 1, 1);
    simpleDayGrid->setColumnStretch(5, 1);

    lNightBotGain = new CustomResetLineEdit;
    lNightBotGain->setMaxLength(10);
    lNightBotGain->setRegExp(floatingPointRegexp);
    lNightBotOffset = new CustomResetLineEdit;
    lNightBotOffset->setMaxLength(10);
    lNightBotOffset->setRegExp(floatingPointRegexp);
    lNightTopGain = new CustomResetLineEdit;
    lNightTopGain->setMaxLength(10);
    lNightTopGain->setRegExp(floatingPointRegexp);
    lNightTopOffset = new CustomResetLineEdit;
    lNightTopOffset->setMaxLength(10);
    lNightTopOffset->setRegExp(floatingPointRegexp);
    lNightSparGain = new CustomResetLineEdit;
    lNightSparGain->setMaxLength(10);
    lNightSparGain->setRegExp(floatingPointRegexp);
    lNightSparOffset = new CustomResetLineEdit;
    lNightSparOffset->setMaxLength(10);
    lNightSparOffset->setRegExp(floatingPointRegexp);

    simpleNightGrid->addWidget(lNightBotGain, 0, 2, 1, 1);
    simpleNightGrid->addWidget(lNightBotOffset, 0, 4, 1, 1);
    simpleNightGrid->addWidget(lNightTopGain, 1, 2, 1, 1);
    simpleNightGrid->addWidget(lNightTopOffset, 1, 4, 1, 1);
    simpleNightGrid->addWidget(lNightSparGain, 2, 2, 1, 1);
    simpleNightGrid->addWidget(lNightSparOffset, 2, 4, 1, 1);
    simpleNightGrid->setColumnStretch(5, 1);

    multiDayGrid->addWidget(new QLabel(tr("Bottom :"), this), 0, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>bot</sub> - T<sub>a</sub> = "), this), 0, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" + "), this), 0, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 0, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + "), this), 0, 7, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("* U"), this), 0, 9, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("Top :"), this), 1, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>top</sub> - T<sub>a</sub> = "), this), 1, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" + "), this), 1, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 1, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + "), this), 1, 7, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("* U"), this), 1, 9, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("Spar :"), this), 2, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>spar</sub> - T<sub>a</sub> = "), this), 2, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" + "), this), 2, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 2, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + "), this), 2, 7, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("* U"), this), 2, 9, 1, 1);

    mDayBot1 = new CustomResetLineEdit;
    mDayBot1->setMaxLength(10);
    mDayBot1->setRegExp(floatingPointRegexp);
    mDayBot2 = new CustomResetLineEdit;
    mDayBot2->setMaxLength(10);
    mDayBot2->setRegExp(floatingPointRegexp);
    mDayBot3 = new CustomResetLineEdit;
    mDayBot3->setMaxLength(10);
    mDayBot3->setRegExp(floatingPointRegexp);
    mDayBot4 = new CustomResetLineEdit;
    mDayBot4->setMaxLength(10);
    mDayBot4->setRegExp(floatingPointRegexp);
    mDayTop1 = new CustomResetLineEdit;
    mDayTop1->setMaxLength(10);
    mDayTop1->setRegExp(floatingPointRegexp);
    mDayTop2 = new CustomResetLineEdit;
    mDayTop2->setMaxLength(10);
    mDayTop2->setRegExp(floatingPointRegexp);
    mDayTop3 = new CustomResetLineEdit;
    mDayTop3->setMaxLength(10);
    mDayTop3->setRegExp(floatingPointRegexp);
    mDayTop4 = new CustomResetLineEdit;
    mDayTop4->setMaxLength(10);
    mDayTop4->setRegExp(floatingPointRegexp);
    mDaySpar1 = new CustomResetLineEdit;
    mDaySpar1->setMaxLength(10);
    mDaySpar1->setRegExp(floatingPointRegexp);
    mDaySpar2 = new CustomResetLineEdit;
    mDaySpar2->setMaxLength(10);
    mDaySpar2->setRegExp(floatingPointRegexp);
    mDaySpar3 = new CustomResetLineEdit;
    mDaySpar3->setMaxLength(10);
    mDaySpar3->setRegExp(floatingPointRegexp);
    mDaySpar4 = new CustomResetLineEdit;
    mDaySpar4->setMaxLength(10);
    mDaySpar4->setRegExp(floatingPointRegexp);

    multiDayGrid->addWidget(mDayBot1, 0, 2, 1, 1);
    multiDayGrid->addWidget(mDayBot2, 0, 4, 1, 1);
    multiDayGrid->addWidget(mDayBot3, 0, 6, 1, 1);
    multiDayGrid->addWidget(mDayBot4, 0, 8, 1, 1);
    multiDayGrid->addWidget(mDayTop1, 1, 2, 1, 1);
    multiDayGrid->addWidget(mDayTop2, 1, 4, 1, 1);
    multiDayGrid->addWidget(mDayTop3, 1, 6, 1, 1);
    multiDayGrid->addWidget(mDayTop4, 1, 8, 1, 1);
    multiDayGrid->addWidget(mDaySpar1, 2, 2, 1, 1);
    multiDayGrid->addWidget(mDaySpar2, 2, 4, 1, 1);
    multiDayGrid->addWidget(mDaySpar3, 2, 6, 1, 1);
    multiDayGrid->addWidget(mDaySpar4, 2, 8, 1, 1);
    multiDayGrid->setColumnMinimumWidth(2, 75);
    multiDayGrid->setColumnMinimumWidth(4, 75);
    multiDayGrid->setColumnMinimumWidth(6, 75);
    multiDayGrid->setColumnMinimumWidth(8, 75);
    multiDayGrid->setColumnStretch(9, 1);

    multiNightGrid->addWidget(new QLabel(tr("Bottom :"), this), 0, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>bot</sub> - T<sub>a</sub> = "), this), 0, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" + "), this), 0, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 0, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + "), this), 0, 7, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("* U"), this), 0, 9, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("Top :"), this), 1, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>top</sub> - T<sub>a</sub> = "), this), 1, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" + "), this), 1, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 1, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + "), this), 1, 7, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("* U"), this), 1, 9, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("Spar :"), this), 2, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr(" + "), this), 2, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>spar</sub> - T<sub>a</sub> = "), this), 2, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + "), this), 2, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + "), this), 2, 7, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("* U"), this), 2, 9, 1, 1);

    mNightBot1 = new CustomResetLineEdit;
    mNightBot1->setMaxLength(10);
    mNightBot1->setRegExp(floatingPointRegexp);
    mNightBot2 = new CustomResetLineEdit;
    mNightBot2->setMaxLength(10);
    mNightBot2->setRegExp(floatingPointRegexp);
    mNightBot3 = new CustomResetLineEdit;
    mNightBot3->setMaxLength(10);
    mNightBot3->setRegExp(floatingPointRegexp);
    mNightBot4 = new CustomResetLineEdit;
    mNightBot4->setMaxLength(10);
    mNightBot4->setRegExp(floatingPointRegexp);
    mNightTop1 = new CustomResetLineEdit;
    mNightTop1->setMaxLength(10);
    mNightTop1->setRegExp(floatingPointRegexp);
    mNightTop2 = new CustomResetLineEdit;
    mNightTop2->setMaxLength(10);
    mNightTop2->setRegExp(floatingPointRegexp);
    mNightTop3 = new CustomResetLineEdit;
    mNightTop3->setMaxLength(10);
    mNightTop3->setRegExp(floatingPointRegexp);
    mNightTop4 = new CustomResetLineEdit;
    mNightTop4->setMaxLength(10);
    mNightTop4->setRegExp(floatingPointRegexp);
    mNightSpar1 = new CustomResetLineEdit;
    mNightSpar1->setMaxLength(10);
    mNightSpar1->setRegExp(floatingPointRegexp);
    mNightSpar2 = new CustomResetLineEdit;
    mNightSpar2->setMaxLength(10);
    mNightSpar2->setRegExp(floatingPointRegexp);
    mNightSpar3 = new CustomResetLineEdit;
    mNightSpar3->setMaxLength(10);
    mNightSpar3->setRegExp(floatingPointRegexp);
    mNightSpar4 = new CustomResetLineEdit;
    mNightSpar4->setMaxLength(10);
    mNightSpar4->setRegExp(floatingPointRegexp);

    multiNightGrid->addWidget(mNightBot1, 0, 2, 1, 1);
    multiNightGrid->addWidget(mNightBot2, 0, 4, 1, 1);
    multiNightGrid->addWidget(mNightBot3, 0, 6, 1, 1);
    multiNightGrid->addWidget(mNightBot4, 0, 8, 1, 1);
    multiNightGrid->addWidget(mNightTop1, 1, 2, 1, 1);
    multiNightGrid->addWidget(mNightTop2, 1, 4, 1, 1);
    multiNightGrid->addWidget(mNightTop3, 1, 6, 1, 1);
    multiNightGrid->addWidget(mNightTop4, 1, 8, 1, 1);
    multiNightGrid->addWidget(mNightSpar1, 2, 2, 1, 1);
    multiNightGrid->addWidget(mNightSpar2, 2, 4, 1, 1);
    multiNightGrid->addWidget(mNightSpar3, 2, 6, 1, 1);
    multiNightGrid->addWidget(mNightSpar4, 2, 8, 1, 1);
    multiNightGrid->setColumnMinimumWidth(2, 75);
    multiNightGrid->setColumnMinimumWidth(4, 75);
    multiNightGrid->setColumnMinimumWidth(6, 75);
    multiNightGrid->setColumnMinimumWidth(8, 75);
    multiNightGrid->setColumnStretch(9, 1);

    burbaSimpleDay->setLayout(simpleDayGrid);
    burbaSimpleNight->setLayout(simpleNightGrid);
    burbaMultiDay->setLayout(multiDayGrid);
    burbaMultiNight->setLayout(multiNightGrid);

    connect(lDayBotGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDayBotGain(s.toDouble()); });
    connect(lDayBotOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDayBotOffset(s.toDouble()); });
    connect(lDayTopGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDayTopGain(s.toDouble()); });
    connect(lDayTopOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDayTopOffset(s.toDouble()); });
    connect(lDaySparGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDaySparGain(s.toDouble()); });
    connect(lDaySparOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLDaySparOffset(s.toDouble()); });

    connect(lNightBotGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightBotGain(s.toDouble()); });
    connect(lNightBotOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightBotOffset(s.toDouble()); });
    connect(lNightTopGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightTopGain(s.toDouble()); });
    connect(lNightTopOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightTopOffset(s.toDouble()); });
    connect(lNightSparGain, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightSparGain(s.toDouble()); });
    connect(lNightSparOffset, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenLNightSparOffset(s.toDouble()); });

    connect(mDayBot1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayBot1(s.toDouble()); });
    connect(mDayBot2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayBot2(s.toDouble()); });
    connect(mDayBot3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayBot3(s.toDouble()); });
    connect(mDayBot4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayBot4(s.toDouble()); });

    connect(mDayTop1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayTop1(s.toDouble()); });
    connect(mDayTop2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayTop2(s.toDouble()); });
    connect(mDayTop3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayTop3(s.toDouble()); });
    connect(mDayTop4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDayTop4(s.toDouble()); });

    connect(mDaySpar1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDaySpar1(s.toDouble()); });
    connect(mDaySpar2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDaySpar2(s.toDouble()); });
    connect(mDaySpar3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDaySpar3(s.toDouble()); });
    connect(mDaySpar4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMDaySpar4(s.toDouble()); });

    connect(mNightBot1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightBot1(s.toDouble()); });
    connect(mNightBot2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightBot2(s.toDouble()); });
    connect(mNightBot3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightBot3(s.toDouble()); });
    connect(mNightBot4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightBot4(s.toDouble()); });

    connect(mNightTop1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightTop1(s.toDouble()); });
    connect(mNightTop2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightTop2(s.toDouble()); });
    connect(mNightTop3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightTop3(s.toDouble()); });
    connect(mNightTop4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightTop4(s.toDouble()); });

    connect(mNightSpar1, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightSpar1(s.toDouble()); });
    connect(mNightSpar2, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightSpar2(s.toDouble()); });
    connect(mNightSpar3, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightSpar3(s.toDouble()); });
    connect(mNightSpar4, &CustomResetLineEdit::textChanged, [=](const QString &s) { ecProject_->setScreenMNightSpar4(s.toDouble()); });
}

void AdvProcessingOptions::setBurbaDefaultValues()
{
    // init
    lDayBotGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_bot_gain, 'f', 3));
    lDayBotOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_bot_offset, 'f', 2));
    lDayTopGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_top_gain, 'f', 3));
    lDayTopOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_top_offset, 'f', 2));
    lDaySparGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_spar_gain, 'f', 3));
    lDaySparOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_day_spar_offset, 'f', 2));

    lNightBotGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_bot_gain, 'f', 3));
    lNightBotOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_bot_offset, 'f', 2));
    lNightTopGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_top_gain, 'f', 3));
    lNightTopOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_top_offset, 'f', 2));
    lNightSparGain->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_spar_gain, 'f', 3));
    lNightSparOffset->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.l_night_spar_offset, 'f', 2));

    mDayBot1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_bot1, 'f', 1));
    mDayBot2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_bot2, 'f', 4));
    mDayBot3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_bot3, 'f', 4));
    mDayBot4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_bot4, 'f', 3));
    mDayTop1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_top1, 'f', 1));
    mDayTop2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_top2, 'f', 4));
    mDayTop3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_top3, 'f', 4));
    mDayTop4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_top4, 'f', 3));
    mDaySpar1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_spar1, 'f', 1));
    mDaySpar2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_spar2, 'f', 4));
    mDaySpar3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_spar3, 'f', 4));
    mDaySpar4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_day_spar4, 'f', 3));

    mNightBot1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_bot1, 'f', 1));
    mNightBot2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_bot2, 'f', 4));
    mNightBot3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_bot3, 'f', 4));
    mNightBot4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_bot4, 'f', 3));
    mNightTop1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_top1, 'f', 1));
    mNightTop2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_top2, 'f', 4));
    mNightTop3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_top3, 'f', 4));
    mNightTop4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_top4, 'f', 3));
    mNightSpar1->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_spar1, 'f', 1));
    mNightSpar2->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_spar2, 'f', 4));
    mNightSpar3->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_spar3, 'f', 4));
    mNightSpar4->setDefaultText(QString::number(ecProject_->defaultSettings.screenSetting.m_night_spar4, 'f', 3));
}

void AdvProcessingOptions::on_setDefaultsButton_clicked()
{
    if (requestBurbaSettingsReset())
    {
        setBurbaDefaultValues();
    }
}

void AdvProcessingOptions::updateWplMeth_1(bool b)
{
    ecProject_->setGeneralWplMeth(b);
}

void AdvProcessingOptions::enableBurbaCorrectionArea(bool b)
{
    burbaTypeLabel->setEnabled(b);
    burbaSimpleRadio->setEnabled(b);
    burbaMultiRadio->setEnabled(b);
    burbaParamWidget->setEnabled(b);
    burbaMultiTab->setEnabled(b);
    setDefaultsButton->setEnabled(b);
}

void AdvProcessingOptions::updateBurbaType_2(int n)
{
    ecProject_->setScreenBuMulti(n);
    burbaParamWidget->setCurrentIndex(n);
}

// update when click "Compensate density fluctuations" checkbox
void AdvProcessingOptions::updateBurbaGroup(bool b)
{
    burbaCorrCheckBox->setEnabled(b);
    enableBurbaCorrectionArea(b && burbaCorrCheckBox->isChecked());
}

void AdvProcessingOptions::createQuestionMark()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_2 = new QPushButton;
    questionMark_2->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_3 = new QPushButton;
    questionMark_3->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_1);
    connect(questionMark_2, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_4);
    connect(questionMark_3, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_11);
}

void AdvProcessingOptions::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/windspeed-offsets.html")));
}

void AdvProcessingOptions::onlineHelpTrigger_4()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/calculate-turbulent-flux.html")));
}

void AdvProcessingOptions::onlineHelpTrigger_11()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics/raw-processing-options.html")));
}

void AdvProcessingOptions::updateTooltip(int i)
{
    auto senderCombo = qobject_cast<QComboBox *>(sender());
    WidgetUtils::updateComboItemTooltip(senderCombo, i);
}

bool AdvProcessingOptions::requestBurbaSettingsReset()
{
    return WidgetUtils::yesNoQuestion(this,
                                      tr("Reset Surface Heating Correction"),
                                      tr("<p>Do you want to reset the surface heating correction "
                                         "to the default values of Burba et al. (2008)?</p>"),
                                      tr("<p>You cannot undo this action.</p>"));
}

void AdvProcessingOptions::openToviHomepage()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://tovi.io/?utm_source=EddyPro%20Software&utm_medium=Tovi%20Ads&utm_campaign=EP_Tovi_ads")));
}
