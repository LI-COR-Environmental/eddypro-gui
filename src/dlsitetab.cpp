/***************************************************************************
  dlsitetab.cpp
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
 ***************************************************************************/

#include "dlsitetab.h"

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSpinBox>
#include <QTextStream>
#include <QUrl>

#include <cmath>

#include "clicklabel.h"
#include "dlproject.h"
#include "widget_utils.h"

DlSiteTab::DlSiteTab(QWidget *parent, DlProject *dlProject) :
    QWidget(parent),
    project_(dlProject)
{
    timestampRefLabel = new ClickLabel;
    timestampRefLabel->setText(tr("Timestamps refers to :"));
    timestampRefLabel->setToolTip(tr("<b>Time stamp refers to:</b> Choose whether the timestamp provided in raw file names refers to the beginning or end of the data interval."));
    timestampRefStartRadio = new QRadioButton(tr("Beginning of dataset period"), this);
    timestampRefStartRadio->setChecked(true);
    timestampRefStartRadio->setToolTip(tr("<b>Beginning of dataset period:</b> Select this option if timestamps in the raw file names refer to the beginning of the data interval. Note that timestamps on EddyPro output files always refer to the end of the averaging interval."));
    timestampRefEndRadio = new QRadioButton(tr("End of dataset period"), this);
    timestampRefEndRadio->setToolTip(tr("<b>End of dataset period:</b> Select this option if timestamps in the raw file names refer to the end of the data interval."));

    auto timestampRadioGroup = new QButtonGroup(this);
    timestampRadioGroup->addButton(timestampRefStartRadio, 0);
    timestampRadioGroup->addButton(timestampRefEndRadio, 1);

    durationLabel = new ClickLabel;
    durationLabel->setText(tr("File duration :"));
    durationLabel->setToolTip(tr("<b> File duration:</b> Enter the time span covered by each raw file."));
    fileDurationSpin = new QSpinBox;
    fileDurationSpin->setRange(0, 1440);
    fileDurationSpin->setValue(30);
    fileDurationSpin->setSingleStep(1);
    fileDurationSpin->setAccelerated(true);
    fileDurationSpin->setSuffix(tr(" [min] ", "minute"));
    fileDurationSpin->setToolTip(durationLabel->toolTip());
    fileDurationSpin->setMinimumWidth(120);
    fileDurationSpin->setMaximumWidth(120);

    freqLabel = new ClickLabel;
    freqLabel->setText(tr("Acquisition frequency :"));
    freqLabel->setToolTip(tr("<b>Acquisition frequency:</b> The number of records (samples) per second in the raw data files. Also referred to as <i>Update Rate</i> or <i>Output Rate</i>."));
    acquisitionFreqSpin = new QDoubleSpinBox;
    acquisitionFreqSpin->setDecimals(3);
    acquisitionFreqSpin->setRange(0.001, 100.0);
    acquisitionFreqSpin->setSingleStep(1);
    acquisitionFreqSpin->setAccelerated(true);
    acquisitionFreqSpin->setValue(10.0);
    acquisitionFreqSpin->setSuffix(tr(" [Hz] ", "Hertz"));
    acquisitionFreqSpin->setMinimumWidth(120);
    acquisitionFreqSpin->setMaximumWidth(120);
    acquisitionFreqSpin->setToolTip(freqLabel->toolTip());

    canopyLabel = new ClickLabel;
    canopyLabel->setText(tr("Canopy height :"));
    canopyLabel->setToolTip(tr("<b>Canopy height:</b> Effective average distance between the ground and the top of the plant canopy. Also referred to as aerodynamic canopy height."));
    canopyHeightSpin = new QDoubleSpinBox;
    canopyHeightSpin->setDecimals(2);
    canopyHeightSpin->setRange(0.00, 120.00);
    canopyHeightSpin->setSingleStep(1);
    canopyHeightSpin->setAccelerated(true);
    canopyHeightSpin->setSuffix(tr(" [m] ", "meter"));
    canopyHeightSpin->setToolTip(canopyLabel->toolTip());
    canopyHeightSpin->setMinimumWidth(120);
    canopyHeightSpin->setMaximumWidth(120);

    displacementLabel = new ClickLabel;
    displacementLabel->setText(tr("Displacement height :"));
    displacementLabel->setToolTip(tr("<b>Displacement height:</b> Zero plane displacement height is the average level at which elements of the plant community absorb momentum. If left blank, this parameter is automatically estimated based on canopy height."));
    displacementLabel->setProperty("optionalField", true);
    displacementHeightSpin = new QDoubleSpinBox;
    displacementHeightSpin->setDecimals(2);
    displacementHeightSpin->setRange(0.00, 120.00);
    displacementHeightSpin->setSingleStep(1);
    displacementHeightSpin->setAccelerated(true);
    displacementHeightSpin->setSuffix(tr(" [m] ", "meter"));
    displacementHeightSpin->setToolTip(displacementLabel->toolTip());
    displacementHeightSpin->setMinimumWidth(120);
    displacementHeightSpin->setMaximumWidth(120);

    roughnessLabel = new ClickLabel;
    roughnessLabel->setText(tr("Roughness length :"));
    roughnessLabel->setToolTip(tr("<b>Roughness length:</b> Canopy roughness length is a quantification of the surface roughness. If left blank, this parameter is automatically estimated based on canopy height."));
    roughnessLabel->setProperty("optionalField", true);
    roughnessLengthSpin = new QDoubleSpinBox;
    roughnessLengthSpin->setDecimals(2);
    roughnessLengthSpin->setRange(0.00, 120.00);
    roughnessLengthSpin->setSingleStep(1);
    roughnessLengthSpin->setAccelerated(true);
    roughnessLengthSpin->setSuffix(tr(" [m] ", "meter"));
    roughnessLengthSpin->setToolTip(roughnessLabel->toolTip());
    roughnessLengthSpin->setMinimumWidth(120);
    roughnessLengthSpin->setMaximumWidth(120);

    altitudeLabel = new ClickLabel;
    altitudeLabel->setText(tr("Altitude :"));
    altitudeLabel->setToolTip(tr("<b>Altitude:</b> Elevation above sea level to the base of the flux tower."));

    altitudeSpin = new QDoubleSpinBox;
    altitudeSpin->setDecimals(0);
    altitudeSpin->setRange(-1000, 8000);
    altitudeSpin->setSingleStep(10);
    altitudeSpin->setAccelerated(true);
    altitudeSpin->setSuffix(tr(" [m] ", "meter"));
    altitudeSpin->setToolTip(altitudeLabel->toolTip());
    altitudeSpin->setMinimumWidth(120);
    altitudeSpin->setMaximumWidth(120);

    latitudeLabel = new ClickLabel;
    latitudeLabel->setText(tr("Latitude :"));
    latitudeLabel->setToolTip(tr("<b>Latitude:</b> Latitude at the site. Use N and S for north and south latitudes, respectively."));
    latitudeEdit = new QLineEdit;
    latitudeEdit->setToolTip(latitudeLabel->toolTip());
    latitudeEdit->setText(tr("00%1 00' 00.000'' N").arg(Defs::DEGREE));
    QString lat_pattern = tr(R"((?:([0-8]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(N|S)))").arg(Defs::DEGREE);
            lat_pattern += tr(R"(|(?:(90)%1\s(00)'\s(00)\.(000)''\s(N|S)))").arg(Defs::DEGREE);
    QRegularExpression latRx(lat_pattern);
    auto latValidator = new QRegularExpressionValidator(latRx, latitudeEdit);
    latitudeEdit->setValidator(latValidator);
    latitudeEdit->setInputMask(tr("00%1 00' 00.000'' >A;x").arg(Defs::DEGREE));
    latitudeEdit->setAlignment(Qt::AlignRight);
    latitudeEdit->setMinimumWidth(120);
    latitudeEdit->setMaximumWidth(120);

    longitudeLabel = new ClickLabel;
    longitudeLabel->setText(tr("Longitude :"));
    longitudeLabel->setToolTip(tr("<b>Longitude:</b> Longitude at the site. Use E and W for east and west longitudes, respectively."));
    longitudeEdit = new QLineEdit;
    longitudeEdit->setToolTip(longitudeLabel->toolTip());
    longitudeEdit->setText(tr("000%1 00' 00.000'' E").arg(Defs::DEGREE));
    QString lon_pattern = tr(R"((?:(0\d\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
            lon_pattern += tr(R"((?:(1[0-7]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
            lon_pattern += tr(R"((?:(180)%1\s(00)'\s(00)\.(000)''\s(E|W)))").arg(Defs::DEGREE);
    QRegularExpression lonRx(lon_pattern);
    auto lonValidator = new QRegularExpressionValidator(lonRx, longitudeEdit);
    longitudeEdit->setValidator(lonValidator);
    longitudeEdit->setInputMask(tr("000%1 00' 00.000'' >A;x").arg(Defs::DEGREE));
    longitudeEdit->setAlignment(Qt::AlignRight);
    longitudeEdit->setMinimumWidth(120);
    longitudeEdit->setMaximumWidth(120);

    sexDegRadio = new QRadioButton(tr("Sexagesimal Degrees"), this);
    decDegRadio = new QRadioButton(tr("Decimal Degrees"), this);
    sexDegRadio->setChecked(true);
    // NOTE: sexDegRadio and sexDegRadio buttons temporary disabled
    sexDegRadio->setEnabled(false);
    sexDegRadio->setVisible(false);
    decDegRadio->setEnabled(false);
    decDegRadio->setVisible(false);

    createQuestionMark();

    auto spacer_1 = new QWidget;
    spacer_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto infoLayout = new QGridLayout;
    infoLayout->addWidget(timestampRefLabel, 0, 0, Qt::AlignRight);
    infoLayout->addWidget(timestampRefStartRadio, 0, 1);
    infoLayout->addWidget(questionMark_1, 0, 2, 1, 1, Qt::AlignLeft);
    infoLayout->addWidget(timestampRefEndRadio, 1, 1, 1, 2);
    infoLayout->addWidget(durationLabel, 2, 0, Qt::AlignRight);
    infoLayout->addWidget(fileDurationSpin, 2, 1);
    infoLayout->addWidget(spacer_1, 2, 2);
    infoLayout->addWidget(freqLabel, 3, 0, Qt::AlignRight);
    infoLayout->addWidget(acquisitionFreqSpin, 3, 1);
    infoLayout->addWidget(canopyLabel, 2, 2, Qt::AlignRight);
    infoLayout->addWidget(canopyHeightSpin, 2, 3);
    infoLayout->addWidget(displacementLabel, 3, 2, Qt::AlignRight);
    infoLayout->addWidget(displacementHeightSpin, 3, 3);
    infoLayout->addWidget(questionMark_2, 3, 4, Qt::AlignLeft);
    infoLayout->addWidget(roughnessLabel, 4, 2, Qt::AlignRight);
    infoLayout->addWidget(roughnessLengthSpin, 4, 3);
    infoLayout->addWidget(questionMark_3, 4, 4, Qt::AlignLeft);
    infoLayout->addWidget(altitudeLabel, 2, 5, Qt::AlignRight);
    infoLayout->addWidget(altitudeSpin, 2, 6);
    infoLayout->addWidget(latitudeLabel, 3, 5, Qt::AlignRight);
    infoLayout->addWidget(latitudeEdit, 3, 6);
    infoLayout->addWidget(longitudeLabel, 4, 5, Qt::AlignRight);
    infoLayout->addWidget(longitudeEdit, 4, 6);
    infoLayout->setRowStretch(5, 1);
    infoLayout->setColumnStretch(0, 1);
    infoLayout->setColumnStretch(1, 1);
    infoLayout->setColumnStretch(2, 2);
    infoLayout->setColumnStretch(3, 1);
    infoLayout->setColumnStretch(4, 0);
    infoLayout->setColumnStretch(5, 2);
    infoLayout->setColumnStretch(6, 1);
    infoLayout->setColumnStretch(7, 1);
    infoLayout->setVerticalSpacing(3);

    auto siteGroup = new QGroupBox(tr("Station Info"));
    siteGroup->setObjectName(QStringLiteral("simpleGroupBox"));
    siteGroup->setFlat(true);
    siteGroup->setLayout(infoLayout);

    auto siteLayout = new QVBoxLayout;
    siteLayout->addWidget(siteGroup);

    auto siteContainer = new QFrame;
    siteContainer->setFrameStyle(QFrame::NoFrame);
    siteContainer->setObjectName(QStringLiteral("instrContainer"));
    siteContainer->setLayout(siteLayout);

    auto siteScrollArea = new QScrollArea;
    siteScrollArea->setWidget(siteContainer);
    siteScrollArea->setWidgetResizable(true);

    auto mainlayout = new QHBoxLayout(this);
    mainlayout->addWidget(siteScrollArea);
    mainlayout->setSizeConstraint(QLayout::SetNoConstraint);
    setLayout(mainlayout);

    decimalDegrees_ = false;

    connect(altitudeLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickAltitudeLabel);
    connect(altitudeSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateAltitude(double)));

    connect(canopyLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickCanopyLabel);
    connect(canopyHeightSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateCanopyHeight(double)));

    connect(displacementLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickDisplacementLabel);
    connect(displacementHeightSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateDisplacementHeight(double)));

    connect(roughnessLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickRoughnessLabel);
    connect(roughnessLengthSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateRoughnessLength(double)));

    connect(latitudeLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickLatitudeLabel);
    connect(latitudeEdit, &QLineEdit::textChanged,
            this, &DlSiteTab::updateLatitude);

    connect(longitudeLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickLongitudeLabel);
    connect(longitudeEdit, &QLineEdit::textChanged,
            this, &DlSiteTab::updateLongitude);

    connect(sexDegRadio, &QRadioButton::toggled,
            this, &DlSiteTab::sexDegRadioToogled);
    connect(decDegRadio, &QRadioButton::toggled,
            this, &DlSiteTab::decDegRadioToogled);

    connect(freqLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickFreqLabel);
    connect(acquisitionFreqSpin, SIGNAL(valueChanged(double)),
            this, SLOT(updateAcquisitionFrequency(double)));

    connect(timestampRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateTimestampRef(int)));
    connect(durationLabel, &ClickLabel::clicked,
            this, &DlSiteTab::onClickDurationLabel);
    connect(fileDurationSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateFileDuration(int)));
}

