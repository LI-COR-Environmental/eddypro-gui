/***************************************************************************
  updatedialog.cpp
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
****************************************************************************/

#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QUrl>
#include <QTimer>
#include <QDesktopServices>

#include "ftpmanager.h"
#include "stringutils.h"
#include "defs.h"
#include "dbghelper.h"
#include "updatedialog.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ftp(0),
    isNewVersionAvailable_(false),
    ftpTimer_(0)
{
    setWindowTitle(tr("Check for Updates"));
    Qt::WindowFlags winFflags = windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    setWindowFlags(winFflags);
    setWindowModality(Qt::WindowModal);

    QLabel *groupTitle = new QLabel;
    groupTitle->setText(tr("Checking for newer versions."));

    QLabel *hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));
    hrLabel->setMinimumWidth(400);

    msgLabel = new QLabel(tr("Retrieving information..."));
    msgLabel->setStyleSheet(QStringLiteral("QLabel {margin-bottom: 15px;}"));

    okButton = new QPushButton(tr("Ok"));
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okButton->setDefault(true);
    okButton->setProperty("commonButton", true);

    yesButton = new QPushButton(tr("Yes"));
    yesButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    yesButton->setDefault(true);
    yesButton->setProperty("commonButton", true);

    noButton = new QPushButton(tr("No"));
    noButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    noButton->setDefault(true);
    noButton->setProperty("commonButton", true);

    QGridLayout *dialogLayout = new QGridLayout(this);
    dialogLayout->addWidget(groupTitle, 0, 0, 1, -1);
    dialogLayout->addWidget(hrLabel, 1, 0, 1, -1);
    dialogLayout->addWidget(msgLabel, 2, 0, 1, -1);
    dialogLayout->addWidget(okButton, 3, 0, 1, 2, Qt::AlignCenter);
    dialogLayout->addWidget(yesButton, 3, 0, 1, 1, Qt::AlignCenter);
    dialogLayout->addWidget(noButton, 3, 1, 1, 1, Qt::AlignCenter);
    dialogLayout->setVerticalSpacing(10);
    dialogLayout->setContentsMargins(30, 30, 30, 30);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(dialogLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(yesButton, SIGNAL(clicked()), this, SLOT(showDownloadPage()));
    connect(noButton, SIGNAL(clicked()), this, SLOT(close()));

    QTimer::singleShot(0, this, SLOT(initialize()));

    ftpTimer_ = new QTimer(this);
    ftpTimer_->setInterval(10000);
    ftpTimer_->setSingleShot(true);
    connect(ftpTimer_, SIGNAL(timeout()), this, SLOT(ftpTimeout()));
}

UpdateDialog::~UpdateDialog()
{
    DEBUG_FUNC_NAME

    if (ftp)
    {
        qDebug() << "deleting ftp...";
    }
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
    DEBUG_FUNC_NAME
    if (ftp)
        ftp->abort();

    if (isVisible())
        hide();

    ftpTimer_->stop();
}

void UpdateDialog::getNewVersion(const QString& version)
{
    msgLabel->setText(tr("<b>A newer version of %1 (version %2) is available from %3.<br />"
                         "Do you want to upgrade your copy?</b>"
                         "<p>%1 can automatically check for new and updated "
                         "versions using its Software Update Notification feature.<br />"
                         "The new version does not overwrite previously installed versions."
                         "</p>").arg(Defs::APP_NAME).arg(version).arg(Defs::ORG_NAME));
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

void UpdateDialog::connectionError()
{
    msgLabel->setText(tr("<b>Connection error.</b>"));
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
    DEBUG_FUNC_NAME
    if (!ftp)
    {
        qDebug() << "!ftp";

        ftp = new FtpManager(this);

        connect(ftp, SIGNAL(getFinished()), this, SLOT(useFtpResults()));
    }
    QTimer::singleShot(0, ftp, SLOT(execute()));

    ftpTimer_->start();
    qDebug() << "ftpTimer_->start()";
}

bool UpdateDialog::hasNewVersion()
{
    return isNewVersionAvailable_;
}

void UpdateDialog::showDownloadPage()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://infoenv.licor.com/EddyProDownloads.html")));
    close();
}

void UpdateDialog::ftpTimeout()
{
    DEBUG_FUNC_NAME

    qDebug() << "ftp" << ftp;

    noConnection();
}

void UpdateDialog::useFtpResults()
{
    QByteArray versionNr = ftp->getVersionNr();
    qDebug() << "versionNr" << versionNr.trimmed().constData();

    QString newVersion(QLatin1String(versionNr.trimmed().constData()));

    if (!newVersion.isEmpty())
    {
        if (StringUtils::isNewVersion(newVersion, Defs::APP_VERSION_STR))
        {
            qDebug() << "NEW VERSION";
            isNewVersionAvailable_ = true;
            getNewVersion(newVersion);
        }
        else
        {
            qDebug() << "NO NEW VERSION";
            isNewVersionAvailable_ = false;
            noNewVersion();
        }
    }
    else
    {
        qDebug() << "EMPTY VERSION";
        isNewVersionAvailable_ = false;
        downloadError();
    }
    ftpTimer_->stop();
}
