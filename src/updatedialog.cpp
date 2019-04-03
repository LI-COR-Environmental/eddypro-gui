/***************************************************************************
  updatedialog.cpp
  ----------------
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
****************************************************************************/

#include "updatedialog.h"

#include <QDebug>
#include <QDesktopServices>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <QUrl>

#include "defs.h"
#include "downloadmanager.h"
#include "stringutils.h"
#include "widget_utils.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    updateManager(nullptr),
    isNewVersionAvailable_(false),
    downloadTimer_(nullptr)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Check for Updates"));
    WidgetUtils::removeContextHelpButton(this);

    auto groupTitle = new QLabel;
    groupTitle->setText(tr("Checking for newer versions."));

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));
    hrLabel->setMinimumWidth(400);

    msgLabel = new QLabel(tr("Retrieving information..."));
    msgLabel->setStyleSheet(QStringLiteral("QLabel {margin-bottom: 15px;}"));

    okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    yesButton = new QPushButton(tr("Yes"));
    yesButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    yesButton->setDefault(true);
    yesButton->setProperty("commonButton", true);

    noButton = new QPushButton(tr("No"));
    noButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    noButton->setDefault(true);
    noButton->setProperty("commonButton", true);

    auto dialogLayout = new QGridLayout(this);
    dialogLayout->addWidget(groupTitle, 0, 0, 1, -1);
    dialogLayout->addWidget(hrLabel, 1, 0, 1, -1);
    dialogLayout->addWidget(msgLabel, 2, 0, 1, -1);
    dialogLayout->addWidget(okButton, 3, 0, 1, 2, Qt::AlignCenter);
    dialogLayout->addWidget(yesButton, 3, 0, 1, 1, Qt::AlignCenter);
    dialogLayout->addWidget(noButton, 3, 1, 1, 1, Qt::AlignCenter);
    dialogLayout->setVerticalSpacing(10);
    dialogLayout->setContentsMargins(30, 30, 15, 30);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(dialogLayout);

    connect(okButton, &QPushButton::clicked,
            this, &UpdateDialog::close);
    connect(yesButton, &QPushButton::clicked,
            this, &UpdateDialog::showDownloadPage);
    connect(noButton, &QPushButton::clicked,
            this, &UpdateDialog::close);

    QTimer::singleShot(0, this, SLOT(initialize()));

    downloadTimer_ = new QTimer(this);
    downloadTimer_->setInterval(10000);
    downloadTimer_->setSingleShot(true);
    connect(downloadTimer_, &QTimer::timeout,
            this, &UpdateDialog::downloadTimeout);
}

void UpdateDialog::initialize()
{
    msgLabel->clear();
    msgLabel->setText(tr("<b>Retrieving information...</b>"));
    okButton->setVisible(true);
    yesButton->setVisible(false);
    noButton->setVisible(false);
}

void UpdateDialog::close()
{
    if (updateManager)
    {
        updateManager->abort();
    }

    if (isVisible())
    {
        hide();
    }

    downloadTimer_->stop();
}

void UpdateDialog::getNewVersion(const QString& version)
{
    msgLabel->setText(tr("<p><b>A newer version of %1 (version %2) is available from %3.<br />"
                         "Do you want to upgrade your copy?</b></p>"
                         "<p>If you have the <b>SMARTFlux<sup>&reg;</sup> System</b>, we also "
                         "recommend that you <br />"
                         "check for updates to the embedded SMARTFlux firmware.</p>"
                         "<p>%1 can automatically check for new and updated "
                         "versions using <br />its Software Update Notification feature.<br />"
                         "The new version does not overwrite previously installed versions."
                         "</p>").arg(Defs::APP_NAME, version, Defs::ORG_NAME));
    msgLabel->setOpenExternalLinks(true);
    okButton->setVisible(false);
    yesButton->setVisible(true);
    noButton->setVisible(true);
}

void UpdateDialog::noNewVersion()
{
    msgLabel->setText(tr("<b>No newer version of %1 is available at this time.</b>").arg(Defs::APP_NAME));
    okButton->setVisible(true);
    yesButton->setVisible(false);
    noButton->setVisible(false);
}

void UpdateDialog::noConnection()
{
    msgLabel->setText(tr("<b>No connection available or connection error.</b>"));
    okButton->setVisible(true);
    yesButton->setVisible(false);
    noButton->setVisible(false);
}

void UpdateDialog::downloadError()
{
    msgLabel->setText(tr("<b>Download error.</b>"));
    okButton->setVisible(true);
    yesButton->setVisible(false);
    noButton->setVisible(false);
}

void UpdateDialog::checkUpdate()
{
    if (!updateManager)
    {
        updateManager = new DownloadManager(this);

        connect(updateManager, SIGNAL(downloadComplete()),
                this, SLOT(useDownloadResults()));
    }
    QTimer::singleShot(0, updateManager, SLOT(execute()));

    downloadTimer_->start();
}

bool UpdateDialog::hasNewVersion()
{
    return isNewVersionAvailable_;
}

void UpdateDialog::showDownloadPage()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.licor.com/env/support/EddyPro/software.html")));
    close();
}

void UpdateDialog::downloadTimeout()
{
    noConnection();
}

void UpdateDialog::useDownloadResults()
{
    QByteArray versionNr = updateManager->getVersionNr();
    QString newVersion(QLatin1String(versionNr.trimmed().constData()));

    if (!newVersion.isEmpty())
    {
        if (StringUtils::isNewVersion(newVersion, Defs::APP_VERSION_STR))
        {
            isNewVersionAvailable_ = true;
            getNewVersion(newVersion);
        }
        else
        {
            isNewVersionAvailable_ = false;
            noNewVersion();
        }
    }
    else
    {
        isNewVersionAvailable_ = false;
        downloadError();
    }
    downloadTimer_->stop();
}