// get latitude in signed decimal degrees from ddmmss.sss string
double DlSiteTab::numLatitude(const QString &text)
{
    double lat = 0.0;

    if (!decimalDegrees_)
    {
        QString lat_pattern = tr(R"((?:([0-8]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(N|S)))").arg(Defs::DEGREE);
                lat_pattern += tr(R"(|(?:(90)%1\s(00)'\s(00)\.(000)''\s(N|S)))").arg(Defs::DEGREE);
        QRegularExpression latRx(lat_pattern);
        auto match = latRx.match(text);
        if (match.hasMatch())
        {
            bool ok;
            if (!match.captured(1).isEmpty())
            {
                // first case: pattern from cap(1) to cap(5)
                lat = match.captured(1).toDouble(&ok);
                lat += match.captured(2).toDouble(&ok) / 60.0;
                lat += QString(match.captured(3)
                               + QStringLiteral(".")
                               + match.captured(4)).toDouble(&ok) / 60.0 / 60.0;
            }
            else
            {
                // second case: pattern from cap(6) to cap(10)
                lat = 90.0;
            }

            // negative coordinates case
            if ((match.captured(5) == QLatin1String("S"))
                    || (match.captured(10) == QLatin1String("S")))
            {
                lat = 0.0 - lat;
            }
        }
    }
    else
    {
        QString lat_pattern = QStringLiteral("(?:(?:\\+|-)(?:[0-8]\\d)\\.(?:\\d+))|");
                lat_pattern += QLatin1String("(?:(?:\\+|-)(?:90)\\.(?:0+))");
        QRegularExpression latRx(lat_pattern);
        auto match = latRx.match(text);

        if (match.hasMatch())
        {
            lat = text.toDouble();
        }
    }
    return lat;
}

