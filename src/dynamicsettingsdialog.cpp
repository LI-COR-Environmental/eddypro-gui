/***************************************************************************
  dynamicsettingsdialog.cpp
  -------------------
  A dialog for the generic binary file settings
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011, LI-COR Biosciences

  This file is part of EddyPro (TM).

  EddyPro (TM) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (TM) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (TM). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include <QtCore/QDebug>
#include <QFrame>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>

#include "ecproject.h"
#include "dynamicsettingsdialog.h"

DynamicSettingsDialog::DynamicSettingsDialog(QWidget* parent, EcProject *ecProject)
    : QDialog(),
    ecProject_(ecProject)
{
    Qt::WindowFlags winFflags = windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    setWindowFlags(winFflags);

    QLabel *groupTitle = new QLabel;
    groupTitle->setText(tr("Enter settings for dynamic metadata and meteo data files."));

    timelineFileCheckBox = new QCheckBox;
    timelineFileCheckBox->setText(tr("Use dynamic metadata file  :"));
    timelineFileEdit = new QLineEdit;
    timelineFileEdit->setReadOnly(true);
    timelineFileEdit->setEnabled(false);
    timelineFileEdit->setProperty("asLabel", true);
    timelineFileLoad = new QPushButton(tr("Load..."));
    timelineFileLoad->setProperty("commonButton", true);
    timelineFileLoad->setEnabled(false);
    timelineFileLoad->setMaximumWidth(timelineFileLoad->sizeHint().width());

    meteoFileCheckBox = new QCheckBox;
    meteoFileCheckBox->setText(tr("Use high quality meteo file  :"));
    meteoFileEdit = new QLineEdit;
    meteoFileEdit->setReadOnly(true);
    meteoFileEdit->setEnabled(false);
    meteoFileEdit->setProperty("asLabel", true);
    meteoFileLoad = new QPushButton(tr("Load..."));
    meteoFileLoad->setProperty("commonButton", true);
    meteoFileLoad->setEnabled(false);
    meteoFileLoad->setMaximumWidth(meteoFileLoad->sizeHint().width());

    QGridLayout *dynPropertiesLayout = new QGridLayout;
    dynPropertiesLayout->addWidget(timelineFileCheckBox, 0, 0, 1, 1);
    dynPropertiesLayout->addWidget(timelineFileEdit, 0, 1, 1, 2);
    dynPropertiesLayout->addWidget(timelineFileLoad, 0, 3, 1, 1);
    dynPropertiesLayout->addWidget(meteoFileCheckBox, 2, 0, 1, 1);
    dynPropertiesLayout->addWidget(meteoFileEdit, 2, 1, 1, 2);
    dynPropertiesLayout->addWidget(meteoFileLoad, 2, 3, 1, 1);
    dynPropertiesLayout->setVerticalSpacing(3);
    dynPropertiesLayout->setMargin(3);
    dynPropertiesLayout->setRowMinimumHeight(1, 20);
    dynPropertiesLayout->setColumnMinimumWidth(1, 250);

    QFrame *dynPropertiesFrame = new QFrame;
//    dynPropertiesFrame->setObjectName(QLatin1String("binPropertiesFrame"));
//    dynPropertiesFrame->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dynPropertiesFrame->setFrameStyle(QFrame::Panel);
    dynPropertiesFrame->setLineWidth(1);
    dynPropertiesFrame->setLayout(dynPropertiesLayout);

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okButton->setDefault(true);
    okButton->setProperty("commonButton", true);

    QGridLayout *dynSettingsLayout = new QGridLayout(this);
    dynSettingsLayout->addWidget(groupTitle, 0, 0);
    dynSettingsLayout->addWidget(dynPropertiesFrame, 1, 0);
    dynSettingsLayout->addWidget(okButton, 2, 0, 1, 1, Qt::AlignCenter);
    dynSettingsLayout->setVerticalSpacing(10);
    dynSettingsLayout->setMargin(10);
    dynSettingsLayout->setSizeConstraint(QLayout::SetFixedSize);
    dynSettingsLayout->setRowMinimumHeight(0, 25);
    setLayout(dynSettingsLayout);

    connect(timelineFileCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onTimelineFileCheckBoxClicked(bool)));
    connect(timelineFileCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateUseTimelineFile(bool)));
    connect(timelineFileEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateTimelineFile(const QString &)));
    connect(timelineFileLoad, SIGNAL(clicked()),
            this, SLOT(timelineFileLoad_clicked()));

    connect(meteoFileCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(onMeteoFileCheckBoxClicked(bool)));
    connect(meteoFileCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(updateUseMeteoFile(bool)));
    connect(meteoFileEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(updateMeteoFile(const QString &)));
    connect(meteoFileLoad, SIGNAL(clicked()),
            this, SLOT(meteoFileLoad_clicked()));

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(ecProject_, SIGNAL(ecProjectNew()), this, SLOT(reset()));
}

DynamicSettingsDialog::~DynamicSettingsDialog()
{
    qDebug() << Q_FUNC_INFO;
}

void DynamicSettingsDialog::close()
{
    if (isVisible())
        hide();
}

void DynamicSettingsDialog::reset()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    timelineFileCheckBox->setChecked(false);
    timelineFileEdit->clear();

    meteoFileCheckBox->setChecked(false);
    meteoFileEdit->clear();

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void DynamicSettingsDialog::refresh()
{
    // save the modified flag to prevent side effects of setting widgets
    bool oldmod = ecProject_->modified();
    ecProject_->blockSignals(true);

    timelineFileCheckBox->setChecked(ecProject_->generalUseTimelineFile());
    timelineFileEdit->setText(QDir::toNativeSeparators(ecProject_->generalTimelineFilepath()));

    meteoFileCheckBox->setChecked(ecProject_->generalUseMeteoFile());
    meteoFileEdit->setText(QDir::toNativeSeparators(ecProject_->generalMeteoFilepath()));

    // restore modified flag
    ecProject_->setModified(oldmod);
    ecProject_->blockSignals(false);
}

void DynamicSettingsDialog::onTimelineFileCheckBoxClicked(bool b)
{
    timelineFileEdit->setEnabled(b);
//    timelineFileEdit->setProperty("mandatoryField", b);
    timelineFileLoad->setEnabled(b);
}

void DynamicSettingsDialog::updateUseTimelineFile(const bool& b)
{
    ecProject_->setGeneralUseTimelineFile(b);
}

void DynamicSettingsDialog::updateTimelineFile(const QString& fp)
{
    ecProject_->setGeneralTimelineFilepath(QDir::cleanPath(fp));
}

void DynamicSettingsDialog::timelineFileLoad_clicked()
{
    QString searchPath = ecProject_->timelagDataPath();
    if (!ecProject_->screenDataPath().isEmpty())
        searchPath = ecProject_->screenDataPath();

    QString paramFile = QFileDialog::getOpenFileName(this,
                    tr("Select the dynamic metadata file"),
                    searchPath,
                    tr("All Files (*.*)")
                    );
    if (!paramFile.isEmpty())
    {
        QFileInfo paramFilePath(paramFile);
        QString canonicalParamFile = paramFilePath.canonicalFilePath();
        timelineFileEdit->setText(QDir::toNativeSeparators(canonicalParamFile));
    }
}

void DynamicSettingsDialog::onMeteoFileCheckBoxClicked(bool b)
{
    meteoFileEdit->setEnabled(b);
//    meteoFileEdit->setProperty("mandatoryField", b);
    meteoFileLoad->setEnabled(b);
}

void DynamicSettingsDialog::updateUseMeteoFile(const bool& b)
{
    ecProject_->setGeneralUseMeteoFile(b);
}

void DynamicSettingsDialog::updateMeteoFile(const QString& fp)
{
    ecProject_->setGeneralMeteoFilepath(QDir::cleanPath(fp));
}

void DynamicSettingsDialog::meteoFileLoad_clicked()
{
    QString searchPath = ecProject_->generalMeteoFilepath();
    if (!ecProject_->screenDataPath().isEmpty())
        searchPath = ecProject_->screenDataPath();

    QString paramFile = QFileDialog::getOpenFileName(this,
                        tr("Select the high quality meteo file"),
                        searchPath,
                        tr("All Files (*.*)")
                        );
    if (!paramFile.isEmpty())
    {
        QFileInfo paramFilePath(paramFile);
        QString canonicalParamFile = paramFilePath.canonicalFilePath();
        meteoFileEdit->setText(QDir::toNativeSeparators(canonicalParamFile));
    }
}
