/***************************************************************************
  advprocessingoptions.cpp
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

#include <QwwResetLineEdit/QwwResetLineEdit>

#include "clicklabel.h"
#include "configstate.h"
#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "infomessage.h"
#include "planarfitsettingsdialog.h"
#include "timelagsettingsdialog.h"
#include "widget_utils.h"

AdvProcessingOptions::AdvProcessingOptions(QWidget *parent,
                                           DlProject *dlProject,
                                           EcProject *ecProject,
                                           ConfigState *config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config)
{
    DEBUG_FUNC_NAME

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
#elif defined (Q_OS_MAC)
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
#elif defined(Q_OS_MAC)
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
#elif defined(Q_OS_MAC)
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

    aoaCheckBox = new QCheckBox;
    aoaCheckBox->setToolTip(tr("<b>Angle-of-attack correction:</b> Applies only to vertical mount Gill sonic anemometers with the same geometry of the R3 (e.g., R2, WindMaster, WindMaster Pro). This correction is meant to compensate the effects of flow distortion induced by the anemometer frame on the turbulent flow field. We recommend applying this correction whenever an R3-shaped anemometer was used."));
    aoaLabel = new ClickLabel(tr("Angle-of-attack correction for wind components (Gill's only)"));
    aoaLabel->setToolTip(aoaCheckBox->toolTip());
    connect(aoaLabel, &ClickLabel::clicked,
            aoaCheckBox, &QCheckBox::toggle);

    auto aoaContainerLayout = new QHBoxLayout;
    aoaContainerLayout->addWidget(aoaCheckBox);
    aoaContainerLayout->addWidget(aoaLabel);
    aoaContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    aoaContainerLayout->addWidget(questionMark_2);
    aoaContainerLayout->setSpacing(0);
    aoaContainerLayout->addStretch();

    aoaMethLabel = new ClickLabel(tr("Method :"));
    aoaMethLabel->setEnabled(false);
    aoaMethCombo = new QComboBox;
    aoaMethCombo->addItem(tr("Field calibration (Nakai and Shimoyama, 2012)"));
    aoaMethCombo->addItem(tr("Wind tunnel calibration (Nakai et al., 2006)"));
    aoaMethCombo->setItemData(0, tr("<b>Field calibration:</b> Select this option to apply the angle-of-attack correction according to the method described in the referenced paper, which makes use of a field calibration instead of the wind tunnel calibration."), Qt::ToolTipRole);
    aoaMethCombo->setItemData(1, tr("<b>Wind tunnel calibration:</b> Select this option to apply the angle-of-attack correction according to the method described in the referenced paper, which makes use of a wind tunnel calibration instead of the field calibration."), Qt::ToolTipRole);
    aoaMethCombo->setEnabled(false);

    rotCheckBox = new QCheckBox;
    rotCheckBox->setToolTip(tr("<b>Axis rotation for tilt correction:</b> Select the appropriate method for compensating anemometer tilt with respect to local streamlines. Uncheck the box to <i>not perform</i> any rotation (not recommnended). If your site has a complex or sloping topography, a planar-fit method is advisable. Click on the <b><i>Planar Fit Settings...</i></b> to configure the procedure."));
    auto rotLabel = new ClickLabel(tr("Axis rotations for tilt correction"));
    rotLabel->setToolTip(rotCheckBox->toolTip());
    connect(rotLabel, &ClickLabel::clicked,
            rotCheckBox, &QCheckBox::toggle);

    auto rotContainerLayout = new QHBoxLayout;
    rotContainerLayout->addWidget(rotCheckBox);
    rotContainerLayout->addWidget(rotLabel);
    rotContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    rotContainerLayout->addWidget(questionMark_3);
    rotContainerLayout->setSpacing(0);
    rotContainerLayout->addStretch();

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

    timeLagCheckBox = new QCheckBox;
    timeLagCheckBox->setToolTip(tr("<b>Time lags compensation:</b> Select the method to compensate time lags between anemometric measurements and any other high frequency measurements included in the raw files. Time lags arise due mainly to sensors physical distances and to the passage of air into sampling lines. Uncheck this box to instruct EddyPro not to compensate time lags (not recommended)."));
    auto timeLagLabel = new ClickLabel(tr("Time lags compensation"));
    timeLagLabel->setToolTip(timeLagCheckBox->toolTip());
    connect(timeLagLabel, &ClickLabel::clicked,
            timeLagCheckBox, &QCheckBox::toggle);

    auto timeLagContainerLayout = new QHBoxLayout;
    timeLagContainerLayout->addWidget(timeLagCheckBox);
    timeLagContainerLayout->addWidget(timeLagLabel);
    timeLagContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    timeLagContainerLayout->addWidget(questionMark_5);
    timeLagContainerLayout->setSpacing(0);
    timeLagContainerLayout->addStretch();

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
    nBinsLabel->setToolTip(tr("<b>Frequency bins for spectra and cospectra reduction:</b> Select the number of exponentially-spaced frequency bins to reduce spectra and cospectra. All spectral samples falling in a given bin are averaged, so that smoother curves result, greatly reduced in length. In EddyPro binned spectra are used for in-situ spectral assessments."));
    nBinsSpin = new QSpinBox;
    nBinsSpin->setToolTip(nBinsLabel->toolTip());
    nBinsSpin->setRange(10, 3000);
    nBinsSpin->setSingleStep(10);
    nBinsSpin->setAccelerated(true);
    nBinsSpin->setValue(50);
    nBinsSpin->setMaximumWidth(nBinsSpin->sizeHint().width() * 2);
    fftCheckBox = new QCheckBox(tr("Use power-of-two samples to speed up the FFT"));
    fftCheckBox->setStyleSheet(QStringLiteral("QCheckBox {margin-left: 0px;}"));
    fftCheckBox->setToolTip(tr("<b>Use power-of-two samples to speed up the FFT:</b> Check this box to instruct EddyPro to use a number of samples equal to the power-of-two closest to the currently available samples, for calculating spectra. This option greatly speeds up the FFT procedure and is therefore recommended."));

    qcCheckBox = new QCheckBox;
    qcCheckBox->setToolTip(tr("<b>Quality check:</b> Select the quality flagging policy. Flux quality flags are obtained from the combination of two partial flags that result from the application of the steady-state and the developed turbulence tests. Select the flag combination policy."));
    auto qccLabel = new ClickLabel(tr("Quality check"));
    qccLabel->setToolTip(qcCheckBox->toolTip());
    connect(qccLabel, &ClickLabel::clicked,
            qcCheckBox, &QCheckBox::toggle);

    auto qcContainerLayout = new QHBoxLayout;
    qcContainerLayout->addWidget(qcCheckBox);
    qcContainerLayout->addWidget(qccLabel);
    qcContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    qcContainerLayout->addWidget(questionMark_9);
    qcContainerLayout->setSpacing(0);
    qcContainerLayout->addStretch();

    qcLabel = new ClickLabel(tr("Flagging policy :"));
    qcMethodCombo = new QComboBox;
    qcMethodCombo->setToolTip(tr("<b>Mauder and Foken 2004:</b> Policy described in the documentation of the TK2 Eddy Covariance software that also constituted the standard of the CarboEurope IP project and is widely adopted. \"0\" means high quality fluxes, \"1\" means fluxes are ok for budget analysis, \"2\" fluxes should be discarded from the result dataset."));
    qcMethodCombo->addItem(tr("Mauder and Foken (2004) (0-1-2 system)"));
    qcMethodCombo->addItem(tr("Foken (2003) (1 to 9 system)"));
    qcMethodCombo->addItem(tr("Goeckede et al. (2004) (1 to 5 system)"));
    qcMethodCombo->setItemData(0, tr("<b>Mauder and Foken 2004:</b> Policy described in the documentation of the TK2 Eddy Covariance software that also constituted the standard of the CarboEurope IP project and is widely adopted. \"0\" means high quality fluxes, \"1\" means fluxes are ok for budget analysis, \"2\" fluxes should be discarded from the result dataset."), Qt::ToolTipRole);
    qcMethodCombo->setItemData(1, tr("<b>Foken 2003:</b> A system based on 9 quality grades. \"1\" is best, \"9\" is worst. The system of Mauder and Foken (2004) and of Goeckede et al. (2006) are based on a rearrangement of these system."), Qt::ToolTipRole);
    qcMethodCombo->setItemData(2, tr("<b>Goeckede et al., 2004:</b> A system based on 5 quality grades. \"1\" is best, \"5\" is worst."), Qt::ToolTipRole);

    fpCheckBox = new QCheckBox;
    fpCheckBox->setToolTip(tr("<b>Footprint estimation:</b> Select whether to calculate flux footprint estimations and which method should be used. Flux crosswind-integrated footprints are provided as distances from the tower contributing for 10%, 30%, 50%, 70% and 90% to measured fluxes. Also, the location of the peak contribution is given."));
    auto fpcLabel = new ClickLabel(tr("Footprint estimation"));
    fpcLabel->setToolTip(fpCheckBox->toolTip());
    connect(fpcLabel, &ClickLabel::clicked,
            fpCheckBox, &QCheckBox::toggle);

    auto fpContainerLayout = new QHBoxLayout;
    fpContainerLayout->addWidget(fpCheckBox);
    fpContainerLayout->addWidget(fpcLabel);
    fpContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    fpContainerLayout->addWidget(questionMark_10);
    fpContainerLayout->setSpacing(0);
    fpContainerLayout->addStretch();

    fpLabel = new ClickLabel(tr("Footprint method :"));
    fpMethodCombo = new QComboBox;
    fpMethodCombo->setToolTip(tr("<b>Kljun et al. (2004):</b> A cross-wind integrated parameterization of footprint estimations obtained with a 3D Lagrangian model by means of a scaling procedure."));
    fpMethodCombo->addItem(tr("Kljun et al. (2004)"));
    fpMethodCombo->addItem(tr("Kormann and Meixner (2001)"));
    fpMethodCombo->addItem(tr("Hsieh et al. (2000)"));
    fpMethodCombo->setItemData(0, tr("<b>Kljun et al. (2004):</b> A cross-wind integrated parameterization of footprint estimations obtained with a 3D Lagrangian model by means of a scaling procedure."), Qt::ToolTipRole);
    fpMethodCombo->setItemData(1, tr("<b>Kormann and Meixner (2001):</b> A cross-wind integrated model based on the solution of the two dimensional advection-diffusion equation given by van Ulden (1978) and others for power-law profiles in wind velocity and eddy diffusivity."), Qt::ToolTipRole);
    fpMethodCombo->setItemData(2, tr("<b>Hsien et al. (2000):</b> A cross-wind integrated model based based on the former model of Gash (1986) and on simulations with a Lagrangian stochastic model."), Qt::ToolTipRole);

    wplCheckBox = new QCheckBox;
    wplCheckBox->setToolTip(tr("<b>Compensate density fluctuations:</b> This is the so-called WPL correction (Webb et al., 1980). Choose whether to apply the compensation of density fluctuations to raw gas concentrations available as molar densities or mole fractions (moles gas per mole of wet air). The correction does not apply if raw concentrations are available as mixing ratios (mole gas per mole dry air)."));
    auto wplLabel = new ClickLabel(tr("Compensate density fluctuations"));
    wplLabel->setToolTip(wplCheckBox->toolTip());
    connect(wplLabel, &ClickLabel::clicked,
            wplCheckBox, &QCheckBox::toggle);

    auto wplContainerLayout = new QHBoxLayout;
    wplContainerLayout->addWidget(wplCheckBox);
    wplContainerLayout->addWidget(wplLabel);
    wplContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    wplContainerLayout->addWidget(questionMark_6);
    wplContainerLayout->setSpacing(0);
    wplContainerLayout->addStretch();

    wplMethLabel = new ClickLabel(tr("Method :"));
    wplMethCombo = new QComboBox;
    wplMethCombo->addItem(tr("Use/convert to mixing ratio, if possible (Burba et al. 2012)"));
    wplMethCombo->addItem(tr("Webb et al. 1980 (open-path) / Ibrom et al. 2007 (closed-path)"));
    wplMethCombo->setItemData(0, 1);
    wplMethCombo->setItemData(1, 0);
    wplMethCombo->setItemData(0, tr("<b>Use/convert to mixing ratio, if possible:</b> Selecting this option, you instruct EddyPro to use mixing ratio (if selected in <b><i>Basic Settings</i></b>) or to convert available measurement into mixing ratio, at the raw data level, as a method to compensate air density fluctuations. This method is only applicable in closed path analyzers and is applied only if all necessary data is available (e.g., cell temperature and pressure, high frequency H<sub>2</sub>O measurement), otherwise EddyPro automatically switches to the alternative formulation."), Qt::ToolTipRole);
    wplMethCombo->setItemData(1, tr("<b>Webb et al. 1980 / Ibrom et al. 2007:</b> The traditional \"WPL terms,\" implemented as an <i>a posteriori</i> correction. For open path systems, it corrects density fluctuations using measured sensible heat and evapotranspiration fluxes, corrected for spectral losses and water vapor effects, as applicable. For closed/enclosed path instruments, uses the formulation proposed by Ibrom et al. (2007). However, in EddyPro all terms are used, including the temperature-induced and pressure-induced expansion/contraction terms, neglected in the original publications."), Qt::ToolTipRole);
    wplMethLabel->setToolTip(wplMethCombo->toolTip());

    burbaCorrCheckBox = new QCheckBox;
    burbaCorrCheckBox->setToolTip(tr("<b>Add instrument sensible heat components, only for LI-7500:</b> Only applies to the LI-7500. It takes into account air density fluctuations due to temperature fluctuations induced by heat exchange processes at the instrument surfaces, as from Burba et al. (2008)."));
    burbaLabel = new ClickLabel(tr("Add instrument sensible heat components, only for LI-7500"));
    burbaLabel->setToolTip(burbaCorrCheckBox->toolTip());
    connect(burbaLabel, &ClickLabel::clicked,
            burbaCorrCheckBox, &QCheckBox::toggle);

    auto burbaContainerLayout = new QHBoxLayout;
    burbaContainerLayout->addWidget(burbaCorrCheckBox);
    burbaContainerLayout->addWidget(burbaLabel);
    burbaContainerLayout->addSpacerItem(new QSpacerItem(18, 1));
    burbaContainerLayout->addWidget(questionMark_7);
    burbaContainerLayout->setSpacing(0);
    burbaContainerLayout->addStretch();

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

    auto wplTitle = new QLabel(tr("Compensation of density fluctuations (WPL terms)"));
    wplTitle->setProperty("groupLabel", true);

    auto fftTitle = new QLabel(tr("Fast Fourier Transform"));
    fftTitle->setProperty("groupLabel", true);

    auto qcTitle = new QLabel(tr("Other options"));
    qcTitle->setProperty("groupLabel", true);

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));
    auto hrLabel_2 = new QLabel;
    hrLabel_2->setObjectName(QStringLiteral("hrLabel"));
    auto hrLabel_3 = new QLabel;
    hrLabel_3->setObjectName(QStringLiteral("hrLabel"));

    auto qBox_1 = new QHBoxLayout;
    qBox_1->addWidget(windOffsetLabel);
    qBox_1->addWidget(questionMark_1);
    qBox_1->addStretch();

    auto qBox_2 = new QHBoxLayout;
    qBox_2->addWidget(detrendLabel);
    qBox_2->addWidget(questionMark_4);
    qBox_2->addStretch();

    auto qBox_3 = new QHBoxLayout;
    qBox_3->addWidget(fftTitle);
    qBox_3->addWidget(questionMark_8);
    qBox_3->addStretch();

    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(rawProcessingTitle, 0, 0);
    settingsLayout->addLayout(qBox_1, 1, 0, 1, 2);
    settingsLayout->addLayout(windComponentLayout, 1, 2, 1, 1);
    settingsLayout->addLayout(aoaContainerLayout, 2, 0, 1, -1);
    settingsLayout->addWidget(aoaMethLabel, 2, 1, Qt::AlignRight);
    settingsLayout->addWidget(aoaMethCombo, 2, 2);
    settingsLayout->addLayout(rotContainerLayout, 3, 0);
    settingsLayout->addWidget(rotMethLabel, 3, 1, Qt::AlignRight);
    settingsLayout->addWidget(rotMethCombo, 3, 2);
    settingsLayout->addWidget(pfSettingsButton, 3, 3);
    settingsLayout->addLayout(qBox_2, 4, 0);
    settingsLayout->addWidget(detrendMethLabel, 4, 1, Qt::AlignRight);
    settingsLayout->addWidget(detrendCombo, 4, 2);
    settingsLayout->addWidget(timeConstantLabel, 5, 1, Qt::AlignRight);
    settingsLayout->addWidget(timeConstantSpin, 5, 2);
    settingsLayout->addLayout(timeLagContainerLayout, 6, 0);
    settingsLayout->addWidget(timeLagMethodLabel, 6, 1, Qt::AlignRight);
    settingsLayout->addWidget(timeLagMethodCombo, 6, 2);
    settingsLayout->addWidget(tlSettingsButton, 6, 3);
    settingsLayout->addWidget(hrLabel, 7, 0, 1, 4);
    settingsLayout->addWidget(wplTitle, 8, 0);
    settingsLayout->addLayout(wplContainerLayout, 10, 0);
    settingsLayout->addWidget(wplMethLabel, 10, 1, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(wplMethCombo, 10, 2);
    settingsLayout->addLayout(burbaContainerLayout, 11, 0);
    settingsLayout->addWidget(burbaTypeLabel, 11, 1, 1, 1, Qt::AlignRight);
    settingsLayout->addWidget(burbaSimpleRadio, 11, 2);
    settingsLayout->addWidget(burbaMultiRadio, 12, 2);
    settingsLayout->addWidget(burbaParamWidget, 13, 0, 1, 4);
    settingsLayout->addWidget(defaultContainer, 14, 0, 1, 4);
    settingsLayout->addWidget(hrLabel_2, 15, 0, 1, 4);
    settingsLayout->addLayout(qBox_3, 16, 0, 1, 3);
    settingsLayout->addWidget(filterLabel, 17, 1, Qt::AlignRight);
    settingsLayout->addWidget(filterCombo, 17, 2);
    settingsLayout->addWidget(nBinsLabel, 18, 1, Qt::AlignRight);
    settingsLayout->addWidget(nBinsSpin, 18, 2);
    settingsLayout->addWidget(fftCheckBox, 19, 2);
    settingsLayout->addWidget(hrLabel_3, 20, 0, 1, 4);
    settingsLayout->addWidget(qcTitle, 21, 0);
    settingsLayout->addLayout(qcContainerLayout, 22, 0);
    settingsLayout->addWidget(qcLabel, 22, 1, Qt::AlignRight);
    settingsLayout->addWidget(qcMethodCombo, 22, 2);
    settingsLayout->addLayout(fpContainerLayout, 23, 0);
    settingsLayout->addWidget(fpLabel, 23, 1, Qt::AlignRight);
    settingsLayout->addWidget(fpMethodCombo, 23, 2);
    settingsLayout->setRowStretch(24, 1);
    settingsLayout->setColumnStretch(4, 1);

    auto overallFrame = new QWidget;
    overallFrame->setProperty("scrollContainerWidget", true);
    overallFrame->setLayout(settingsLayout);

    auto scrollArea = new QScrollArea;
    scrollArea->setWidget(overallFrame);
    scrollArea->setWidgetResizable(true);

    auto settingsGroupLayout = new QHBoxLayout;
    settingsGroupLayout->addWidget(scrollArea);

    auto settingsGroupTitle = new QLabel(tr("Raw Processing Options"));
    settingsGroupTitle->setProperty("groupTitle2", true);

    auto qBox_11 = new QHBoxLayout;
    qBox_11->addWidget(settingsGroupTitle, 0, Qt::AlignRight | Qt::AlignBottom);
    qBox_11->addWidget(questionMark_11);
    qBox_11->addStretch();

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(qBox_11);
    mainLayout->addLayout(settingsGroupLayout);
    mainLayout->setContentsMargins(15, 15, 0, 10);
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

    connect(aoaCheckBox, &QCheckBox::toggled,
            aoaMethLabel, &ClickLabel::setEnabled);
    connect(aoaCheckBox, &QCheckBox::toggled,
            aoaMethCombo, &QComboBox::setEnabled);
    connect(aoaMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickAoaMethLabel);
    connect(aoaCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateAoaMethod_1);
    connect(aoaMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateAoaMethod_2(int)));

    connect(rotCheckBox, &QCheckBox::toggled,
            rotMethLabel, &ClickLabel::setEnabled);
    connect(rotCheckBox, &QCheckBox::toggled,
            rotMethCombo, &QComboBox::setEnabled);
    connect(rotMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickRotMethLabel);
    connect(rotCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateRotMethod_1);
    connect(rotMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateRotMethod_2(int)));
    connect(rotCheckBox, &QCheckBox::toggled,
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

    connect(timeLagCheckBox, &QCheckBox::toggled,
            timeLagMethodLabel, &ClickLabel::setEnabled);
    connect(timeLagCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateTlSettingsButton);
    connect(timeLagCheckBox, &QCheckBox::toggled,
            timeLagMethodCombo, &QComboBox::setEnabled);
    connect(timeLagMethodLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickTimeLagMethLabel);
    connect(timeLagCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateTlagMeth_1);
    connect(timeLagMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateTlagMeth_2(int)));

    connect(filterLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickFilterLabel);
    connect(filterCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFilter(int)));

    connect(nBinsLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onNBinsLabelClicked);
    connect(nBinsSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateNBins(int)));

    connect(fftCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setScreenlPowerOfTwo(checked); });

    connect(qcCheckBox, &QCheckBox::toggled,
            qcLabel, &ClickLabel::setEnabled);
    connect(qcCheckBox, &QCheckBox::toggled,
            qcMethodCombo, &QComboBox::setEnabled);
    connect(qcLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickQcMethodLabel);
    connect(qcCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateQcMeth_1);
    connect(qcMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateQcMeth_2(int)));

    connect(fpCheckBox, &QCheckBox::toggled,
            fpLabel, &ClickLabel::setEnabled);
    connect(fpCheckBox, &QCheckBox::toggled,
            fpMethodCombo, &QComboBox::setEnabled);
    connect(fpLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickFpMethodLabel);
    connect(fpCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateFpMeth_1);
    connect(fpMethodCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateFpMeth_2(int)));

    connect(wplMethLabel, &ClickLabel::clicked,
            this, &AdvProcessingOptions::onClickWplMethLabel);
    connect(wplCheckBox, &QCheckBox::toggled,
            wplMethLabel, &ClickLabel::setEnabled);
    connect(wplCheckBox, &QCheckBox::toggled,
            wplMethCombo, &QComboBox::setEnabled);
    connect(wplCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateWplMeth_1);
    connect(wplCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::updateBurbaGroup);
    connect(wplMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateToMixingRatio(int)));
    connect(burbaCorrCheckBox, &QCheckBox::toggled, [=](bool checked)
            { ecProject_->setScreenBuCorr(checked); });
    connect(burbaCorrCheckBox, &QCheckBox::toggled,
            this, &AdvProcessingOptions::enableBurbaCorrectionArea);
    connect(burbaRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateBurbaType_2(int)));
    connect(setDefaultsButton, &QPushButton::clicked,
            this, &AdvProcessingOptions::on_setDefaultsButton_clicked);

    connect(ecProject_, &EcProject::ecProjectNew,
            this, &AdvProcessingOptions::reset);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &AdvProcessingOptions::refresh);

    foreach (QComboBox *combo,
             QList<QComboBox *>() << aoaMethCombo
                                  << rotMethCombo
                                  << detrendCombo
                                  << timeLagMethodCombo
                                  << wplMethCombo
                                  << qcMethodCombo
                                  << fpMethodCombo)
    {
        connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(updateTooltip(int)));
    }

    createPfSettingsDialog();
    createTlSettingsDialog();
    QTimer::singleShot(0, this, SLOT(reset()));
}

AdvProcessingOptions::~AdvProcessingOptions()
{
    DEBUG_FUNC_NAME

    if (pfDialog_)
        delete pfDialog_;

    if (tlDialog_)
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
    DEBUG_FUNC_NAME
    if (b)
    {
        ecProject_->setScreenFlowDistortion(aoaMethCombo->currentIndex() + 1);
    }
    else
    {
        ecProject_->setScreenFlowDistortion(0);
    }
}

// update project properties
void AdvProcessingOptions::updateAoaMethod_2(int n)
{
    ecProject_->setScreenFlowDistortion(n + 1);
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
    DEBUG_FUNC_NAME

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
        if (timeLagMethodCombo->currentIndex() == 3)
        {
//            toLabel->setEnabled(true);
//            toEdit->setEnabled(true);
//            toBrowse->setEnabled(true);
//            meLabel->setEnabled(true);
//            meEdit->setEnabled(true);
//            meBrowse->setEnabled(true);
        }
    }
    else
    {
        ecProject_->setScreenTlagMeth(0);

//        toLabel->setEnabled(false);
//        toEdit->setEnabled(false);
//        toBrowse->setEnabled(false);
//        meLabel->setEnabled(false);
//        meEdit->setEnabled(false);
//        meBrowse->setEnabled(false);
    }
}

void AdvProcessingOptions::updateTlagMeth_2(int n)
{
    ecProject_->setScreenTlagMeth(n + 1);

    // timelag optimization button
    tlSettingsButton->setEnabled(n == 3);
}

void AdvProcessingOptions::updateFilter(int n)
{
    ecProject_->setScreenTapWin(n);
}

void AdvProcessingOptions::updateNBins(int n)
{
    ecProject_->setScreenNBins(n);
}

void AdvProcessingOptions::onClickDetrendCombo(int newDetrendMethod)
{
    DEBUG_FUNC_NAME

    qDebug() << "previousDetrendMethod" << static_cast<int>(previousDetrendMethod_);
    qDebug() << "timeConstantSpin->value()" << timeConstantSpin->value();

    DetrendMethod currDetrendMethod = static_cast<DetrendMethod>(newDetrendMethod);

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
        if (timeConstantSpin->value() == 4.2)
            timeConstantSpin->setValue(0.0);
        timeConstantSpin->setSingleStep(1.0);
        timeConstantSpin->setSuffix(tr("  [m]", "Minute"));
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

void AdvProcessingOptions::onClickFilterLabel()
{
    if (filterCombo->isEnabled())
    {
        filterCombo->showPopup();
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

void AdvProcessingOptions::onNBinsLabelClicked()
{
    nBinsSpin->setFocus();
    nBinsSpin->selectAll();
}

void AdvProcessingOptions::reset()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    uOffsetSpin->setValue(0.0);
    vOffsetSpin->setValue(0.0);
    wOffsetSpin->setValue(0.0);
    aoaCheckBox->setChecked(false);
    aoaMethCombo->setCurrentIndex(0);
    // ecProject_->setScreenFlowDistortion(0);

    rotCheckBox->setChecked(true);
    rotMethCombo->setCurrentIndex(0);
    pfSettingsButton->setEnabled(false);
    detrendCombo->setCurrentIndex(0);
    timeConstantSpin->setValue(250.0);
    timeLagCheckBox->setChecked(true);
    timeLagMethodCombo->setCurrentIndex(1);
    tlSettingsButton->setEnabled(false);
    filterCombo->setCurrentIndex(3);
    nBinsSpin->setValue(50);
    fftCheckBox->setChecked(true);

    pfDialog_->reset();
    tlDialog_->reset();

    qcLabel->setEnabled(true);
    qcCheckBox->setChecked(true);
    qcMethodCombo->setCurrentIndex(0);

    fpLabel->setEnabled(true);
    fpCheckBox->setChecked(true);
    fpMethodCombo->setCurrentIndex(0);

    wplMethCombo->setCurrentIndex(0);
    wplCheckBox->setChecked(true);

    setBurbaDefaultValues();

    burbaLabel->setEnabled(false);
    burbaCorrCheckBox->setEnabled(false);
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
    WidgetUtils::updateComboItemTooltip(wplMethCombo, 0);
    WidgetUtils::updateComboItemTooltip(qcMethodCombo, 0);
    WidgetUtils::updateComboItemTooltip(fpMethodCombo, 0);

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void AdvProcessingOptions::refresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    uOffsetSpin->setValue(ecProject_->screenUOffset());
    vOffsetSpin->setValue(ecProject_->screenVOffset());
    wOffsetSpin->setValue(ecProject_->screenWOffset());

    refreshAngleOfAttack();

    rotCheckBox->setChecked(ecProject_->screenRotMethod());
    if (ecProject_->screenRotMethod())
    {
        rotMethCombo->setCurrentIndex(ecProject_->screenRotMethod() - 1);
    }
    else
    {
        rotMethCombo->setCurrentIndex(0);
    }
    pfSettingsButton->setEnabled((ecProject_->screenRotMethod() == 3)
                                 || (ecProject_->screenRotMethod() == 4));

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

    filterCombo->setCurrentIndex(ecProject_->screenTapWin());
    nBinsSpin->setValue(ecProject_->screenNBins());
    fftCheckBox->setChecked(ecProject_->screenPowerOfTwo());

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
    wplMethCombo->setCurrentIndex(wplMethCombo->findData(ecProject_->screenToMixRatio()));

    burbaCorrCheckBox->setChecked(ecProject_->screenBuCorr());
    burbaCorrCheckBox->setEnabled(wplMethCombo->currentIndex());
    burbaLabel->setEnabled(wplMethCombo->currentIndex());

    burbaRadioGroup->buttons().at(ecProject_->screenBuMulti())->setChecked(true);

    burbaParamWidget->setCurrentIndex(ecProject_->screenBuMulti());
    burbaSimpleTab->setCurrentIndex(0);
    burbaMultiTab->setCurrentIndex(0);

    burbaTypeLabel->setEnabled(burbaCorrCheckBox->isChecked());
    burbaSimpleRadio->setEnabled(burbaCorrCheckBox->isChecked());
    burbaParamWidget->setEnabled(burbaCorrCheckBox->isChecked());
    burbaMultiTab->setEnabled(burbaCorrCheckBox->isChecked());
    setDefaultsButton->setEnabled(burbaCorrCheckBox->isChecked());

    lDayBotGain->setText(QString::number(ecProject_->screenLDayBotGain()));
    lDayBotOffset->setText(QString::number(ecProject_->screenLDayBotOffset()));
    lDayTopGain->setText(QString::number(ecProject_->screenLDayTopGain()));
    lDayTopOffset->setText(QString::number(ecProject_->screenLDayTopOffset()));
    lDaySparGain->setText(QString::number(ecProject_->screenLDaySparGain()));
    lDaySparOffset->setText(QString::number(ecProject_->screenLDaySparOffset()));
    lNightBotGain->setText(QString::number(ecProject_->screenLNightBotGain()));
    lNightBotOffset->setText(QString::number(ecProject_->screenLNightBotOffset()));
    lNightTopGain->setText(QString::number(ecProject_->screenLNightTopGain()));
    lNightTopOffset->setText(QString::number(ecProject_->screenLNightTopOffset()));
    lNightSparGain->setText(QString::number(ecProject_->screenLNightSparGain()));
    lNightSparOffset->setText(QString::number(ecProject_->screenLNightSparOffset()));
    mDayBot1->setText(QString::number(ecProject_->screenMDayBot1()));
    mDayBot2->setText(QString::number(ecProject_->screenMDayBot2()));
    mDayBot3->setText(QString::number(ecProject_->screenMDayBot3()));
    mDayBot4->setText(QString::number(ecProject_->screenMDayBot4()));
    mDayTop1->setText(QString::number(ecProject_->screenMDayTop1()));
    mDayTop2->setText(QString::number(ecProject_->screenMDayTop2()));
    mDayTop3->setText(QString::number(ecProject_->screenMDayTop3()));
    mDayTop4->setText(QString::number(ecProject_->screenMDayTop4()));
    mDaySpar1->setText(QString::number(ecProject_->screenMDaySpar1()));
    mDaySpar2->setText(QString::number(ecProject_->screenMDaySpar2()));
    mDaySpar3->setText(QString::number(ecProject_->screenMDaySpar3()));
    mDaySpar4->setText(QString::number(ecProject_->screenMDaySpar4()));
    mNightBot1->setText(QString::number(ecProject_->screenMNightBot1()));
    mNightBot2->setText(QString::number(ecProject_->screenMNightBot2()));
    mNightBot3->setText(QString::number(ecProject_->screenMNightBot3()));
    mNightBot4->setText(QString::number(ecProject_->screenMNightBot4()));
    mNightTop1->setText(QString::number(ecProject_->screenMNightTop1()));
    mNightTop2->setText(QString::number(ecProject_->screenMNightTop2()));
    mNightTop3->setText(QString::number(ecProject_->screenMNightTop3()));
    mNightTop4->setText(QString::number(ecProject_->screenMNightTop4()));
    mNightSpar1->setText(QString::number(ecProject_->screenMNightSpar1()));
    mNightSpar2->setText(QString::number(ecProject_->screenMNightSpar2()));
    mNightSpar3->setText(QString::number(ecProject_->screenMNightSpar3()));
    mNightSpar4->setText(QString::number(ecProject_->screenMNightSpar4()));

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void AdvProcessingOptions::createPfSettingsDialog()
{
    DEBUG_FUNC_NAME

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
    DEBUG_FUNC_NAME

    if (!tlDialog_)
    {
        tlDialog_ = new TimeLagSettingsDialog(this, ecProject_, configState_);
    }
}

void AdvProcessingOptions::showTlSettingsDialog()
{
    DEBUG_FUNC_NAME

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

    simpleDayGrid->addWidget(new QLabel(tr("Bottom :")), 0, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>bot</sub> = ")), 0, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 0, 3, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr("Top :")), 1, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>top</sub> = ")), 1, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 1, 3, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr("Spar :")), 2, 0, 1, 1, Qt::AlignRight);
    simpleDayGrid->addWidget(new QLabel(tr("T<sub>spar</sub> = ")), 2, 1, 1, 1);
    simpleDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 2, 3, 1, 1);

    simpleNightGrid->addWidget(new QLabel(tr("Bottom :")), 0, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>bot</sub> = ")), 0, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 0, 3, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr("Top :")), 1, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>top</sub> = ")), 1, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 1, 3, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr("Spar :")), 2, 0, 1, 1, Qt::AlignRight);
    simpleNightGrid->addWidget(new QLabel(tr("T<sub>spar</sub> = ")), 2, 1, 1, 1);
    simpleNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 2, 3, 1, 1);

    // matching floating point number with no exponents
    QString floatingPointRegexp = QStringLiteral("[-+]?[0-9]*\\.?[0-9]+");
    // TODO: use a QDoubleValidator with also range specs

    lDayBotGain = new QwwResetLineEdit;
    lDayBotGain->setMaxLength(10);
    lDayBotGain->setRegExp(floatingPointRegexp);
    lDayBotGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lDayBotOffset = new QwwResetLineEdit;
    lDayBotOffset->setMaxLength(10);
    lDayBotOffset->setRegExp(floatingPointRegexp);
    lDayBotOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lDayTopGain = new QwwResetLineEdit;
    lDayTopGain->setMaxLength(10);
    lDayTopGain->setRegExp(floatingPointRegexp);
    lDayTopGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lDayTopOffset = new QwwResetLineEdit;
    lDayTopOffset->setMaxLength(10);
    lDayTopOffset->setRegExp(floatingPointRegexp);
    lDayTopOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lDaySparGain = new QwwResetLineEdit;
    lDaySparGain->setMaxLength(10);
    lDaySparGain->setRegExp(floatingPointRegexp);
    lDaySparGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lDaySparOffset = new QwwResetLineEdit;
    lDaySparOffset->setMaxLength(10);
    lDaySparOffset->setRegExp(floatingPointRegexp);
    lDaySparOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));

    simpleDayGrid->addWidget(lDayBotGain, 0, 2, 1, 1);
    simpleDayGrid->addWidget(lDayBotOffset, 0, 4, 1, 1);
    simpleDayGrid->addWidget(lDayTopGain, 1, 2, 1, 1);
    simpleDayGrid->addWidget(lDayTopOffset, 1, 4, 1, 1);
    simpleDayGrid->addWidget(lDaySparGain, 2, 2, 1, 1);
    simpleDayGrid->addWidget(lDaySparOffset, 2, 4, 1, 1);
    simpleDayGrid->setColumnStretch(5, 1);

    lNightBotGain = new QwwResetLineEdit;
    lNightBotGain->setMaxLength(10);
    lNightBotGain->setRegExp(floatingPointRegexp);
    lNightBotGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lNightBotOffset = new QwwResetLineEdit;
    lNightBotOffset->setMaxLength(10);
    lNightBotOffset->setRegExp(floatingPointRegexp);
    lNightBotOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lNightTopGain = new QwwResetLineEdit;
    lNightTopGain->setMaxLength(10);
    lNightTopGain->setRegExp(floatingPointRegexp);
    lNightTopGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lNightTopOffset = new QwwResetLineEdit;
    lNightTopOffset->setMaxLength(10);
    lNightTopOffset->setRegExp(floatingPointRegexp);
    lNightTopOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lNightSparGain = new QwwResetLineEdit;
    lNightSparGain->setMaxLength(10);
    lNightSparGain->setRegExp(floatingPointRegexp);
    lNightSparGain->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    lNightSparOffset = new QwwResetLineEdit;
    lNightSparOffset->setMaxLength(10);
    lNightSparOffset->setRegExp(floatingPointRegexp);
    lNightSparOffset->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));

    simpleNightGrid->addWidget(lNightBotGain, 0, 2, 1, 1);
    simpleNightGrid->addWidget(lNightBotOffset, 0, 4, 1, 1);
    simpleNightGrid->addWidget(lNightTopGain, 1, 2, 1, 1);
    simpleNightGrid->addWidget(lNightTopOffset, 1, 4, 1, 1);
    simpleNightGrid->addWidget(lNightSparGain, 2, 2, 1, 1);
    simpleNightGrid->addWidget(lNightSparOffset, 2, 4, 1, 1);
    simpleNightGrid->setColumnStretch(5, 1);

    multiDayGrid->addWidget(new QLabel(tr("Bottom :")), 0, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>bot</sub> - T<sub>a</sub> = ")), 0, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 0, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + ")), 0, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * U + ")), 0, 7, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("Top :")), 1, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>top</sub> - T<sub>a</sub> = ")), 1, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 1, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + ")), 1, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * U + ")), 1, 7, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr("Spar :")), 2, 0, 1, 1, Qt::AlignRight);
    multiDayGrid->addWidget(new QLabel(tr("T<sub>spar</sub> - T<sub>a</sub> = ")), 2, 1, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 2, 3, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * R<sub>g</sub> + ")), 2, 5, 1, 1);
    multiDayGrid->addWidget(new QLabel(tr(" * U + ")), 2, 7, 1, 1);

    mDayBot1 = new QwwResetLineEdit;
    mDayBot1->setMaxLength(10);
    mDayBot1->setRegExp(floatingPointRegexp);
    mDayBot1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayBot2 = new QwwResetLineEdit;
    mDayBot2->setMaxLength(10);
    mDayBot2->setRegExp(floatingPointRegexp);
    mDayBot2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayBot3 = new QwwResetLineEdit;
    mDayBot3->setMaxLength(10);
    mDayBot3->setRegExp(floatingPointRegexp);
    mDayBot3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayBot4 = new QwwResetLineEdit;
    mDayBot4->setMaxLength(10);
    mDayBot4->setRegExp(floatingPointRegexp);
    mDayBot4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayTop1 = new QwwResetLineEdit;
    mDayTop1->setMaxLength(10);
    mDayTop1->setRegExp(floatingPointRegexp);
    mDayTop1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayTop2 = new QwwResetLineEdit;
    mDayTop2->setMaxLength(10);
    mDayTop2->setRegExp(floatingPointRegexp);
    mDayTop2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayTop3 = new QwwResetLineEdit;
    mDayTop3->setMaxLength(10);
    mDayTop3->setRegExp(floatingPointRegexp);
    mDayTop3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDayTop4 = new QwwResetLineEdit;
    mDayTop4->setMaxLength(10);
    mDayTop4->setRegExp(floatingPointRegexp);
    mDayTop4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDaySpar1 = new QwwResetLineEdit;
    mDaySpar1->setMaxLength(10);
    mDaySpar1->setRegExp(floatingPointRegexp);
    mDaySpar1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDaySpar2 = new QwwResetLineEdit;
    mDaySpar2->setMaxLength(10);
    mDaySpar2->setRegExp(floatingPointRegexp);
    mDaySpar2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDaySpar3 = new QwwResetLineEdit;
    mDaySpar3->setMaxLength(10);
    mDaySpar3->setRegExp(floatingPointRegexp);
    mDaySpar3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mDaySpar4 = new QwwResetLineEdit;
    mDaySpar4->setMaxLength(10);
    mDaySpar4->setRegExp(floatingPointRegexp);
    mDaySpar4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));

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

    multiNightGrid->addWidget(new QLabel(tr("Bottom :")), 0, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>bot</sub> - T<sub>a</sub> = ")), 0, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 0, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + ")), 0, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * U + ")), 0, 7, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("Top :")), 1, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>top</sub> - T<sub>a</sub> = ")), 1, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 1, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + ")), 1, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * U + ")), 1, 7, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr("Spar :")), 2, 0, 1, 1, Qt::AlignRight);
    multiNightGrid->addWidget(new QLabel(tr("T<sub>spar</sub> - T<sub>a</sub> = ")), 2, 1, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * T<sub>a</sub> + ")), 2, 3, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * LWin + ")), 2, 5, 1, 1);
    multiNightGrid->addWidget(new QLabel(tr(" * U + ")), 2, 7, 1, 1);

    mNightBot1 = new QwwResetLineEdit;
    mNightBot1->setMaxLength(10);
    mNightBot1->setRegExp(floatingPointRegexp);
    mNightBot1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightBot2 = new QwwResetLineEdit;
    mNightBot2->setMaxLength(10);
    mNightBot2->setRegExp(floatingPointRegexp);
    mNightBot2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightBot3 = new QwwResetLineEdit;
    mNightBot3->setMaxLength(10);
    mNightBot3->setRegExp(floatingPointRegexp);
    mNightBot3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightBot4 = new QwwResetLineEdit;
    mNightBot4->setMaxLength(10);
    mNightBot4->setRegExp(floatingPointRegexp);
    mNightBot4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightTop1 = new QwwResetLineEdit;
    mNightTop1->setMaxLength(10);
    mNightTop1->setRegExp(floatingPointRegexp);
    mNightTop1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightTop2 = new QwwResetLineEdit;
    mNightTop2->setMaxLength(10);
    mNightTop2->setRegExp(floatingPointRegexp);
    mNightTop2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightTop3 = new QwwResetLineEdit;
    mNightTop3->setMaxLength(10);
    mNightTop3->setRegExp(floatingPointRegexp);
    mNightTop3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightTop4 = new QwwResetLineEdit;
    mNightTop4->setMaxLength(10);
    mNightTop4->setRegExp(floatingPointRegexp);
    mNightTop4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightSpar1 = new QwwResetLineEdit;
    mNightSpar1->setMaxLength(10);
    mNightSpar1->setRegExp(floatingPointRegexp);
    mNightSpar1->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightSpar2 = new QwwResetLineEdit;
    mNightSpar2->setMaxLength(10);
    mNightSpar2->setRegExp(floatingPointRegexp);
    mNightSpar2->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightSpar3 = new QwwResetLineEdit;
    mNightSpar3->setMaxLength(10);
    mNightSpar3->setRegExp(floatingPointRegexp);
    mNightSpar3->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));
    mNightSpar4 = new QwwResetLineEdit;
    mNightSpar4->setMaxLength(10);
    mNightSpar4->setRegExp(floatingPointRegexp);
    mNightSpar4->setIcon(QIcon(QStringLiteral(":/icons/reset-line")));

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

    connect(lDayBotGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDayBotGain(s.toDouble()); });
    connect(lDayBotOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDayBotOffset(s.toDouble()); });
    connect(lDayTopGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDayTopGain(s.toDouble()); });
    connect(lDayTopOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDayTopOffset(s.toDouble()); });
    connect(lDaySparGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDaySparGain(s.toDouble()); });
    connect(lDaySparOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLDaySparOffset(s.toDouble()); });

    connect(lNightBotGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightBotGain(s.toDouble()); });
    connect(lNightBotOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightBotOffset(s.toDouble()); });
    connect(lNightTopGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightTopGain(s.toDouble()); });
    connect(lNightTopOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightTopOffset(s.toDouble()); });
    connect(lNightSparGain, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightSparGain(s.toDouble()); });
    connect(lNightSparOffset, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenLNightSparOffset(s.toDouble()); });

    connect(mDayBot1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayBot1(s.toDouble()); });
    connect(mDayBot2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayBot2(s.toDouble()); });
    connect(mDayBot3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayBot3(s.toDouble()); });
    connect(mDayBot4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayBot4(s.toDouble()); });

    connect(mDayTop1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayTop1(s.toDouble()); });
    connect(mDayTop2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayTop2(s.toDouble()); });
    connect(mDayTop3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayTop3(s.toDouble()); });
    connect(mDayTop4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDayTop4(s.toDouble()); });

    connect(mDaySpar1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDaySpar1(s.toDouble()); });
    connect(mDaySpar2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDaySpar2(s.toDouble()); });
    connect(mDaySpar3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDaySpar3(s.toDouble()); });
    connect(mDaySpar4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMDaySpar4(s.toDouble()); });

    connect(mNightBot1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightBot1(s.toDouble()); });
    connect(mNightBot2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightBot2(s.toDouble()); });
    connect(mNightBot3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightBot3(s.toDouble()); });
    connect(mNightBot4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightBot4(s.toDouble()); });

    connect(mNightTop1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightTop1(s.toDouble()); });
    connect(mNightTop2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightTop2(s.toDouble()); });
    connect(mNightTop3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightTop3(s.toDouble()); });
    connect(mNightTop4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightTop4(s.toDouble()); });

    connect(mNightSpar1, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightSpar1(s.toDouble()); });
    connect(mNightSpar2, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightSpar2(s.toDouble()); });
    connect(mNightSpar3, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightSpar3(s.toDouble()); });
    connect(mNightSpar4, &QwwResetLineEdit::textChanged, [=](const QString &s)
            { ecProject_->setScreenMNightSpar4(s.toDouble()); });
}

void AdvProcessingOptions::setBurbaDefaultValues()
{
    // init
    lDayBotGain->setText(tr("0.944"));
    lDayBotOffset->setText(tr("2.57"));
    lDayTopGain->setText(tr("1.005"));
    lDayTopOffset->setText(tr("0.24"));
    lDaySparGain->setText(tr("1.010"));
    lDaySparOffset->setText(tr("0.36"));

    lNightBotGain->setText(tr("0.883"));
    lNightBotOffset->setText(tr("2.17"));
    lNightTopGain->setText(tr("1.008"));
    lNightTopOffset->setText(tr("-0.41"));
    lNightSparGain->setText(tr("1.010"));
    lNightSparOffset->setText(tr("-0.17"));

    mNightBot1->setText(tr("0.5"));
    mNightBot2->setText(tr("-0.1160"));
    mNightBot3->setText(tr("0.0087"));
    mNightBot4->setText(tr("-0.206"));
    mNightTop1->setText(tr("-1.7"));
    mNightTop2->setText(tr("-0.0160"));
    mNightTop3->setText(tr("0.0051"));
    mNightTop4->setText(tr("-0.029"));
    mNightSpar1->setText(tr("-2.1"));
    mNightSpar2->setText(tr("-0.0200"));
    mNightSpar3->setText(tr("0.0070"));
    mNightSpar4->setText(tr("0.026"));

    mDayBot1->setText(tr("2.8"));
    mDayBot2->setText(tr("-0.0681"));
    mDayBot3->setText(tr("0.0021"));
    mDayBot4->setText(tr("-0.334"));
    mDayTop1->setText(tr("-0.1"));
    mDayTop2->setText(tr("-0.0044"));
    mDayTop3->setText(tr("0.0011"));
    mDayTop4->setText(tr("-0.022"));
    mDaySpar1->setText(tr("0.3"));
    mDaySpar2->setText(tr("-0.0007"));
    mDaySpar3->setText(tr("0.0006"));
    mDaySpar4->setText(tr("-0.044"));
}

void AdvProcessingOptions::on_setDefaultsButton_clicked()
{
    DEBUG_FUNC_NAME

    if (requestBurbaSettingsReset())
    {
        setBurbaDefaultValues();
    }
}

void AdvProcessingOptions::onClickWplMethLabel()
{
    wplMethCombo->setFocus();
    wplMethCombo->showPopup();
}

void AdvProcessingOptions::updateWplMeth_1(bool b)
{
    DEBUG_FUNC_NAME
    ecProject_->setGeneralWplMeth(b);
}

// update when change "Compensate density fluctuations Method" combobox
void AdvProcessingOptions::updateToMixingRatio(int n)
{
    DEBUG_FUNC_NAME
    ecProject_->setScreenToMixRatio(wplMethCombo->itemData(n).toInt());

    burbaCorrCheckBox->setEnabled(n == 1);
    burbaLabel->setEnabled(n == 1);

    if (n == 1)
    {
        enableBurbaCorrectionArea(burbaCorrCheckBox->isChecked());
    }
    else
    {
        enableBurbaCorrectionArea(false);
    }
}

void AdvProcessingOptions::enableBurbaCorrectionArea(bool b)
{
    DEBUG_FUNC_NAME
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
    if (b)
    {
        if (WidgetUtils::currentComboItemData(wplMethCombo).toInt() == 0)
        {
            burbaCorrCheckBox->setEnabled(true);
            burbaLabel->setEnabled(true);
            enableBurbaCorrectionArea(burbaCorrCheckBox->isChecked());
        }
        else
        {
            burbaCorrCheckBox->setEnabled(false);
            burbaLabel->setEnabled(false);
            enableBurbaCorrectionArea(false);
        }
    }
    else
    {
        burbaCorrCheckBox->setEnabled(false);
        burbaLabel->setEnabled(false);
        enableBurbaCorrectionArea(false);
    }
}

void AdvProcessingOptions::createQuestionMark()
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
    questionMark_7 = new QPushButton;
    questionMark_7->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_8 = new QPushButton;
    questionMark_8->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_9 = new QPushButton;
    questionMark_9->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_10 = new QPushButton;
    questionMark_10->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_11 = new QPushButton;
    questionMark_11->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_1);
    connect(questionMark_2, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_2);
    connect(questionMark_3, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_3);
    connect(questionMark_4, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_4);
    connect(questionMark_5, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_5);
    connect(questionMark_6, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_6);
    connect(questionMark_7, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_7);
    connect(questionMark_8, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_8);
    connect(questionMark_9, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_9);
    connect(questionMark_10, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_10);
    connect(questionMark_11, &QPushButton::clicked,
            this, &AdvProcessingOptions::onlineHelpTrigger_11);
}

void AdvProcessingOptions::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Wind_Speed_Offsets.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_2()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Angle_of_Attack_Correction.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_3()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Anemometer_Tilt_Correction.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_4()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Calculate_Turbulent_Flux.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_5()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Time_Lag_Detect_Correct.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_6()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Converting_to_Mixing_Ratio.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_7()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Calculating_Off-season_Uptake_Correction.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_8()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Calculating_Spectra_Cospectra_and_Ogives.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_9()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Flux_Quality_Flags.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_10()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Estimating_Flux_Footprint.htm")));
}

void AdvProcessingOptions::onlineHelpTrigger_11()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Raw_Processing_Options.htm")));
}

// gillGroup == 0 -> no gill, generic sonic
// gillGroup == 1 -> wm, wmpro
// gillGroup == 2 -> r2, r3-50, r3-100
// gillGroup == 3 -> r3a-100, hs-50, hs-100
int AdvProcessingOptions::detectAngleOfAttackAnemGroups(const QString& anemModel)
{
    int gillGroup = 0;
    // wm, wmpro
    foreach (const QString& gillModel, DlProject::gillModelGroup_1())
    {
        if (anemModel.contains(gillModel))
        {
            gillGroup = 1;
            break;
        }
    }

    // test only if needed
    // r2, r3-50, r3-100
    if (gillGroup == 0)
    {
        foreach (const QString& gillModel, DlProject::gillModelGroup_2())
        {
            if (anemModel.contains(gillModel))
            {
                gillGroup = 2;
                break;
            }
        }
    }

    // test only if needed
    // r3a-100, hs-50, hs-100
    if (gillGroup == 0)
    {
        foreach (const QString& gillModel, DlProject::gillModelGroup_3())
        {
            if (anemModel.contains(gillModel))
            {
                gillGroup = 3;
                break;
            }
        }
    }

    qDebug() << "gillGroup"<< gillGroup;
    return gillGroup;
}

void AdvProcessingOptions::updateAngleOfAttack(const QString& anemModel)
{
    DEBUG_FUNC_NAME
    qDebug() << "anemModel" << anemModel;

    if (anemModel.isEmpty())
    {
        setEnabledAngleOfAttack();
        return;
    }

    setEnabledAndCheckAngleOfAttack(detectAngleOfAttackAnemGroups(anemModel));
    InfoMessage::showAoaSelectionMsg();
}

void AdvProcessingOptions::setEnabledAndCheckAngleOfAttack(int gillGroup)
{
    DEBUG_FUNC_NAME
    qDebug() << "gillGroup" << gillGroup;
    qDebug() << "flow distorsion" << ecProject_->screenFlowDistortion();

    aoaCheckBox->blockSignals(true);
    aoaMethCombo->blockSignals(true);

    switch (gillGroup)
    {
    case 1:
        qDebug() << "case 1";
        aoaLabel->setEnabled(true);
        aoaCheckBox->setEnabled(true);
        aoaCheckBox->setChecked(true);

        aoaMethLabel->setEnabled(true);
        aoaMethCombo->setEnabled(true);
        // Nakai, 2012
        aoaMethCombo->setCurrentIndex(0);

        ecProject_->setScreenFlowDistortion(1);
        break;
    case 2:
        qDebug() << "case 2";
        aoaLabel->setEnabled(true);
        aoaCheckBox->setEnabled(true);
        aoaCheckBox->setChecked(true);

        aoaMethLabel->setEnabled(true);
        aoaMethCombo->setEnabled(true);
        // Nakai, 2006
        aoaMethCombo->setCurrentIndex(1);

        ecProject_->setScreenFlowDistortion(2);
        break;
    case 3:
        qDebug() << "case 3";
        aoaLabel->setEnabled(true);
        aoaCheckBox->setEnabled(true);
        aoaCheckBox->setChecked(false);

        aoaMethLabel->setEnabled(false);
        aoaMethCombo->setEnabled(false);
        // none
        aoaMethCombo->setCurrentIndex(-1);

        ecProject_->setScreenFlowDistortion(0);
        break;
    default:
        qDebug() << "case default";
        aoaLabel->setEnabled(false);
        aoaCheckBox->setEnabled(false);
        aoaCheckBox->setChecked(false);

        aoaMethLabel->setEnabled(false);
        aoaMethCombo->setEnabled(false);
        // none
        aoaMethCombo->setCurrentIndex(-1);

        ecProject_->setScreenFlowDistortion(0);
        break;
    }

    aoaCheckBox->blockSignals(false);
    aoaMethCombo->blockSignals(false);
}

void AdvProcessingOptions::setSmartfluxUI()
{

}

void AdvProcessingOptions::refreshAngleOfAttack()
{
    int gillGroup = detectAngleOfAttackAnemGroups(ecProject_->generalColMasterSonic());

    aoaCheckBox->blockSignals(true);
    aoaMethCombo->blockSignals(true);

    switch (gillGroup)
    {
    case 1:
    case 2:
        aoaLabel->setEnabled(true);
        aoaCheckBox->setEnabled(true);

        aoaCheckBox->setChecked(ecProject_->screenFlowDistortion());
        if (ecProject_->screenFlowDistortion())
        {
            aoaMethLabel->setEnabled(true);
            aoaMethCombo->setEnabled(true);
            aoaMethCombo->setCurrentIndex(ecProject_->screenFlowDistortion() - 1);
        }
        else
        {
            aoaMethLabel->setEnabled(false);
            aoaMethCombo->setEnabled(false);
            aoaMethCombo->setCurrentIndex(-1);
        }
        break;
    case 3:
        aoaLabel->setEnabled(true);
        aoaCheckBox->setEnabled(true);

        aoaCheckBox->setChecked(ecProject_->screenFlowDistortion());
        if (ecProject_->screenFlowDistortion())
        {
            aoaMethLabel->setEnabled(true);
            aoaMethCombo->setEnabled(true);
            aoaMethCombo->setCurrentIndex(ecProject_->screenFlowDistortion() - 1);
        }
        else
        {
            aoaMethLabel->setEnabled(false);
            aoaMethCombo->setEnabled(false);
            aoaMethCombo->setCurrentIndex(-1);
        }
        break;
    default:
        aoaLabel->setEnabled(false);
        aoaCheckBox->setEnabled(false);
        aoaMethLabel->setEnabled(false);
        aoaMethCombo->setEnabled(false);

        aoaCheckBox->setChecked(false);
        aoaMethCombo->setCurrentIndex(-1);
        break;
    }
    aoaCheckBox->blockSignals(false);
    aoaMethCombo->blockSignals(false);
}

void AdvProcessingOptions::setEnabledAngleOfAttack()
{
    aoaLabel->setEnabled(true);

    aoaCheckBox->blockSignals(true);
    aoaCheckBox->setEnabled(true);
    aoaMethLabel->setEnabled(true);
    aoaMethCombo->setEnabled(false);
    aoaCheckBox->blockSignals(false);
}

void AdvProcessingOptions::updateTooltip(int i)
{
    QComboBox* senderCombo = qobject_cast<QComboBox *>(sender());

    WidgetUtils::updateComboItemTooltip(senderCombo, i);
}

bool AdvProcessingOptions::requestBurbaSettingsReset()
{
    return WidgetUtils::okToQuestion(nullptr,
                tr("Reset Surface Heating Correction"),
                tr("<p>Do you want to reset the surface heating correction "
                   "to the default values of Burba et al. (2008)?</p>"),
                tr("<p>You cannot undo this action.</p>"));
}
