/***************************************************************************
  advspectraloptions.cpp
  -------------------
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include <QDebug>
#include <QTimer>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QRadioButton>
#include <QDateEdit>
#include <QButtonGroup>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QScrollArea>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QUrl>

#include <QwwButtonLineEdit/QwwButtonLineEdit>

#include "customcombomodel.h"
#include "dbghelper.h"
#include "fileutils.h"
#include "alia.h"
#include "clicklabel.h"
#include "ecproject.h"
#include "dlproject.h"
#include "advspectraloptions.h"

AdvSpectralOptions::AdvSpectralOptions(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* config) :
    QWidget(parent),
    dlProject_(dlProject),
    ecProject_(ecProject),
    configState_(config)
{
    lowFreqTitle = new QLabel(tr("Low frequency range"));
    lowFreqTitle->setProperty("groupLabel", true);

    highFreqTitle = new QLabel(tr("High frequency range"));
    highFreqTitle->setProperty("groupLabel", true);

    spectraExistingRadio = new QRadioButton(tr("Spectral assessment file available       :"));
    spectraExistingRadio->setToolTip(tr("<b>Spectral assessment file available:</b> If you have a spectral assessment file from a previous run, and it applies to the current dataset, you can use the same file to by providing the path to the file named \"eddypro_spectral_assessment_ID.txt\". This file includes the results of the assessment. It can be used to shorten program execution time and assure full comparability between previous and current results."));
    spectraExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 0px;}"));

    spectraNonExistingRadio = new QRadioButton(tr("Spectral assessment file not available :"));
    spectraNonExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 0px;}"));
    spectraNonExistingRadio->setToolTip(tr("<b>Spectral assessment file not available:</b> Choose this option and provide the following information if you need to calculate cut-off frequencies for your system. The assessment will be performed as an intermediate step, after all binned spectra for the current dataset are calculated and before calculating and correcting fluxes."));

    spectraFileEdit = new QwwButtonLineEdit();
    spectraFileEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    spectraFileEdit->setButtonVisible(false);
    spectraFileEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    spectraFileEdit->setReadOnly(true);
    spectraFileEdit->setProperty("asButtonLineEdit", true);
    spectraFileLoad = new QPushButton(tr("Load..."));
    spectraFileLoad->setProperty("loadButton", true);
    spectraFileLoad->setToolTip(tr("<b>Load:</b> Load an existing spectral assessment file"));

    QHBoxLayout* spectraFileContainerLayout = new QHBoxLayout;
    spectraFileContainerLayout->addWidget(spectraFileEdit);
    spectraFileContainerLayout->addWidget(spectraFileLoad);
    spectraFileContainerLayout->setStretch(2, 1);
    spectraFileContainerLayout->setContentsMargins(0, 0, 0, 0);
    spectraFileContainerLayout->setSpacing(0);
    QWidget* spectraFileContainer = new QWidget();
    spectraFileContainer->setLayout(spectraFileContainerLayout);

    spectraRadioGroup = new QButtonGroup(this);
    spectraRadioGroup->addButton(spectraExistingRadio, 0);
    spectraRadioGroup->addButton(spectraNonExistingRadio, 1);

    lfMethodCheck = new QCheckBox(tr("Analytic correction of high-pass filtering effects (Moncrieff et al. 2004)"));
    lfMethodCheck->setToolTip(tr("<b>Analytic correction of high-pass filtering effects:</b> Check this option to apply a low frequency spectral correction, to compensate flux losses due to finite averaging length and detrending. The method is adapted to the selected fluctuation computation procedure and its time constant as applicable."));

    hfMethodCheck = new QCheckBox(tr("Correction of low-pass filtering effects"));
    hfMethodCheck->setToolTip(tr("<b>Correction of low-pass filtering effects:</b> Check this option to apply a high frequency spectral correction, to compensate flux losses due to finite sensors separation, signal attenuation, path averaging, time response, etc. Select the most appropriate method according to your EC setup."));

    hfMethLabel = new ClickLabel(tr("Method :"));
    hfMethCombo = new QComboBox();
    hfMethCombo->setModel(new CustomComboModel(hfMethCombo));
    hfMethCombo->addItem(tr("Moncrieff et al. (1997) - Fully analytic"));
    hfMethCombo->addItem(tr("Horst (1997) - Analytic with in situ parameterization"));
    hfMethCombo->addItem(tr("Ibrom et al. (2007) - In situ/analytic"));
    hfMethCombo->addItem(tr("Fratini et al. (2012) - In situ/analytic"));
    hfMethCombo->setItemData(0, tr("<b>Moncrieff et al. (1997):</b> This method models all major sources of flux attenuation by means of a mathematical formulation. The use of this method is suggested for open path EC systems or for closed path systems if the sampling line is short and heated. This method may seriously underestimate the attenuation (and hence the correction) - notably for water vapor - when the sampling line is long and/or not heated, because of the dependency of attenuation of H<sub>2</sub>O on relative humidity."), Qt::ToolTipRole);
    hfMethCombo->setItemData(1, tr("<b>Horst (1997):</b> Correction method based on an analytical formulation of the spectral correction factor that requires an in-situ assessment of the system's cut-off frequency. Provide the information below to specify how to perform such assessment."), Qt::ToolTipRole);
    hfMethCombo->setItemData(2, tr("<b>Ibrom et al. (2007):</b> Correction method based on an analytical formulation of the spectra correction factors, that requires an in-situ assessment of the system's cut-off frequencies, separately for each instrument and gas, and as a function of relative humidity for water vapor. Provide the settings in the <i>Assessment of high-frequency attenuation</i> to specify how to perform the assessment. This method is recommended in most cases, notably for closed-path systems placed high over rough canopies."), Qt::ToolTipRole);
    hfMethCombo->setItemData(3, tr("<b>Fratini et al. (2012):</b> Correction method based on the combination of a direct approach (similar to Hollinger et al., 2009) and the analytical formulation of Ibrom et al., 2007. It requires an in-situ assessment of the system's cut-off frequencies, separately for each instrument and gas, and as a function of relative humidity for water vapor. It also requires full length cospectra of measured sensible heat. This method is recommendable in most cases, notably for closed-path systems placed low over smooth surfaces."), Qt::ToolTipRole);

    horstCheck = new QCheckBox(tr("Correction for instruments separation"));
    horstCheck->setToolTip(tr("<b>Correction for instrument separation:</b> Check this option and select the corresponding method to add an extra correction term to that calculated with the method by Ibrom et al. (2007). This accounts for any separation between the inlet of the sampling line (closed path instruments) or the center of the open path instrument and the center of the anemometer."));
    horstMethodLabel = new ClickLabel(tr("Method :"));
    horstCombo = new QComboBox();
    horstCombo->addItem(tr("Horst and Lenschow (2009), along-wind, crosswind and vertical"));
    horstCombo->addItem(tr("Horst and Lenschow (2009), only crosswind and vertical"));
    horstCombo->setItemData(0, tr("<b>Horst and Lenschow (2009), along-wind, crosswind and vertical:</b> Select this option to account for sensor separations in any direction. Note that correcting for along-wind separations may result in overcorrection, if any time lag compensation method was also selected."), Qt::ToolTipRole);
    horstCombo->setItemData(1, tr("<b>Horst and Lenschow (2009), only crosswind and vertical:</b> Select this option to account for sensor separations only in the crosswind and vertical directions. Recommended when a time lag compensation method is selected."), Qt::ToolTipRole);

    freqAttenuationTitle = new QLabel(tr("Assessment of high-frequency attenuation"));
    freqAttenuationTitle->setProperty("groupLabel", true);

    binnedSpectraExistingRadio = new QRadioButton(tr("Binned spectra files available for this dataset :"));
    binnedSpectraExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 0px;}"));
    binnedSpectraExistingRadio->setToolTip(tr("<b>Binned spectra files available:</b> Select this option if you already obtained <i>Binned spectra and cospectra files</i> for the current dataset in a previous run of EddyPro. Note that the binned (co)spectra files do not need to correspond exactly to the current dataset, rather they need to be representative of it. Binned spectra are used here for quantification of spectral attenuations, thus they must have been collected in conditions comparable to those of the current dataset (e.g., the same EC system and similar canopy heights, measurement height, instrument spatial separations). At least one month of spectra files is needed for a robust spectral attenuation assessment. If you select this option, the option <i>All binned spectra and cospectra</i> in the Output Files page will be automatically deselected and activated."));

    binnedSpectraNonExistingRadio = new QRadioButton(tr("Binned spectra files not available"));
    binnedSpectraNonExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 65px;}"));
    binnedSpectraNonExistingRadio->setToolTip(tr("<b>Binned spectra files not available:</b> Select this option if you did not yet obtain &lt;i&gt;Binned spectra and cospectra files&lt;/i&gt; for the current dataset in a previous run of EddyPro. Note that such binned (co)spectra files do not need to correspond exactly to the current dataset, rather they need to be representative of it. Binned spectra are used to quantify the spectral attenuations, thus they must have been collected in conditions comparable to those of the current dataset (e.g., same EC system and similar canopy heights, measurement height, instrument spatial separations, etc.). At least one month of spectra files is needed for a robust spectral attenuation assessment. If you select this option, the option <i>All binned spectra and cospectra</i> in the Output Files page will be automatically selected."));

    binnedSpectraDirEdit = new QwwButtonLineEdit();
    binnedSpectraDirEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    binnedSpectraDirEdit->setButtonVisible(false);
    binnedSpectraDirEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    binnedSpectraDirEdit->setReadOnly(true);
    binnedSpectraDirEdit->setProperty("asButtonLineEdit", true);
    binnedSpectraDirBrowse = new QPushButton(tr("Browse..."));
    binnedSpectraDirBrowse->setProperty("loadButton", true);
    binnedSpectraDirBrowse->setToolTip(tr("<b>Browse:</b> Specify the folder that contains the binned spectra files."));

    QHBoxLayout* binnedSpectraFileContainerLayout = new QHBoxLayout;
    binnedSpectraFileContainerLayout->addWidget(binnedSpectraDirEdit);
    binnedSpectraFileContainerLayout->addWidget(binnedSpectraDirBrowse);
    binnedSpectraFileContainerLayout->setStretch(2, 1);
    binnedSpectraFileContainerLayout->setContentsMargins(0, 0, 0, 0);
    binnedSpectraFileContainerLayout->setSpacing(0);
    QWidget* binnedSpectraFileContainer = new QWidget();
    binnedSpectraFileContainer->setLayout(binnedSpectraFileContainerLayout);

    binnedSpectraRadioGroup = new QButtonGroup(this);
    binnedSpectraRadioGroup->addButton(binnedSpectraNonExistingRadio, 0);
    binnedSpectraRadioGroup->addButton(binnedSpectraExistingRadio, 1);

    subsetCheckBox = new QCheckBox();
    subsetCheckBox->setText(tr("Select a subperiod"));
    subsetCheckBox->setToolTip(tr("<b>Select a subperiod:</b> Select this option if you only want to process a subset of data in the raw data directory. Leave it blank to process all the raw data in the directory."));
    subsetCheckBox->setStyleSheet(QStringLiteral("QCheckBox {margin-left: 47px;}"));

    lockedIcon = new QLabel();
    lockedIcon->setPixmap(QPixmap(QStringLiteral(":/icons/vlink-locked")));

    startDateLabel = new ClickLabel(this);
    startDateLabel->setText(tr("Start :"));
    startDateLabel->setToolTip(tr("<b>Start:</b> Starting date of the dataset to be used for the spectral assessment. We recommend using a time period that is as long as possible. However, make sure that the instrument setup (sampling line, instrument separations) did not undergo any major change during the selected time period."));
    startDateEdit = new QDateEdit();
    startDateEdit->setToolTip(startDateLabel->toolTip());
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate(2000, 1, 1));
    Alia::customizeCalendar(startDateEdit->calendarWidget());

    endDateLabel = new ClickLabel(this);
    endDateLabel->setText(tr("End :"));
    endDateLabel->setToolTip(tr("<b>End:</b> Ending date of the dataset to be used for the spectral assessment. We recommend using a time period that is as long as possible. However, make sure that the instrument setup (sampling line, instrument separations) did not undergo any major change during the selected time period."));
    endDateEdit = new QDateEdit();
    endDateEdit->setToolTip(endDateLabel->toolTip());
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate::currentDate());
    Alia::customizeCalendar(endDateEdit->calendarWidget());

    minSmplLabel = new ClickLabel(tr("Minimum number of spectra for valid averages :"));
    minSmplLabel->setToolTip(tr("<b>Minimum number of spectra for valid averages:</b> Select the minimum number of spectra that should be found in each class, for the corresponding ensemble average to be valid. Currently classes are defined only for H<sub>2</sub>O with respect to ambient relative humidity: 9 classes are defined between RH = 5% and RH = 95%. We expect to add classes also for passive gases, related to time periods. Entering a number that is too high may imply that, for certain classes, average spectra cannot be calculated. A number that is too small may result in poor characterization of average spectra. The higher this number, the longer the time period needed."));
    minSmplSpin = new QSpinBox;
    minSmplSpin->setRange(1, 1000);
    minSmplSpin->setSingleStep(1);
    minSmplSpin->setAccelerated(true);
    minSmplSpin->setToolTip(minSmplLabel->toolTip());

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

    noiseFreqLabel = new QLabel(tr("Removal of high frequency noise "
                                           "prior to fitting in-situ transfer functions"));
    noiseFreqLabel->setProperty("blueLabel", true);

    minSpinCo2Label = new ClickLabel(tr("Minimum (absolute) %1 flux :").arg(Defs::CO2_STRING));
    minSpinCo2Label->setToolTip(tr("<b>Minimum (absolute) CO<sub>2</sub> flux:</b> CO<sub>2</sub> spectra corresponding to fluxes smaller (in module) than this value will not be considered in the ensemble averaging procedure. Selecting high-enough fluxes assures that well developed turbulent conditions are met and the corresponding gas concentration spectra are well characterized."));

    minSpinCh4Label = new ClickLabel(tr("Minimum (absolute) %1 flux :").arg(Defs::CH4_STRING));
    minSpinCh4Label->setToolTip(tr("<b>Minimum (absolute) CH<sub>4</sub> flux:</b> CH<sub>4</sub> spectra corresponding to fluxes smaller (in module) than this value will not be considered in the ensemble averaging procedure. Selecting high-enough fluxes assures that well developed turbulent conditions are met and the corresponding gas concentration spectra are well characterized."));

    minSpinGas4Label = new ClickLabel(tr("Minimum (absolute) %1 gas flux :").arg(Defs::GAS4_STRING));
    minSpinGas4Label->setToolTip(tr("<b>Minimum (absolute) 4<sup>th</sup> gas flux:</b> 4<sup>th</sup> gas spectra corresponding to fluxes smaller (in module) than this value will not be considered in the ensemble averaging procedure. Selecting high-enough fluxes assures that well developed turbulent conditions are met and the corresponding gas concentration spectra are well characterized."));

    minSpinLeLabel = new ClickLabel(tr("Minimum latent heat flux :"));
    minSpinLeLabel->setToolTip(tr("<b>Minimum latent heat flux:</b> H<sub>2</sub>O spectra corresponding to latent heat fluxes smaller than this value will not be considered in the ensemble averaging procedure. Selecting high-enough fluxes assures that well developed turbulent conditions are met and the corresponding gas concentration spectra are well characterized."));

    minSpinHLabel = new ClickLabel(tr("Minimum sensible heat flux :"));
    minSpinHLabel->setToolTip(tr("<b>Minimum sensible heat flux:</b> Temperature spectra corresponding to sensible heat fluxes smaller than this value will not be considered in the ensemble averaging procedure. Selecting high-enough fluxes assures that well developed turbulent conditions are met and the corresponding gas concentration spectra are well characterized."));

    minSpinCo2 = new QDoubleSpinBox;
    minSpinCo2->setRange(0.0, 1000.0);
    minSpinCo2->setSingleStep(10.0);
    minSpinCo2->setDecimals(4);
    minSpinCo2->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    minSpinCo2->setAccelerated(true);
    minSpinCo2->setToolTip(minSpinCo2Label->toolTip());

    minSpinCh4 = new QDoubleSpinBox;
    minSpinCh4->setRange(0.0, 1000.0);
    minSpinCh4->setSingleStep(10.0);
    minSpinCh4->setDecimals(4);
    minSpinCh4->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    minSpinCh4->setAccelerated(true);
    minSpinCh4->setToolTip(minSpinCh4Label->toolTip());

    minSpinGas4 = new QDoubleSpinBox;
    minSpinGas4->setRange(0.0, 1000.0);
    minSpinGas4->setSingleStep(10.0);
    minSpinGas4->setDecimals(4);
    minSpinGas4->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    minSpinGas4->setAccelerated(true);
    minSpinGas4->setToolTip(minSpinGas4Label->toolTip());

    minSpinLe = new QDoubleSpinBox;
    minSpinLe->setRange(-1000.0, 1000.0);
    minSpinLe->setSingleStep(10.0);
    minSpinLe->setDecimals(4);
    minSpinLe->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    minSpinLe->setAccelerated(true);
    minSpinLe->setToolTip(minSpinLeLabel->toolTip());

    minSpinH = new QDoubleSpinBox;
    minSpinH->setRange(-1000.0, 1000.0);
    minSpinH->setSingleStep(10.0);
    minSpinH->setDecimals(4);
    minSpinH->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    minSpinH->setAccelerated(true);
    minSpinH->setToolTip(minSpinHLabel->toolTip());

    spin10Label = new ClickLabel(tr("Lowest frequency :"));
    spin10Label->setToolTip(tr("<b>Lowest frequency:</b> The assessment of the system transfer function implies the frequency-wise ratio of gas concentration to temperature spectra (temperature considered as proxy for un-attenuated atmospheric scalar spectra). This ratio must be taken in the frequency range where the system filtering is expected to occur. At lower frequencies, slow-paced atmospheric and source/sink dynamics may imply a breakdown of the similarity assumption. Default values can be good in most occasions, but the lower frequency should be adapted based mostly on the averaging interval."));

    spin20Label = new ClickLabel(tr("Highest frequency :"));
    spin20Label->setToolTip(tr("<b>Highest frequency:</b> The assessment of the system transfer function implies the frequency-wise ratio of gas concentration to temperature spectra (temperature being considered as a proxy for un-attenuated atmospheric scalar spectra). This ratio must be taken in the frequency range where the system filtering is expected to occur. At higher frequencies, noise and aliasing may corrupt the procedure. Default values can be good in most occasions, but the higher frequency should be adapted based on acquisition frequency and instrument performance."));

    spin30Label = new ClickLabel(tr("Lowest noise frequency :"));
    spin30Label->setToolTip(tr("<b>Lowest noise frequency:</b> High-frequency noise (blue noise) can compromise the spectral assessment by modifying the shape of spectra. EddyPro has an option to eliminate such noise. Set the minimum frequency at which you expect the noise to start being relevant. EddyPro will linearly (in a log-log sense) interpolate the high frequency portion of the spectra and subtract it from the spectra before calculating transfer functions. Set 0 Hz to instruct EddyPro to not perform noise elimination. In this case the string <i>Do not remove noise</i> will appear in this field."));

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

    fullSpectraExistingRadio = new QRadioButton(tr("Full w/Ts cospectra files available for this dataset :"));
    fullSpectraExistingRadio->setToolTip(tr("<b>Full w/Ts cospectra files available:</b> Select this option if you already obtained <i>Full cospectra of w/T<sub>s</sub></i> for the current dataset (from a previous run of EddyPro). Note that the cospectra files need to correspond exactly to the current dataset. Full cospectra of w/T<sub>s</sub> (sensible heat) are used for definition of the spectral correction factor for each flux with the method by Fratini et al. (2012). If you select this option, the option <i>Full length cospectra w/T<sub>s</sub></i> in the Output Files page will be automatically deselected and activated."));
    fullSpectraExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 0px;}"));

    fullSpectraNonExistingRadio = new QRadioButton(tr("Full w/Ts cospectra files not available"));
    fullSpectraNonExistingRadio->setStyleSheet(QStringLiteral("QRadioButton {margin-right: 65px;}"));
    fullSpectraNonExistingRadio->setToolTip(tr("<b>Full w/T<sub>s</sub> cospectra files not available:</b> Select this option if you do not have <i>Full cospectra of w/T<sub>s</sub></i> for the current dataset (from a previous run of EddyPro). Note that existing cospectra files need to correspond exactly to the current dataset. Full cospectra of w/T<sub>s</sub> (sensible heat) are used for definition of the spectral correction factor for each flux with the method by Fratini et al. (2012). If you select this option, the option <i>Full length cospectra w/T<sub>s</sub></i> in the Output Files page will be automatically selected and deactivated."));

    fullSpectraDirEdit = new QwwButtonLineEdit();
    fullSpectraDirEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    fullSpectraDirEdit->setButtonVisible(false);
    fullSpectraDirEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    fullSpectraDirEdit->setReadOnly(true);
    fullSpectraDirEdit->setProperty("asButtonLineEdit", true);
    fullSpectraDirBrowse = new QPushButton(tr("Browse..."));
    fullSpectraDirBrowse->setProperty("loadButton", true);
    fullSpectraDirBrowse->setToolTip(tr("<b>Browse:</b> Specify the folder that contains the full w/T<sub>s</sub> cospectra files."));

    QHBoxLayout* fullSpectraFileContainerLayout = new QHBoxLayout;
    fullSpectraFileContainerLayout->addWidget(fullSpectraDirEdit);
    fullSpectraFileContainerLayout->addWidget(fullSpectraDirBrowse);
    fullSpectraFileContainerLayout->setStretch(2, 1);
    fullSpectraFileContainerLayout->setContentsMargins(0, 0, 0, 0);
    fullSpectraFileContainerLayout->setSpacing(0);
    QWidget* fullSpectraFileContainer = new QWidget();
    fullSpectraFileContainer->setLayout(fullSpectraFileContainerLayout);

    fullSpectraRadioGroup = new QButtonGroup(this);
    fullSpectraRadioGroup->addButton(fullSpectraNonExistingRadio, 0);
    fullSpectraRadioGroup->addButton(fullSpectraExistingRadio, 1);

    // require binnedSpectraDirEdit and fullSpectraDirEdit
    binnedSpectraDirEdit->installEventFilter(const_cast<AdvSpectralOptions*>(this));
    fullSpectraDirEdit->installEventFilter(const_cast<AdvSpectralOptions*>(this));
    spectraFileEdit->installEventFilter(const_cast<AdvSpectralOptions*>(this));

    fratiniTitle = new QLabel(tr("Fratini et al. (2012) method settings"));
    fratiniTitle->setProperty("groupLabel", true);

    addSonicCheck = new QCheckBox(tr("Include anemometer losses for path averaging and time response"));
    addSonicCheck->setToolTip(tr("<b>Include anemometer losses for path averaging and time response:</b> Select this option to instruct EddyPro to correct sensible heat cospectra for those losses, before using them as a model to calculate correction factors according to Fratini et al. (2012)."));

    thresholdLabel = new QLabel(tr("Threshold fluxes for using model and direct method "
                                           "(model is used for small fluxes)"));
    thresholdLabel->setProperty("blueLabel", true);
    thresholdLabel->setToolTip(tr("<b>Threshold fluxes for using model and direct method:</b> Select threshold flux values: for fluxes smaller than the thresholds, EddyPro will use the correction factor model of Ibrom et al. (2007). For fluxes larger than the threshold, EddyPro will use the direct method as from Fratini et al. (2012)."));

    f10Spin1Label = new ClickLabel(tr("%1 :").arg(Defs::CO2_STRING));
    f10Spin2Label = new ClickLabel(tr("%1 :").arg(Defs::CH4_STRING));
    f10Spin3Label = new ClickLabel(tr("%1 gas :").arg(Defs::GAS4_STRING));
    f10Spin4Label = new ClickLabel(tr("Latent heat :"));
    f10Spin5Label = new ClickLabel(tr("Sensible heat :"));

    f10Co2Spin = new QDoubleSpinBox;
    f10Co2Spin->setRange(-1000.0, 1000.0);
    f10Co2Spin->setSingleStep(10.0);
    f10Co2Spin->setDecimals(4);
    f10Co2Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    f10Co2Spin->setAccelerated(true);

    f10Ch4Spin = new QDoubleSpinBox;
    f10Ch4Spin->setRange(0.0-1000.0, 1000.0);
    f10Ch4Spin->setSingleStep(10.0);
    f10Ch4Spin->setDecimals(4);
    f10Ch4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    f10Ch4Spin->setAccelerated(true);

    f10Gas4Spin = new QDoubleSpinBox;
    f10Gas4Spin->setRange(-1000.0, 1000.0);
    f10Gas4Spin->setSingleStep(10.0);
    f10Gas4Spin->setDecimals(4);
    f10Gas4Spin->setSuffix(tr(" [%1]").arg(Defs::UMOL_M2S_STRING));
    f10Gas4Spin->setAccelerated(true);

    f10LeSpin = new QDoubleSpinBox;
    f10LeSpin->setRange(-1000.0, 1000.0);
    f10LeSpin->setSingleStep(10.0);
    f10LeSpin->setDecimals(4);
    f10LeSpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    f10LeSpin->setAccelerated(true);

    f10HSpin = new QDoubleSpinBox;
    f10HSpin->setRange(-1000.0, 1000.0);
    f10HSpin->setSingleStep(10.0);
    f10HSpin->setDecimals(4);
    f10HSpin->setSuffix(tr(" [%1]").arg(Defs::W_M2_STRING));
    f10HSpin->setAccelerated(true);

    QLabel *hrLabel_1 = new QLabel;
    hrLabel_1->setObjectName(QStringLiteral("hrLabel"));
    QLabel *hrLabel_2 = new QLabel;
    hrLabel_2->setObjectName(QStringLiteral("hrLabel"));
    QLabel *hrLabel_3 = new QLabel;
    hrLabel_3->setObjectName(QStringLiteral("hrLabel"));
    hrLabel_3->setStyleSheet(QStringLiteral("QLabel {margin: 0px;}"));

    QLabel *hrLabel_5 = new QLabel;
    hrLabel_5->setObjectName(QStringLiteral("hrLabel"));
    hrLabel_5->setStyleSheet(QStringLiteral("QLabel {margin: 0px 0px 10px 0px;}"));

    QGridLayout* linkedLayout = new QGridLayout;
    linkedLayout->addWidget(lockedIcon, 0, 0, 2, 1, Qt::AlignHCenter | Qt::AlignVCenter);
    linkedLayout->addWidget(startDateEdit, 0, 1);
    linkedLayout->addWidget(endDateEdit, 1, 1);
    linkedLayout->setVerticalSpacing(3);

    settingsGroupTitle = new QLabel(tr("Spectral Correction Options"));
    settingsGroupTitle->setProperty("groupTitle2", true);

    createQuestionMark();

    QHBoxLayout* qBox_1 = new QHBoxLayout;
    qBox_1->addWidget(settingsGroupTitle);
    qBox_1->addWidget(questionMark_1, 0, Qt::AlignRight | Qt::AlignBottom);
    qBox_1->addStretch();

    QHBoxLayout* qBox_2 = new QHBoxLayout;
    qBox_2->addWidget(lowFreqTitle);
    qBox_2->addWidget(questionMark_2, 0, Qt::AlignLeft | Qt::AlignVCenter);
    qBox_2->addStretch();

    QHBoxLayout* qBox_3 = new QHBoxLayout;
    qBox_3->addWidget(highFreqTitle);
    qBox_3->addWidget(questionMark_3, 0, Qt::AlignLeft | Qt::AlignVCenter);
    qBox_3->addStretch();

    QHBoxLayout* qBox_4 = new QHBoxLayout;
    qBox_4->addWidget(freqAttenuationTitle);
    qBox_4->addWidget(questionMark_4, 0, Qt::AlignLeft | Qt::AlignVCenter);
    qBox_4->addStretch();

    suggestedRangeButton = new QPushButton(tr("Set suggested ranges"));
    suggestedRangeButton->setProperty("mdButton", true);
    suggestedRangeButton->setText(tr("Set Suggested Ranges"));
    suggestedRangeButton->setToolTip(tr("<b>Set Suggested Ranges</b>."));

    QGridLayout *hfLayout = new QGridLayout();
    hfLayout->addLayout(qBox_4, 9, 0);
    hfLayout->addWidget(binnedSpectraNonExistingRadio, 10, 0, Qt::AlignRight);
    hfLayout->addWidget(binnedSpectraExistingRadio, 11, 0, Qt::AlignRight);
    hfLayout->addWidget(binnedSpectraFileContainer, 11, 1, 1, 3);
    hfLayout->addWidget(subsetCheckBox, 12, 0, 1, 1, Qt::AlignLeft);
    hfLayout->addWidget(startDateLabel, 12, 0, Qt::AlignRight);
    hfLayout->addLayout(linkedLayout, 12, 1, 2, 1);
    hfLayout->addWidget(endDateLabel, 13, 0, Qt::AlignRight);
    hfLayout->addWidget(minSmplLabel, 14, 0, Qt::AlignRight);
    hfLayout->addWidget(minSmplSpin, 14, 1);
    hfLayout->addWidget(minSpinCo2Label, 15, 0, Qt::AlignRight);
    hfLayout->addWidget(minSpinCo2, 15, 1);
    hfLayout->addWidget(minSpinCh4Label, 16, 0, Qt::AlignRight);
    hfLayout->addWidget(minSpinCh4, 16, 1);
    hfLayout->addWidget(minSpinGas4Label, 17, 0, Qt::AlignRight);
    hfLayout->addWidget(minSpinGas4, 17, 1);
    hfLayout->addWidget(minSpinLeLabel, 18, 0, Qt::AlignRight);
    hfLayout->addWidget(minSpinLe, 18, 1);
    hfLayout->addWidget(minSpinHLabel, 19, 0, Qt::AlignRight);
    hfLayout->addWidget(minSpinH, 19, 1);
    hfLayout->addWidget(spin11Label, 20, 1, Qt::AlignCenter);
    hfLayout->addWidget(spin12Label, 20, 2, Qt::AlignCenter);
    hfLayout->addWidget(spin13Label, 20, 3, Qt::AlignCenter);
    hfLayout->addWidget(spin14Label, 20, 4, Qt::AlignCenter);
    hfLayout->addWidget(hrLabel_3, 21, 1, 1, 4);
    hfLayout->addWidget(minMaxFreqLabel, 22, 1, 1, 4, Qt::AlignLeft);
    hfLayout->addWidget(spin10Label, 23, 0, Qt::AlignRight);
    hfLayout->addWidget(spin11, 23, 1);
    hfLayout->addWidget(spin12, 23, 2);
    hfLayout->addWidget(spin13, 23, 3);
    hfLayout->addWidget(spin14, 23, 4);
    hfLayout->addWidget(spin20Label, 24, 0, Qt::AlignRight);
    hfLayout->addWidget(spin21, 24, 1);
    hfLayout->addWidget(spin22, 24, 2);
    hfLayout->addWidget(spin23, 24, 3);
    hfLayout->addWidget(spin24, 24, 4);
    hfLayout->addWidget(noiseFreqLabel, 25, 1, 1, 4, Qt::AlignLeft);
    hfLayout->addWidget(spin30Label, 26, 0, Qt::AlignRight);
    hfLayout->addWidget(spin31, 26, 1);
    hfLayout->addWidget(spin32, 26, 2);
    hfLayout->addWidget(spin33, 26, 3);
    hfLayout->addWidget(spin34, 26, 4);
    hfLayout->addWidget(suggestedRangeButton, 27, 1, 1, 1);
    hfLayout->setRowStretch(28, 1);
    hfLayout->setColumnStretch(5, 1);
    hfLayout->setContentsMargins(100, 0, 0, 0);

    QGridLayout *hffLayout = new QGridLayout();
    hffLayout->addWidget(fullSpectraNonExistingRadio, 0, 0, Qt::AlignRight);
    hffLayout->addWidget(fullSpectraExistingRadio, 1, 0, Qt::AlignRight);
    hffLayout->addWidget(fullSpectraFileContainer, 1, 1, 1, 2);
    hffLayout->addWidget(addSonicCheck, 2, 1, 1, 3);
    hffLayout->addWidget(thresholdLabel, 3, 1, 1, 4, Qt::AlignLeft);
    hffLayout->addWidget(f10Spin1Label, 4, 0, Qt::AlignRight);
    hffLayout->addWidget(f10Co2Spin, 4, 1);
    hffLayout->addWidget(f10Spin2Label, 5, 0, Qt::AlignRight);
    hffLayout->addWidget(f10Ch4Spin, 5, 1);
    hffLayout->addWidget(f10Spin3Label, 6, 0, Qt::AlignRight);
    hffLayout->addWidget(f10Gas4Spin, 6, 1);
    hffLayout->addWidget(f10Spin4Label, 7, 0, Qt::AlignRight);
    hffLayout->addWidget(f10LeSpin, 7, 1);
    hffLayout->addWidget(f10Spin5Label, 8, 0, Qt::AlignRight);
    hffLayout->addWidget(f10HSpin, 8, 1);
    hffLayout->setRowStretch(9, 1);
    hffLayout->setContentsMargins(112, 0, 0, 0);

    QGridLayout *settingsLayout = new QGridLayout();
    settingsLayout->addLayout(qBox_2, 0, 0);
    settingsLayout->addWidget(lfMethodCheck, 1, 0, 1, 2);
    settingsLayout->addWidget(hrLabel_1, 2, 0, 1, -1);
    settingsLayout->addLayout(qBox_3, 3, 0);
    settingsLayout->addWidget(hfMethodCheck, 4, 0);
    settingsLayout->addWidget(hfMethLabel, 4, 0, Qt::AlignRight);
    settingsLayout->addWidget(hfMethCombo, 4, 1, 1, 3);
    settingsLayout->addWidget(horstCheck, 5, 0);
    settingsLayout->addWidget(horstMethodLabel, 5, 0, Qt::AlignRight);
    settingsLayout->addWidget(horstCombo, 5, 1, 1, 3);
    settingsLayout->addWidget(spectraExistingRadio, 6, 0, Qt::AlignRight);
    settingsLayout->addWidget(spectraFileContainer, 6, 1, 1, 3);
    settingsLayout->addWidget(spectraNonExistingRadio, 7, 0, Qt::AlignRight);
    settingsLayout->addLayout(hfLayout, 8, 0, 1, 5);
    settingsLayout->addWidget(fratiniTitle, 9, 0);
    settingsLayout->addLayout(hffLayout, 10, 0, 1, 5);
    settingsLayout->setColumnStretch(4, 1);

    QWidget *overallFrame = new QWidget();
    overallFrame->setProperty("scrollContainerWidget", true);
    overallFrame->setLayout(settingsLayout);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(overallFrame);
    scrollArea->setWidgetResizable(true);

    QHBoxLayout* settingsGroupLayout = new QHBoxLayout;
    settingsGroupLayout->addWidget(scrollArea);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(qBox_1);
    mainLayout->addLayout(settingsGroupLayout);
    mainLayout->setContentsMargins(15, 15, 0, 10);
    setLayout(mainLayout);

    connect(spectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(spectraRadioClicked(int)));

    connect(spectraFileEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearSpectraFileEdit()));
    connect(spectraFileEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateSpectraFile(QString)));
    connect(spectraFileLoad, SIGNAL(clicked()),
            this, SLOT(spectraFileLoad_clicked()));

    connect(binnedSpectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(binnedSpectraRadioClicked(int)));

    connect(binnedSpectraDirEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearBinnedSpectraDirEdit()));
    connect(binnedSpectraDirEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateBinnedSpectraFile(QString)));
    connect(binnedSpectraDirBrowse, SIGNAL(clicked()),
            this, SLOT(binnedSpectraDirBrowse_clicked()));

    connect(subsetCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateSubsetSelection(bool)));

    connect(startDateLabel, SIGNAL(clicked()),
            this, SLOT(onStartDateLabelClicked()));
    connect(startDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(updateStartDate(QDate)));

    connect(endDateLabel, SIGNAL(clicked()),
            this, SLOT(onEndDateLabelClicked()));
    connect(endDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(updateEndDate(QDate)));

    connect(lfMethodCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateLfMethod(bool)));

    connect(hfMethodCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateHfMethod_1(bool)));
    connect(hfMethodCheck, SIGNAL(toggled(bool)),
            hfMethLabel, SLOT(setEnabled(bool)));
    connect(hfMethodCheck, SIGNAL(toggled(bool)),
            hfMethCombo, SLOT(setEnabled(bool)));
    connect(hfMethLabel, SIGNAL(clicked()),
            this, SLOT(onClickHfMethLabel()));
    connect(hfMethCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateHfMethod_2(int)));

    connect(horstCheck, SIGNAL(toggled(bool)),
            horstMethodLabel, SLOT(setEnabled(bool)));
    connect(horstCheck, SIGNAL(toggled(bool)),
            horstCombo, SLOT(setEnabled(bool)));
    connect(horstMethodLabel, SIGNAL(clicked()),
            this, SLOT(onClickHorstLabel()));
    connect(horstCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateHorst_1(bool)));
    connect(horstCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateHorst_2(int)));

    connect(minSmplLabel, SIGNAL(clicked()),
            this, SLOT(onMinSmplLabelClicked()));
    connect(minSmplSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateMinSmpl(int)));

    connect(minSpinCo2Label, SIGNAL(clicked()),
            this, SLOT(onMinCo2LabelClicked()));
    connect(minSpinCo2, SIGNAL(valueChanged(double)),
            this, SLOT(updateMinCo2(double)));

    connect(minSpinCh4Label, SIGNAL(clicked()),
            this, SLOT(onMinCh4LabelClicked()));
    connect(minSpinCh4, SIGNAL(valueChanged(double)),
            this, SLOT(updateMinCh4(double)));

    connect(minSpinGas4Label, SIGNAL(clicked()),
            this, SLOT(onMinGas4LabelClicked()));
    connect(minSpinGas4, SIGNAL(valueChanged(double)),
            this, SLOT(updateMinGas4(double)));

    connect(minSpinLeLabel, SIGNAL(clicked()),
            this, SLOT(onMinLeLabelClicked()));
    connect(minSpinLe, SIGNAL(valueChanged(double)),
            this, SLOT(updateMinLe(double)));

    connect(minSpinHLabel, SIGNAL(clicked()),
            this, SLOT(onMinHLabelClicked()));
    connect(minSpinH, SIGNAL(valueChanged(double)),
            this, SLOT(updateMinH(double)));

    connect(spin10Label, SIGNAL(clicked()),
            this, SLOT(onSpin10LabelClicked()));
    connect(spin11, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminCo2(double)));
    connect(spin12, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminH2o(double)));
    connect(spin13, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminCh4(double)));
    connect(spin14, SIGNAL(valueChanged(double)),
            this, SLOT(updateFminGas4(double)));

    connect(spin20Label, SIGNAL(clicked()),
            this, SLOT(onSpin20LabelClicked()));
    connect(spin21, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxCo2(double)));
    connect(spin22, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxH2o(double)));
    connect(spin23, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxCh4(double)));
    connect(spin24, SIGNAL(valueChanged(double)),
            this, SLOT(updateFmaxGas4(double)));

    connect(spin30Label, SIGNAL(clicked()),
            this, SLOT(onSpin30LabelClicked()));
    connect(spin31, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnCo2(double)));
    connect(spin32, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnH2o(double)));
    connect(spin33, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnCh4(double)));
    connect(spin34, SIGNAL(valueChanged(double)),
            this, SLOT(updateHfnGas4(double)));

    connect(suggestedRangeButton, SIGNAL(clicked()),
            this, SLOT(updateSuggestedFrequencyRanges()));

    connect(fullSpectraRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(fullSpectraRadioClicked(int)));

    connect(fullSpectraDirEdit, SIGNAL(buttonClicked()),
            this, SLOT(clearFullSpectraDirEdit()));
    connect(fullSpectraDirEdit, SIGNAL(textChanged(QString)),
            this, SLOT(updateFullSpectraFile(QString)));
    connect(fullSpectraDirBrowse, SIGNAL(clicked()),
            this, SLOT(fullSpectraDirBrowse_clicked()));

    connect(addSonicCheck, SIGNAL(toggled(bool)),
            this, SLOT(updateAddSonic(bool)));

    connect(f10Spin1Label, SIGNAL(clicked()),
            this, SLOT(onF10Co2LabelClicked()));
    connect(f10Co2Spin, SIGNAL(valueChanged(double)),
            this, SLOT(updateF10Co2(double)));

    connect(f10Spin2Label, SIGNAL(clicked()),
            this, SLOT(onF10Ch4LabelClicked()));
    connect(f10Ch4Spin, SIGNAL(valueChanged(double)),
            this, SLOT(updateF10Ch4(double)));

    connect(f10Spin3Label, SIGNAL(clicked()),
            this, SLOT(onF10Gas4LabelClicked()));
    connect(f10Gas4Spin, SIGNAL(valueChanged(double)),
            this, SLOT(updateF10Gas4(double)));

    connect(f10Spin4Label, SIGNAL(clicked()),
            this, SLOT(onF10LeLabelClicked()));
    connect(f10LeSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateF10Le(double)));

    connect(f10Spin5Label, SIGNAL(clicked()),
            this, SLOT(onF10HLabelClicked()));
    connect(f10HSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateF10H(double)));

    connect(ecProject_, SIGNAL(ecProjectNew()),
            this, SLOT(reset()));
    connect(ecProject_, SIGNAL(ecProjectChanged()),
            this, SLOT(refresh()));

    foreach (QComboBox *combo,
             QList<QComboBox *>() << hfMethCombo
                                  << horstCombo)
    {
        connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(updateTooltip(int)));
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
        hfMethCombo->setItemData(3, QStringLiteral("disabled"), Qt::UserRole);
    }
    else
    {
        spectraNonExistingRadio->setEnabled(spectraNonExistingRadioOldEnabled);
        hfMethCombo->setItemData(3, QStringLiteral("enabled"), Qt::UserRole);
    }

    if (on)
    {
        spectraRadioGroup->button(0)->click();
    }
}

void AdvSpectralOptions::reset()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    spectraExistingRadio->setEnabled(false);
    spectraExistingRadio->setChecked(false);
    spectraFileEdit->setEnabled(false);
    spectraFileLoad->setEnabled(false);
    spectraNonExistingRadio->setEnabled(false);
    spectraNonExistingRadio->setChecked(true);
    spectraFileEdit->clear();

    binnedSpectraExistingRadio->setEnabled(false);
    binnedSpectraExistingRadio->setChecked(false);
    binnedSpectraDirEdit->setEnabled(false);
    binnedSpectraDirBrowse->setEnabled(false);
    binnedSpectraNonExistingRadio->setEnabled(false);
    binnedSpectraNonExistingRadio->setChecked(true);
    binnedSpectraDirEdit->clear();

    lfMethodCheck->setChecked(true);
    hfMethodCheck->setChecked(true);
    Alia::resetComboToItem(hfMethCombo, 0);
    horstMethodLabel->setEnabled(false);
    horstCheck->setEnabled(false);
    horstCheck->setChecked(false);
    Alia::resetComboToItem(horstCombo, 1);
    horstCombo->setEnabled(false);
    subsetCheckBox->setEnabled(false);
    subsetCheckBox->setChecked(false);
    startDateLabel->setEnabled(false);
    startDateEdit->setDate(QDate(2000, 1, 1));
    startDateEdit->setEnabled(false);
    lockedIcon->setEnabled(false);
    endDateLabel->setEnabled(false);
    endDateEdit->setDate(QDate::currentDate());
    endDateEdit->setEnabled(false);
    forceEndDatePolicy();

    minSmplLabel->setEnabled(false);
    minSmplSpin->setValue(10);
    minSmplSpin->setEnabled(false);
    minSpinCo2Label->setEnabled(false);
    minSpinCo2->setValue(2.0);
    minSpinCo2->setEnabled(false);
    minSpinCh4Label->setEnabled(false);
    minSpinCh4->setValue(0.2);
    minSpinCh4->setEnabled(false);
    minSpinGas4Label->setEnabled(false);
    minSpinGas4->setValue(0.02);
    minSpinGas4->setEnabled(false);
    minSpinLeLabel->setEnabled(false);
    minSpinLe->setValue(20.0);
    minSpinLe->setEnabled(false);
    minSpinHLabel->setEnabled(false);
    minSpinH->setValue(20.0);
    minSpinH->setEnabled(false);
    spin11Label->setEnabled(false);
    spin12Label->setEnabled(false);
    spin13Label->setEnabled(false);
    spin14Label->setEnabled(false);
    spin10Label->setEnabled(false);
    spin20Label->setEnabled(false);
    spin30Label->setEnabled(false);
    minMaxFreqLabel->setEnabled(false);
    noiseFreqLabel->setEnabled(false);
    thresholdLabel->setEnabled(false);

    spin11->setEnabled(false);
    spin12->setEnabled(false);
    spin13->setEnabled(false);
    spin14->setEnabled(false);
    spin21->setEnabled(false);
    spin22->setEnabled(false);
    spin23->setEnabled(false);
    spin24->setEnabled(false);
    spin31->setEnabled(false);
    spin32->setEnabled(false);
    spin33->setEnabled(false);
    spin34->setEnabled(false);
    updateSuggestedFrequencyRanges();

    fullSpectraExistingRadio->setEnabled(false);
    fullSpectraExistingRadio->setChecked(false);
    fullSpectraDirEdit->setEnabled(false);
    fullSpectraDirBrowse->setEnabled(false);
    fullSpectraNonExistingRadio->setEnabled(false);
    fullSpectraNonExistingRadio->setChecked(true);
    fullSpectraDirEdit->clear();

    addSonicCheck->setChecked(true);
    addSonicCheck->setEnabled(false);
    f10Spin1Label->setEnabled(false);
    f10Co2Spin->setValue(2.0);
    f10Co2Spin->setEnabled(false);
    f10Spin2Label->setEnabled(false);
    f10Ch4Spin->setValue(0.2);
    f10Ch4Spin->setEnabled(false);
    f10Spin3Label->setEnabled(false);
    f10Gas4Spin->setValue(0.02);
    f10Gas4Spin->setEnabled(false);
    f10Spin4Label->setEnabled(false);
    f10LeSpin->setValue(10.0);
    f10LeSpin->setEnabled(false);
    f10Spin5Label->setEnabled(false);
    f10HSpin->setValue(10.0);
    f10HSpin->setEnabled(false);

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);

    emit updateOutputsRequest(0);
}

void AdvSpectralOptions::refresh()
{
    DEBUG_FUNC_NAME

    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    lfMethodCheck->setChecked(ecProject_->generalLfMethod());
    hfMethodCheck->setChecked(ecProject_->generalHfMethod());
    if (ecProject_->generalHfMethod() > 0)
        hfMethCombo->setCurrentIndex(ecProject_->generalHfMethod() - 1);
    else
        hfMethCombo->setCurrentIndex(0);

    spectraExistingRadio->setEnabled(hfMethodCheck->isChecked()
                                     && isHorstIbromFratini());
    spectraNonExistingRadio->setEnabled(hfMethodCheck->isChecked()
                                        && isHorstIbromFratini());
    spectraExistingRadio->setChecked(!ecProject_->spectraMode());
    spectraNonExistingRadio->setChecked(ecProject_->spectraMode());
    spectraFileEdit->setEnabled(hfMethodCheck->isChecked()
                                && spectraExistingRadio->isChecked());
    spectraFileEdit->setText(QDir::toNativeSeparators(ecProject_->spectraFile()));
    Alia::updateLineEditToolip(spectraFileEdit);
    spectraFileLoad->setEnabled(hfMethodCheck->isChecked()
                                && spectraExistingRadio->isChecked());

    subsetCheckBox->setEnabled(isHorstIbromFratini()
                               && spectraNonExistingRadio->isEnabled()
                               && spectraNonExistingRadio->isChecked());
    subsetCheckBox->setChecked(ecProject_->spectraSubset());

    binnedSpectraNonExistingRadio->setEnabled(subsetCheckBox->isEnabled());
    binnedSpectraNonExistingRadio->setChecked(!ecProject_->generalBinSpectraAvail());
    binnedSpectraExistingRadio->setEnabled(subsetCheckBox->isEnabled());
    binnedSpectraExistingRadio->setChecked(ecProject_->generalBinSpectraAvail());
    binnedSpectraDirEdit->setEnabled(subsetCheckBox->isEnabled()
                                     && binnedSpectraExistingRadio->isChecked());
    binnedSpectraDirEdit->setText(QDir::toNativeSeparators(ecProject_->spectraBinSpectra()));
    Alia::updateLineEditToolip(binnedSpectraDirEdit);
    binnedSpectraDirBrowse->setEnabled(subsetCheckBox->isEnabled()
                                       && binnedSpectraExistingRadio->isChecked());

    startDateLabel->setEnabled(subsetCheckBox->isEnabled() && subsetCheckBox->isChecked());
    startDateEdit->setEnabled(startDateLabel->isEnabled());
    startDateEdit->setDate(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
    lockedIcon->setEnabled(startDateLabel->isEnabled());
    endDateEdit->setDate(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
    endDateLabel->setEnabled(startDateLabel->isEnabled());
    endDateEdit->setEnabled(startDateLabel->isEnabled());

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

    minSmplSpin->setValue(ecProject_->spectraMinSmpl());
    minSmplLabel->setEnabled(subsetCheckBox->isEnabled());
    minSmplSpin->setEnabled(subsetCheckBox->isEnabled());

    minSpinCo2->setValue(ecProject_->spectraMinCo2());
    minSpinCo2Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinCo2->setEnabled(subsetCheckBox->isEnabled());

    minSpinCh4->setValue(ecProject_->spectraMinCh4());
    minSpinCh4Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinCh4->setEnabled(subsetCheckBox->isEnabled());

    minSpinGas4->setValue(ecProject_->spectraMinGas4());
    minSpinGas4Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinGas4->setEnabled(subsetCheckBox->isEnabled());

    minSpinLe->setValue(ecProject_->spectraMinLe());
    minSpinLeLabel->setEnabled(subsetCheckBox->isEnabled());
    minSpinLe->setEnabled(subsetCheckBox->isEnabled());

    minSpinH->setValue(ecProject_->spectraMinH());
    minSpinHLabel->setEnabled(subsetCheckBox->isEnabled());
    minSpinH->setEnabled(subsetCheckBox->isEnabled());

    spin11Label->setEnabled(subsetCheckBox->isEnabled());
    spin12Label->setEnabled(subsetCheckBox->isEnabled());
    spin13Label->setEnabled(subsetCheckBox->isEnabled());
    spin14Label->setEnabled(subsetCheckBox->isEnabled());
    spin10Label->setEnabled(subsetCheckBox->isEnabled());
    spin20Label->setEnabled(subsetCheckBox->isEnabled());
    spin30Label->setEnabled(subsetCheckBox->isEnabled());
    minMaxFreqLabel->setEnabled(subsetCheckBox->isEnabled());
    noiseFreqLabel->setEnabled(subsetCheckBox->isEnabled());

    spin11->setValue(ecProject_->spectraFminCo2());
    spin11->setEnabled(subsetCheckBox->isEnabled());

    spin12->setValue(ecProject_->spectraFminH2o());
    spin12->setEnabled(subsetCheckBox->isEnabled());

    spin13->setValue(ecProject_->spectraFminCh4());
    spin13->setEnabled(subsetCheckBox->isEnabled());

    spin14->setValue(ecProject_->spectraFminGas4());
    spin14->setEnabled(subsetCheckBox->isEnabled());

    spin21->setValue(ecProject_->spectraFmaxCo2());
    spin21->setEnabled(subsetCheckBox->isEnabled());

    spin22->setValue(ecProject_->spectraFmaxH2o());
    spin22->setEnabled(subsetCheckBox->isEnabled());

    spin23->setValue(ecProject_->spectraFmaxCh4());
    spin23->setEnabled(subsetCheckBox->isEnabled());

    spin24->setValue(ecProject_->spectraFmaxGas4());
    spin24->setEnabled(subsetCheckBox->isEnabled());

    spin31->setValue(ecProject_->spectraHfnCo2());
    spin31->setEnabled(subsetCheckBox->isEnabled());

    spin32->setValue(ecProject_->spectraHfnH2o());
    spin32->setEnabled(subsetCheckBox->isEnabled());

    spin33->setValue(ecProject_->spectraHfnCh4());
    spin33->setEnabled(subsetCheckBox->isEnabled());

    spin34->setValue(ecProject_->spectraHfnGas4());
    spin34->setEnabled(subsetCheckBox->isEnabled());

    suggestedRangeButton->setEnabled(subsetCheckBox->isEnabled());

    thresholdLabel->setEnabled(isFratini());

    fullSpectraExistingRadio->setEnabled(thresholdLabel->isEnabled());
    fullSpectraExistingRadio->setChecked(ecProject_->generalFullSpectraAvail());
    fullSpectraNonExistingRadio->setEnabled(thresholdLabel->isEnabled());
    fullSpectraNonExistingRadio->setChecked(!ecProject_->generalFullSpectraAvail());
    fullSpectraDirEdit->setEnabled(thresholdLabel->isEnabled()
                                   && fullSpectraExistingRadio->isChecked());
    fullSpectraDirEdit->setText(QDir::toNativeSeparators(ecProject_->spectraFullSpectra()));
    Alia::updateLineEditToolip(fullSpectraDirEdit);
    fullSpectraDirBrowse->setEnabled(thresholdLabel->isEnabled()
                                     && fullSpectraExistingRadio->isChecked());

    addSonicCheck->setChecked(ecProject_->spectraAddSonic());
    addSonicCheck->setEnabled(thresholdLabel->isEnabled());

    f10Spin1Label->setEnabled(thresholdLabel->isEnabled());
    f10Co2Spin->setValue(ecProject_->spectraF10Co2());
    f10Co2Spin->setEnabled(thresholdLabel->isEnabled());

    f10Spin2Label->setEnabled(thresholdLabel->isEnabled());
    f10Ch4Spin->setValue(ecProject_->spectraF10Ch4());
    f10Ch4Spin->setEnabled(thresholdLabel->isEnabled());

    f10Spin3Label->setEnabled(thresholdLabel->isEnabled());
    f10Gas4Spin->setValue(ecProject_->spectraF10Gas4());
    f10Gas4Spin->setEnabled(thresholdLabel->isEnabled());

    f10Spin4Label->setEnabled(thresholdLabel->isEnabled());
    f10LeSpin->setValue(ecProject_->spectraF10Le());
    f10LeSpin->setEnabled(thresholdLabel->isEnabled());

    f10Spin5Label->setEnabled(thresholdLabel->isEnabled());
    f10HSpin->setValue(ecProject_->spectraF10H());
    f10HSpin->setEnabled(thresholdLabel->isEnabled());

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);

    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::updateSpectraFile(const QString& fp)
{
    ecProject_->setSpectraFile(QDir::cleanPath(fp));
    spectraFileEdit->setButtonVisible(spectraFileEdit->isEnabled()
                                      && !spectraFileEdit->text().isEmpty());
    Alia::updateLineEditToolip(spectraFileEdit);
}

void AdvSpectralOptions::updateBinnedSpectraFile(const QString& fp)
{
    DEBUG_FUNC_NAME
    ecProject_->setSpectraBinSpectra(QDir::cleanPath(fp));
    binnedSpectraDirEdit->setButtonVisible(binnedSpectraDirEdit->isEnabled()
                                      && !binnedSpectraDirEdit->text().isEmpty());
    Alia::updateLineEditToolip(binnedSpectraDirEdit);
}

void AdvSpectralOptions::updateFullSpectraFile(const QString& fp)
{
    ecProject_->setSpectraFullSpectra(QDir::cleanPath(fp));
    fullSpectraDirEdit->setButtonVisible(fullSpectraDirEdit->isEnabled()
                                         && !fullSpectraDirEdit->text().isEmpty());
    Alia::updateLineEditToolip(fullSpectraDirEdit);
}

void AdvSpectralOptions::spectraFileLoad_clicked()
{
    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString paramFile = QFileDialog::getOpenFileName(this,
                        tr("Select the Spectral Correction File"),
                        searchPath,
                        tr("All Files (*.*)")
                        );
    if (!paramFile.isEmpty())
    {
        QFileInfo paramFilePath(paramFile);
        QString canonicalParamFile = paramFilePath.canonicalFilePath();
        spectraFileEdit->setText(QDir::toNativeSeparators(canonicalParamFile));

        QString lastPath = paramFilePath.canonicalPath();
        configState_->window.last_data_path = lastPath;
        Alia::updateLastDatapath(lastPath);
    }
}

void AdvSpectralOptions::binnedSpectraDirBrowse_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString dir = QFileDialog::getExistingDirectory(this,
                    tr("Select the Binned Spectra Files Directory"),
                    searchPath
                    );

    if (!dir.isEmpty())
    {
        QDir dataDir(dir);
        QString canonicalDataDir = dataDir.canonicalPath();
        binnedSpectraDirEdit->setText(QDir::toNativeSeparators(canonicalDataDir));

        configState_->window.last_data_path = canonicalDataDir;
        Alia::updateLastDatapath(canonicalDataDir);
    }
}

void AdvSpectralOptions::fullSpectraDirBrowse_clicked()
{
    DEBUG_FUNC_NAME

    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString dir = QFileDialog::getExistingDirectory(this,
                    tr("Select the Full Spectra Files Directory"),
                    searchPath
                    );

    if (!dir.isEmpty())
    {
        QDir dataDir(dir);
        QString canonicalDataDir = dataDir.canonicalPath();
        fullSpectraDirEdit->setText(QDir::toNativeSeparators(canonicalDataDir));

        configState_->window.last_data_path = canonicalDataDir;
        Alia::updateLastDatapath(canonicalDataDir);
    }
}

void AdvSpectralOptions::spectraRadioClicked(int radioButton)
{
    if (radioButton == 0)
    {
        ecProject_->setSpectraMode(0);

        spectraFileEdit->setEnabled(true);
        spectraFileLoad->setEnabled(true);
        binnedSpectraExistingRadio->setEnabled(false);
        binnedSpectraNonExistingRadio->setEnabled(false);
        binnedSpectraDirEdit->setEnabled(false);
        binnedSpectraDirBrowse->setEnabled(false);
        subsetCheckBox->setEnabled(false);
        startDateLabel->setEnabled(false);
        startDateEdit->setEnabled(false);
        lockedIcon->setEnabled(false);
        endDateLabel->setEnabled(false);
        endDateEdit->setEnabled(false);
        minSmplLabel->setEnabled(false);
        minSmplSpin->setEnabled(false);
        minSpinCo2Label->setEnabled(false);
        minSpinCo2->setEnabled(false);
        minSpinCh4Label->setEnabled(false);
        minSpinCh4->setEnabled(false);
        minSpinGas4Label->setEnabled(false);
        minSpinGas4->setEnabled(false);
        minSpinLeLabel->setEnabled(false);
        minSpinLe->setEnabled(false);
        minSpinHLabel->setEnabled(false);
        minSpinH->setEnabled(false);
        spin11Label->setEnabled(false);
        spin12Label->setEnabled(false);
        spin13Label->setEnabled(false);
        spin14Label->setEnabled(false);
        spin10Label->setEnabled(false);
        spin20Label->setEnabled(false);
        spin30Label->setEnabled(false);
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
        spin31->setEnabled(false);
        spin32->setEnabled(false);
        spin33->setEnabled(false);
        spin34->setEnabled(false);
    }
    else
    {
        ecProject_->setSpectraMode(1);

        spectraFileEdit->setEnabled(false);
        spectraFileLoad->setEnabled(false);

        subsetCheckBox->setEnabled(hfMethodCheck->isChecked()
                                   && isHorstIbromFratini());
        binnedSpectraNonExistingRadio->setEnabled(subsetCheckBox->isEnabled());
        binnedSpectraExistingRadio->setEnabled(subsetCheckBox->isEnabled());
        binnedSpectraDirEdit->setEnabled(subsetCheckBox->isEnabled()
                                         && binnedSpectraExistingRadio->isChecked());
        binnedSpectraDirBrowse->setEnabled(subsetCheckBox->isEnabled()
                                           && binnedSpectraExistingRadio->isChecked());

        startDateLabel->setEnabled(isHorstIbromFratini() && subsetCheckBox->isChecked());
        startDateEdit->setEnabled(startDateLabel->isEnabled());
        lockedIcon->setEnabled(startDateLabel->isEnabled());
        endDateLabel->setEnabled(startDateLabel->isEnabled());
        endDateEdit->setEnabled(startDateLabel->isEnabled());
        minSmplLabel->setEnabled(subsetCheckBox->isEnabled());
        minSmplSpin->setEnabled(subsetCheckBox->isEnabled());
        minSpinCo2Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinCo2->setEnabled(subsetCheckBox->isEnabled());
        minSpinCh4Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinCh4->setEnabled(subsetCheckBox->isEnabled());
        minSpinGas4Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinGas4->setEnabled(subsetCheckBox->isEnabled());
        minSpinLeLabel->setEnabled(subsetCheckBox->isEnabled());
        minSpinLe->setEnabled(subsetCheckBox->isEnabled());
        minSpinHLabel->setEnabled(subsetCheckBox->isEnabled());
        minSpinH->setEnabled(subsetCheckBox->isEnabled());
        spin11Label->setEnabled(subsetCheckBox->isEnabled());
        spin12Label->setEnabled(subsetCheckBox->isEnabled());
        spin13Label->setEnabled(subsetCheckBox->isEnabled());
        spin14Label->setEnabled(subsetCheckBox->isEnabled());
        spin10Label->setEnabled(subsetCheckBox->isEnabled());
        spin20Label->setEnabled(subsetCheckBox->isEnabled());
        spin30Label->setEnabled(subsetCheckBox->isEnabled());
        minMaxFreqLabel->setEnabled(subsetCheckBox->isEnabled());
        noiseFreqLabel->setEnabled(subsetCheckBox->isEnabled());
        spin11->setEnabled(subsetCheckBox->isEnabled());
        spin12->setEnabled(subsetCheckBox->isEnabled());
        spin13->setEnabled(subsetCheckBox->isEnabled());
        spin14->setEnabled(subsetCheckBox->isEnabled());
        spin21->setEnabled(subsetCheckBox->isEnabled());
        spin22->setEnabled(subsetCheckBox->isEnabled());
        spin23->setEnabled(subsetCheckBox->isEnabled());
        spin24->setEnabled(subsetCheckBox->isEnabled());
        spin31->setEnabled(subsetCheckBox->isEnabled());
        spin32->setEnabled(subsetCheckBox->isEnabled());
        spin33->setEnabled(subsetCheckBox->isEnabled());
        spin34->setEnabled(subsetCheckBox->isEnabled());
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::binnedSpectraRadioClicked(int radioButton)
{
    if (radioButton == 0)
    {
        ecProject_->setGeneralBinSpectraAvail(0);

        binnedSpectraDirEdit->setEnabled(false);
        binnedSpectraDirBrowse->setEnabled(false);
    }
    else
    {
        ecProject_->setGeneralBinSpectraAvail(1);

        binnedSpectraDirEdit->setEnabled(true);
        binnedSpectraDirBrowse->setEnabled(true);
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::fullSpectraRadioClicked(int radioButton)
{
    if (radioButton == 0)
    {
        ecProject_->setGeneralFullSpectraAvail(0);

        fullSpectraDirEdit->setEnabled(false);
        fullSpectraDirBrowse->setEnabled(false);
    }
    else
    {
        ecProject_->setGeneralFullSpectraAvail(1);

        fullSpectraDirEdit->setEnabled(true);
        fullSpectraDirBrowse->setEnabled(true);
    }
    emit updateOutputsRequest(hfMethCombo->currentIndex());
}

void AdvSpectralOptions::onStartDateLabelClicked()
{
    DEBUG_FUNC_NAME
    startDateEdit->setFocus();
    Alia::showCalendarOf(startDateEdit);
}

void AdvSpectralOptions::onEndDateLabelClicked()
{
    DEBUG_FUNC_NAME
    endDateEdit->setFocus();
    Alia::showCalendarOf(endDateEdit);
}

void AdvSpectralOptions::updateStartDate(const QDate &d)
{
    ecProject_->setSpectraStartDate(d.toString(Qt::ISODate));
    forceEndDatePolicy();
}

void AdvSpectralOptions::updateEndDate(const QDate &d)
{
    ecProject_->setSpectraEndDate(d.toString(Qt::ISODate));
}

bool AdvSpectralOptions::eventFilter(QObject* watched, QEvent* event)
{
    QEvent::Type eventType = event->type();
    QwwButtonLineEdit* lineEdit1 = spectraFileEdit;
    QwwButtonLineEdit* lineEdit2 = binnedSpectraDirEdit;
    QwwButtonLineEdit* lineEdit3 = fullSpectraDirEdit;

    if (lineEdit1)
    {
        if (watched == lineEdit1 && eventType == QEvent::EnabledChange)
        {
            lineEdit1->setButtonVisible(lineEdit1->isEnabled() && !lineEdit1->text().isEmpty());
        }
    }

    if (lineEdit2)
    {
        if (watched == lineEdit2 && eventType == QEvent::EnabledChange)
        {
            lineEdit2->setButtonVisible(lineEdit2->isEnabled() && !lineEdit2->text().isEmpty());
        }
    }

    if (lineEdit3)
    {
        if (watched == lineEdit3 && eventType == QEvent::EnabledChange)
        {
            lineEdit3->setButtonVisible(lineEdit3->isEnabled() && !lineEdit3->text().isEmpty());
        }
    }
    return QObject::eventFilter(watched, event);
}

void AdvSpectralOptions::updateLfMethod(bool b)
{
    ecProject_->setGeneralLfMethod(b);
}

void AdvSpectralOptions::onClickHfMethLabel()
{
    if (hfMethCombo->isEnabled())
    {
        hfMethCombo->showPopup();
    }
}

void AdvSpectralOptions::updateHfMethod_1(bool b)
{
    bool smartfluxOn = configState_->project.smartfluxMode;

    if (b)
    {
        ecProject_->setGeneralHfMethod(hfMethCombo->currentIndex() + 1);

        spectraExistingRadio->setEnabled(isHorstIbromFratini());
        spectraNonExistingRadio->setEnabled(isHorstIbromFratini()
                                            && !smartfluxOn);
        spectraFileEdit->setEnabled(isHorstIbromFratini()
                                    && spectraExistingRadio->isChecked());
        spectraFileLoad->setEnabled(isHorstIbromFratini()
                                    && spectraExistingRadio->isChecked());

        subsetCheckBox->setEnabled(isHorstIbromFratini() && spectraNonExistingRadio->isChecked());
        startDateLabel->setEnabled(subsetCheckBox->isEnabled() && subsetCheckBox->isChecked());
        startDateEdit->setEnabled(startDateLabel->isEnabled());
        lockedIcon->setEnabled(startDateLabel->isEnabled());
        endDateLabel->setEnabled(startDateLabel->isEnabled());
        endDateEdit->setEnabled(startDateLabel->isEnabled());
        minSmplLabel->setEnabled(subsetCheckBox->isEnabled());
        minSmplSpin->setEnabled(subsetCheckBox->isEnabled());
        horstCheck->setEnabled(isIbrom() || isFratini());
        horstMethodLabel->setEnabled(horstCheck->isEnabled());
        horstCombo->setEnabled(horstCheck->isEnabled());
        binnedSpectraNonExistingRadio->setEnabled(subsetCheckBox->isEnabled());
        binnedSpectraExistingRadio->setEnabled(subsetCheckBox->isEnabled());
        binnedSpectraDirEdit->setEnabled(subsetCheckBox->isEnabled()
                                         && binnedSpectraExistingRadio->isChecked());
        binnedSpectraDirBrowse->setEnabled(subsetCheckBox->isEnabled()
                                           && binnedSpectraExistingRadio->isChecked());
        minSpinCo2Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinCo2->setEnabled(subsetCheckBox->isEnabled());
        minSpinCh4Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinCh4->setEnabled(subsetCheckBox->isEnabled());
        minSpinGas4Label->setEnabled(subsetCheckBox->isEnabled());
        minSpinGas4->setEnabled(subsetCheckBox->isEnabled());
        minSpinLeLabel->setEnabled(subsetCheckBox->isEnabled());
        minSpinLe->setEnabled(subsetCheckBox->isEnabled());
        minSpinHLabel->setEnabled(subsetCheckBox->isEnabled());
        minSpinH->setEnabled(subsetCheckBox->isEnabled());
        spin11Label->setEnabled(subsetCheckBox->isEnabled());
        spin12Label->setEnabled(subsetCheckBox->isEnabled());
        spin13Label->setEnabled(subsetCheckBox->isEnabled());
        spin14Label->setEnabled(subsetCheckBox->isEnabled());
        spin10Label->setEnabled(subsetCheckBox->isEnabled());
        spin20Label->setEnabled(subsetCheckBox->isEnabled());
        spin30Label->setEnabled(subsetCheckBox->isEnabled());
        minMaxFreqLabel->setEnabled(subsetCheckBox->isEnabled());
        noiseFreqLabel->setEnabled(subsetCheckBox->isEnabled());
        spin11->setEnabled(subsetCheckBox->isEnabled());
        spin12->setEnabled(subsetCheckBox->isEnabled());
        spin13->setEnabled(subsetCheckBox->isEnabled());
        spin14->setEnabled(subsetCheckBox->isEnabled());
        spin21->setEnabled(subsetCheckBox->isEnabled());
        spin22->setEnabled(subsetCheckBox->isEnabled());
        spin23->setEnabled(subsetCheckBox->isEnabled());
        spin24->setEnabled(subsetCheckBox->isEnabled());
        spin31->setEnabled(subsetCheckBox->isEnabled());
        spin32->setEnabled(subsetCheckBox->isEnabled());
        spin33->setEnabled(subsetCheckBox->isEnabled());
        spin34->setEnabled(subsetCheckBox->isEnabled());
        suggestedRangeButton->setEnabled(subsetCheckBox->isEnabled());

        thresholdLabel->setEnabled(isFratini());
        fullSpectraNonExistingRadio->setEnabled(thresholdLabel->isEnabled());
        fullSpectraExistingRadio->setEnabled(thresholdLabel->isEnabled());
        fullSpectraNonExistingRadio->setEnabled(thresholdLabel->isEnabled());
        fullSpectraDirEdit->setEnabled(thresholdLabel->isEnabled()
                                       && fullSpectraExistingRadio->isChecked());
        fullSpectraDirBrowse->setEnabled(thresholdLabel->isEnabled()
                                         && fullSpectraExistingRadio->isChecked());
        addSonicCheck->setEnabled(thresholdLabel->isEnabled());
        f10Spin1Label->setEnabled(thresholdLabel->isEnabled());
        f10Co2Spin->setEnabled(thresholdLabel->isEnabled());
        f10Spin2Label->setEnabled(thresholdLabel->isEnabled());
        f10Ch4Spin->setEnabled(thresholdLabel->isEnabled());
        f10Spin3Label->setEnabled(thresholdLabel->isEnabled());
        f10Gas4Spin->setEnabled(thresholdLabel->isEnabled());
        f10Spin4Label->setEnabled(thresholdLabel->isEnabled());
        f10LeSpin->setEnabled(thresholdLabel->isEnabled());
        f10Spin5Label->setEnabled(thresholdLabel->isEnabled());
        f10HSpin->setEnabled(thresholdLabel->isEnabled());

        emit updateOutputsRequest(hfMethCombo->currentIndex());
    }
    else
    {
        ecProject_->setGeneralHfMethod(0);

        subsetCheckBox->setEnabled(false);
        startDateLabel->setEnabled(false);
        startDateEdit->setEnabled(false);
        lockedIcon->setEnabled(false);
        endDateLabel->setEnabled(false);
        endDateEdit->setEnabled(false);
        minSmplLabel->setEnabled(false);
        minSmplSpin->setEnabled(false);
        horstMethodLabel->setEnabled(false);
        horstCheck->setEnabled(false);
        horstCombo->setEnabled(false);
        spectraExistingRadio->setEnabled(false);
        spectraNonExistingRadio->setEnabled(false);
        spectraFileEdit->setEnabled(false);
        spectraFileLoad->setEnabled(false);
        binnedSpectraExistingRadio->setEnabled(false);
        binnedSpectraNonExistingRadio->setEnabled(false);
        binnedSpectraDirEdit->setEnabled(false);
        binnedSpectraDirBrowse->setEnabled(false);
        minSpinCo2Label->setEnabled(false);
        minSpinCo2->setEnabled(false);
        minSpinCh4Label->setEnabled(false);
        minSpinCh4->setEnabled(false);
        minSpinGas4Label->setEnabled(false);
        minSpinGas4->setEnabled(false);
        minSpinLeLabel->setEnabled(false);
        minSpinLe->setEnabled(false);
        minSpinHLabel->setEnabled(false);
        minSpinH->setEnabled(false);
        spin11Label->setEnabled(false);
        spin12Label->setEnabled(false);
        spin13Label->setEnabled(false);
        spin14Label->setEnabled(false);
        spin10Label->setEnabled(false);
        spin20Label->setEnabled(false);
        spin30Label->setEnabled(false);
        minMaxFreqLabel->setEnabled(false);
        noiseFreqLabel->setEnabled(false);
        thresholdLabel->setEnabled(false);
        spin11->setEnabled(false);
        spin12->setEnabled(false);
        spin13->setEnabled(false);
        spin14->setEnabled(false);
        spin21->setEnabled(false);
        spin22->setEnabled(false);
        spin23->setEnabled(false);
        spin24->setEnabled(false);
        spin31->setEnabled(false);
        spin32->setEnabled(false);
        spin33->setEnabled(false);
        spin34->setEnabled(false);
        suggestedRangeButton->setEnabled(false);

        fullSpectraExistingRadio->setEnabled(false);
        fullSpectraNonExistingRadio->setEnabled(false);
        fullSpectraDirEdit->setEnabled(false);
        fullSpectraDirBrowse->setEnabled(false);
        addSonicCheck->setEnabled(false);
        f10Spin1Label->setEnabled(false);
        f10Co2Spin->setEnabled(false);
        f10Spin2Label->setEnabled(false);
        f10Ch4Spin->setEnabled(false);
        f10Spin3Label->setEnabled(false);
        f10Gas4Spin->setEnabled(false);
        f10Spin4Label->setEnabled(false);
        f10LeSpin->setEnabled(false);
        f10Spin5Label->setEnabled(false);
        f10HSpin->setEnabled(false);

        emit updateOutputsRequest(0);
    }
}

// update project properties and fluxes rotation choices
void AdvSpectralOptions::updateHfMethod_2(int n)
{
    bool smartfluxOn = configState_->project.smartfluxMode;

    ecProject_->setGeneralHfMethod(n + 1);

    spectraExistingRadio->setEnabled(n != 0);
    spectraNonExistingRadio->setEnabled((n != 0) && !smartfluxOn);
    spectraFileEdit->setEnabled(n != 0 && spectraExistingRadio->isChecked());
    spectraFileLoad->setEnabled(n != 0 && spectraExistingRadio->isChecked());

    subsetCheckBox->setEnabled(isHorstIbromFratini() && spectraNonExistingRadio->isChecked());

    binnedSpectraNonExistingRadio->setEnabled(subsetCheckBox->isEnabled());
    binnedSpectraExistingRadio->setEnabled(subsetCheckBox->isEnabled());
    binnedSpectraDirEdit->setEnabled(subsetCheckBox->isEnabled()
                                     && binnedSpectraExistingRadio->isChecked());
    binnedSpectraDirBrowse->setEnabled(subsetCheckBox->isEnabled()
                                       && binnedSpectraExistingRadio->isChecked());

    startDateLabel->setEnabled(isHorstIbromFratini() && subsetCheckBox->isChecked());
    startDateEdit->setEnabled(startDateLabel->isEnabled());
    lockedIcon->setEnabled(startDateLabel->isEnabled());
    endDateLabel->setEnabled(startDateLabel->isEnabled());
    endDateEdit->setEnabled(startDateLabel->isEnabled());
    minSmplLabel->setEnabled(subsetCheckBox->isEnabled());
    minSmplSpin->setEnabled(subsetCheckBox->isEnabled());
    horstCheck->setEnabled(isIbrom() || isFratini());
    horstMethodLabel->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());
    horstCombo->setEnabled(horstCheck->isEnabled() && horstCheck->isChecked());
    minSpinCo2Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinCo2->setEnabled(subsetCheckBox->isEnabled());
    minSpinCh4Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinCh4->setEnabled(subsetCheckBox->isEnabled());
    minSpinGas4Label->setEnabled(subsetCheckBox->isEnabled());
    minSpinGas4->setEnabled(subsetCheckBox->isEnabled());
    minSpinLeLabel->setEnabled(subsetCheckBox->isEnabled());
    minSpinLe->setEnabled(subsetCheckBox->isEnabled());
    minSpinHLabel->setEnabled(subsetCheckBox->isEnabled());
    minSpinH->setEnabled(subsetCheckBox->isEnabled());
    spin11Label->setEnabled(subsetCheckBox->isEnabled());
    spin12Label->setEnabled(subsetCheckBox->isEnabled());
    spin13Label->setEnabled(subsetCheckBox->isEnabled());
    spin14Label->setEnabled(subsetCheckBox->isEnabled());
    spin10Label->setEnabled(subsetCheckBox->isEnabled());
    spin20Label->setEnabled(subsetCheckBox->isEnabled());
    spin30Label->setEnabled(subsetCheckBox->isEnabled());
    minMaxFreqLabel->setEnabled(subsetCheckBox->isEnabled());
    noiseFreqLabel->setEnabled(subsetCheckBox->isEnabled());
    spin11->setEnabled(subsetCheckBox->isEnabled());
    spin12->setEnabled(subsetCheckBox->isEnabled());
    spin13->setEnabled(subsetCheckBox->isEnabled());
    spin14->setEnabled(subsetCheckBox->isEnabled());
    spin21->setEnabled(subsetCheckBox->isEnabled());
    spin22->setEnabled(subsetCheckBox->isEnabled());
    spin23->setEnabled(subsetCheckBox->isEnabled());
    spin24->setEnabled(subsetCheckBox->isEnabled());
    spin31->setEnabled(subsetCheckBox->isEnabled());
    spin32->setEnabled(subsetCheckBox->isEnabled());
    spin33->setEnabled(subsetCheckBox->isEnabled());
    spin34->setEnabled(subsetCheckBox->isEnabled());
    suggestedRangeButton->setEnabled(subsetCheckBox->isEnabled());

    thresholdLabel->setEnabled(n == 3);
    fullSpectraExistingRadio->setEnabled(thresholdLabel->isEnabled());
    fullSpectraNonExistingRadio->setEnabled(thresholdLabel->isEnabled());
    fullSpectraDirEdit->setEnabled(thresholdLabel->isEnabled()
                                   && fullSpectraExistingRadio->isChecked());
    fullSpectraDirBrowse->setEnabled(thresholdLabel->isEnabled()
                                     && fullSpectraExistingRadio->isChecked());
    addSonicCheck->setEnabled(thresholdLabel->isEnabled());
    f10Spin1Label->setEnabled(thresholdLabel->isEnabled());
    f10Co2Spin->setEnabled(thresholdLabel->isEnabled());
    f10Spin2Label->setEnabled(thresholdLabel->isEnabled());
    f10Ch4Spin->setEnabled(thresholdLabel->isEnabled());
    f10Spin3Label->setEnabled(thresholdLabel->isEnabled());
    f10Gas4Spin->setEnabled(thresholdLabel->isEnabled());
    f10Spin4Label->setEnabled(thresholdLabel->isEnabled());
    f10LeSpin->setEnabled(thresholdLabel->isEnabled());
    f10Spin5Label->setEnabled(thresholdLabel->isEnabled());
    f10HSpin->setEnabled(thresholdLabel->isEnabled());

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
    minSpinCo2->setFocus();
    minSpinCo2->selectAll();
}

void AdvSpectralOptions::onMinCh4LabelClicked()
{
    minSpinCh4->setFocus();
    minSpinCh4->selectAll();
}

void AdvSpectralOptions::onMinGas4LabelClicked()
{
    minSpinGas4->setFocus();
    minSpinGas4->selectAll();
}

void AdvSpectralOptions::onMinLeLabelClicked()
{
    minSpinLe->setFocus();
    minSpinLe->selectAll();
}

void AdvSpectralOptions::onMinHLabelClicked()
{
    minSpinH->setFocus();
    minSpinH->selectAll();
}

void AdvSpectralOptions::updateAddSonic(bool b)
{
    ecProject_->setSpectraAddSonic(b);
}

void AdvSpectralOptions::onF10Co2LabelClicked()
{
    f10Co2Spin->setFocus();
    f10Co2Spin->selectAll();
}

void AdvSpectralOptions::onF10Ch4LabelClicked()
{
    f10Ch4Spin->setFocus();
    f10Ch4Spin->selectAll();
}

void AdvSpectralOptions::onF10Gas4LabelClicked()
{
    f10Gas4Spin->setFocus();
    f10Gas4Spin->selectAll();
}

void AdvSpectralOptions::onF10LeLabelClicked()
{
    f10LeSpin->setFocus();
    f10LeSpin->selectAll();
}

void AdvSpectralOptions::onF10HLabelClicked()
{
    f10HSpin->setFocus();
    f10HSpin->selectAll();
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

void AdvSpectralOptions::updateMinCo2(double d)
{
    ecProject_->setSpectraMinCo2(d);
}

void AdvSpectralOptions::updateMinCh4(double d)
{
    ecProject_->setSpectraMinCh4(d);
}

void AdvSpectralOptions::updateMinGas4(double d)
{
    ecProject_->setSpectraMinGas4(d);
}

void AdvSpectralOptions::updateMinLe(double d)
{
    ecProject_->setSpectraMinLe(d);
}

void AdvSpectralOptions::updateMinH(double d)
{
    ecProject_->setSpectraMinH(d);
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

void AdvSpectralOptions::updateF10Co2(double d)
{
    ecProject_->setSpectraF10Co2(d);
}

void AdvSpectralOptions::updateF10Ch4(double d)
{
    ecProject_->setSpectraF10Ch4(d);
}

void AdvSpectralOptions::updateF10Gas4(double d)
{
    ecProject_->setSpectraF10Gas4(d);
}

void AdvSpectralOptions::updateF10Le(double d)
{
    ecProject_->setSpectraF10Le(d);
}

void AdvSpectralOptions::updateF10H(double d)
{
    ecProject_->setSpectraF10H(d);
}

// enforce (start date&time) <= (end date&time)
void AdvSpectralOptions::forceEndDatePolicy()
{
    DEBUG_FUNC_NAME

    endDateEdit->setMinimumDate(startDateEdit->date());
}

void AdvSpectralOptions::createQuestionMark()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_2 = new QPushButton;
    questionMark_2->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_3 = new QPushButton;
    questionMark_3->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_4 = new QPushButton;
    questionMark_4->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_1()));
    connect(questionMark_2, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_2()));
    connect(questionMark_3, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_3()));
    connect(questionMark_4, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_4()));
}

void AdvSpectralOptions::onlineHelpTrigger_1()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Calculating_Spectral_Correction_Factors.htm")));
}

void AdvSpectralOptions::onlineHelpTrigger_2()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#High-pass_Filtering.htm")));
}

void AdvSpectralOptions::onlineHelpTrigger_3()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Low-pass_Filtering.htm")));
}

void AdvSpectralOptions::onlineHelpTrigger_4()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Spectral_Corrections.htm#Assessme")));
}

void AdvSpectralOptions::updateTooltip(int i)
{
    QComboBox* senderCombo = qobject_cast<QComboBox *>(sender());

    Alia::updateComboItemTooltip(senderCombo, i);
}

void AdvSpectralOptions::updateSubsetSelection(bool b)
{
    ecProject_->setSpectraSubset(b);

    foreach (QWidget *w, QList<QWidget *>()
             << startDateLabel
             << startDateEdit
             << lockedIcon
             << endDateLabel
             << endDateEdit)
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

void AdvSpectralOptions::updateSuggestedFrequencyRanges()
{
    DEBUG_FUNC_NAME

    double  lowestValue = getLowestFrequencyValue();

    spin11->setValue(lowestValue);
    spin12->setValue(lowestValue);
    spin13->setValue(lowestValue);
    spin14->setValue(lowestValue);

    double  highestValue = getHighestFrequencyValue();
    spin21->setValue(highestValue);
    spin22->setValue(highestValue);
    spin23->setValue(highestValue);
    spin24->setValue(highestValue);
    spin31->setValue(highestValue);
    spin32->setValue(highestValue);
    spin33->setValue(highestValue);
    spin34->setValue(highestValue);
}

void AdvSpectralOptions::clearSpectraFileEdit()
{
    spectraFileEdit->clear();
    Alia::updateLineEditToolip(spectraFileEdit);
}

void AdvSpectralOptions::clearBinnedSpectraDirEdit()
{
    DEBUG_FUNC_NAME
    binnedSpectraDirEdit->clear();
    Alia::updateLineEditToolip(binnedSpectraDirEdit);
}

void AdvSpectralOptions::clearFullSpectraDirEdit()
{
    fullSpectraDirEdit->clear();
    Alia::updateLineEditToolip(fullSpectraDirEdit);
}
