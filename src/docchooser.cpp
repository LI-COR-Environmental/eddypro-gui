/***************************************************************************
  docchooser.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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
***************************************************************************/

#include <QPushButton>
#include <QLabel>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QCoreApplication>
#include <QSettings>
#include <QDebug>

#include "defs.h"
#include "dbghelper.h"
#include "docchooser.h"

DocChooserDialog::DocChooserDialog(const QUrl& url, QWidget *parent) :
    QDialog(parent),
    url_(url)
{
    DEBUG_FUNC_NAME

    resize(390, 250);
    setMinimumSize(390, 250);

    // NOTE: remove all window buttons
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    setWindowTitle(tr("Remote/Local Help"));

    QLabel* connectText = new QLabel();

    QHBoxLayout* connectLayout = new QHBoxLayout();
    connectLayout->addWidget(connectText);
    connectLayout->addStretch();

    onlineHelpRadio = new QRadioButton();
    onlineHelpRadio->setText(tr("Use online help"));

    offlineHelpRadio = new QRadioButton();
    offlineHelpRadio->setText(tr("Use local help"));

    QPushButton *okButton = new QPushButton(tr("&Ok"));
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okButton->setDefault(true);
    okButton->setProperty("commonButton", true);
    connectText->setText(tr("Use online help (recommended) if you are connected to the Internet.\n"
                                       "Change this setting under the Help menu."));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(connectLayout);
    layout->addStretch();
    layout->addWidget(onlineHelpRadio);
    layout->addWidget(offlineHelpRadio);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);
    layout->setContentsMargins(30, 30, 30, 30);
    setLayout(layout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(offlineHelpRadio, SIGNAL(toggled(bool)), this, SLOT(setOfflineHelp(bool)));

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
    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        config.setValue(Defs::CONF_WIN_AUTOHELP, automatic);
    config.endGroup();
    config.sync();
}

void DocChooserDialog::setOfflineHelp(bool yes)
{
    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        config.setValue(Defs::CONF_WIN_OFFLINEHELP, yes);
    config.endGroup();
    config.sync();
}

void DocChooserDialog::restoreAutoChooserState()
{
    QSettings config;

    // NOTE: not used
    bool autoChooseHelp = false;

    // read state
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    if (config.contains(Defs::CONF_WIN_AUTOHELP))
    {
        autoChooseHelp = config.value(Defs::CONF_WIN_AUTOHELP).toBool();
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
        offlineHelp = config.value(Defs::CONF_WIN_OFFLINEHELP).toBool();
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
        QString htmlHelpPath = QCoreApplication::applicationDirPath()
                + QStringLiteral("/docs/help/index.htm#EddyPro_Home.htm");
        qDebug() << QDesktopServices::openUrl(QUrl::fromLocalFile(htmlHelpPath));
    }
    setAutoChooser(true);
    done(0);
}
