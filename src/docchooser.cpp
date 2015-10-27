/***************************************************************************
  docchooser.cpp
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
***************************************************************************/

#include "docchooser.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QVBoxLayout>

#include "dbghelper.h"
#include "defs.h"
#include "globalsettings.h"
#include "widget_utils.h"

DocChooserDialog::DocChooserDialog(const QUrl& url, QWidget *parent) :
    QDialog(parent),
    url_(url)
{
    DEBUG_FUNC_NAME

    resize(390, 250);
    setMinimumSize(390, 250);

    // NOTE: remove all window buttons
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    setWindowTitle(tr("Set Remote/Local Help"));

    auto connectText = new QLabel;
    connectText->setText(tr("Use online help (recommended) if you are connected "
                            "to the Internet.\n"
                            "Change this setting under the Help menu."));

    auto connectLayout = new QHBoxLayout;
    connectLayout->addWidget(connectText);
    connectLayout->addStretch();

    onlineHelpRadio = new QRadioButton;
    onlineHelpRadio->setText(tr("Use online help"));

    offlineHelpRadio = new QRadioButton;
    offlineHelpRadio->setText(tr("Use local help"));

    auto okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    auto layout = new QVBoxLayout(this);
    layout->addLayout(connectLayout);
    layout->addStretch();
    layout->addWidget(onlineHelpRadio);
    layout->addWidget(offlineHelpRadio);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);
    layout->setContentsMargins(30, 30, 30, 30);
    setLayout(layout);

    connect(okButton, &QPushButton::clicked, this, &DocChooserDialog::close);
    connect(offlineHelpRadio, &QRadioButton::toggled,
            this, &DocChooserDialog::setOfflineHelp);

    onlineHelpRadio->setChecked(true);
    restoreAutoChooserState();
    restoreOfflineState();
}

DocChooserDialog::~DocChooserDialog()
{
    qDebug() << Q_FUNC_INFO;
}

void DocChooserDialog::setAutoChooser(bool automatic)
{
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                             Defs::CONF_WIN_AUTOHELP,
                                             automatic);
}

void DocChooserDialog::setOfflineHelp(bool yes)
{
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                             Defs::CONF_WIN_OFFLINEHELP,
                                             yes);
}

void DocChooserDialog::restoreAutoChooserState()
{
    QSettings config;

    // NOTE: set but not used in fact
    bool autoChooseHelp = false;

    // read state
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    if (config.contains(Defs::CONF_WIN_AUTOHELP))
    {
        autoChooseHelp = config.value(Defs::CONF_WIN_AUTOHELP, false).toBool();
    }
    else
    {
        config.setValue(Defs::CONF_WIN_AUTOHELP, true);
    }
    config.endGroup();
    config.sync();
}

void DocChooserDialog::restoreOfflineState()
{
    QSettings config;
    bool offlineHelp = false;

    // read state
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    if (config.contains(Defs::CONF_WIN_AUTOHELP))
    {
        offlineHelp = config.value(Defs::CONF_WIN_OFFLINEHELP, false).toBool();
    }
    else
    {
        config.setValue(Defs::CONF_WIN_OFFLINEHELP, false);
    }
    config.endGroup();
    config.sync();

    offlineHelpRadio->setChecked(offlineHelp);
}

void DocChooserDialog::close()
{
    DEBUG_FUNC_NAME
    if (onlineHelpRadio->isChecked())
    {
        // browse online help version
        QDesktopServices::openUrl(url_);
    }
    else
    {
        // open local help
        QString htmlHelpPath = qApp->applicationDirPath()
                + QStringLiteral("/docs/help/Content/EddyPro_Home.html");
        qDebug() << QDesktopServices::openUrl(QUrl::fromLocalFile(htmlHelpPath));
    }

    // set the variable to true to indicate that the dialog ran once
    // this is the only reason to date to use it.
    // NOTE: refactor to simplify the unnecessary code
    setAutoChooser(true);

    done(0);
}
