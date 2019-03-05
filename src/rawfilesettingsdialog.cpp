/***************************************************************************
  rawfilesettingsdialog.cpp
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

#include "rawfilesettingsdialog.h"

#include <QComboBox>
#include <QDebug>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QUrl>

#include "clicklabel.h"
#include "customclearlineedit.h"
#include "dlproject.h"
#include "widget_utils.h"

RawFileSettingsDialog::RawFileSettingsDialog(QWidget* parent, DlProject *dlProject) :
    QDialog(parent),
    dlProject_(dlProject)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Raw File Settings"));
    WidgetUtils::removeContextHelpButton(this);

    auto groupTitle = new QLabel;
    groupTitle->setText(tr("Enter further raw file information."));

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));

    fieldSepLabel = new ClickLabel;
    fieldSepLabel->setToolTip(tr("<b>Field separator character:</b> Specify the character that separates individual values within the same sample in the raw files."));
    fieldSepLabel->setText(tr("Field separator (ASCII formats only) :"));
    fieldSepCombo = new QComboBox;
    fieldSepCombo->addItem(tr("Comma"), QStringLiteral("comma"));
    fieldSepCombo->addItem(tr("Semicolon"), QStringLiteral("semicolon"));
    fieldSepCombo->addItem(tr("Space"), QStringLiteral("space"));
    fieldSepCombo->addItem(tr("Tab"), QStringLiteral("tab"));
    fieldSepCombo->setStyleSheet(QStringLiteral("QComboBox {min-width: 72px;"
                                                 "max-width: 72px;}"));
    fieldSepCombo->setToolTip(fieldSepLabel->toolTip());

    headerRowsLabel = new ClickLabel;
    headerRowsLabel->setText(tr("Number of header rows :"));
    headerRowsLabel->setToolTip(tr("<b>Number of header rows:</b> Enter the number of rows in the header of the file, if present. In most cases, the software will be able to filter away individual text lines that do not comply with the description provided here. Therefore, most files with a variable number of header lines are supported."));
    headerRowsSpin = new QSpinBox;
    headerRowsSpin->setRange(-1, 100);
    headerRowsSpin->setSingleStep(1);
    headerRowsSpin->setAccelerated(true);
    headerRowsSpin->setAlignment(Qt::AlignRight);
    headerRowsSpin->setToolTip(headerRowsLabel->toolTip());
    headerRowsSpin->setSpecialValueText(tr("Not set"));

    // NOTE: disabled for lack of use cases
    dataRecLabel = new ClickLabel;
    dataRecLabel->setText(tr("Label of data records :"));
    dataRecLabel->setVisible(false);
    dataRecLabelEdit = new CustomClearLineEdit;
    dataRecLabelEdit->setToolTip(dataRecLabel->toolTip());
    dataRecLabelEdit->setVisible(false);

    auto rawPropertiesLayout = new QGridLayout;
    rawPropertiesLayout->addWidget(fieldSepLabel, 0, 0, 1, 1, Qt::AlignRight);
    rawPropertiesLayout->addWidget(fieldSepCombo, 0, 2);
    rawPropertiesLayout->addWidget(headerRowsLabel, 1, 0, 1, 1, Qt::AlignRight);
    rawPropertiesLayout->addWidget(headerRowsSpin, 1, 2);
    rawPropertiesLayout->addWidget(dataRecLabel, 2, 0, 1, 1, Qt::AlignRight);
    rawPropertiesLayout->addWidget(dataRecLabelEdit, 2, 2);
    rawPropertiesLayout->setVerticalSpacing(3);
    rawPropertiesLayout->setContentsMargins(3, 3, 3, 3);

    auto rawPropertiesFrame = new QWidget;
    rawPropertiesFrame->setLayout(rawPropertiesLayout);

    auto okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    auto rawSettingsLayout = new QGridLayout(this);
    rawSettingsLayout->addWidget(groupTitle, 0, 0);
    rawSettingsLayout->addWidget(hrLabel, 1, 0);
    rawSettingsLayout->addWidget(rawPropertiesFrame, 2, 0);
    rawSettingsLayout->addWidget(okButton, 3, 0, 1, 1, Qt::AlignCenter);
    rawSettingsLayout->setVerticalSpacing(10);
    rawSettingsLayout->setContentsMargins(30, 30, 30, 30);
    rawSettingsLayout->setSizeConstraint(QLayout::SetFixedSize);
    rawSettingsLayout->setRowMinimumHeight(3, 30);
    setLayout(rawSettingsLayout);

    connect(fieldSepLabel, &ClickLabel::clicked,
            this, &RawFileSettingsDialog::onClickFieldSepLabel);
    connect(fieldSepCombo, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(updateFieldSep(QString)));

    connect(headerRowsLabel, &ClickLabel::clicked,
            this, &RawFileSettingsDialog::onClickHeaderRowsLabel);
    connect(headerRowsSpin, SIGNAL(valueChanged(int)),
            this, SLOT(updateHeaderRows(int)));

    connect(dataRecLabel, &ClickLabel::clicked,
            this, &RawFileSettingsDialog::onClickDataRecLabel);
    connect(dataRecLabelEdit, &CustomClearLineEdit::textChanged,
            this, &RawFileSettingsDialog::updateDataRecLabel);

    connect(okButton, &QPushButton::clicked,
            [=](){ if (this->isVisible()) hide(); });
}

void RawFileSettingsDialog::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = dlProject_->modified();
    dlProject_->blockSignals(true);

    QString currFielSep = dlProject_->fieldSep();

    fieldSepCombo->setCurrentIndex(fieldSepCombo->findText(currFielSep, Qt::MatchFixedString));

    headerRowsSpin->setValue(dlProject_->headerRows());

    dataRecLabelEdit->setText(dlProject_->dataLabel());

    // restore modified flag
    dlProject_->setModified(oldmod);
    dlProject_->blockSignals(false);
}

void RawFileSettingsDialog::onClickFieldSepLabel()
{
    fieldSepCombo->setFocus();
    fieldSepCombo->showPopup();
}

void RawFileSettingsDialog::updateFieldSep(const QString& s)
{
    dlProject_->setFieldSep(s.simplified().toLower());
}

void RawFileSettingsDialog::onClickHeaderRowsLabel()
{
    headerRowsSpin->setFocus();
    headerRowsSpin->selectAll();
}

void RawFileSettingsDialog::updateHeaderRows(int n)
{
    dlProject_->setHeaderRows(n);
}

void RawFileSettingsDialog::onClickDataRecLabel()
{
    dataRecLabelEdit->setFocus();
    dataRecLabelEdit->selectAll();
}

void RawFileSettingsDialog::updateDataRecLabel(const QString& s)
{
    dlProject_->setDataLabel(s);
}
