/***************************************************************************
  smartfluxbar.cpp
  ----------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
#include "defs.h"
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
    auto pixmap = QPixmap(QStringLiteral(":/icons/smartflux-white"));
#if defined(Q_OS_MACOS)
    pixmap.setDevicePixelRatio(2.0);
#endif
    smartfluxLogo->setPixmap(pixmap);
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
                               "under the File menu or with ")
#if defined(Q_OS_MACOS)
                               + Defs::MAC_COMMAND_KEY + tr("+F."));
#else
                               + tr("Ctrl+F."));
#endif
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
                  "configuration mode (")
#if defined(Q_OS_MACOS)
                               + Defs::MAC_COMMAND_KEY + tr("+F to exit)."));
#else
                               + tr("Ctrl+F to exit)."));
#endif

    setVisible(false);
}

SmartFluxBar::~SmartFluxBar()
{
    // NOTE: qobject with parent, so it should not require deletion
    delete cpDialog_;
}

// NOTE: necessary to accept styling with css files
void SmartFluxBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // NOTE: useful for visual inspection/debugging purpose
//    if (layout())
//        paintLayout(&p, layout());
}

// NOTE: useful for visual inspection/debugging purpose
void SmartFluxBar::paintLayout(QPainter* painter, QLayoutItem* item)
{
    QLayout* layout = item->layout();
    if (layout)
    {
        for (int i = 0; i < layout->count(); ++i)
        {
            paintLayout(painter, layout->itemAt(i));
        }
    }
    painter->drawRect(item->geometry());
}

void SmartFluxBar::closeRequest()
{
    if (WidgetUtils::okToCloseSmartFlux(this))
    {
        emit showSmartfluxBarRequest(false);
    }
}

void SmartFluxBar::createPackage()
{
    // set run_fcc value
    ecProject_->setGeneralRunFcc(ecProject_->isEngineStep2Needed());

    if (ecProject_->generalFileName()
        == Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME)
    {
        emit saveRequest();
    }
    else
    {
        emit saveSilentlyRequest();
    }

    auto homePath = configState_->general.env;

    auto smfDir = homePath
                     + QLatin1Char('/')
                     + Defs::SMF_FILE_DIR;

    auto packageFilename = GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_PROJECT,
                                        Defs::CONF_PROJ_SMARTFLUX_FILENAME,
                                        QString()).toString();

    auto packageFilepath = QDir::fromNativeSeparators(
                GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_PROJECT,
                                        Defs::CONF_PROJ_SMARTFLUX_FILEPATH,
                                        QString()).toString());

    auto smartfluxFilename = packageFilename;

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

    auto smartfluxFile = packageFilepath
                                + QLatin1Char('/')
                                + smartfluxFilename;

    auto smfIniDir = smfDir
                     + QLatin1Char('/')
                     + Defs::INI_FILE_DIR;

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

    if (!mdFileList.isEmpty())
    {
        for (const auto &str : mdFileList)
        {
            FileUtils::chmod_644(smfDir + QLatin1Char('/') + str);
            if (!QFile::copy(smfDir + QLatin1Char('/') + str,
                             smfIniDir + QLatin1Char('/') + str))
            {
                qDebug() << "Unable to copy metadata file in smf/ini folder";
            }
        }
    }

    // psrtial cleaning (just files, not also subdirs)
    FileUtils::cleanDir(smfDir);

    // copy ancillary files in smf/ini
    QString saFile = ecProject_->spectraFile();

    if (!QFile::copy(ecProject_->spectraFile(),
                     smfIniDir
                     + QLatin1Char('/')
                     + saFile.mid(saFile.lastIndexOf(QLatin1Char('/')))))
    {
        qDebug() << "Unable to copy spectral assessment file in smf/ini folder";
    }

    QString pfFile = ecProject_->planarFitFile();

    if (!QFile::copy(ecProject_->planarFitFile(),
                     smfIniDir
                     + QLatin1Char('/')
                     + pfFile.mid(pfFile.lastIndexOf(QLatin1Char('/')))))
    {
        qDebug() << "Unable to copy planar fit file in smf/ini folder";
    }

    QString tlFile = ecProject_->timelagOptFile();

    if (!QFile::copy(ecProject_->timelagOptFile(),
                     smfIniDir
                     + QLatin1Char('/')
                     + tlFile.mid(tlFile.lastIndexOf(QLatin1Char('/')))))
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
            if (!WidgetUtils::okToOverwrite(this, smartfluxFile))
            {
                // cleanup before exit if not overwriting
                FileUtils::removeDirRecursively(smfIniDir);
                return;
            }
        }

        // compress ini dir, i.e. create package and show result
        cpDialog_->showResult(JlCompress::compressDir(smartfluxFile, smfDir, true),
                          smartfluxFilename);
#if 0
        Process zipper(this, QApplication::applicationDirPath());
        cpDialog_->showResult(zipper.zipProcessAddStart(smartfluxFile,
                                                        Defs::INI_FILE_DIR
                                                        + QLatin1Char('/'),
                                                        smfDir),
                              smartfluxFilename);
#endif
    }

    // on mac nested dialogs don't work well,
    // because cpDialog->close() does not hide the dialog as expected,
    // so it's preferable to delete it
    // TODO: use a std::unique_ptr
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    cpDialog_->close();
#elif defined(Q_OS_MACOS)
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
    if (!cpDialog_)
    {
        cpDialog_ = new CreatePackageDialog(ecProject_, configState_, this);
        connect(cpDialog_, &CreatePackageDialog::createPackageRequest,
                this, &SmartFluxBar::createPackage);
    }
}

void SmartFluxBar::showCreatePackageDialog()
{
    if (ecProject_->screenDataPath().isEmpty())
    {
        WidgetUtils::information(this,
                        tr("SMARTFlux package creation"),
                        tr("<p>Choose a Raw Data directory "
                         "from the Basic Settings page.</p>"),
                        QString());
        return;
    }

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    cpDialog_->refresh();
#elif defined(Q_OS_MACOS)
    makeCreatePackageDialog();
#endif

    cpDialog_->show();
    cpDialog_->raise();
    cpDialog_->activateWindow();
}
