/***************************************************************************
  smartfluxbar.cpp
  -------------------
  Copyright (C) 2013-2015, LI-COR Biosciences
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

#include "smartfluxbar.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScopedPointer>
#include <QStyleOption>
#include <QVariant>

#include "JlCompress.h"

#include "clicklabel.h"
#include "createpackagedialog.h"
#include "dbghelper.h"
#include "ecproject.h"
#include "fileutils.h"
#include "globalsettings.h"
#include "process.h"
#include "widget_utils.h"

SmartFluxBar::SmartFluxBar(EcProject* ecProject,
                           ConfigState* config,
                           QWidget* parent) :
    QWidget(parent),
    ecProject_(ecProject),
    configState_(config),
    cpDialog_(nullptr)
{
    auto smartfluxLogo = new QLabel;
    smartfluxLogo->setPixmap(QPixmap(QLatin1String(":/icons/smartflux-white")));
    smartfluxLogo->setProperty("smartfluxLogoWhite", true);

    auto smartfluxTitle = new QLabel;
    smartfluxTitle->setText(tr("Configuration"));
    smartfluxTitle->setProperty("smartfluxBarTitle", true);

    auto createButton = new QPushButton;
    createButton->setText(tr("Create Package"));
    createButton->setProperty("smartfluxButton", true);
    createButton->setToolTip(tr("Click to create the SMARTFlux configuration "
                                "file package when you are done configuring "
                                "EddyPro."));
    connect(createButton, &QPushButton::clicked,
            this, &SmartFluxBar::showCreatePackageDialog);

    auto closeButton = new ClickLabel;
    closeButton->setProperty("smartfluxBarClose", true);
    closeButton->setToolTip(tr("Click to exit SMARTFlux configuration mode. "
                               "You can enter SMARTFlux configuration mode "
                               "under the File menu or with Ctrl+F."));
    connect(closeButton, &ClickLabel::clicked,
            this, &SmartFluxBar::closeRequest);

    auto layout = new QHBoxLayout(this);
    layout->addWidget(smartfluxLogo, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->addWidget(smartfluxTitle, 0, Qt::AlignLeft | Qt::AlignBottom);
    layout->addStretch(1);
    layout->addWidget(createButton, 0, Qt::AlignLeft);
    layout->addSpacerItem(new QSpacerItem(15, 35));
    layout->addWidget(closeButton);
    layout->setSizeConstraint(QLayout::SetMaximumSize);
    layout->setContentsMargins(30, 0, 15, 0);
    setLayout(layout);

    makeCreatePackageDialog();

    setToolTip(tr("EddyPro is in SMARTFlux "
                  "configuration mode (Ctrl+F to exit)"));
    setVisible(false);
}

SmartFluxBar::~SmartFluxBar()
{
    DEBUG_FUNC_NAME

    // NOTE: qobject with parent, so it should not require deletion
    if (cpDialog_)
    {
        delete cpDialog_;
        qDebug() << "delete cpDialog";
    }
}

// NOTE: necessary to accept styling with css files
void SmartFluxBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

//    if (layout())
//        paintLayout(&p, layout());
}

// NOTE: useful for visual inspection/debugging purpose
void SmartFluxBar::paintLayout(QPainter* painter, QLayoutItem* item)
{
    QLayout* layout = item->layout();
    if (layout) {
        for (int i = 0; i < layout->count(); ++i)
            paintLayout(painter, layout->itemAt(i));
    }
    painter->drawRect(item->geometry());
}

void SmartFluxBar::closeRequest()
{
    if (WidgetUtils::okToQuestion(nullptr,
                tr("SmartFlux Configuration Exit"),
                tr("Do you want to leave the SmartFlux Configuration?")))
    {
        emit showSmartfluxBarRequest(false);
    }
}

void SmartFluxBar::createPackage()
{
    DEBUG_FUNC_NAME
    // set run_fcc value
    ecProject_->setGeneralRunFcc(ecProject_->isEngineStep2Needed());

    qDebug() << "project file name" << ecProject_->generalFileName();
    if (ecProject_->generalFileName()
        == QLatin1String("project-untitled-smartflux.eddypro"))
    {
        emit saveRequest();
    }
    else
    {
        emit saveSilentlyRequest();
    }

    QString homePath;
    homePath = configState_->general.env;

    QString smfDir = homePath
                     + QStringLiteral("/")
                     + Defs::SMF_FILE_DIR;

    QString packageFilename = GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_PROJECT,
                                        Defs::CONF_PROJ_SMARTFLUX_FILENAME,
                                        QString()).toString();

    QString packageFilepath = QDir::fromNativeSeparators(
                GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_PROJECT,
                                        Defs::CONF_PROJ_SMARTFLUX_FILEPATH,
                                        QString()).toString());

    QString smartfluxFilename = packageFilename;

    // append extension
    if (!smartfluxFilename.endsWith(QStringLiteral(".")
                                    + Defs::SMARTFLUX_FILE_EXT, Qt::CaseInsensitive))
    {
        smartfluxFilename += QLatin1String(".") + Defs::SMARTFLUX_FILE_EXT;
    }
    // force lower case
    smartfluxFilename.replace(smartfluxFilename.lastIndexOf(Defs::SMARTFLUX_FILE_EXT, -1, Qt::CaseInsensitive),
                                  Defs::SMARTFLUX_FILE_EXT.size(),
                                  Defs::SMARTFLUX_FILE_EXT);

    QString smartfluxFile = packageFilepath
                                + QStringLiteral("/")
                                + smartfluxFilename;

    QString smfIniDir = smfDir
                     + QStringLiteral("/")
                     + Defs::INI_FILE_DIR;
    qDebug() << "smf ini dir" << smfIniDir;

    // create dir smf/ini
    FileUtils::createDir(Defs::INI_FILE_DIR, smfDir);

    // copy current project from ini to smf/ini
    if (!FileUtils::projectFileForcedCopy(ecProject_->generalFileName(),
                                          smfIniDir))
    {
        qDebug() << "Unable to copy project file in smf/ini folder";
    }

    // copy metadata files in smf/ini
    QStringList mdFilters;
    mdFilters << QStringLiteral("*.") + Defs::METADATA_FILE_EXT;
    QDir mdDir(smfDir);
    mdDir.setNameFilters(mdFilters);
    mdDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    QStringList mdFileList = mdDir.entryList();
    qDebug() << "mdFileList" << mdFileList;

    if (!mdFileList.isEmpty())
    {
        foreach (const QString& str, mdFileList)
        {
            // chmod 644
            QFile::setPermissions(smfDir + QStringLiteral("/") + str, QFileDevice::ReadUser
                                                                      | QFileDevice::WriteUser
                                                                      | QFileDevice::ReadGroup
                                                                      | QFileDevice::ReadOther);
            if (!QFile::copy(smfDir + QStringLiteral("/") + str,
                             smfIniDir + QStringLiteral("/") + str))
            {
                qDebug() << "Unable to copy metadata file in smf/ini folder";
            }
        }
    }

    // NOTE: soft cleaning (just files, not also subdirs, to fix...) required by quazip
//    FileUtils::cleanDir(smfDir);

    // copy ancillary files in smf/ini
    QString saFile = ecProject_->spectraFile();
    qDebug() << "saFile" << ecProject_->spectraFile();
    if (!QFile::copy(ecProject_->spectraFile(),
                     smfIniDir
                     + QStringLiteral("/")
                     + saFile.mid(saFile.lastIndexOf(QStringLiteral("/")))))
    {
        qDebug() << smfIniDir
                    + QStringLiteral("/")
                    + saFile.mid(saFile.lastIndexOf(QStringLiteral("/")));
        qDebug() << "Unable to copy spectral assessment file in smf/ini folder";
    }

    QString pfFile = ecProject_->planarFitFile();
    qDebug() << "pfFile" << ecProject_->planarFitFile();
    if (!QFile::copy(ecProject_->planarFitFile(),
                     smfIniDir
                     + QStringLiteral("/")
                     + pfFile.mid(pfFile.lastIndexOf(QStringLiteral("/")))))
    {
        qDebug() << "Unable to copy planar fit file in smf/ini folder";
    }

    QString tlFile = ecProject_->timelagOptFile();
    qDebug() << "tlFile" << ecProject_->timelagOptFile();
    if (!QFile::copy(ecProject_->timelagOptFile(),
                     smfIniDir
                     + QStringLiteral("/")
                     + tlFile.mid(tlFile.lastIndexOf(QStringLiteral("/")))))
    {
        qDebug() << "Unable to copy time lag file in smf/ini folder";
    }

    if (FileUtils::isDirEmpty(smfIniDir))
    {
        cpDialog_->showResult(false, smartfluxFilename);
    }
    else
    {
        // overwrite confirmation request
        if (QFile::exists(smartfluxFile))
        {
            if (!WidgetUtils::okToOverwrite(smartfluxFile))
            {
                // cleanup
                FileUtils::removeDirRecursively(smfIniDir);
                return;
            }
        }

        // compress ini dir, i.e. create package and show result
#if defined(Q_OS_MAC)
        // NOTE: verify that quazip compression does work with smartflux embedded
        cpDialog_->showResult(JlCompress::compressDir(smartfluxFile, smfDir, false),
                          smartfluxFilename);
#else
        Process zipper(this, qApp->applicationDirPath());
        cpDialog_->showResult(zipper.zipProcessAddStart(smartfluxFile,
                                                        Defs::INI_FILE_DIR
                                                        + QStringLiteral("/"),
                                                        smfDir),
                              smartfluxFilename);
#endif
}

    qDebug() << "cpDialog_->close()";
    // on mac nested dialogs don't work well,
    // making cpDialog->close() not hiding as expected,
    // so it's preferable to delete the dialog
#if defined(Q_OS_WIN)
    cpDialog_->close();
#elif defined(Q_OS_MAC)
    if (cpDialog_)
    {
        delete cpDialog_;
        cpDialog_ = nullptr;
    }
#endif

    // cleanup
    FileUtils::removeDirRecursively(smfIniDir);
}

void SmartFluxBar::makeCreatePackageDialog()
{
    DEBUG_FUNC_NAME

    if (!cpDialog_)
    {
        cpDialog_ = new CreatePackageDialog(ecProject_, configState_, this);
        connect(cpDialog_, &CreatePackageDialog::createPackageRequest,
                this, &SmartFluxBar::createPackage);
    }
}

void SmartFluxBar::showCreatePackageDialog()
{
    DEBUG_FUNC_NAME

    if (ecProject_->screenDataPath().isEmpty())
    {
        WidgetUtils::information(this,
                      tr("SMARTFlux package creation"),
                      tr("<p>Choose a Raw Data directory "
                         "from the Basic Settings page.</p>"),
                      tr(""));
        return;
    }

#if defined(Q_OS_WIN)
    cpDialog_->refresh();
#elif defined(Q_OS_MAC)
    makeCreatePackageDialog();
#endif

    cpDialog_->show();
    cpDialog_->raise();
    cpDialog_->activateWindow();
}