// compute latitude string in ddmmss.sss from signed decimal degrees
QString DlSiteTab::strLatitude(double lat)
{
    QString dms = QString();

    double degrees;
    modf(lat, &degrees);

    double rest = lat - degrees;
    double min_rest = rest * 60.0;

    double min_d;
    modf(min_rest, &min_d);

    auto minutes = static_cast<int>(min_d);

    rest = min_rest - minutes;
    double seconds = rest * 60.0;

    QString degrees_str;
    QTextStream d(&degrees_str);
    d << qSetFieldWidth(2)
        << qSetPadChar(QLatin1Char('0'))
        << right
        << qAbs( degrees );

    QString minutes_str;
    QTextStream m(&minutes_str);
    m << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << right << qAbs( minutes );

    QString seconds_str;
    QTextStream s(&seconds_str);
    s << qSetFieldWidth(6) << qSetRealNumberPrecision(3) << qSetPadChar(QLatin1Char('0'))
      << forcepoint << fixed << qAbs( seconds );

    dms.append(degrees_str);
    dms.append(tr("%1 ").arg(Defs::DEGREE));
    dms.append(minutes_str);
    dms.append(QLatin1String("' "));
    dms.append(seconds_str);
    dms.append(QLatin1String("'' "));

    if ( lat >= 0 )
    {
        dms.append( QLatin1String("N") );
    }
    else
    {
        dms.append( QLatin1String("S") );
    }

    return dms;
}

