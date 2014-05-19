/***************************************************************************
  infomessages.cpp
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
#include <QSettings>
#include <QTimer>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "defs.h"
#include "dbghelper.h"
#include "infomessage.h"

InfoMessage::InfoMessage(QDialogButtonBox::StandardButtons buttons, QWidget *parent) :
    QDialog(parent),
    title_(QString()),
    message_(new QLabel()),
    type_(RUN_EXPRESS),
    doNotShoAgainVisible_(true)
{
    Qt::WindowFlags winFflags = windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    setWindowFlags(winFflags);

    doNotShowAgainCheckbox_ = new QCheckBox(tr("Do not show again."));

    buttonBox_ = new QDialogButtonBox(buttons);

    foreach (QAbstractButton* button, buttonBox_->buttons())
    {
        button->setProperty("mdDialogButton", true);
    }

    // map dialog risults to messagebox buttons
    foreach (QAbstractButton* button, buttonBox_->buttons())
    {
        switch (buttonBox_->buttonRole(button))
        {
            case QDialogButtonBox::AcceptRole:
                connect(button, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));
                break;
            case QDialogButtonBox::YesRole:
                connect(button, SIGNAL(clicked()), this, SLOT(onYesButtonClicked()));
                break;
            case QDialogButtonBox::NoRole:
                connect(button, SIGNAL(clicked()), this, SLOT(onNoButtonClicked()));
                break;
            case QDialogButtonBox::RejectRole:
                connect(button, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
                break;
            default:
                break;
        }
    }

    message_->setWordWrap(true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(message_);
    mainLayout->addWidget(doNotShowAgainCheckbox_);
    mainLayout->addWidget(buttonBox_);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    setLayout(mainLayout);

    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
    connect(doNotShowAgainCheckbox_, SIGNAL(toggled(bool)),
            this, SLOT(onDoNotShowAgainCheckboxToggled_(bool)));

    QTimer::singleShot(0, this, SLOT(initialize()));
}

void InfoMessage::initialize()
{
    doNotShowAgainCheckbox_->setChecked(false);
}

void InfoMessage::refresh()
{
    DEBUG_FUNC_NAME
    bool showDialog = false;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    switch (type_)
    {
        case RUN_EXPRESS:
            showDialog = config.value(Defs::CONF_WIN_RUN_EXP_MSG, true).toBool();
            break;
        case RUN_ADVANCED:
            showDialog = config.value(Defs::CONF_WIN_RUN_ADV_MSG, true).toBool();
            break;
        case RUN_RETRIEVER:
            showDialog = config.value(Defs::CONF_WIN_RUN_RET_MSG, true).toBool();
            break;
        case SELECTION_CLEANING:
            showDialog = config.value(Defs::CONF_WIN_BASIC_SETTINGS_CLEARING_MSG, true).toBool();
            break;
        case SMARTFLUX_CONFIG:
            showDialog = config.value(Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG, true).toBool();
            break;
        case NOAA_WEBSITE:
            showDialog = config.value(Defs::CONF_WIN_NOAA_WEBSITE_MSG, true).toBool();
            break;
        default:
            break;
    }
    config.endGroup();

    qDebug() << "showDialog" << showDialog;

    doNotShowAgainCheckbox_->setChecked(!showDialog);
}

void InfoMessage::onDoNotShowAgainCheckboxToggled_(bool toggled)
{
    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    switch (type_)
    {
        case RUN_EXPRESS:
            config.setValue(Defs::CONF_WIN_RUN_EXP_MSG, !toggled);
            break;
        case RUN_ADVANCED:
            config.setValue(Defs::CONF_WIN_RUN_ADV_MSG, !toggled);
            break;
        case RUN_RETRIEVER:
            config.setValue(Defs::CONF_WIN_RUN_RET_MSG, !toggled);
            break;
        case SELECTION_CLEANING:
            config.setValue(Defs::CONF_WIN_BASIC_SETTINGS_CLEARING_MSG, !toggled);
            break;
        case SMARTFLUX_CONFIG:
            config.setValue(Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG, !toggled);
            break;
        case NOAA_WEBSITE:
            config.setValue(Defs::CONF_WIN_NOAA_WEBSITE_MSG, !toggled);
            break;
        default:
            break;
    }
    config.endGroup();
    config.sync();
}

void InfoMessage::setTitle(const QString &title)
{
    setWindowTitle(title);
}

void InfoMessage::setMessage(const QString& text)
{
    message_->setText(text);
}

void InfoMessage::setType(int type)
{
    type_ = type;
}

void InfoMessage::setDoNotShowAgainVisible(bool visible)
{
    doNotShoAgainVisible_ = visible;
    doNotShowAgainCheckbox_->setVisible(doNotShoAgainVisible_);
}

void InfoMessage::onOkButtonClicked()
{
    this->setResult(QMessageBox::Ok);
}

void InfoMessage::onYesButtonClicked()
{
    this->setResult(QMessageBox::Yes);
}

void InfoMessage::onNoButtonClicked()
{
    this->setResult(QMessageBox::No);
}

void InfoMessage::onCancelButtonClicked()
{
    this->setResult(QMessageBox::Cancel);
}
