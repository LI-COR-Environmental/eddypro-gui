/***************************************************************************
  docchooser.cpp
  --------------
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

#include "docchooser.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QVBoxLayout>

#include "defs.h"
#include "globalsettings.h"
#include "widget_utils.h"

DocChooserDialog::DocChooserDialog(QUrl url, QWidget *parent) :
    QDialog(parent),
    url_(std::move(url))
{
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

    // read state
    config.beginGroup(Defs::CONFGROUP_WINDOW);
    if (!config.contains(Defs::CONF_WIN_AUTOHELP))
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
    if (onlineHelpRadio->isChecked())
    {
        // browse online help version
        QDesktopServices::openUrl(url_);
    }
    else
    {
        // open local help
        auto htmlHelpPath = QApplication::applicationDirPath()
                + QStringLiteral("/docs/help/eddypro/home.html");
    }

    // set the variable to true to indicate that the dialog ran once
    // this is the only reason to date to use it.
    // NOTE: refactor to simplify the unnecessary code
    setAutoChooser(true);

    done(0);
}