// get longitude in signed decimal degrees from dddmmss.sss string
double DlSiteTab::numLongitude(const QString &text)
{
    double lon = 0.0;

    if (!decimalDegrees_)
    {
        QString lon_pattern = tr(R"((?:(0\d\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
                lon_pattern += tr(R"((?:(1[0-7]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
                lon_pattern += tr(R"((?:(180)%1\s(00)'\s(00)\.(000)''\s(E|W)))").arg(Defs::DEGREE);
        QRegularExpression lonRx(lon_pattern);
        auto match = lonRx.match(text);
        if (match.hasMatch())
        {
            bool ok;
            if (!match.captured(1).isEmpty())
            {
                // first case: pattern from cap(1) to cap(5)
                lon = match.captured(1).toDouble(&ok);
                lon += match.captured(2).toDouble(&ok) / 60.0;
                lon += QString(match.captured(3) + QStringLiteral(".")
                               + match.captured(4)).toDouble(&ok) / 60.0 / 60.0;
            }
            else if (!match.captured(6).isEmpty())
            {
                // second case: pattern from cap(6) to cap(10)
                lon = match.captured(6).toDouble(&ok);
                lon += match.captured(7).toDouble(&ok) / 60.0;
                lon += QString(match.captured(8) + QStringLiteral(".")
                               + match.captured(9)).toDouble(&ok) / 60.0 / 60.0;
            }
            else
            {
                // third case: pattern from cap(11) to cap(15)
                lon = 180.0;
            }
            // negative coordinates case
            if ((match.captured(5) == QLatin1String("W"))
                    || (match.captured(10) == QLatin1String("W"))
                    || (match.captured(15) == QLatin1String("W")))
            {
                lon = 0.0 - lon;
            }
        }
    }
    else
    {
        QString lon_pattern = QStringLiteral("(?:(?:\\+|-)(?:0\\d\\d)\\.(?:\\d+))|");
                lon_pattern += QLatin1String(R"((?:(?:\+|-)(?:1[0-7]\d)\.(?:\d+))|)");
                lon_pattern += QLatin1String("(?:(?:\\+|-)(?:180)\\.(?:0+))");
        QRegularExpression lonRx(lon_pattern);
        auto match = lonRx.match(text);

        if (match.hasMatch())
        {
            lon = text.toDouble();
        }
    }
    return lon;
}

// compute longitude string in dddmmss.sss from signed decimal degrees
QString DlSiteTab::strLongitude(double lon)
{
    QString dms = QString();

    double degrees;
    modf(lon, &degrees);

    double rest = lon - degrees;
    double min_rest = rest * 60.0;

    double min_d;
    modf(min_rest, &min_d);

    auto minutes = static_cast<int>(min_d);

    rest = min_rest - minutes;
    double seconds = rest * 60.0;

    QString degrees_str;
    QTextStream d(&degrees_str);
    d << qSetFieldWidth(3) << qSetPadChar(QLatin1Char('0')) << right << qAbs( degrees );

    QString minutes_str;
    QTextStream m(&minutes_str);
    m << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << right << qAbs( minutes );

    QString seconds_str;
    QTextStream s(&seconds_str);
    s << qSetFieldWidth(6) << qSetRealNumberPrecision(3) << qSetPadChar(QLatin1Char('0'))
      << forcepoint << fixed << qAbs( seconds );

    dms.append(degrees_str);
    dms.append(tr("%1 ").arg(Defs::DEGREE));
    dms.append(minutes_str);
    dms.append(QLatin1String("' "));
    dms.append(seconds_str);
    dms.append(QLatin1String("'' "));

    if ( lon >= 0 )
    {
        dms.append( QLatin1String("E") );
    }
    else
    {
        dms.append( QLatin1String("W") );
    }
    return dms;
}

void DlSiteTab::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = project_->modified();
    project_->blockSignals(true);

    altitudeSpin->setValue(0.0);
    canopyHeightSpin->setValue(0.0);
    displacementHeightSpin->setValue(0.0);
    roughnessLengthSpin->setValue(0.0);

    if (!decimalDegrees_)
    {
        latitudeEdit->setText(strLatitude(0.0));
        longitudeEdit->setText(strLongitude(0.0));
    }
    else
    {
        latitudeEdit->setText(QString::number(0.0, 'd', 6));
        longitudeEdit->setText(QString::number(0.0, 'd', 6));
    }

    timestampRefStartRadio->setChecked(true);
    fileDurationSpin->setValue(30);
    acquisitionFreqSpin->setValue(10.0);

    // restore modified flag
    project_->setModified(oldmod);
    project_->blockSignals(false);
}

void DlSiteTab::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = project_->modified();
    project_->blockSignals(true);

    altitudeSpin->setValue(project_->siteAltitude());
    canopyHeightSpin->setValue(project_->siteCanopyHeight());
    displacementHeightSpin->setValue(project_->siteDisplacementHeight());
    roughnessLengthSpin->setValue(project_->siteRoughnessLength());

    if (!decimalDegrees_)
    {
        latitudeEdit->setText(strLatitude(project_->siteLatitude()));
        longitudeEdit->setText(strLongitude(project_->siteLongitude()));
    }
    else
    {
        latitudeEdit->setText(QString::number(project_->siteLatitude(), 'd', 6));
        longitudeEdit->setText(QString::number(project_->siteLongitude(), 'd', 6));
    }

    timestampRefStartRadio->setChecked(!project_->timestampEnd());
    timestampRefEndRadio->setChecked(project_->timestampEnd());
    fileDurationSpin->setValue(project_->fileDuration());
    acquisitionFreqSpin->setValue(project_->acquisitionFrequency());

    // restore modified flag
    project_->setModified(oldmod);
    project_->blockSignals(false);
}

