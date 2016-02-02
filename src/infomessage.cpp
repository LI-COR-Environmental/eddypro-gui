/***************************************************************************
  infomessages.cpp
  -------------------
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

#include "infomessage.h"

#include <QCheckBox>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>

#include "dbghelper.h"
#include "defs.h"
#include "globalsettings.h"
#include "widget_utils.h"

InfoMessage::InfoMessage(QDialogButtonBox::StandardButtons buttons, QWidget *parent) :
    QDialog(parent),
    title_(),
    message_(new QLabel(this))
{
    WidgetUtils::removeContextHelpButton(this);

    doNotShowAgainCheckbox_ = new QCheckBox(tr("Do not show again."));

    buttonBox_ = new QDialogButtonBox(buttons);

    foreach (QAbstractButton* button, buttonBox_->buttons())
    {
        button->setProperty("mdDialogButton", true);
    }

    // NOTE: use QSignalMapper
    // map dialog results to messagebox buttons
    foreach (QAbstractButton* button, buttonBox_->buttons())
    {
        switch (buttonBox_->buttonRole(button))
        {
            case QDialogButtonBox::AcceptRole:
                connect(button, &QAbstractButton::clicked,
                        this, &InfoMessage::onOkButtonClicked);
                break;
            case QDialogButtonBox::YesRole:
                connect(button, &QAbstractButton::clicked,
                        this, &InfoMessage::onYesButtonClicked);
                break;
            case QDialogButtonBox::NoRole:
                connect(button, &QAbstractButton::clicked,
                        this, &InfoMessage::onNoButtonClicked);
                break;
            case QDialogButtonBox::RejectRole:
                connect(button, &QAbstractButton::clicked,
                        this, &InfoMessage::onCancelButtonClicked);
                break;
            case QDialogButtonBox::ActionRole:
            case QDialogButtonBox::DestructiveRole:
            case QDialogButtonBox::InvalidRole:
            case QDialogButtonBox::ApplyRole:
            case QDialogButtonBox::ResetRole:
            case QDialogButtonBox::HelpRole:
            case QDialogButtonBox::NRoles:
                break;
        }
    }

    message_->setWordWrap(true);

    icon_ = new QLabel;
    auto pixmap = QPixmap(QStringLiteral(":/icons/msg-info"));
#if defined(Q_OS_MAC)
    pixmap.setDevicePixelRatio(2.0);
#endif
    icon_->setPixmap(pixmap);

    auto firstRowLayout = new QHBoxLayout;
    firstRowLayout->addWidget(icon_);
    firstRowLayout->insertSpacing(1, 15);
    firstRowLayout->addWidget(message_);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(firstRowLayout);
    mainLayout->addWidget(doNotShowAgainCheckbox_, 0, Qt::AlignCenter);
    mainLayout->insertSpacing(2, 15);
    mainLayout->addWidget(buttonBox_, 0, Qt::AlignCenter);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));
    connect(doNotShowAgainCheckbox_, &QCheckBox::toggled,
            this, &InfoMessage::onDoNotShowAgainCheckboxToggled_);

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
        case Type::RUN_EXPRESS:
            showDialog = config.value(Defs::CONF_WIN_RUN_EXP_MSG, true).toBool();
            break;
        case Type::RUN_ADVANCED:
            showDialog = config.value(Defs::CONF_WIN_RUN_ADV_MSG, true).toBool();
            break;
        case Type::RUN_RETRIEVER:
            showDialog = config.value(Defs::CONF_WIN_RUN_RET_MSG, true).toBool();
            break;
        case Type::SELECTION_CLEANING:
            showDialog = config.value(Defs::CONF_WIN_BASIC_SETTINGS_CLEARING_MSG, true).toBool();
            break;
        case Type::SMARTFLUX_CONFIG:
            showDialog = config.value(Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG, true).toBool();
            break;
        case Type::NOAA_WEBSITE:
            showDialog = config.value(Defs::CONF_WIN_NOAA_WEBSITE_MSG, true).toBool();
            break;
        case Type::ANGLE_OF_ATTACK_SELECTION:
            showDialog = config.value(Defs::CONF_WIN_AOA_SELECTION_MSG, true).toBool();
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
        case Type::RUN_EXPRESS:
            config.setValue(Defs::CONF_WIN_RUN_EXP_MSG, !toggled);
            break;
        case Type::RUN_ADVANCED:
            config.setValue(Defs::CONF_WIN_RUN_ADV_MSG, !toggled);
            break;
        case Type::RUN_RETRIEVER:
            config.setValue(Defs::CONF_WIN_RUN_RET_MSG, !toggled);
            break;
        case Type::SELECTION_CLEANING:
            config.setValue(Defs::CONF_WIN_BASIC_SETTINGS_CLEARING_MSG, !toggled);
            break;
        case Type::SMARTFLUX_CONFIG:
            config.setValue(Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG, !toggled);
            break;
        case Type::NOAA_WEBSITE:
            config.setValue(Defs::CONF_WIN_NOAA_WEBSITE_MSG, !toggled);
            break;
        case Type::ANGLE_OF_ATTACK_SELECTION:
            config.setValue(Defs::CONF_WIN_AOA_SELECTION_MSG, !toggled);
            break;
    }
    config.endGroup();
    config.sync();
}

void InfoMessage::setTitle(const QString &title)
{
    setWindowTitle(title);
}

void InfoMessage::setIcon(const QPixmap &icon)
{
    auto pixmap = const_cast<QPixmap&>(icon);
#if defined(Q_OS_MAC)
    pixmap.setDevicePixelRatio(2.0);
#endif
    icon_->setPixmap(pixmap);
}

void InfoMessage::setMessage(const QString& text)
{
    message_->setText(text);
}

void InfoMessage::setType(Type type)
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
