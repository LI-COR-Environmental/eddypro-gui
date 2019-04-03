/***************************************************************************
  binarysettingsdialog.cpp
  ------------------------
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

#include "binarysettingsdialog.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>

#include "clicklabel.h"
#include "ecproject.h"
#include "widget_utils.h"

BinarySettingsDialog::BinarySettingsDialog(QWidget* parent, EcProject *ecProject)
    : QDialog(parent),
    ecProject_(ecProject)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Generic Binary File Settings"));
    WidgetUtils::removeContextHelpButton(this);

    auto groupTitle = new QLabel;
    groupTitle->setText(tr("Please provide details of your binary format."));

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));

    binaryHLinesLabel = new ClickLabel(tr("Number of ASCII header lines :"));
    binaryHLinesLabel->setToolTip(tr("<b>Number of ASCII header lines:</b> Enter the number of ASCII (readable text) header lines present at the beginning of your binary files. Enter 0 if there is no ASCII header."));
    binaryHLinesSpin = new QSpinBox(this);
    binaryHLinesSpin->setRange(-1, 100);
    binaryHLinesSpin->setSingleStep(1);
    binaryHLinesSpin->setSpecialValueText(tr("Not set"));
    binaryHLinesSpin->setAccelerated(true);
    binaryHLinesSpin->setToolTip(binaryHLinesLabel->toolTip());

    binaryEolLabel = new ClickLabel(tr("ASCII header end of line :"));
    binaryEolLabel->setToolTip(tr("<b>ASCII header end of line:</b> If an ASCII header is present in the files, specify the line terminator. Typically, Windows OS uses <i>CR+LF</i> (0x0D+0x0A), Linux OS and macOS use <i>LF</i> (0x0A), while Mac OS up to version 9<i>CR</i> (0x0D)."));
    binaryEolCombo = new QComboBox;
    binaryEolCombo->addItem(tr("CR+LF"));
    binaryEolCombo->addItem(tr("CR"));
    binaryEolCombo->addItem(tr("LF"));
    binaryEolCombo->setStyleSheet(QStringLiteral("QComboBox {min-width: 110px;"
                                                 "max-width: 110px;}"));
    binaryEolCombo->setToolTip(binaryEolLabel->toolTip());

    binaryNBytesLabel = new ClickLabel(tr("Number of bytes per variable :"));
    binaryNBytesLabel->setToolTip(tr("<b>Number of bytes per variable:</b> Specify the number of bytes reserved for each variable stored as a single precision number. Typically 2 bytes are reserved for each number."));
    binaryNBytesSpin = new QSpinBox(this);
    binaryNBytesSpin->setRange(0, 8);
    binaryNBytesSpin->setSingleStep(1);
    binaryNBytesSpin->setSpecialValueText(tr("Not set"));
    binaryNBytesSpin->setAccelerated(true);
    binaryNBytesSpin->setToolTip(binaryNBytesLabel->toolTip());

    binaryEndianessLabel = new ClickLabel(tr("Endianess :"));
    binaryEndianessLabel->setToolTip(tr("<b>Endianess:</b> In a multi-bytes binary word, <i>Little endian</i> means that the most significant byte is the last byte (highest address); <i>Big endian</i> means that the most significant byte is the first byte (lowest address)."));
    binaryEndianessCombo = new QComboBox;
    binaryEndianessCombo->addItem(tr("Big endian"));
    binaryEndianessCombo->addItem(tr("Little endian"));
    binaryEndianessCombo->setStyleSheet(QStringLiteral("QComboBox {min-width: 110px;"
                                                 "max-width: 110px;}"));
    binaryEndianessCombo->setToolTip(binaryEndianessLabel->toolTip());

    auto binPropertiesLayout = new QGridLayout;
    binPropertiesLayout->addWidget(binaryHLinesLabel, 0, 0, 1, 1, Qt::AlignRight);
    binPropertiesLayout->addWidget(binaryHLinesSpin, 0, 1);
    binPropertiesLayout->addWidget(binaryEolLabel, 1, 0, 1, 1, Qt::AlignRight);
    binPropertiesLayout->addWidget(binaryEolCombo, 1, 1);
    binPropertiesLayout->addWidget(binaryNBytesLabel, 2, 0, 1, 1, Qt::AlignRight);
    binPropertiesLayout->addWidget(binaryNBytesSpin, 2, 1);
    binPropertiesLayout->addWidget(binaryEndianessLabel, 3, 0, 1, 1, Qt::AlignRight);
    binPropertiesLayout->addWidget(binaryEndianessCombo, 3, 1);
    binPropertiesLayout->setVerticalSpacing(3);
    binPropertiesLayout->setContentsMargins(3, 3, 3, 3);

    auto binPropertiesWidget = new QWidget;
    binPropertiesWidget->setLayout(binPropertiesLayout);

    auto okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    auto mainLayout = new QGridLayout(this);
    mainLayout->addWidget(groupTitle, 0, 0);
    mainLayout->addWidget(hrLabel, 1, 0);
    mainLayout->addWidget(binPropertiesWidget, 2, 0);
    mainLayout->addWidget(okButton, 3, 0, 1, 1, Qt::AlignCenter);
    mainLayout->setVerticalSpacing(10);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    connect(binaryHLinesLabel, &ClickLabel::clicked,
            this, &BinarySettingsDialog::onBinaryHLinesLabelClicked);
    connect(binaryHLinesSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateBinaryHLine(int)));

    connect(binaryEolLabel, &ClickLabel::clicked,
            this, &BinarySettingsDialog::onBinaryClickEolLabel);
    connect(binaryEolCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateBinaryEol(int)));

    connect(binaryNBytesLabel, &ClickLabel::clicked,
            this, &BinarySettingsDialog::onBinaryNBytesLabelClicked);
    connect(binaryNBytesSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateBinaryNBytes(int)));

    connect(binaryEndianessLabel, &ClickLabel::clicked,
            this, &BinarySettingsDialog::onBinaryClickEndianessLabel);
    connect(binaryEndianessCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateBinaryEndianess(int)));

    connect(okButton, &QPushButton::clicked,
            this, &BinarySettingsDialog::close);
}

void BinarySettingsDialog::close()
{
    if (isVisible())
        hide();
    emit saveRequest();
}

void BinarySettingsDialog::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    binaryHLinesSpin->setValue(ecProject_->generalBinaryHNLines());
    binaryEolCombo->setCurrentIndex(ecProject_->generalBinaryEol());
    binaryNBytesSpin->setValue(ecProject_->generalBinaryNBytes());
    binaryEndianessCombo->setCurrentIndex(ecProject_->generalBinaryLittleEnd());

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void BinarySettingsDialog::onBinaryHLinesLabelClicked()
{
    binaryHLinesSpin->setFocus();
    binaryHLinesSpin->selectAll();
}

void BinarySettingsDialog::onBinaryNBytesLabelClicked()
{
    binaryNBytesSpin->setFocus();
    binaryNBytesSpin->selectAll();
}

void BinarySettingsDialog::onBinaryClickEolLabel()
{
    if (binaryEolCombo->isEnabled())
    {
        binaryEolCombo->showPopup();
    }
}

void BinarySettingsDialog::onBinaryClickEndianessLabel()
{
    if (binaryEndianessCombo->isEnabled())
    {
        binaryEndianessCombo->showPopup();
    }
}

void BinarySettingsDialog::updateBinaryHLine(int n)
{
    ecProject_->setGeneralBinaryHNLines(n);
}

void BinarySettingsDialog::updateBinaryEol(int n)
{
    ecProject_->setGeneralBinaryEol(n);
}

void BinarySettingsDialog::updateBinaryNBytes(int n)
{
    ecProject_->setGeneralBinaryNBytes(n);
}

void BinarySettingsDialog::updateBinaryEndianess(int n)
{
    ecProject_->setGeneralBinaryLittleEnd(n);
}