void DlSiteTab::sexDegRadioToogled(bool checked)
{
    if (checked)
    {
        decimalDegrees_ = false;

        double currentLat = project_->siteLatitude();
        double currentLon = project_->siteLongitude();
        latitudeEdit->blockSignals(true);
        longitudeEdit->blockSignals(true);

        QString lat_pattern = tr(R"((?:([0-8]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(N|S)))").arg(Defs::DEGREE);
                lat_pattern += tr(R"(|(?:(90)%1\s(00)'\s(00)\.(000)''\s(N|S)))").arg(Defs::DEGREE);
        QRegularExpression latRx(lat_pattern);
        auto latValidator = new QRegularExpressionValidator(latRx, latitudeEdit);
        latitudeEdit->setValidator(latValidator);
        latitudeEdit->setInputMask(tr("00%1 00' 00.000'' >A;x").arg(Defs::DEGREE));

        QString lon_pattern = tr(R"((?:(0\d\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
                lon_pattern += tr(R"((?:(1[0-7]\d)%1\s([0-5]\d)'\s([0-5]\d)\.(\d\d\d)''\s(E|W))|)").arg(Defs::DEGREE);
                lon_pattern += tr(R"((?:(180)%1\s(00)'\s(00)\.(000)''\s(E|W)))").arg(Defs::DEGREE);
        QRegularExpression lonRx(lon_pattern);
        auto lonValidator = new QRegularExpressionValidator(lonRx, longitudeEdit);
        longitudeEdit->setValidator(lonValidator);
        longitudeEdit->setInputMask(tr("000%1 00' 00.000'' >A;x").arg(Defs::DEGREE));

        latitudeEdit->setText(strLatitude(currentLat));
        longitudeEdit->setText(strLongitude(currentLon));
        latitudeEdit->blockSignals(false);
        longitudeEdit->blockSignals(false);
    }
}

void DlSiteTab::decDegRadioToogled(bool checked)
{
    if (checked)
    {
        decimalDegrees_ = true;

        double currentLat = project_->siteLatitude();
        double currentLon = project_->siteLongitude();
        latitudeEdit->blockSignals(true);
        longitudeEdit->blockSignals(true);

        QString lat_pattern = QStringLiteral("(?:(?:\\+|-)(?:[0-8]\\d)\\.(?:\\d+))|");
                lat_pattern += QLatin1String("(?:(?:\\+|-)(?:90)\\.(?:0+))");
        QRegularExpression latRx(lat_pattern);
        auto latValidator = new QRegularExpressionValidator(latRx, latitudeEdit);
        latitudeEdit->setValidator(latValidator);
        latitudeEdit->setInputMask(QStringLiteral("#00.000000"));

        QString lon_pattern = QStringLiteral("(?:(?:\\+|-)(?:0\\d\\d)\\.(?:\\d+))|");
                lon_pattern += QLatin1String(R"((?:(?:\+|-)(?:1[0-7]\d)\.(?:\d+))|)");
                lon_pattern += QLatin1String("(?:(?:\\+|-)(?:180)\\.(?:0+))");
        QRegularExpression lonRx(lon_pattern);
        auto lonValidator = new QRegularExpressionValidator(lonRx, longitudeEdit);
        longitudeEdit->setValidator(lonValidator);
        longitudeEdit->setInputMask(QStringLiteral("#000.000000"));

        QString currentLat_mod = QString::number(currentLat, 'd', 6);
        if (currentLat >= 0)
        {
            currentLat_mod = currentLat_mod.rightJustified(9, QLatin1Char('0'));
            currentLat_mod.prepend(QLatin1String("+"));
        }
        else
        {
            currentLat_mod.remove(0, 1);
            currentLat_mod = currentLat_mod.rightJustified(9, QLatin1Char('0'));
            currentLat_mod.prepend(QString());
        }

        QString currentLon_mod = QString::number(currentLon, 'd', 6);
        if (currentLon >= 0)
        {
            currentLon_mod = currentLon_mod.rightJustified(10, QLatin1Char('0'));
            currentLon_mod.prepend(QLatin1String("+"));
        }
        else
        {
            currentLon_mod.remove(0, 1);
            currentLon_mod = currentLon_mod.rightJustified(10, QLatin1Char('0'));
            currentLon_mod.prepend(QString());
        }

        latitudeEdit->setText(currentLat_mod);
        longitudeEdit->setText(currentLon_mod);

        latitudeEdit->blockSignals(false);
        longitudeEdit->blockSignals(false);
    }
}

void DlSiteTab::updateAltitude(double n)
{
    project_->setSiteAltitude(n);
}

void DlSiteTab::updateLatitude(const QString& lat)
{
    QString lat_str = QString::number(numLatitude(lat), 'd', 6);
    project_->setSiteLatitude(lat_str.toDouble());
}

void DlSiteTab::updateLongitude(const QString& lon)
{
    QString lon_str = QString::number(numLongitude(lon), 'd', 6);
    project_->setSiteLongitude(lon_str.toDouble());
}

void DlSiteTab::updateCanopyHeight(double d)
{
    project_->setSiteCanopyHeight(d);

    // min/max constraint
    if (d < displacementHeightSpin->value())
    {
        displacementHeightSpin->setValue(d);
    }
    if (d < roughnessLengthSpin->value())
    {
        roughnessLengthSpin->setValue(d);
    }
}

void DlSiteTab::updateDisplacementHeight(double d)
{
    project_->setSiteDisplacementHeight(d);

    // min/max constraint
    if (d > canopyHeightSpin->value())
    {
        canopyHeightSpin->setValue(d);
    }
}

void DlSiteTab::updateRoughnessLength(double d)
{
    project_->setSiteRoughnessLength(d);

    // min/max constraint
    if (d > canopyHeightSpin->value())
    {
        canopyHeightSpin->setValue(d);
    }
}

void DlSiteTab::onClickAltitudeLabel()
{
    altitudeSpin->setFocus();
    altitudeSpin->selectAll();
}

void DlSiteTab::onClickCanopyLabel()
{
    canopyHeightSpin->setFocus();
    canopyHeightSpin->selectAll();
}

void DlSiteTab::onClickDisplacementLabel()
{
    displacementHeightSpin->setFocus();
    displacementHeightSpin->selectAll();
}

void DlSiteTab::onClickRoughnessLabel()
{
    roughnessLengthSpin->setFocus();
    roughnessLengthSpin->selectAll();
}

void DlSiteTab::onClickLatitudeLabel()
{
    latitudeEdit->setFocus();
    latitudeEdit->selectAll();
}

void DlSiteTab::onClickLongitudeLabel()
{
    longitudeEdit->setFocus();
    longitudeEdit->selectAll();
}

void DlSiteTab::onClickFreqLabel()
{
    acquisitionFreqSpin->setFocus();
    acquisitionFreqSpin->selectAll();
}

void DlSiteTab::updateAcquisitionFrequency(double d)
{
    project_->setAcquisitionFrequency(d);
}

void DlSiteTab::updateTimestampRef(int n)
{
    project_->setTimestampEnd(n);
}

void DlSiteTab::updateFileDuration(int n)
{
    project_->setFileDuration(n);
}

void DlSiteTab::onClickDurationLabel()
{
    fileDurationSpin->setFocus();
    fileDurationSpin->selectAll();
}

void DlSiteTab::createQuestionMark()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_2 = new QPushButton;
    questionMark_2->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_3 = new QPushButton;
    questionMark_3->setObjectName(QStringLiteral("questionMarkImg"));

    connect(questionMark_1, &QPushButton::clicked,
            this, &DlSiteTab::onlineHelpTrigger_1);
    connect(questionMark_2, &QPushButton::clicked,
            this, &DlSiteTab::onlineHelpTrigger_2);
    connect(questionMark_3, &QPushButton::clicked,
            this, &DlSiteTab::onlineHelpTrigger_3);
}

void DlSiteTab::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/beginning-of-dataset.html")));
}

void DlSiteTab::onlineHelpTrigger_2()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/displacement-height.html")));
}

void DlSiteTab::onlineHelpTrigger_3()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/support/EddyPro/topics/roughness-length.html")));
}
