/***************************************************************************
  mainwindow.cpp
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
#include <QSettings>
#include <QTimer>
#include <QUrl>
#include <QTextDocumentFragment>
#include <QNetworkProxyFactory>
#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolBar>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QDesktopServices>
#include <QScrollBar>
#include <QWhatsThis>
#include <QPushButton>
#include <QErrorMessage>
#include <QGridLayout>

#include "infomessage.h"
#include "projectpage.h"
#include "basicsettingspage.h"
#include "tooltipfilter.h"
#include "wheeleventfilter.h"
#include "dbghelper.h"
#include "alia.h"
#include "stringutils.h"
#include "clicklabel.h"
#include "runpage.h"
#include "dlproject.h"
#include "ecproject.h"
#include "mainwidget.h"
#include "welcomepage.h"
#include "clocklabel.h"
#include "updatedialog.h"
#include "aboutdialog.h"
#include "mymenu.h"
#include "fileutils.h"
#include "advancedsettingspage.h"
#include "advsettingscontainer.h"
#include "advprocessingoptions.h"
#include "planarfitsettingsdialog.h"
#include "timelagsettingsdialog.h"
#include "binarysettingsdialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(const QString& filename, const QString& appEnvPath, QWidget *parent) :
    QMainWindow(parent),
    aboutDialog(0),
    mainWidget_(0),
    configState_(ConfigState()),
    dlProject_(0),
    ecProject_(0),
    currEcProjectFilename_(filename),
    appEnvPath_(appEnvPath),
    notificationTimer_(0),
    newFlag_(true),
    modifiedFlag_(false),
    openingFlag_(false),
    engineResumableFlag_(false),
    metadataReadFlag_(false),
    neededEngineStep2_(false),
    guidedModeOn_(true),
    basicSettingsPageAvailable_(false),
    runExpressAvailable_(false),
    runAdvancedAvailable_(false),
    newClicked_(false),
    previousPage_(Defs::CurrPageWelcome),
    currentPage_(Defs::CurrPageWelcome),
    currentStatus_(Defs::CurrStatusReady),
    currentRunStatus_(Defs::CurrRunStatusExp),
    expressClicked_(false),
    advancedClicked_(false),
    retrieverClicked_(false),
    engineProcess_(0),
    updateDialog(0),
    argFilename_(false)
{
    DEBUG_FUNC_NAME

    readSettings();

    qDebug() << "appEnvPath_" << appEnvPath_;
    saveEnvSettings(appEnvPath_);

    // setup tooltip filter
    tooltipFilter_ = new TooltipFilter(configState_.window.tooltips, this);
    qApp->installEventFilter(tooltipFilter_);

    WheelEventFilter* wheelFilter_ = new WheelEventFilter(this);
    qApp->installEventFilter(wheelFilter_);

    // create metadata file
    dlProject_ = new DlProject(this, configState_.project);

    // create project file
    ecProject_ = new EcProject(this, configState_.project);

    // set main window components
    setWindowTitle(Defs::APP_NAME);

#if defined(Q_OS_WIN)
    // inserted fake (inexistent) icon to prevent icon in the menu bar
    setWindowIcon(QIcon(QStringLiteral(":/icons/img/app_ico.png")));
#elif defined(Q_OS_MAC)
    setWindowIcon(QIcon(QStringLiteral(":/icons/img/app.icns")));
#endif

    setDockOptions(QMainWindow::ForceTabbedDocks);
    createInfoDockWin();
    createConsoleDockWin();

    installEventFilter(this);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    // if filename is passed as argument
    argFilename_ =
        (!currentProjectFile().isEmpty()
        &&
        (currentProjectFile() != Defs::DEFAULT_PROJECT_FILENAME));

    // load the most recent project file name if exists
    if (!argFilename_
       && configState_.general.loadlastproject
       && (!configState_.general.recentfiles.isEmpty()))
    {
        QString firstFile = configState_.general.recentfiles.first();
        if (QFile::exists(firstFile))
        {
            currEcProjectFilename_ = firstFile;
        }
    }

    // create a new ec project file
    ecProject_->newEcProject(configState_.project);

    // set central widget
    mainWidget_ = new MainWidget(this, dlProject_, ecProject_, &configState_);
    setCentralWidget(mainWidget_);

    //
    setMinimumSize(800, 600);

    connectActions();

    createEngineProcess();

    // connections
    connect(ecProject_, SIGNAL(ecProjectNew()),
            this, SLOT(updateInfoMessages()));
    connect(ecProject_, SIGNAL(ecProjectModified()),
            this, SLOT(setEcProjectModified()));
    connect(ecProject_, SIGNAL(ecProjectChanged()),
            this, SLOT(updateInfoMessages()));
    connect(ecProject_, SIGNAL(updateInfo()),
            this, SLOT(updateInfoMessages()));
    connect(ecProject_, SIGNAL(updateInfo()),
            this, SLOT(updateRunButtonsAvailability()));
    connect(fileMenuOpenRecent, SIGNAL(aboutToShow()),
            this, SLOT(recentMenuShow()));

    connect(dlProject_, SIGNAL(projectModified()),
            this, SLOT(updateInfoMessages()));
    connect(dlProject_, SIGNAL(projectChanged()),
            this, SLOT(updateInfoMessages()));

    // from MainWidget
    connect(mainWidget_->runPage(), SIGNAL(updateConsoleRequest(QByteArray&)),
            this, SLOT(updateConsole(QByteArray&)));
    connect(mainWidget_->runPage(), SIGNAL(runExpRequest()),
            this, SLOT(getRunExpress()));
    connect(mainWidget_->runPage(), SIGNAL(runAdvRequest()),
            this, SLOT(getRunAdvanced()));
    connect(mainWidget_->runPage(), SIGNAL(runRetRequest()),
            this, SLOT(getRunRetriever()));
    connect(mainWidget_, SIGNAL(showSmartfluxBarRequest(bool)),
            this, SLOT(setSmartfluxMode(bool)));
    connect(mainWidget_, SIGNAL(saveSilentlyRequest()),
            this, SLOT(saveSilently()));
    connect(mainWidget_, SIGNAL(saveRequest()),
            this, SLOT(fileSave()));

    qDebug() << "dialogs connections"
             << mainWidget_->advancedPage()->advancedSettingPages()->processingOptions();

    // dialogs connections
    connect(mainWidget_->projectPage(),
            SIGNAL(connectBinarySettingsRequest()),
            this, SLOT(connectBinarySettingsDialog()));

    // from BasicSettingsPage
    connect(mainWidget_, SIGNAL(updateMetadataReadResult(bool)),
            this, SLOT(setMetadataRead(bool)));

    connect(this, SIGNAL(recentUpdated()),
            mainWidget_, SIGNAL(recentUpdated()));

    connect(mainWidget_, SIGNAL(openProjectRequest(QString)),
            this, SLOT(fileOpenRequest(QString)));
    connect(mainWidget_, SIGNAL(newProjectRequest()),
            this, SLOT(fileNewRequest()));
    connect(mainWidget_, SIGNAL(checkUpdatesRequest()),
            this, SLOT(showUpdateDialog()));

    connectPlanarFitDialog();
    connectTimeLagDialog();

    // restore window state
    QTimer::singleShot(0, this, SLOT(restorePreviousStatus()));

    QTimer::singleShot(0, this, SLOT(initialize()));

    QTimer::singleShot(0, this, SLOT(checkInternetConnection()));

    QTimer::singleShot(0, this, SLOT(showAutoUpdateDialog()));

    // notify every 24 hrs from the last start
    notificationTimer_ = new QTimer(this);
    connect(notificationTimer_, SIGNAL(timeout()),
            this, SLOT(showAutoUpdateDialog()));
    notificationTimer_->start(1000 * 60 * 60 * 24);

    // NOTE: for testing only
//    notificationTimer_->start(5000);
}

MainWindow::~MainWindow()
{
    DEBUG_FUNC_NAME
    qDebug() << Q_FUNC_INFO;

    if (notificationTimer_)
    {
        qDebug() << "delete notificationTimer_";
        delete notificationTimer_;
    }

    if (aboutDialog)
    {
        qDebug() << "delete aboutDialog";
        delete aboutDialog;
    }

    if (engineProcess_->isRunning())
    {
        qDebug() << "stop engineProcess_";
        engineProcess_->processStop();
    }

    if (engineProcess_)
    {
        qDebug() << "delete engineProcess_";
        delete engineProcess_;
    }
}

void MainWindow::initialize()
{
    DEBUG_FUNC_NAME
    if (guidedModeOn_)
    {
        runExpressAction->setEnabled(false);
        runAdvancedAction->setEnabled(false);
        runRetrieverAction->setEnabled(false);
        stopAction->setEnabled(false);

        viewWelcomeAction->setCheckable(true);
        viewProjectCreationAction->setCheckable(true);
        viewBasicSettingsAction->setCheckable(false);
        viewAdvancedAction->setCheckable(false);
        viewRunPageAction->setCheckable(true);

        // must be after restorePreviousStatus()
        updatePage(Defs::CurrPageWelcome);
        showInfoMessages_1();

        QNetworkProxyFactory::setUseSystemConfiguration(true);

        // open filename argument
        if (argFilename_)
        {
            qDebug() << "argFilename_" << currentProjectFile();
            fileOpen(currentProjectFile());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DEBUG_FUNC_NAME

    writeSettings();

    int continueAction = continueBeforeClose();

    // cancel the operation
    if (continueAction == QMessageBox::Cancel)
    {
        event->ignore();
        return;
    }

    // continue the operation
    // NOTE: hack prevent non interactive changes
    disconnect(toggleInfoOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewInfoOutput(bool)));
    disconnect(toggleTooltipOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewTooltipOutput(bool)));
    disconnect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewConsoleOutput(bool)));

    event->accept();
}

void MainWindow::setCurrentProjectFile(const QString& fileName, bool modified)
{
    DEBUG_FUNC_NAME

    if (fileName.isEmpty())
    {
        if (configState_.project.smartfluxMode)
        {
            currEcProjectFilename_ = Defs::DEFAULT_PROJECT_FILENAME;
            // add smartflux suffix
            const QString epExt = QStringLiteral(".") + Defs::PROJECT_FILE_EXT;
            currEcProjectFilename_.insert(currEcProjectFilename_.indexOf(epExt), Defs::DEFAULT_SMARTFLUX_SUFFIX);
        }
        else
        {
            currEcProjectFilename_ = Defs::DEFAULT_PROJECT_FILENAME;
        }
    }
    else
    {
        currEcProjectFilename_ = fileName;
    }

    setFileCaption(currentProjectFile());
    updateStatusBar();

    // if the project file version was just updated, trigger a modified status
    if (modified)
    {
        qDebug() << "project modified!";
        ecProject_->setGeneralFileName(QDir::fromNativeSeparators(currentProjectFile()));
        return;
    }

    ecProject_->blockSignals(true);
    ecProject_->setGeneralFileName(QDir::fromNativeSeparators(currentProjectFile()));
    ecProject_->blockSignals(false);
    modifiedFlag_ = false;
    ecProject_->setModified(false);
}

const QString& MainWindow::currentProjectFile() const
{
    return currEcProjectFilename_;
}

// FIXME: QWidget::setWindowModified: The window title does not contain a '[*]' placeholder
// this happens sometimes
void MainWindow::setFileCaption(const QString& filename, bool clearStar)
{
    DEBUG_FUNC_NAME

    qDebug() << "clear" << clearStar;

    QString shownName;
    if (filename.isEmpty())
    {
        shownName = Defs::DEFAULT_PROJECT_FILENAME;
    }
    else
    {
        shownName = QFileInfo(filename).fileName();
    }

    setWindowTitle(tr("%1 - [%2[*]]").arg(Defs::APP_NAME).arg(shownName));

    if (clearStar)
    {
        setWindowModified(false); // clear the star from the window title
    }
}

void MainWindow::fileNew()
{
    DEBUG_FUNC_NAME

    int continueAction = QMessageBox::Yes;

    if (!configState_.project.smartfluxMode)
    {
        continueAction = continueBeforeClose();
    }

    // cancel the operation
    if (continueAction == QMessageBox::Cancel)
    {
        return;
    }

    // continue the operation
    newFile();
    if (configState_.project.smartfluxMode)
    {
        changePage(Defs::CurrPageBasicSettings, false);
        mainWidget_->updateSmartfluxBarStatus();
    }
    else
    {
        changePage(Defs::CurrPageProjectCreation, false);
    }
    showStatusTip(tr("New project created"));
    updateInfoDock(false);
    updateMenuActionStatus(currentPage());
    newClicked_ = true;
}

void MainWindow::fileOpen(const QString &fileName)
{
    DEBUG_FUNC_NAME

    qDebug() << "fileName" << fileName;
    QString fileStr;

    int continueAction = continueBeforeClose();

    // cancel the operation
    if (continueAction == QMessageBox::Cancel)
    {
        return;
    }

    // continue the operation
    showStatusTip(tr("Opening..."));

    FileUtils::cleanSmfDir(configState_.general.env);

    // action triggered by user interaction
    if (fileName.isEmpty())
    {
        QString searchPath = QDir::homePath();
        if (!configState_.window.last_data_path.isEmpty()
            && FileUtils::existsPath(configState_.window.last_data_path))
        {
            searchPath = configState_.window.last_data_path;
        }
        fileStr = QFileDialog::getOpenFileName(this,
                        tr("Select an %1 Project File").arg(Defs::APP_NAME),
                        searchPath,
                        tr("%1 Project Files (*.%2);;All Files (*.*)").arg(Defs::APP_NAME).arg(Defs::PROJECT_FILE_EXT),
                        0
                        // , QFileDialog::DontUseNativeDialog
                        );
    }
    // programmatically
    else
    {
        fileStr = fileName;
    }

    // NOTE: redundant check?
    if (!fileStr.isEmpty())
    {
        QFileInfo projectDir(fileStr);
        QString projectPath = projectDir.canonicalPath();
        configState_.window.last_data_path = projectPath;
        Alia::updateLastDatapath(projectPath);
    }

    if (openFile(fileStr))
    {
        showStatusTip(tr("Project loaded"));
        consoleOutput->clear();

        if (currentPage() != Defs::CurrPageProjectCreation)
        {
            if (configState_.project.smartfluxMode)
            {
                if (currentPage() == Defs::CurrPageWelcome)
                {
                    changePage(Defs::CurrPageBasicSettings);
                }
                mainWidget_->updateSmartfluxBarStatus();
            }
            else
            {
                changePage(Defs::CurrPageProjectCreation);
            }
        }
        updateInfoDock(true);
    }
    else
    {
        showStatusTip(tr("Project loading failed"));
    }
    openingFlag_ = false;
}

//
bool MainWindow::openFile(const QString& filename)
{
    DEBUG_FUNC_NAME

    if (!openingFlag_)
    {
        openingFlag_ = true;
        if (!filename.isEmpty())
        {
            if (QFile::exists(filename))
            {
                if (ecProject_->nativeFormat(filename))
                {
                    bool modified = false;
                    if (ecProject_->loadEcProject(filename, true, &modified))
                    {
                        qDebug() << "window title 1:" << windowTitle();
                        qDebug() << "modified:" << modified;
                        setCurrentProjectFile(filename, modified);
                        qDebug() << "window title 2:" << windowTitle();
                        newFlag_ = false;
                        addRecent(currentProjectFile());
                        saveAction->setEnabled(false);

                        if (modified)
                        {
                            // load was successful
                            QMessageBox::warning(this,
                                                 tr("Load Information"),
                                                 tr("The project was successfully imported and updated.<br />"
                                                    "Save the project to complete the update."));
                        }
                        return true;
                    }
                    else
                    {
                        // load was unsuccessful
                        QMessageBox::warning(this,
                                             tr("Load Error"),
                                             tr("Unable to load the project."));

                        // close the current open project to prevent partial loading
                        // of ec project settings (currently there is no roll-back
                        // of the loadEcProject() function)
                        fileClose();
                        return false;
                    }
                }
                // file not in native format
                else
                {
                    return false;
                }
            }
            // file does not exist
            else
            {
                return false;
            }
        }
    }
    // empty file name
    return false;
}

void MainWindow::fileRecent()
{
    DEBUG_FUNC_NAME

    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) return;

    QString fname = action->text();

    qDebug() << configState_.project.smartfluxMode;

    if (configState_.project.smartfluxMode)
    {
        loadSmartfluxProjectCopy(fname);
    }
    else
    {
        int continueAction = continueBeforeClose();
        // cancel the operation
        if (continueAction == QMessageBox::Cancel)
        {
            return;
        }

        // continue the operation
        if (openFile(fname))
        {
            showStatusTip(tr("Recent project loaded"));
            consoleOutput->clear();

            if (currentPage() != Defs::CurrPageProjectCreation)
            {
                changePage(Defs::CurrPageProjectCreation);
            }
            updateInfoDock(true);
            if (configState_.project.smartfluxMode)
            {
                mainWidget_->updateSmartfluxBarStatus();
            }
        }
        else
        {
            showStatusTip(tr("Recent project loading failed"));
        }
        openingFlag_ = false;
    }
}

// Save a project
void MainWindow::fileSave()
{
    DEBUG_FUNC_NAME

    if (newFlag_)
    {
        fileSaveAs();
    }
    else
    {
        noticeAboutChangesDuringRun();

        if (saveFile(currentProjectFile()))
        {
            // successful in saving file
            newFlag_ = false;
            saveAction->setEnabled(false);
            setCurrentProjectFile(currentProjectFile());
            showStatusTip(tr("Project saved"));
        }
        else
        {
            // error in saving file
            QMessageBox::warning(this,
                                 tr("Save Error"),
                                 tr("%1 was unable to save %2")
                                 .arg(Defs::APP_NAME).arg(currentProjectFile()));
            showStatusTip(tr("Error in saving project"));
        }
    }
}

void MainWindow::fileSaveAs(const QString& fileName)
{
    DEBUG_FUNC_NAME

    noticeAboutChangesDuringRun();

    QString searchPath = QDir::homePath();
    if (!configState_.window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_.window.last_data_path))
    {
        searchPath = configState_.window.last_data_path;
    }

    QString filenameHint;
    if ((ecProject_->generalFileName() == Defs::DEFAULT_PROJECT_FILENAME)
        && (!ecProject_->generalTitle().isEmpty()))
    {
        qDebug() << "1";
        filenameHint = searchPath + QLatin1Char('/') + ecProject_->generalTitle();
    }
    else if ((ecProject_->generalFileName() == Defs::DEFAULT_PROJECT_FILENAME)
        && (ecProject_->generalTitle().isEmpty()))
    {
        qDebug() << "2";
        filenameHint = searchPath;
    }
    else if (FileUtils::existsPath(QFileInfo(ecProject_->generalFileName()).canonicalPath()))
    {
        qDebug() << "3" << QFileInfo(ecProject_->generalFileName()).canonicalPath();
        filenameHint = ecProject_->generalFileName();
    }
    else
    {
        qDebug() << "4";
        filenameHint = searchPath;
    }
    qDebug() << "filenameHint" << ecProject_->generalFileName()
             << searchPath
             << ecProject_->generalTitle()
             << filenameHint;

    showStatusTip(tr("Saving project..."));

    QString fileToSave;
    if (fileName.isEmpty())
    {
        fileToSave = QFileDialog::getSaveFileName(this,
                tr("Save the Project File Name As..."),
                filenameHint,
                tr("%1 Project Files (*.%2)").arg(Defs::APP_NAME).arg(Defs::PROJECT_FILE_EXT),
                0
                // see after
                                                 ,
                QFileDialog::DontConfirmOverwrite
                                                 );
    }
    else
    {
        fileToSave = fileName;
    }

    // we got a filename
    if (!fileToSave.isEmpty())
    {
        // add suffix if there is none
        QString epExt = QStringLiteral(".") + Defs::PROJECT_FILE_EXT;
        if (fileToSave.indexOf(epExt) == -1)
        {
            fileToSave += epExt;
        }

        qDebug() << "fname" << fileToSave;

// see before, alternative if DontConfirmOverwrite
        // overwrite?
        if (QFile::exists(fileToSave))
        {
            switch (Alia::queryOverwrite(fileToSave))
            {
                case QMessageBox::Yes:
                    break;
                case QMessageBox::Cancel:
                    showStatusTip(tr("Canceled..."));
                    return;
            }
        }

        if (saveFile(fileToSave))
        {
            // successfully saved
            newFlag_ = false;
            saveAction->setEnabled(false);
            setCurrentProjectFile(fileToSave);
            showStatusTip(tr("Project saved"));
        }
        else
        {
            // error in saving
            QMessageBox::warning(this,
                                 tr("Save Error"),
                                 tr("Unable to save to %1").arg(QFileInfo(fileToSave).fileName()));
            showStatusTip(tr("Error in saving project"));
        }
    }
    else
    {
        // no file chosen
        showStatusTip(tr("Saving aborted"));
    }
}

void MainWindow::fileClose()
{
    DEBUG_FUNC_NAME

    int continueAction = continueBeforeClose();

    // cancel the operation
    if (continueAction == QMessageBox::Cancel)
    {
        return;
    }

    newFile();
    newClicked_ = false;
    changePage(Defs::CurrPageWelcome, false);
    showStatusTip(tr("Project closed"));
    updateInfoDock(false);
    updateRunButtonsAvailability();
    FileUtils::cleanSmfDir(configState_.general.env);
}

void MainWindow::newFile()
{
    DEBUG_FUNC_NAME

    // create a new file
    ecProject_->newEcProject(configState_.project);
    newFlag_ = true;
    saveAction->setEnabled(false);
    setCurrentProjectFile(QString());
    runExpressAvailable_ = false;
    runAdvancedAvailable_ = false;
}

void MainWindow::noticeAboutChangesDuringRun()
{
    if (currentStatus() == Defs::CurrStatusRun)
    {
        QErrorMessage* changesMsgDialog = new QErrorMessage(this);
        changesMsgDialog->setWindowTitle(Defs::APP_NAME);
        changesMsgDialog->showMessage(tr("The changes will take effect during next runs."));
    }
}

int MainWindow::continueBeforeClose()
{
    DEBUG_FUNC_NAME

    if (currentStatus() != Defs::CurrStatusReady)
    {
        int ret = QMessageBox::question(this,
                      tr("Stop EddyPro"),
                      tr("EddyPro is processing.\n"
                      "Do you want to stop the computations before proceeding?"),
                      QMessageBox::Yes | QMessageBox::Cancel,
                      QMessageBox::Cancel);
        // engine needs to be stopped
        switch (ret)
        {
            case QMessageBox::Yes:
                stopEngineProcess();
                break;
            case QMessageBox::Cancel:
                showStatusTip(tr("Canceled..."));
                return QMessageBox::Cancel;
        }
    }

    if (modifiedFlag_)
    {
        int ret = QMessageBox::question(this,
                      tr("Save Project"),
                      tr("Current project has been modified.\n"
                      "Do you want to save your changes?"),
                      QMessageBox::Save | QMessageBox::No | QMessageBox::Cancel,
                      QMessageBox::Save);
        // file needs to be saved
        switch (ret)
        {
            case QMessageBox::Save:
                fileSave();
                break;
            case QMessageBox::No:
                return QMessageBox::No;
            case QMessageBox::Cancel:
                showStatusTip(tr("Canceled..."));
                return QMessageBox::Cancel;
        }
    }
    return QMessageBox::Yes;
}

bool MainWindow::saveFile(const QString& fileName)
{
    DEBUG_FUNC_NAME

    bool status = false;

    qDebug() << "ecProject_" << ecProject_;
    status = ecProject_->saveEcProject(fileName);
    qDebug() << "status" << status;

    if (status)
    {
        if (!FileUtils::projectFileForcedCopy(fileName,
                                              appEnvPath_
                                              + QStringLiteral("/")
                                              + Defs::INI_FILE_DIR
                                              ))
        {
            qDebug() << "Unable to create default proc ini file";
        }

        // new
        currEcProjectFilename_ = fileName;
        addRecent(currentProjectFile());
    }
    return status;
}

void MainWindow::about()
{
    DEBUG_FUNC_NAME

    if (!aboutDialog)
    {
        qDebug() << "create dialog";
        aboutDialog = new AboutDialog(this);
    }

    aboutDialog->show();
    aboutDialog->raise();
    aboutDialog->activateWindow();
}

void MainWindow::setEcProjectModified()
{
    DEBUG_FUNC_NAME

    saveAction->setEnabled(true);
    setWindowModified(true);
    modifiedFlag_ = true;
}

void MainWindow::updateInfoMessages()
{
    DEBUG_FUNC_NAME

    if (currentPage() == Defs::CurrPageProjectCreation)
    {
        showInfoMessages_1();
    }
    else if (currentPage() == Defs::CurrPageBasicSettings)
    {
        showInfoMessages_2();
    }
    else if (currentPage() == Defs::CurrPageAdvancedSettings)
    {
        showInfoMessages_3();
    }
}

void MainWindow::createActions()
{
    newAction = new QAction(this);
    newAction->setText(tr("&New\nProject"));
    newAction->setToolTip(tr("<b>New Project:</b> Start a new project."));
    newAction->setIcon(QIcon(QStringLiteral(":/icons/new")));
    newAction->setShortcut(tr("Ctrl+N"));

    openAction = new QAction(this);
    openAction->setText(tr("&Open\nProject..."));
    openAction->setToolTip(tr("<b>Open Project:</b> Open an existing project."));
    openAction->setIcon(QIcon(QStringLiteral(":/icons/open")));
    openAction->setShortcut(tr("Ctrl+O"));

    closeAction = new QAction(this);
    closeAction->setText(tr("&Close"));
    closeAction->setToolTip(tr("<b>Close Project:</b> Close the current project."));
    closeAction->setIcon(QIcon(QStringLiteral(":/icons/close")));
    closeAction->setShortcut(tr("Ctrl+W"));
    closeAction->setShortcutContext(Qt::ApplicationShortcut);

    recentOpenAction = new QAction(this);
    recentOpenAction->setText(tr("Open &Recent..."));
    recentOpenAction->setToolTip(tr("<b>Open Recent:</b> Open a recent project."));
    recentOpenAction->setIcon(QIcon(QStringLiteral(":/icons/open")));

    saveAction = new QAction(this);
    saveAction->setText(tr("&Save\n"));
    saveAction->setToolTip(tr("<b>Save:</b> Save the current project."));
    saveAction->setIcon(QIcon(QStringLiteral(":/icons/save")));
    saveAction->setShortcut(tr("Ctrl+S"));

    saveAsAction = new QAction(this);
    saveAsAction->setText(tr("Save\n&As..."));
    saveAsAction->setToolTip(tr("<b>Save As:</b> Save the current project with a new name."));
    saveAsAction->setIcon(QIcon(QStringLiteral(":/icons/save-as")));
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));

    smartfluxAction = new QAction(this);
    smartfluxAction->setCheckable(true);
    smartfluxAction->setText(tr("SmartFlux Configuration"));
    smartfluxAction->setToolTip(tr("<b>SmartFlux Configuration:</b> ..."));
    smartfluxAction->setShortcut(tr("Ctrl+F"));

    quitAction = new QAction(this);
    quitAction->setText(tr("&Quit"));
    quitAction->setToolTip(tr("<b>Quit:</b> Quit the application"));
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setMenuRole(QAction::QuitRole);

    // view start menu action
    viewWelcomeAction = new QAction(this);
    viewWelcomeAction->setText(tr("Welcome"));
    viewWelcomeAction->setIcon(QIcon(QStringLiteral(":/icons/welcome")));
    viewWelcomeAction->setToolTip(tr("Go to the <i>Welcome page</i>"));

    viewProjectCreationAction = new QAction(this);
    viewProjectCreationAction->setText(tr("Project\nCreation"));
    viewProjectCreationAction->setIcon(QIcon(QStringLiteral(":/icons/project")));
    viewProjectCreationAction->setToolTip(tr("Go to the <i>Project Creation Page</i>"));

    viewBasicSettingsAction = new QAction(this);
    viewBasicSettingsAction->setText(tr("Basic\nSettings"));
    viewBasicSettingsAction->setIcon(QIcon(QStringLiteral(":/icons/dataset")));
    viewBasicSettingsAction->setToolTip(tr("Go to the <i>Basic Settings Page</i>"));

    viewAdvancedAction = new QAction(this);
    viewAdvancedAction->setText(tr("Advanced\nSettings"));
    viewAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/advanced")));
    viewAdvancedAction->setToolTip(tr("Go to the <i>Advanced Settings Page</i>"));

    viewRunPageAction = new QAction(this);
    viewRunPageAction->setText(tr("Output\nConsole"));
    viewRunPageAction->setIcon(QIcon(QStringLiteral(":/icons/console")));
    viewRunPageAction->setToolTip(tr("Go to the <i>Output Console Page</i>"));

    viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(viewWelcomeAction);
    viewActionGroup->addAction(viewProjectCreationAction);
    viewActionGroup->addAction(viewBasicSettingsAction);
    viewActionGroup->addAction(viewAdvancedAction);
    viewActionGroup->addAction(viewRunPageAction);

    runExpressAction = new QAction(this);
    runExpressAction->setText(tr("Express\nMode"));
    runExpressAction->setIcon(QIcon(QStringLiteral(":/icons/run-exp")));
    runExpressAction->setToolTip(tr("<i>Process in Express mode</i>"));
    runExpressAction->setPriority(QAction::HighPriority);
    runExpressAction->setProperty("show-tooltip", false);
    runExpressAction->setShortcut(tr("Ctrl+E"));

    runAdvancedAction = new QAction(this);
    runAdvancedAction->setText(tr("Advanced\nMode"));
    runAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/run-adv")));
    runAdvancedAction->setToolTip(tr("<i>Process in Advanced mode</i>"));
    runAdvancedAction->setPriority(QAction::HighPriority);
    runAdvancedAction->setProperty("show-tooltip", false);
    runAdvancedAction->setShortcut(tr("Ctrl+A"));

    runRetrieverAction = new QAction(this);
    runRetrieverAction->setText(tr("Metadata\nRetriever"));
    runRetrieverAction->setIcon(QIcon(QStringLiteral(":/icons/run-ret")));
    runRetrieverAction->setToolTip(tr("<b>Metadata retriever:</b> Only available for GHG files, this tool retrieves metadata from GHG files and organizes them into a unique file that can be examined (e.g., to create a time series of canopy heights at the site) and can also be provided to EddyPro as a dynamic metadata file."));
    runRetrieverAction->setPriority(QAction::HighPriority);
    runRetrieverAction->setProperty("show-tooltip", true);
    runRetrieverAction->setShortcut(tr("Ctrl+R"));

    stopAction = new QAction(this);
    stopAction->setText(tr("Stop"));
    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop")));
    stopAction->setToolTip(tr("<i>Stop processing</i>"));
    stopAction->setPriority(QAction::HighPriority);
    stopAction->setProperty("show-tooltip", false);
    stopAction->setShortcut(tr("Ctrl+T"));

#if 1 // !defined(Q_OS_MAC)
    // Full Screen Action
    toggleFullScreenAction = new QAction(this);
    toggleFullScreenAction->setText(tr("Full Screen"));
    toggleFullScreenAction->setCheckable(true);
    toggleFullScreenAction->setShortcut(QKeySequence(tr("F11")));
    toggleFullScreenAction->setStatusTip(tr("Full screen mode view"));
#endif

    toggleStatusbarAct = new QAction(this);
    toggleStatusbarAct->setText(tr("&Statusbar"));
    toggleStatusbarAct->setCheckable(true);

    toggleInfoOutputAct = new QAction(this);
    toggleInfoOutputAct->setText(tr("&Message Info"));
    toggleInfoOutputAct = infoDock->toggleViewAction();
    toggleInfoOutputAct->setCheckable(true);

    toggleTooltipOutputAct = new QAction(this);
    toggleTooltipOutputAct->setText(tr("&Tooltip Help"));
    toggleTooltipOutputAct->setCheckable(true);
    toggleTooltipOutputAct->setChecked(configState_.window.tooltips);

    toggleConsoleOutputAct = new QAction(this);
    toggleConsoleOutputAct->setText(tr("&Output Console"));
    toggleConsoleOutputAct = consoleDock->toggleViewAction();
    toggleConsoleOutputAct->setCheckable(true);

    helpAction = new QAction(this);
    helpAction->setText(tr("%1 Help").arg(Defs::APP_NAME));
    helpAction->setIcon(QIcon(QStringLiteral(":/icons/img/menu-help.png")));
    helpAction->setShortcut(QKeySequence(tr("F1")));

    pdfHelpAction = new QAction(this);
    pdfHelpAction->setText(tr("Instruction Manual (PDF)"));
    pdfHelpAction->setIcon(QIcon(QStringLiteral(":/icons/img/menu-help.png")));
    pdfHelpAction->setShortcut(tr("Ctrl+H"));

    starterPdfHelpAction = new QAction(this);
    starterPdfHelpAction->setText(tr("Getting Started (PDF)"));
    starterPdfHelpAction->setIcon(QIcon(QStringLiteral(":/icons/img/menu-help.png")));

    toggleOfflineHelpAct = new QAction(this);
    toggleOfflineHelpAct->setText(tr("Use Offline Help"));
    toggleOfflineHelpAct->setCheckable(true);

    videoTutorialsAction = new QAction(this);
    videoTutorialsAction->setText(tr("Video Tutorials"));

    swWebpageAction = new QAction(this);
    swWebpageAction->setText(tr("EddyPro Web Page"));

    checkUpdateAction = new QAction(this);
    checkUpdateAction->setText(tr("Check for Updates..."));
    checkUpdateAction->setShortcut(tr("Ctrl+U"));

    aboutAction = new QAction(this);
    aboutAction->setText(tr("&About..."));
    aboutAction->setMenuRole(QAction::AboutRole);

    aboutQtAction = new QAction(this);
    aboutQtAction->setText(tr("About Qt..."));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
}

void MainWindow::connectActions()
{
    DEBUG_FUNC_NAME

    // File actions
    connect(newAction, SIGNAL(triggered()), this, SLOT(fileNew()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(closeAction, SIGNAL(triggered()), this, SLOT(fileClose()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(fileSave()));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    connect(smartfluxAction, SIGNAL(triggered(bool)), this, SLOT(setSmartfluxMode(bool)));
//    connect(smartfluxAction, SIGNAL(triggered(bool)), mainDialog_->startPage(), SLOT(updateSmartfluxCheckBox()));

    // View actions
    connect(viewWelcomeAction, SIGNAL(triggered()),
            this, SLOT(viewWelcomePage()));
    connect(viewProjectCreationAction, SIGNAL(triggered()),
            this, SLOT(viewProjectPage()));
    connect(viewBasicSettingsAction, SIGNAL(triggered()),
            this, SLOT(viewBasicSettingsPage()));
    connect(viewAdvancedAction, SIGNAL(triggered()),
            this, SLOT(viewAdvancedSettingsPage()));
    connect(viewRunPageAction, SIGNAL(triggered()),
            this, SLOT(viewRunPage()));

    connect(runExpressAction, SIGNAL(triggered()),
            this, SLOT(getRunExpress()));
    connect(runAdvancedAction, SIGNAL(triggered()),
            this, SLOT(getRunAdvanced()));
    connect(runRetrieverAction, SIGNAL(triggered()),
            this, SLOT(getRunRetriever()));
    connect(stopAction, SIGNAL(triggered()),
            this, SLOT(stopEngine()));

#if 1 // !defined(Q_OS_MAC)
    connect(toggleFullScreenAction, SIGNAL(toggled(bool)),
            this, SLOT(setFullScreen(bool)));
#endif

    connect(toggleStatusbarAct, SIGNAL(triggered(bool)),
            this, SLOT(toggleStatusbar(bool)));

    // Tools actions
    connect(toggleInfoOutputAct, SIGNAL(triggered(bool)),
            this, SLOT(viewInfoOutput(bool)));
    connect(toggleTooltipOutputAct, SIGNAL(triggered(bool)),
            this, SLOT(viewTooltipOutput(bool)));
    connect(toggleConsoleOutputAct, SIGNAL(triggered(bool)),
            this, SLOT(viewConsoleOutput(bool)));

    connect(toggleInfoOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewInfoOutput(bool)));
    connect(toggleTooltipOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewTooltipOutput(bool)));
    connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
            this, SLOT(viewConsoleOutput(bool)));

    // Help actions
    connect(helpAction, SIGNAL(triggered()),
            this, SLOT(showHelp()));
    connect(pdfHelpAction, SIGNAL(triggered()),
            this, SLOT(showPdfHelp()));
    connect(starterPdfHelpAction, SIGNAL(triggered()),
            this, SLOT(showStarterPdfHelp()));
    connect(toggleOfflineHelpAct, SIGNAL(triggered(bool)),
            this, SLOT(setOfflineHelp(bool)));
    connect(swWebpageAction, SIGNAL(triggered()),
            this, SLOT(showSwWebPage()));
    connect(checkUpdateAction, SIGNAL(triggered()),
            this, SLOT(showUpdateDialog()));
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(about()));
    connect(aboutQtAction, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    // to disable context menu on application menu bar
    setContextMenuPolicy(Qt::NoContextMenu);

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);

    // submenu open recent file
    fileMenuOpenRecent = fileMenu->addMenu(tr("&Open Recent"));
    fileMenuOpenRecent->addAction(recentOpenAction);
    fileMenuOpenRecent->setIcon(QIcon(QStringLiteral(":/icons/img/fileopen.png")));
    fileMenuOpenRecent->setEnabled(false);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(smartfluxAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addActions(viewActionGroup->actions());

    viewMenu->addSeparator();
    viewMenu->addAction(toggleConsoleOutputAct);
    viewMenu->addAction(toggleInfoOutputAct);
    viewMenu->addAction(toggleTooltipOutputAct);
    viewMenu->addAction(toggleStatusbarAct);
#if 1 // !defined(Q_OS_MAC)
    viewMenu->addAction(toggleFullScreenAction);
#endif

    MyMenu *toolsMenu = new MyMenu(this);
    toolsMenu->setTitle(tr("&Run"));
    menuBar()->addMenu(toolsMenu);
    toolsMenu->addAction(runExpressAction);
    toolsMenu->addAction(runAdvancedAction);
    toolsMenu->addAction(stopAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(runRetrieverAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(pdfHelpAction);
    helpMenu->addAction(starterPdfHelpAction);
    helpMenu->addAction(toggleOfflineHelpAct);
    helpMenu->addSeparator();
    helpMenu->addAction(swWebpageAction);
    helpMenu->addAction(checkUpdateAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    DEBUG_FUNC_NAME

    fileToolBar = addToolBar(tr("&File ToolBar"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->addAction(saveAsAction);
    fileToolBar->addAction(closeAction);
    fileToolBar->setObjectName(QStringLiteral("fileToolBar"));
    fileToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    sep1 = new ClickLabel();
    sep1->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
    sep2 = new ClickLabel();
    sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
    sep3 = new ClickLabel();
    sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
    sep4 = new ClickLabel();
    sep4->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));

    viewToolBar = addToolBar(tr("&View ToolBar"));
    viewToolBar->setObjectName(QStringLiteral("viewToolBar"));
    viewToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewToolBar->addAction(viewWelcomeAction);
    viewToolBar->addWidget(sep1);
    viewToolBar->addAction(viewProjectCreationAction);
    viewToolBar->addWidget(sep2);
    viewToolBar->addAction(viewBasicSettingsAction);
    viewToolBar->addWidget(sep3);
    viewToolBar->addAction(viewAdvancedAction);
    viewToolBar->addWidget(sep4);
    viewToolBar->addAction(viewRunPageAction);
    viewToolBar->setObjectName(QStringLiteral("viewToolBar"));
    viewToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    runToolBar = addToolBar(tr("&Tools ToolBar"));

    QLabel* runLabel = new QLabel(tr("  Run:  "));
    runLabel->setObjectName(QStringLiteral("runLabel"));

    QAction* tSep = new QAction(this);
    tSep->setSeparator(true);
    tSep->setIcon(QIcon(QStringLiteral(":/icons/sep")));

    QLabel* sep5 = new QLabel();
    sep5->setPixmap(QPixmap(QStringLiteral(":/icons/tsep")));
    QLabel* sep6 = new QLabel();
    sep6->setPixmap(QPixmap(QStringLiteral(":/icons/tsep")));
    QLabel* sep7 = new QLabel();
    sep7->setPixmap(QPixmap(QStringLiteral(":/icons/tsep")));

    runToolBar->addWidget(runLabel);
    runToolBar->addAction(tSep);
    runToolBar->addAction(runExpressAction);
    runToolBar->addWidget(sep5);
    runToolBar->addAction(runAdvancedAction);
    runToolBar->addWidget(sep6);
    runToolBar->addAction(stopAction);
    runToolBar->addWidget(sep7);
    runToolBar->setObjectName(QStringLiteral("pageToolBar"));
    runToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    fileToolBar->setIconSize(QSize(26, 42));
    viewToolBar->setIconSize(QSize(42, 42));
    runToolBar->setIconSize(QSize(32, 42));

    // to add small licor logo right aligned
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ClickLabel *licorLogo = new ClickLabel();
    licorLogo->setObjectName(QStringLiteral("licorLogo"));

    runToolBar->addWidget(spacer);
    runToolBar->addWidget(licorLogo);
    connect(licorLogo, SIGNAL(clicked()), this, SLOT(openLicorSite()));
}

void MainWindow::createStatusBar()
{
    DEBUG_FUNC_NAME

    showStatusTip(tr("Ready"));

    currentProjectLabel = new QLabel(this);
    currentProjectLabel->setAlignment(Qt::AlignCenter);
    currentProjectLabel->setText(currentProjectFile());

    statusBar()->addPermanentWidget(currentProjectLabel);
    statusBar()->setToolTip(currentProjectFile());
}

void MainWindow::updateStatusBar()
{
    Alia::setTextToLabel(currentProjectLabel, currentProjectFile(), Qt::ElideMiddle, this->size().width());
    statusBar()->setToolTip(currentProjectFile());
}

void MainWindow::createConsoleDockWin()
{
    DEBUG_FUNC_NAME
    consoleOutput = new QPlainTextEdit();
    consoleOutput->setObjectName(QStringLiteral("consoleDockEdit"));
    consoleOutput->setReadOnly(true);
    consoleOutput->setLineWrapMode(QPlainTextEdit::NoWrap);
    consoleOutput->setMaximumBlockCount(10000);
    consoleOutput->setUndoRedoEnabled(false);
    consoleOutput->setMinimumWidth(this->width() / 2);

    QPushButton *clearConsoleButton = new QPushButton(tr("Clear"));
    clearConsoleButton->setProperty("mdButton", true);
    clearConsoleButton->setMaximumWidth(clearConsoleButton->sizeHint().width());

    connect(clearConsoleButton, SIGNAL(clicked()),
            consoleOutput, SLOT(clear()));

    QVBoxLayout* consoleLayout = new QVBoxLayout;
    consoleLayout->addWidget(consoleOutput);
    consoleLayout->addWidget(clearConsoleButton, 0, Qt::AlignBottom | Qt::AlignRight);

    QWidget *consoleFrame = new QWidget(this);
    consoleFrame->setLayout(consoleLayout);

    consoleDock = new QDockWidget(tr("Output Console"), this);
    consoleDock->setObjectName(QStringLiteral("consoleDock"));
    consoleDock->setWidget(consoleFrame);
    consoleDock->setVisible(false);

    addDockWidget(Qt::RightDockWidgetArea, consoleDock, Qt::Horizontal);
}

void MainWindow::createInfoDockWin()
{
    infoOutput = new QPlainTextEdit();
    infoOutput->setObjectName(QStringLiteral("infoDockEdit"));
    infoOutput->setReadOnly(true);
    infoOutput->setUndoRedoEnabled(false);
    infoOutput->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    infoOutput->setMinimumHeight(50);
    infoOutput->setMaximumBlockCount(50);

    infoDock = new QDockWidget(tr("Messages"), this);
    infoDock->setObjectName(QStringLiteral("msgDock"));
    infoDock->setWidget(infoOutput);
    infoDock->setVisible(false);

    addDockWidget(Qt::BottomDockWidgetArea, infoDock, Qt::Horizontal);
    connect(infoDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
                               this, SLOT(updateDockBorder(Qt::DockWidgetArea)));
}

void MainWindow::updateHtmlDock(QPlainTextEdit* dock, const QString& htmlMsg)
{
    QString newMsg = QTextDocumentFragment::fromHtml(htmlMsg).toPlainText();
    QString currMsg = dock->toPlainText();

    if (newMsg != currMsg)
    {
        dock->clear();
        dock->appendHtml(htmlMsg);
        dock->verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
    }
}

// read the stored mainwindow application settings
void MainWindow::readSettings()
{
    QSettings settings;

    // read window state
    settings.beginGroup(Defs::CONFGROUP_WINDOW);
        configState_.window.statusBar = settings.value(Defs::CONF_WIN_STATUSBAR,
                                                configState_.window.statusBar).toBool();
        configState_.window.fullScreen = settings.value(Defs::CONF_WIN_FULLSCREEN,
                                                configState_.window.fullScreen).toBool();
        configState_.window.consoleDock = settings.value(Defs::CONF_WIN_CONSOLEDOCK,
                                                configState_.window.consoleDock).toBool();
        configState_.window.tooltips = settings.value(Defs::CONF_WIN_TOOLTIPS,
                                                configState_.window.tooltips).toBool();
        configState_.window.infoDock = settings.value(Defs::CONF_WIN_INFODOCK,
                                                configState_.window.infoDock).toBool();
        configState_.window.mainwin_state = settings.value(Defs::CONF_WIN_MAINWIN_STATE,
                                                configState_.window.mainwin_state).toByteArray();
        configState_.window.mainwin_geometry = settings.value(Defs::CONF_WIN_MAINWIN_GEOMETRY,
                                                configState_.window.mainwin_geometry).toByteArray();
        configState_.window.offlineHelp = settings.value(Defs::CONF_WIN_OFFLINEHELP,
                                                configState_.window.offlineHelp).toBool();
        configState_.window.last_data_path = settings.value(Defs::CONF_WIN_LAST_DATAPATH,
                                                configState_.window.last_data_path).toString();
    settings.endGroup();

    // read general config
    settings.beginGroup(Defs::CONFGROUP_GENERAL);
        configState_.general.showsplash =
            settings.value(Defs::CONF_GEN_SHOW_SPLASH, configState_.general.showsplash).toBool();
        configState_.general.recentnum =
            settings.value(Defs::CONF_GEN_RECENTNUM, configState_.general.recentnum).toUInt();
        configState_.general.loadlastproject =
            settings.value(Defs::CONF_GEN_LOADLAST, configState_.general.loadlastproject).toBool();
        configState_.general.recentfiles = settings.value(Defs::CONF_GEN_RECENTFILES).toStringList();
        while (configState_.general.recentfiles.count() > configState_.general.recentnum)
        {
            configState_.general.recentfiles.removeLast();
        }
        settings.endGroup();

    // read project config
    settings.beginGroup(Defs::CONFGROUP_PROJECT);
        // test if datapath still exists
        QString defaultDatapath = settings.value(Defs::CONF_PROJ_DEFAULT_PATH,
                                              configState_.project.default_data_path).toString();
        if (FileUtils::existsPath(defaultDatapath))
        {
            configState_.project.default_data_path = defaultDatapath;
        }
        else
        {
            configState_.project.default_data_path = QString();
        }
    settings.endGroup();
}

// write the mainwindow application settings
void MainWindow::writeSettings()
{
    QSettings settings;

    // write window state
    settings.beginGroup(Defs::CONFGROUP_WINDOW);
        settings.setValue(Defs::CONF_WIN_MAINWIN_STATE, saveState());
        settings.setValue(Defs::CONF_WIN_MAINWIN_GEOMETRY, saveGeometry());
        settings.setValue(Defs::CONF_WIN_STATUSBAR, configState_.window.statusBar);
        settings.setValue(Defs::CONF_WIN_FULLSCREEN, configState_.window.fullScreen);
        settings.setValue(Defs::CONF_WIN_CONSOLEDOCK, configState_.window.consoleDock);
        settings.setValue(Defs::CONF_WIN_TOOLTIPS, configState_.window.tooltips);
        settings.setValue(Defs::CONF_WIN_INFODOCK, configState_.window.infoDock);
        settings.setValue(Defs::CONF_WIN_LAST_DATAPATH, configState_.window.last_data_path);
    settings.endGroup();

    // write general state
    settings.beginGroup(Defs::CONFGROUP_GENERAL);
        settings.setValue(Defs::CONF_GEN_RECENTFILES, configState_.general.recentfiles);
        settings.setValue(Defs::CONF_GEN_RECENTNUM, (int)configState_.general.recentnum);
        settings.setValue(Defs::CONF_GEN_LOADLAST, configState_.general.loadlastproject);
    settings.endGroup();

    // write project config
    settings.beginGroup(Defs::CONFGROUP_PROJECT);
        settings.setValue(Defs::CONF_PROJ_DEFAULT_PATH, configState_.project.default_data_path);
    settings.endGroup();

    settings.sync();
}

// Save the configuration (when OK pressed in dialog)
void MainWindow::saveEnvSettings(const QString& env)
{
    configState_.general.env = env;

    QSettings settings;

    // write project config
    settings.beginGroup(Defs::CONFGROUP_GENERAL);
        settings.setValue(Defs::CONF_GEN_ENV, configState_.general.env);
    settings.endGroup();

    settings.sync();
}

void MainWindow::restorePreviousStatus()
{
    DEBUG_FUNC_NAME

    // must be before restoreGeometry(), which restore the possible full screen
    // state, because otherwise setFullScreen() reset restoreGeometry()
#if 1 // !defined(Q_OS_MAC)
    toggleFullScreenAction->setChecked(configState_.window.fullScreen);
#endif

    // the dockwidgets checked state is restored by restoreState()
//    toggleTooltipOutputAct->setChecked(config_state_.window.tooltipDock);
//    toggleConsoleOutputAct->setChecked(config_state_.window.consoleDock);

    restoreState(configState_.window.mainwin_state);
    restoreGeometry(configState_.window.mainwin_geometry);

    // show or hide statusbar depending on initial setting
    // note: QMainWindow::restoreState() acts on toolbars and dockwidgets
    // but not on statusbar
    statusBar()->setVisible(configState_.window.statusBar);
    toggleStatusbarAct->setChecked(configState_.window.statusBar);
    toggleOfflineHelpAct->setChecked(configState_.window.offlineHelp);
}

void MainWindow::setFullScreen(bool on)
{
    configState_.window.fullScreen = on;
    if (on)
    {
        showFullScreen();
    }
    else
    {
        showMaximized();
    }
    writeSettings();
}

// Toggle statusbar status
void MainWindow::toggleStatusbar(bool on)
{
    statusBar()->setVisible(on);
    configState_.window.statusBar = on;
    writeSettings();
    if (on)
        showStatusTip(Defs::READY);
}

void MainWindow::viewTooltipOutput(bool on)
{
    DEBUG_FUNC_NAME
    configState_.window.tooltips = on;
    writeSettings();
    tooltipFilter_->setTooltipAvailable(on);
}

void MainWindow::viewConsoleOutput(bool on)
{
    DEBUG_FUNC_NAME
    configState_.window.consoleDock = on;
    writeSettings();
}

void MainWindow::dbgViewConsoleOutputToggled(bool on)
{
    DEBUG_FUNC_NAME
    qDebug() << "on" << on;
}

void MainWindow::dbgViewConsoleOutputTriggered(bool on)
{
    DEBUG_FUNC_NAME
    qDebug() << "on" << on;
}

void MainWindow::viewInfoOutput(bool on)
{
    DEBUG_FUNC_NAME
    configState_.window.infoDock = on;
    writeSettings();
}

void MainWindow::showHelp()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#EddyPro_Home.htm"), QUrl::StrictMode));
}

void MainWindow::showPdfHelp()
{
    Alia::showHelp(QUrl(QStringLiteral("ftp://ftp.licor.com/perm/env/EddyPro/Manual/EddyPro5_User_Guide.pdf"), QUrl::StrictMode));
}

void MainWindow::showStarterPdfHelp()
{
    Alia::showHelp(QUrl(QStringLiteral("ftp://ftp.licor.com/perm/env/EddyPro/Manual/EddyPro5_Getting_Started.pdf"), QUrl::StrictMode));
}

void MainWindow::showSwWebPage()
{
    Alia::openWebSite();
}

void MainWindow::setOfflineHelp(bool yes)
{
    QSettings config;

    config.beginGroup(Defs::CONFGROUP_WINDOW);
        config.setValue(Defs::CONF_WIN_OFFLINEHELP, yes);
    config.endGroup();
    config.sync();

    toggleOfflineHelpAct->setChecked(yes);
}

void MainWindow::setSmartfluxMode(bool on)
{
    DEBUG_FUNC_NAME

    configState_.project.smartfluxMode = on;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_PROJECT);
        config.setValue(Defs::CONF_PROJ_SMARTFLUX, on);
    config.endGroup();
    config.sync();

    updateInfoMessages();
    updateRunButtonsAvailability();

    if (on)
    {
        bool showDialog = false;

        QSettings config;
        config.beginGroup(Defs::CONFGROUP_WINDOW);
            showDialog = config.value(Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG, true).toBool();
        config.endGroup();

        qDebug() << "showDialog" << showDialog;
        if (showDialog)
        {
            // info message
            InfoMessage smartfluxConfigDialog(QDialogButtonBox::Ok, this);
            smartfluxConfigDialog.setTitle(tr("SmartFlux"));
            smartfluxConfigDialog.setType(InfoMessage::SMARTFLUX_CONFIG);
            smartfluxConfigDialog.setMessage(tr("The SmartFlux configuration will use "
                "a renamed copy of your current project file. <br />"));
            smartfluxConfigDialog.refresh();
            int res = smartfluxConfigDialog.exec();
            qDebug() << "smartfluxConfigDialog res" << res;
        }
        if (!currentProjectFile().contains(Defs::DEFAULT_SMARTFLUX_SUFFIX))
        {
            // laod a renamed copy of the previous loaded project
            loadSmartfluxProjectCopy(currentProjectFile());
        }
    }

    smartfluxAction->setChecked(on);
    mainWidget_->updateSmartfluxBarStatus();

    viewProjectCreationAction->setEnabled(!on);
    viewProjectCreationAction->setCheckable(!on);

    if (currentPage() == Defs::CurrPageWelcome)
    {
        viewBasicSettingsAction->setEnabled(on);
        viewBasicSettingsAction->setCheckable(on);
        if (on)
        {
            viewRunPageAction->setEnabled(false);
            viewRunPageAction->setCheckable(false);
        }

        windowTitleUpdate(currentPage());
    }
    else if (currentPage() == Defs::CurrPageProjectCreation
             || currentPage() == Defs::CurrPageRun)
    {
        changePage(Defs::CurrPageBasicSettings, false);
        viewRunPageAction->setEnabled(!on);
        viewRunPageAction->setCheckable(!on);
    }
    else
    {
        viewRunPageAction->setEnabled(!on);
        viewRunPageAction->setCheckable(!on);
    }
}

void MainWindow::loadSmartfluxProjectCopy(const QString& filename)
{
    DEBUG_FUNC_NAME

    // laod a renamed copy of the previous loaded project
    const QString epExt = QStringLiteral(".") + Defs::PROJECT_FILE_EXT;

    QString filenameCopy = filename;
    if (filenameCopy.indexOf(Defs::DEFAULT_SMARTFLUX_SUFFIX) == -1)
    {
        filenameCopy.insert(filenameCopy.indexOf(epExt), Defs::DEFAULT_SMARTFLUX_SUFFIX);
    }

    qDebug() << "currentProjectFile()" << currentProjectFile();
    qDebug() << "filenameCopy" << filenameCopy;

    if (currentProjectFile() == Defs::DEFAULT_PROJECT_FILENAME)
    {
        setCurrentProjectFile(filenameCopy, true);
        newFlag_ = true;
        return;
    }

    // if we are not reopening the same smartflux project file
    if (currentProjectFile() != filenameCopy)
    {
        // if the corresponding smartflux project file already exists, open it
        if (QFile::exists(filenameCopy))
        {
            fileOpen(filenameCopy);
        }
        // save and open silently a renamed copy of the current project file
        else
        {
            fileSaveAs(filenameCopy);
            newFlag_ = true;
        }
    }
}

// Add a file to the recent files list
void MainWindow::addRecent(const QString &filename)
{
    if (configState_.general.recentnum == 0) return;

    QFileInfo finfo(filename);
    QString filepath = finfo.absoluteFilePath();

    if (configState_.general.recentfiles.contains(filepath))
        configState_.general.recentfiles.removeAll(filepath);
    if (configState_.general.recentfiles.count() >= configState_.general.recentnum)
        configState_.general.recentfiles.removeLast();
    configState_.general.recentfiles.prepend(filepath);

    currEcProjectFilename_ = filename;
    writeSettings();
    emit recentUpdated();
}

void MainWindow::recentMenuShow()
{
    fileMenuOpenRecent->clear();

    QAction *action;
    foreach (const QString& recentfile, configState_.general.recentfiles)
    {
        if (QFile::exists(recentfile))
        {
            action = new QAction(recentfile, fileMenuOpenRecent);
            connect(action, SIGNAL(triggered()), this, SLOT(fileRecent()));
            fileMenuOpenRecent->addAction(action);
        }
    }
}

void MainWindow::viewWelcomePage()
{
    DEBUG_FUNC_NAME
    qDebug() << currentPage();

    if (currentPage() != Defs::CurrPageWelcome)
    {
        changePage(Defs::CurrPageWelcome);
        writeSettings();
        emit recentUpdated();
    }
}

void MainWindow::viewProjectPage()
{
    DEBUG_FUNC_NAME
    qDebug() << currentPage();

    if (currentPage() != Defs::CurrPageProjectCreation)
    {
        if (configState_.project.smartfluxMode)
        {
            changePage(Defs::CurrPageBasicSettings);
            showInfoMessages_2();
        }
        else
        {
            changePage(Defs::CurrPageProjectCreation);
            showInfoMessages_1();
        }
    }
}

void MainWindow::viewBasicSettingsPage()
{
    DEBUG_FUNC_NAME

//    updateInfoDock(true);
    qDebug() << currentPage() << basicSettingsPageAvailable_;

    if (basicSettingsPageAvailable_)
    {
        if (currentPage() != Defs::CurrPageBasicSettings)
        {
            changePage(Defs::CurrPageBasicSettings);
            showInfoMessages_2();
        }
    }
}

void MainWindow::viewAdvancedSettingsPage()
{
    DEBUG_FUNC_NAME
    qDebug() << currentPage();

    if (currentPage() != Defs::CurrPageAdvancedSettings)
    {
        showInfoMessages_3();
        changePage(Defs::CurrPageAdvancedSettings);
    }
}

void MainWindow::viewRunPage()
{
    DEBUG_FUNC_NAME
    qDebug() << currentPage();

    if (currentPage() != Defs::CurrPageRun)
    {
        if (configState_.project.smartfluxMode)
        {
            changePage(Defs::CurrPageAdvancedSettings);
        }
        else
        {
            changePage(Defs::CurrPageRun);
        }
    }
}

Defs::CurrPage MainWindow::currentPage() const
{
    return currentPage_;
}

void MainWindow::setCurrentPage(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME

    if (currentPage_ != page)
    {
        currentPage_ = page;
    }
}

Defs::CurrStatus MainWindow::currentStatus() const
{
    return currentStatus_;
}

void MainWindow::setCurrentStatus(Defs::CurrStatus state)
{
    DEBUG_FUNC_NAME

    if (currentStatus_ != state)
    {
        currentStatus_ = state;
    }
}

Defs::CurrRunStatus MainWindow::currentRunStatus() const
{
    return currentRunStatus_;
}

void MainWindow::setCurrentRunStatus(Defs::CurrRunStatus state)
{
    DEBUG_FUNC_NAME

    if (currentRunStatus_ != state)
    {
        currentRunStatus_ = state;
    }
}

// restore menus and dockwidget state
void MainWindow::updateMenuActionStatus(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME

    qDebug() << "page" << page;

    bool loadedProject = (currentProjectFile() != Defs::DEFAULT_PROJECT_FILENAME
                             && currentProjectFile() != Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME)
                         || (currentProjectFile() == Defs::DEFAULT_PROJECT_FILENAME
                             && ecProject_->modified())
                         || (currentProjectFile() == Defs::DEFAULT_PROJECT_FILENAME
                             && newClicked_)
                         || (currentProjectFile() == Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME
                             && ecProject_->modified())
                         || (currentProjectFile() == Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME
                             && newClicked_);
    qDebug() << "currentProjectFile()" << currentProjectFile();
    qDebug() << "Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME" << Defs::DEFAULT_SMARTFLUX_PROJECT_FILENAME;
    qDebug() << "newClicked_" << newClicked_;
    qDebug() << "ecProject_->modified()" << ecProject_->modified();
    qDebug() << "loadedProject" << loadedProject;

    // show or hide objects depending on previous settings and current page status
    if (page == Defs::CurrPageWelcome)
    {
        newAction->setEnabled(true);
        openAction->setEnabled(true);
        closeAction->setEnabled(loadedProject);
        saveAction->setEnabled(loadedProject && modifiedFlag_);
        saveAsAction->setEnabled(loadedProject);
        fileMenuOpenRecent->setEnabled(true);

        if (configState_.project.smartfluxMode)
        {
            viewProjectCreationAction->setEnabled(false);
            viewProjectCreationAction->setCheckable(false);
            viewBasicSettingsAction->setEnabled(true);
            viewBasicSettingsAction->setCheckable(true);
        }
        else
        {
            viewProjectCreationAction->setEnabled(true);
            viewProjectCreationAction->setCheckable(true);
            viewBasicSettingsAction->setEnabled(false);
            viewBasicSettingsAction->setCheckable(false);
        }
        viewAdvancedAction->setEnabled(false);
        viewRunPageAction->setEnabled(false);

        viewAdvancedAction->setCheckable(false);
        viewRunPageAction->setCheckable(false);

        // NOTE: hack to prevent temporarly the effect of non interactive change of visibility
        disconnect(toggleInfoOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewInfoOutput(bool)));
        disconnect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleInfoOutputAct->setEnabled(false);
        infoDock->setVisible(false);

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleInfoOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewInfoOutput(bool)));
        connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleStatusbarAct->setEnabled(false);
        statusBar()->setVisible(false);
    }
    else if (page == Defs::CurrPageProjectCreation)
    {
        qDebug() << "modifiedFlag_" << modifiedFlag_;
        newAction->setEnabled(true);
        openAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAction->setEnabled(modifiedFlag_);
        saveAsAction->setEnabled(true);
        fileMenuOpenRecent->setEnabled(true);
        setWindowModified(modifiedFlag_);

        if (configState_.project.smartfluxMode)
        {
            viewProjectCreationAction->setEnabled(false);
            viewProjectCreationAction->setCheckable(false);
            viewRunPageAction->setEnabled(false);
            viewRunPageAction->setCheckable(false);
        }
        else
        {
            viewProjectCreationAction->setEnabled(true);
            viewProjectCreationAction->setCheckable(true);
            viewRunPageAction->setEnabled(basicSettingsPageAvailable_);
            viewRunPageAction->setCheckable(basicSettingsPageAvailable_);
        }
        viewBasicSettingsAction->setEnabled(basicSettingsPageAvailable_);
        viewAdvancedAction->setEnabled(basicSettingsPageAvailable_);

        viewBasicSettingsAction->setCheckable(basicSettingsPageAvailable_);
        viewAdvancedAction->setCheckable(basicSettingsPageAvailable_);

        toggleInfoOutputAct->setEnabled(true);
        infoDock->setVisible(configState_.window.infoDock);

        // NOTE: hack to prevent temporarly the effect of non interactive change of visibility
        disconnect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPageBasicSettings)
    {
        newAction->setEnabled(true);
        openAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAction->setEnabled(modifiedFlag_);
        saveAsAction->setEnabled(true);
        fileMenuOpenRecent->setEnabled(true);
        setWindowModified(modifiedFlag_);

        if (configState_.project.smartfluxMode)
        {
            viewProjectCreationAction->setEnabled(false);
            viewProjectCreationAction->setCheckable(false);
            viewRunPageAction->setEnabled(false);
            viewRunPageAction->setCheckable(false);
        }
        else
        {
            viewProjectCreationAction->setEnabled(true);
            viewProjectCreationAction->setCheckable(true);
            viewRunPageAction->setEnabled(basicSettingsPageAvailable_);
            viewRunPageAction->setCheckable(basicSettingsPageAvailable_);
        }
        viewBasicSettingsAction->setEnabled(basicSettingsPageAvailable_);
        viewAdvancedAction->setEnabled(basicSettingsPageAvailable_);

        viewBasicSettingsAction->setCheckable(basicSettingsPageAvailable_);
        viewAdvancedAction->setCheckable(basicSettingsPageAvailable_);

        toggleInfoOutputAct->setEnabled(true);
        infoDock->setVisible(configState_.window.infoDock);

        // NOTE: hack to prevent temporarly the effect of non interactive change of visibility
        disconnect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPageAdvancedSettings)
    {
        newAction->setEnabled(true);
        openAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAction->setEnabled(modifiedFlag_);
        saveAsAction->setEnabled(true);
        fileMenuOpenRecent->setEnabled(true);
        setWindowModified(modifiedFlag_);

        if (configState_.project.smartfluxMode)
        {
            viewProjectCreationAction->setEnabled(false);
            viewProjectCreationAction->setCheckable(false);
            viewRunPageAction->setEnabled(false);
            viewRunPageAction->setCheckable(false);
        }
        else
        {
            viewProjectCreationAction->setEnabled(true);
            viewProjectCreationAction->setCheckable(true);
            viewRunPageAction->setEnabled(basicSettingsPageAvailable_);
            viewRunPageAction->setCheckable(basicSettingsPageAvailable_);
        }

        viewBasicSettingsAction->setEnabled(basicSettingsPageAvailable_);
        viewAdvancedAction->setEnabled(basicSettingsPageAvailable_);

        viewBasicSettingsAction->setCheckable(basicSettingsPageAvailable_);
        viewAdvancedAction->setCheckable(basicSettingsPageAvailable_);

        toggleInfoOutputAct->setEnabled(true);
        infoDock->setVisible(configState_.window.infoDock);

        // NOTE: hack to prevent temporarly the effect of non interactive change of visibility
        disconnect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
                this, SLOT(viewConsoleOutput(bool)));

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPageRun)
    {
        newAction->setEnabled(true);
        openAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAction->setEnabled(modifiedFlag_);
        saveAsAction->setEnabled(true);
        fileMenuOpenRecent->setEnabled(true);
        setWindowModified(modifiedFlag_);

        if (configState_.project.smartfluxMode)
        {
            viewProjectCreationAction->setEnabled(false);
            viewProjectCreationAction->setCheckable(false);
            viewRunPageAction->setEnabled(false);
            viewRunPageAction->setCheckable(false);
        }
        else
        {
            viewProjectCreationAction->setEnabled(true);
            viewProjectCreationAction->setCheckable(true);
            viewRunPageAction->setEnabled(basicSettingsPageAvailable_);
            viewRunPageAction->setCheckable(basicSettingsPageAvailable_);
        }

        viewBasicSettingsAction->setEnabled(basicSettingsPageAvailable_);
        viewAdvancedAction->setEnabled(basicSettingsPageAvailable_);

        viewBasicSettingsAction->setCheckable(basicSettingsPageAvailable_);
        viewAdvancedAction->setCheckable(basicSettingsPageAvailable_);

        toggleInfoOutputAct->setEnabled(true);
        infoDock->setVisible(configState_.window.infoDock);

        toggleConsoleOutputAct->setEnabled(true);
        consoleDock->setVisible(configState_.window.consoleDock);

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    updateRunButtonsAvailability();
}

void MainWindow::whatsHelp()
{
    QWhatsThis::enterWhatsThisMode();
}

void MainWindow::showStatusTip(const QString &text)
{
    statusBar()->showMessage(text, 2000);
}

void MainWindow::windowTitleUpdate(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME

    qDebug() << "page" << page;
    switch (page)
    {
        case Defs::CurrPageWelcome:
            setWindowTitle(Defs::APP_NAME);
            break;
        case Defs::CurrPageProjectCreation:
        case Defs::CurrPageBasicSettings:
        case Defs::CurrPageAdvancedSettings:
        case Defs::CurrPageRun:
            setFileCaption(currentProjectFile(), false);
            break;
        default:
            setFileCaption(QString());
            break;
    }
}

void MainWindow::updateInfoDock(bool yes)
{
    DEBUG_FUNC_NAME
    qDebug() << "yes" << yes;

    if (yes)
    {
        if (!infoDock->isVisible())
            infoDock->toggleViewAction()->trigger();

        infoDock->raise();

        showInfoMessages_2();
        showInfoMessages_3();
        showInfoMessages_1();
    }
    else
    {
        infoOutput->clear();
    }
}

void MainWindow::showInfoMessages_1()
{
    DEBUG_FUNC_NAME

    QString intro;
    QString msg;
    bool doFix = true;

    qDebug() << "runExpressAvailable_" << runExpressAvailable_;
    qDebug() << "runAdvancedAvailable_" << runAdvancedAvailable_;

    if (doFix)
    {
        qDebug() << "doFix" << doFix;
        intro = tr("<p>Some information in the <b>Project Creation</b> page "
                   "is incomplete or erroneous. Please address the "
                   "following issues:</p>");
        msg = tr("<ul>");
        doFix = false;
        qDebug() << "doFix" << doFix;
    }

    qDebug() << "ecProject_->generalUseMetadataFile()" << ecProject_->generalUseAltMdFile();
    if (ecProject_->generalUseAltMdFile())
    {
        qDebug() << "ecProject_->generalMetadataFilepath()" << ecProject_->generalMdFilepath();
        if (ecProject_->generalMdFilepath().isEmpty())
        {
            msg += tr("<li><span style=\"color: red;\">Metadata file:</span> Load or create one using the "
                 "Metadata File Editor.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }

        qDebug() << "dlProject_->fileDuration()" << dlProject_->fileDuration();
        if (dlProject_->fileDuration() == 0)
        {
            msg += tr("<li><span style=\"color: red;\">Metadata File Editor - File Duration:</span> A file duration greater than zero "
                 "is required.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }

        qDebug() << "ecProject_->generalFileType()" << ecProject_->generalFileType();
        qDebug() << "dlProject_->fieldSep()" << dlProject_->fieldSep();
        if ((ecProject_->generalFileType() == Defs::RawFileTypeGHG
            || ecProject_->generalFileType() == Defs::RawFileTypeASCII)
            && dlProject_->fieldSep().isEmpty())
        {
            msg += tr("<li><span style=\"color: red;\">Metadata File Editor - Raw File Description:</span> Field separator required. Click the \"Raw File Settings...\" button.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }

        qDebug() << "dlProject_->headerRows()" << dlProject_->headerRows();
        if ((ecProject_->generalFileType() == Defs::RawFileTypeGHG
            || ecProject_->generalFileType() == Defs::RawFileTypeASCII)
            && dlProject_->headerRows() < 0)
        {
            msg += tr("<li><span style=\"color: red;\">Metadata File Editor - Raw File Description:</span> Number of header rows required. Click the \"Raw File Settings...\" button.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }

        if (!dlProject_->hasOneGoodAnemometer())
        {
            msg += tr("<li><span style=\"color: red;\">Metadata File Editor - Instruments:</span> The complete description of at least one anemometer is required.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }
        else if (dlProject_->hasOneGoodAnemometer())
        {
            if (!dlProject_->hasGoodWindComponentsAndTemperature())
            {
                msg += tr("<li><span style=\"color: red;\">Metadata File Editor - Raw File Description:</span> The complete description of 3-d wind components and at least one fast ambient temperature or speed-of-sound measurement is required.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
        }

        if (dlProject_->hasNullGainVariables())
        {
            msg += tr("<li><span style=\"color: red;\">Metadata File Editor - Raw File Description:</span> At least one variable was selected for linear scaling with a zero gain. The gain must be different from zero for any linear scaling.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }

        if (ecProject_->generalFileType() == Defs::RawFileTypeBIN)
        {
            qDebug() << "ecProject_->generalBinaryNBytes"
                     << ecProject_->generalBinaryNBytes();
            if (ecProject_->generalBinaryHNLines() < 0)
            {
                msg += tr("<li><span style=\"color: red;\">Generic Binary:</span> Number of ASCII header lines required. Click the \"Settings...\" button.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
            qDebug() << "ecProject_->generalBinaryEol"
                     << ecProject_->generalBinaryEol();
            if (ecProject_->generalBinaryEol() < 0)
            {
                msg += tr("<li><span style=\"color: red;\">Generic Binary:</span> ASCII header end of line required. Click the \"Settings...\" button.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
            qDebug() << "ecProject_->generalBinaryNBytes"
                     << ecProject_->generalBinaryNBytes();
            if (ecProject_->generalBinaryNBytes() <= 0)
            {
                qDebug() << "ecProject_->generalBinaryNBytes()" << ecProject_->generalBinaryNBytes();
                msg += tr("<li><span style=\"color: red;\">Generic Binary:</span> Number of bytes per variable required. Click the \"Settings...\" button.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
            qDebug() << "ecProject_->generalBinaryLittleEnd"
                     << ecProject_->generalBinaryLittleEnd();
            if (ecProject_->generalBinaryLittleEnd() < 0)
            {
                msg += tr("<li><span style=\"color: red;\">Generic Binary:</span> Endianess required. Click the \"Settings...\" button.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
        }
    }

    qDebug() << "ecProject_->generalUseTimelineFile()" << ecProject_->generalUseTimelineFile();
    if (ecProject_->generalUseTimelineFile())
    {
        qDebug() << "ecProject_->generalTimelineFilepath()" << ecProject_->generalTimelineFilepath();
        if (ecProject_->generalTimelineFilepath().isEmpty())
        {
            msg += tr("<li><span style=\"color: red;\">Dynamic metadata file: </span>"
                      "Select a file using the \"Load...\" button or uncheck this option.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }
    }

    qDebug() << "ecProject_->generalUseBiomet()" << ecProject_->generalUseBiomet();
    if (ecProject_->generalUseBiomet() == 2)
    {
        if (ecProject_->generalBiomFile().isEmpty())
        {
            msg += tr("<li><span style=\"color: red;\">Biomet file: </span>"
                      "Select a file using the \"Load...\" button.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }
    }
    else if (ecProject_->generalUseBiomet() == 3)
    {
        if (ecProject_->generalBiomDir().isEmpty())
        {
            msg += tr("<li><span style=\"color: red;\">Biomet directory: </span>"
                      "Select a directory using the \"Browse...\" button.</li>");
            doFix = true;
            qDebug() << "doFix" << doFix;
        }
    }

    if (!doFix)
    {
        qDebug() << "doFix" << doFix;
        intro = tr("");
        msg = tr("<ul>");
        doFix = false;
    }

    if (!doFix)
    {
        msg += tr("</ul>");
    }
    qDebug() << "last doFix" << doFix;
    qDebug() << "title" << intro;
    qDebug() << "msg" << msg;

    basicSettingsPageAvailable_ = intro.isEmpty();
    qDebug() << "processingPageAvailable_" << basicSettingsPageAvailable_;

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, intro + msg);
}

void MainWindow::showInfoMessages_2()
{
    DEBUG_FUNC_NAME

    QString intro;
    QString msg;
    bool doFix = true;

    qDebug() << "runExpressAvailable_" << runExpressAvailable_;
    qDebug() << "runAdvancedAvailable_" << runAdvancedAvailable_;

    if (doFix)
    {
        qDebug() << "doFix" << doFix;
        intro = tr("<p>Some information in the <b>Basic Settings</b> page "
                   "is incomplete. Please address the "
                   "following issues:</p>");
        msg = tr("<ul>");
        doFix = false;
    }

    qDebug() << "ecProject_->screenDataPath()" << ecProject_->screenDataPath();
    if (ecProject_->screenDataPath().isEmpty())
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose a Raw data directory.</li>");
        doFix = true;
    }

    qDebug() << "ecProject_->generalFileType()" << ecProject_->generalFileType();
    qDebug() << "Alia::isGoodRawFileNameFormat(ecProject_->generalFilePrototype())" << Alia::isGoodRawFileNameFormat(ecProject_->generalFilePrototype());
    if (ecProject_->generalFileType() != Defs::RawFileTypeGHG
        && !Alia::isGoodRawFileNameFormat(ecProject_->generalFilePrototype()))
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Raw file name format not well described.</li>");
        doFix = true;
    }

    qDebug() << "ecProject_->generalOutPath()" << ecProject_->generalOutPath();
    if (ecProject_->generalOutPath().isEmpty() && !configState_.project.smartfluxMode)
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose an Output directory.</li>");
        doFix = true;
    }

    qDebug() << "ecProject_->generalId()" << ecProject_->generalId();
    if (ecProject_->generalId().isEmpty() && !configState_.project.smartfluxMode)
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose an Output ID.</li>");
        doFix = true;
    }

    AnemDescList *adl = dlProject_->anems();
    if (adl->size() > 0)
    {
        QString anemModel = ecProject_->generalColMasterSonic();
        if (!anemModel.isEmpty())
        {
            int anemIndex = anemModel.mid(anemModel.lastIndexOf(QLatin1Char('_')) + 1).toInt();
            qDebug() << "anemIndex" << anemIndex;

            // check if valid index position in the list (i.e., 0 <= i < size())
            int i = anemIndex - 1;
            if (i >= 0 && i < adl->size())
            {
                AnemDesc anem = adl->at(i);
                qDebug() << "anem.hasGoodTemp()" << anem.hasGoodTemp();
                if (!anem.hasGoodTemp()
                    && dlProject_->hasOneFastTemperature()
                    && ecProject_->generalColTs() == 0)
                {
                    if (!configState_.project.smartfluxMode)
                    {
                        msg += tr("<li><span style=\"color: red;\">Select Items:</span> Select a Fast temperature reading (alternative to sonic temp), or go back to the Project Creation page and describe either a Sonic Temperature or a Speed-of-Sound measurement for the selected anemometer.</li>");
                        doFix = true;
                        qDebug() << "doFix" << doFix;
                    }
                    else
                    {
                        msg += tr("<li><span style=\"color: red;\">Select Items:</span> Select a Fast temperature reading (alternative to sonic temp).</li>");
                        doFix = true;
                        qDebug() << "doFix" << doFix;
                    }
                }
            }
        }
    }

    if (!doFix && runAdvancedAvailable_ && !configState_.project.smartfluxMode)
    {
        intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings or <span style=\"color: #2986f5; \">Advanced Mode</span> using Advanced Settings.<br />"
                    "Please note that running in <span style=\"color: #52893c; \">Express Mode</span> means EddyPro will ignore all your entries in the Advanced Settings pages. In this case, your settings will not be overridden. You will be able to retrieve them at any time, but they will not be used for the computations.");
        msg = tr("<ul>");
        doFix = false;
        qDebug() << "doFix" << doFix;
    }
    else if (!doFix && !runAdvancedAvailable_ && !configState_.project.smartfluxMode)
    {
        intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings.<br />");
        msg = tr("<ul>");
        doFix = false;
        qDebug() << "doFix" << doFix;
    }

    if (!doFix)
    {
        msg += tr("</ul>");
    }
    qDebug() << "last doFix" << doFix;
    qDebug() << "title" << intro;
    qDebug() << "msg" << msg;

    runExpressAvailable_ = intro.contains(tr("You are ready"));

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, intro + msg);
}

void MainWindow::showInfoMessages_3()
{
    DEBUG_FUNC_NAME

    QString intro;
    QString msg;
    bool doFix = true;

    qDebug() << "runExpressAvailable_" << runExpressAvailable_;
    qDebug() << "runAdvancedAvailable_" << runAdvancedAvailable_;

    if (doFix)
    {
        qDebug() << "doFix" << doFix;
        if (runExpressAvailable_)
        {
            intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings.</span><br />"
                   "<p>Additional settings need to be configured to use <span style=\"color: #2986f5; \">Advanced Mode</span>, including:</p>");
        }
        else
        {
            intro = tr("<p>Additional settings need to be configured to use <span style=\"color: #2986f5; \">Advanced Mode</span>, including:</p>");
        }
        msg = tr("<ul>");
        doFix = false;
        qDebug() << "doFix" << doFix;
    }

    if (ecProject_->screenRotMethod() >= 3)
    {
        qDebug() << "ecProject_->screenRotMethod()" << ecProject_->screenRotMethod();
        if (ecProject_->planarFitMode())
        {
            qDebug() << "ecProject_->hasPlanarFitFullAngle()" << ecProject_->hasPlanarFitFullAngle();
            if (!ecProject_->hasPlanarFitFullAngle())
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Complete wind sectors configuration.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }

            qDebug() << "ecProject_->planarFitItemPerSector()" << ecProject_->planarFitItemPerSector();
            if (ecProject_->planarFitItemPerSector() == 0)
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Minimum number of elements per sector.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }

            qDebug() << "ecProject_->planarFitWmax()" << ecProject_->planarFitWmax();
            if (ecProject_->planarFitWmax() == 0.099)
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Maximum mean vertical wind component.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }

            qDebug() << "ecProject_->planarFitUmin()" << ecProject_->planarFitUmin();
            if (ecProject_->planarFitUmin() == -0.001)
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Minimum mean horizontal wind component.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
        }
        else
        {
            qDebug() << "ecProject_->planarFitFile()" << ecProject_->planarFitFile();
            if (ecProject_->planarFitFile().isEmpty())
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Load a Planar fit file.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
        }
    }
    else
    {
        qDebug() << "ecProject_->screenRotMethod()" << ecProject_->screenRotMethod();
        if (!doFix)
            doFix = false;
        qDebug() << "doFix" << doFix;
    }

    if (ecProject_->screenTlagMeth() == 4)
    {
        qDebug() << "ecProject_->timelagOptMode()" << ecProject_->timelagOptMode();
        if (ecProject_->timelagOptMode())
        {
        }
        else
        {
            qDebug() << "ecProject_->timelagOptFile()" << ecProject_->timelagOptFile();
            if (ecProject_->timelagOptFile().isEmpty())
            {
                msg += tr("<li><span style=\"color: red;\">Timelag Optimization Settings:</span> Load a Time lag file.</li>");
                doFix = true;
                qDebug() << "doFix" << doFix;
            }
        }
    }
    else
    {
        qDebug() << "ecProject_->screenTlagMeth()" << ecProject_->screenTlagMeth();
        if (!doFix)
            doFix = false;
        qDebug() << "doFix" << doFix;
    }

    qDebug() << "ecProject_->generalHfMethod()" << ecProject_->generalHfMethod();
    if (ecProject_->generalHfMethod() >= 2)
    {
        qDebug() << "ecProject_->spectraMode()" << ecProject_->spectraMode();
        if (ecProject_->spectraMode() == 0)
        {
            qDebug() << "ecProject_->spectraFile()" << ecProject_->spectraFile();
            if (ecProject_->spectraFile().isEmpty())
            {
                msg += tr("<li><span style=\"color: red;\">Spectral Correction Settings:</span> Load a Spectral assessment file.</li>");
                doFix = true;
            }
        }
        else
        {
            if (ecProject_->generalBinSpectraAvail())
            {
                qDebug() << "ecProject_->generalBinSpectraAvail()" << ecProject_->generalBinSpectraAvail();
                if (ecProject_->spectraBinSpectra().isEmpty())
                {
                    msg += tr("<li><span style=\"color: red;\">Spectral Correction Settings:</span> Choose a valid path, where 'Binned spectra' files are stored.</li>");
                    doFix = true;
                }
            }

            if (ecProject_->generalFullSpectraAvail())
            {
                qDebug() << "ecProject_->generalFullSpectraAvail()" << ecProject_->generalFullSpectraAvail();
                if (ecProject_->spectraFullSpectra().isEmpty())
                {
                    msg += tr("<li><span style=\"color: red;\">Spectral Correction Settings:</span> Choose a valid path, where 'Full w/T<sub>s</sub> cospectra' files are stored.</li>");
                    doFix = true;
                }
            }
        }
    }


    if (!doFix && runExpressAvailable_)
    {
        qDebug() << "doFix" << doFix;
        intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings or <span style=\"color: #2986f5; \">Advanced Mode</span> using Advanced Settings.<br />"
                   "Please note that running in <span style=\"color: #52893c; \">Express Mode</span> means EddyPro will ignore all your entries in the Advanced Settings pages. In this case, your settings will not be overridden. You will be able to retrieve them at any time, but they will not be used for the computations.");
        msg = tr("<ul>");
        doFix = false;
    }
    else if (!doFix && !runExpressAvailable_)
    {
        intro = tr("");
        msg = tr("<ul>");
        doFix = false;
        qDebug() << "doFix" << doFix;
    }

    if (!doFix)
    {
        msg += tr("</ul>");
    }
    qDebug() << "last doFix" << doFix;
    qDebug() << "title" << intro;
    qDebug() << "msg" << msg;

    runAdvancedAvailable_ = !intro.contains(tr("Additional settings")) && runExpressAvailable_;

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, intro + msg);
}

void MainWindow::updateRunButtonsAvailability()
{
    DEBUG_FUNC_NAME

    qDebug() << "runExpressAvailable_" << runExpressAvailable_;
    qDebug() << "runAdvancedAvailable_" << runAdvancedAvailable_;

    if (currentPage() == Defs::CurrPageWelcome
        || configState_.project.smartfluxMode)
    {
        runExpressAction->setEnabled(false);
        runAdvancedAction->setEnabled(false);
        runRetrieverAction->setEnabled(false);
        stopAction->setEnabled(false);
    }
    else
    {
        runExpressAction->setEnabled(basicSettingsPageAvailable_
            && runExpressAvailable_
            && (currentStatus() == Defs::CurrStatusReady
             || (currentStatus() == Defs::CurrStatusRun
                 && currentRunStatus() == Defs::CurrRunStatusExp)
             || (currentStatus() == Defs::CurrStatusPause
                 && currentRunStatus() == Defs::CurrRunStatusExp)));
        runAdvancedAction->setEnabled(basicSettingsPageAvailable_
            && runExpressAvailable_
            && runAdvancedAvailable_
            && (currentStatus() == Defs::CurrStatusReady
                || (currentStatus() == Defs::CurrStatusRun
                    && currentRunStatus() == Defs::CurrRunStatusAdv)
                || (currentStatus() == Defs::CurrStatusPause
                    && currentRunStatus() == Defs::CurrRunStatusAdv)));
        runRetrieverAction->setEnabled(runExpressAction->isEnabled()
            && !ecProject_->generalUseAltMdFile()
            && (currentStatus() == Defs::CurrStatusReady
                || (currentStatus() == Defs::CurrStatusRun
                    && currentRunStatus() == Defs::CurrRunStatusRet)
                || (currentStatus() == Defs::CurrStatusPause
                    && currentRunStatus() == Defs::CurrRunStatusRet)));
        stopAction->setEnabled(basicSettingsPageAvailable_
            && (runExpressAvailable_ || runAdvancedAvailable_));
    }
}

void MainWindow::changePage(Defs::CurrPage page, bool testCurrentPage)
{
    DEBUG_FUNC_NAME

    qDebug() << "dlProject_" << dlProject_;

    if (testCurrentPage)
    {
        if (mainWidget_->currentPage() != page)
        {
            mainWidget_->setCurrentPage(page);
            updatePage(page);
            windowTitleUpdate(page);

            if (page == Defs::CurrPageRun)
            {
                consoleDock->raise();
            }
        }
    }
    else
    {
        mainWidget_->setCurrentPage(page);
        updatePage(page);
        windowTitleUpdate(page);
    }

}

void MainWindow::updatePage(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME
    qDebug() << "dlProject_" << dlProject_;

    if (page == Defs::CurrPageBasicSettings)
    {
        emit updateMetadataReadRequest();
    }
    previousPage_ = currentPage();
    setCurrentPage(page);
    updateMenuActionStatus(page);
    changeViewToolbarSeparators(page);
    togglePageButton(page);
}

void MainWindow::togglePageButton(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME

    switch (page)
    {
        case Defs::CurrPageWelcome:
            if (!viewWelcomeAction->isChecked())
                viewWelcomeAction->setChecked(true);
            break;
        case Defs::CurrPageProjectCreation:
            if (!viewProjectCreationAction->isChecked())
                viewProjectCreationAction->setChecked(true);
            break;
        case Defs::CurrPageBasicSettings:
            if (!viewBasicSettingsAction->isChecked())
                viewBasicSettingsAction->setChecked(true);
            break;
        case Defs::CurrPageAdvancedSettings:
            if (!viewAdvancedAction->isChecked())
                viewAdvancedAction->setChecked(true);
            break;
        case Defs::CurrPageRun:
            if (!viewRunPageAction->isChecked())
                viewRunPageAction->setChecked(true);
            break;
        default:
            break;
    }
}

void MainWindow::changeViewToolbarSeparators(Defs::CurrPage page)
{
    DEBUG_FUNC_NAME

    switch (page)
    {
        case Defs::CurrPageWelcome:
            sep1->setPixmap(QPixmap(QStringLiteral(":/icons/sepleftsel")));
            sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep4->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            connect(sep1, SIGNAL(clicked()), viewProjectCreationAction, SLOT(trigger()));
            sep2->disconnect();
            sep3->disconnect();
            sep4->disconnect();
            break;
        case Defs::CurrPageProjectCreation:
            sep1->setPixmap(QPixmap(QStringLiteral(":/icons/seprightsel")));
            sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sepleftsel")));
            sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep4->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep1->disconnect();
            if (basicSettingsPageAvailable_)
            {
                connect(sep2, SIGNAL(clicked()), viewBasicSettingsAction, SLOT(trigger()));
                connect(sep3, SIGNAL(clicked()), viewAdvancedAction, SLOT(trigger()));
            }
            else
            {
                sep2->disconnect();
                sep3->disconnect();
            }
            connect(sep4, SIGNAL(clicked()), viewRunPageAction, SLOT(trigger()));
            break;
        case Defs::CurrPageBasicSettings:
            sep1->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep2->setPixmap(QPixmap(QStringLiteral(":/icons/seprightsel")));
            sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sepleftsel")));
            sep4->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            connect(sep1, SIGNAL(clicked()), viewBasicSettingsAction, SLOT(trigger()));
            sep2->disconnect();
            connect(sep3, SIGNAL(clicked()), viewAdvancedAction, SLOT(trigger()));
            connect(sep4, SIGNAL(clicked()), viewRunPageAction, SLOT(trigger()));
            break;
        case Defs::CurrPageAdvancedSettings:
            sep1->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep3->setPixmap(QPixmap(QStringLiteral(":/icons/seprightsel")));
            sep4->setPixmap(QPixmap(QStringLiteral(":/icons/sepleftsel")));
            connect(sep1, SIGNAL(clicked()), viewProjectCreationAction, SLOT(trigger()));
            connect(sep2, SIGNAL(clicked()), viewBasicSettingsAction, SLOT(trigger()));
            sep3->disconnect();
            connect(sep4, SIGNAL(clicked()), viewRunPageAction, SLOT(trigger()));
            break;
        case Defs::CurrPageRun:
            sep1->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
            sep4->setPixmap(QPixmap(QStringLiteral(":/icons/seprightsel")));
            connect(sep1, SIGNAL(clicked()), viewProjectCreationAction, SLOT(trigger()));
            connect(sep2, SIGNAL(clicked()), viewBasicSettingsAction, SLOT(trigger()));
            connect(sep3, SIGNAL(clicked()), viewAdvancedAction, SLOT(trigger()));
            sep4->disconnect();
            break;
        default:
            break;
    }
}

void MainWindow::fileOpenRequest(QString file)
{
    DEBUG_FUNC_NAME
    qDebug() << "from mainPage";

    if (configState_.project.smartfluxMode
            && !file.isEmpty())
    {
        loadSmartfluxProjectCopy(file);
    }
    else
    {
        fileOpen(file);
    }
}

void MainWindow::fileNewRequest()
{
    DEBUG_FUNC_NAME
    qDebug() << "from mainPage";

    fileNew();
}

void MainWindow::createEngineProcess()
{
    // create the engine sub-process
    engineProcess_ = new Process(this);
    engineProcess_->setChannelsMode(QProcess::MergedChannels);
    engineProcess_->setReadChannels(QProcess::StandardOutput);

    // add useful GFortran environment variables
    QStringList env = QProcess::systemEnvironment();
    env << QStringLiteral("GFORTRAN_UNBUFFERED_PRECONNECTED=y");
    env << QStringLiteral("GFORTRAN_SHOW_LOCUS=n");
    engineProcess_->setEnv(env);

    connect(engineProcess_, SIGNAL(processFailure()),
            this, SLOT(displayExitDialog()));
    connect(engineProcess_, SIGNAL(processSuccess()),
            this, SLOT(displayExitDialog()));

    connect(engineProcess_, SIGNAL(processFailure()),
            mainWidget_->runPage(), SLOT(resetBuffer()));
    connect(engineProcess_, SIGNAL(processSuccess()),
            mainWidget_->runPage(), SLOT(resetBuffer()));

    connect(engineProcess_, SIGNAL(readyReadStdOut()),
            this, SLOT(updateConsoleReceived()));
    connect(engineProcess_, SIGNAL(readyReadStdErr()),
            this, SLOT(updateConsoleError()));
}

void MainWindow::setMetadataRead(bool b)
{
    metadataReadFlag_ = b;
}

int MainWindow::testBeforeRunningPassed(int step)
{
    DEBUG_FUNC_NAME

    int returnValue = QMessageBox::Yes;

    qDebug() << "metadataReadFlag_" << metadataReadFlag_;
    if (!metadataReadFlag_)
    {
        qDebug() << "md read";
        changePage(Defs::CurrPageBasicSettings);
    }
    qDebug() << "metadataReadFlag_" << metadataReadFlag_;

    qDebug() << "ecProject_->spectraExDir()" << ecProject_->spectraExDir();
    if (step == 0 && !ecProject_->spectraExDir().isEmpty())
    {
        if (expressClicked_)
        {
            qDebug() << "expressClicked_";

            if (testForPreviousData())
            {
                qDebug() << "previous data test";
                int ret = QMessageBox::question(QApplication::activeWindow(),
                    tr("Previous Results Available"),
                    tr("Previous results available!\n\n"
                       "Most likely results of this run will be identical "
                       "to results of a previous run available in the \"Previous "
                       "Output Directory\", so you wouldn't need to proceed.\n\n"
                       "Do you prefer to proceed anyway?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No);

                switch (ret)
                {
                    case QMessageBox::Yes:
                        break;
                    case QMessageBox::No:
                        expressClicked_ = false;
                        showStatusTip(tr("Canceled..."));
                        saveSilently();
                        return QMessageBox::Cancel;
                }
            }
            else
            {
                int ret = QMessageBox::warning(QApplication::activeWindow(),
                              tr("No Previous Results Available"),
                              tr("It is not possible to use results from any previous run. "
                                 "EddyPro will start the processing from the raw files."),
                              QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

                switch (ret)
                {
                    case QMessageBox::Ok:
                        break;
                    case QMessageBox::Cancel:
                        expressClicked_ = false;
                        showStatusTip(tr("Canceled..."));
                        saveSilently();
                        return ret;
                }
            }
        }
        else if (advancedClicked_)
        {
            if (testForPreviousData())
            {
                qDebug() << "advancedClicked_";
                int ret = QMessageBox::question(QApplication::activeWindow(),
                                     tr("Previous Results Available"),
                                     tr("Previous results available!\n\n"
                                        "Do you want to proceed using results "
                                        "from a previous run? (Recommended)\n"),
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                     QMessageBox::Yes);
                switch (ret)
                {
                    case QMessageBox::Yes:
                        returnValue = QMessageBox::No;
                        break;
                    case QMessageBox::No:
                        advancedClicked_ = false;
                        returnValue = QMessageBox::Yes;
                        break;
                    case QMessageBox::Cancel:
                        advancedClicked_ = false;
                        showStatusTip(tr("Canceled..."));
                        saveSilently();
                        return ret;
                }
            }
            else
            {
                int ret = QMessageBox::warning(QApplication::activeWindow(),
                              tr("No Previous Results Available"),
                              tr("It is not possible to use results from any previous run. "
                                 "EddyPro will start the processing from the raw files."),
                              QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);

                switch (ret)
                {
                    case QMessageBox::Ok:
                        break;
                    case QMessageBox::Cancel:
                        advancedClicked_ = false;
                        showStatusTip(tr("Canceled..."));
                        saveSilently();
                        return ret;
                }
            }
        }
        else if (retrieverClicked_)
        {
            // nothing to do
        }
    }

    if (saveSilently() == QMessageBox::No)
    {
        return QMessageBox::No;
    }
    else
    {
        return returnValue;
    }
}

int MainWindow::saveSilently()
{
    DEBUG_FUNC_NAME

    if (modifiedFlag_)
    {
        // silent saving
        fileSave();

        // if saving failed
        if (modifiedFlag_)
        {
            QMessageBox::warning(QApplication::activeWindow(),
                                 Defs::APP_NAME,
                                 tr("The program was unable to save "
                                    "the project file. "
                                    "Make sure the file is not in use "
                                    "by another application."
                                    "If the problem persists, contact envsupport@licor.com ."));
            return QMessageBox::No;
        }
    }
    return QMessageBox::Yes;
}

void MainWindow::getRunExpress()
{
    DEBUG_FUNC_NAME

    bool showDialog = false;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        showDialog = config.value(Defs::CONF_WIN_RUN_EXP_MSG, true).toBool();
    config.endGroup();

    qDebug() << "showDialog" << showDialog;

    if (showDialog && currentStatus() == Defs::CurrStatusReady)
    {
        qDebug() << "create dialog";
        InfoMessage runExpressDialog(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel, this);
        runExpressDialog.setTitle(tr("Running"));
        runExpressDialog.setType(InfoMessage::RUN_EXPRESS);
        runExpressDialog.setMessage(tr("Note that running in Express Mode means "
                                      "EddyPro will ignore all your entries in "
                                      "the Advanced Settings pages. <br />"
                                      "Your settings will not be overridden "
                                      "and you will be able to retrieve them "
                                      "at any time, but they will not be used "
                                      "for this run.<br />"
                                      "Do you really want to run in Express "
                                      "mode?"));
        runExpressDialog.refresh();

        if (runExpressDialog.exec() == QMessageBox::Cancel)
        {
            return;
        }
    }
    emit updateMetadataReadRequest();
    runExpress();
}

void MainWindow::runExpress()
{
    DEBUG_FUNC_NAME

    Defs::CurrStatus status = currentStatus();
    expressClicked_ = true;

    if (status == Defs::CurrStatusReady)
    {
        if (testBeforeRunningPassed(0) == QMessageBox::Yes)
        {
            changePage(Defs::CurrPageRun);
            QString workingDir = QCoreApplication::applicationDirPath()
                                 + QStringLiteral("/")
                                 + Defs::BIN_FILE_DIR
                                 + QStringLiteral("/");
            QString engineFilePath(workingDir + Defs::ENGINE_1_BIN);
            qDebug() << "engineFilePath" << engineFilePath;

            ecProject_->setGeneralRunMode(Defs::CurrRunModeExp);
            fileSave();

            QStringList args;
            args << QStringLiteral("-s");
            args << Defs::HOST_OS;
            args << QStringLiteral("-e");
            args << appEnvPath_;
            engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

            if (engineProcess_->isRunning())
            {
                setCurrentStatus(Defs::CurrStatusRun);
                setCurrentRunStatus(Defs::CurrRunStatusExp);
                mainWidget_->runPage()->startRun(Defs::CurrRunStatusExp);
                setRunExpIcon2Pause();
            }
        }
        // case QMessageBox::Cancel
        else
        {
            return;
        }
    }
    else if (currentStatus() == Defs::CurrStatusRun)
    {
        if (pauseEngine(Defs::CurrRunStatusExp))
        {
            setCurrentStatus(Defs::CurrStatusPause);
            engineResumableFlag_ = true;
            setRunExpIcon2Resume();
        }
    }
    else if (currentStatus() == Defs::CurrStatusPause)
    {
        if (resumeEngine(Defs::CurrRunStatusExp))
        {
            setCurrentStatus(Defs::CurrStatusRun);
            engineResumableFlag_ = false;
            setRunExpIcon2Pause();
        }
    }
}

void MainWindow::getRunAdvanced()
{
    DEBUG_FUNC_NAME

    bool showDialog = false;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        showDialog = config.value(Defs::CONF_WIN_RUN_ADV_MSG, true).toBool();
    config.endGroup();

    if (showDialog && currentStatus() == Defs::CurrStatusReady)
    {
        InfoMessage runAdvancedDialog(QDialogButtonBox::Ok
                                       | QDialogButtonBox::Cancel, this);
        runAdvancedDialog.setTitle(tr("Running"));
        runAdvancedDialog.setType(InfoMessage::RUN_ADVANCED);
        runAdvancedDialog.setMessage(tr("Running in Advanced mode means you "
                                       "run using the <br />"
                                       "advanced settings of EddyPro"));
        runAdvancedDialog.refresh();

        if (runAdvancedDialog.exec() == QMessageBox::Cancel)
        {
            qDebug() << "runAdvancedDialog_.exec()" << runAdvancedDialog.exec();
            return;
        }
    }
    emit updateMetadataReadRequest();
    runAdvancedStep_1();
}

void MainWindow::runAdvancedStep_1()
{
    DEBUG_FUNC_NAME

    int status = currentStatus();

    advancedClicked_ = true;

    if (status == Defs::CurrStatusReady)
    {
        int ret = testBeforeRunningPassed(0);

        // start from step 1
        if (ret == QMessageBox::Yes)
        {
            qDebug() << "go to step 1 (rp)";
            changePage(Defs::CurrPageRun);
            QString workingDir = QCoreApplication::applicationDirPath()
                                 + QStringLiteral("/")
                                 + Defs::BIN_FILE_DIR
                                 + QStringLiteral("/");
            QString engine1FilePath(workingDir + Defs::ENGINE_1_BIN);

            ecProject_->setGeneralRunMode(Defs::CurrRunModeAdv);
            fileSave();

            QStringList args;
            args << QStringLiteral("-s");
            args << Defs::HOST_OS;
            args << QStringLiteral("-e");
            args << appEnvPath_;
            engineProcess_->engineProcessStart(engine1FilePath, workingDir, args);

            qDebug() << "engineFilePath" << engine1FilePath;
            if (engineProcess_->isRunning())
            {
                setCurrentStatus(Defs::CurrStatusRun);
                setCurrentRunStatus(Defs::CurrRunStatusAdv);
                mainWidget_->runPage()->startRun(Defs::CurrRunStatusAdv);
                setRunAdvIcon2Pause();

                neededEngineStep2_ = ecProject_->isEngineStep2Needed();
            }
        }
        // jump to step 2
        else if (ret == QMessageBox::No)
        {
            qDebug() << "go to step 2 (fcc)";
            runAdvancedStep_2();
            return;
        }
        // case QMessageBox::Cancel
        else
        {
            qDebug() << "stop: run canceled";
            return;
        }
    }
    else if (currentStatus() == Defs::CurrStatusRun)
    {
        if (pauseEngine(Defs::CurrRunStatusAdv))
        {
            setCurrentStatus(Defs::CurrStatusPause);
            engineResumableFlag_ = true;
            setRunAdvIcon2Resume();
        }
    }
    else if (currentStatus() == Defs::CurrStatusPause)
    {
        if (resumeEngine(Defs::CurrRunStatusAdv))
        {
            setCurrentStatus(Defs::CurrStatusRun);
            engineResumableFlag_ = false;
            setRunAdvIcon2Pause();
        }
    }
}

void MainWindow::runAdvancedStep_2()
{
    DEBUG_FUNC_NAME

    int status = currentStatus();

    advancedClicked_ = true;

    if (status == Defs::CurrStatusReady)
    {
        if (testBeforeRunningPassed(1) == QMessageBox::Yes)
        {
            changePage(Defs::CurrPageRun);
            QString workingDir = QCoreApplication::applicationDirPath()
                                 + QStringLiteral("/")
                                 + Defs::BIN_FILE_DIR
                                 + QStringLiteral("/");
            QString engineFilePath(workingDir + Defs::ENGINE_2_BIN);

            ecProject_->setGeneralRunMode(Defs::CurrRunModeAdv);
            fileSave();

            QStringList args;
            args << QStringLiteral("-s");
            args << Defs::HOST_OS;
            args << QStringLiteral("-e");
            args << appEnvPath_;
            engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

            qDebug() << "engineFilePath" << engineFilePath;
            if (engineProcess_->isRunning())
            {
                setCurrentStatus(Defs::CurrStatusRun);
                setCurrentRunStatus(Defs::CurrRunStatusAdv);
                mainWidget_->runPage()->startRun(Defs::CurrRunStatusAdv);
                setRunAdvIcon2Pause();
                neededEngineStep2_ = false;
            }
        }
        // case QMessageBox::Cancel
        else
        {
            return;
        }
    }
    else if (currentStatus() == Defs::CurrStatusRun)
    {
        if (pauseEngine(Defs::CurrRunStatusAdv))
        {
            setCurrentStatus(Defs::CurrStatusPause);
            engineResumableFlag_ = true;
            setRunAdvIcon2Resume();
        }
    }
    else if (currentStatus() == Defs::CurrStatusPause)
    {
        if (resumeEngine(Defs::CurrRunStatusAdv))
        {
            setCurrentStatus(Defs::CurrStatusRun);
            engineResumableFlag_ = false;
            setRunAdvIcon2Pause();
        }
    }
}

void MainWindow::getRunRetriever()
{
    bool showDialog = false;

    QSettings config;
    config.beginGroup(Defs::CONFGROUP_WINDOW);
        showDialog = config.value(Defs::CONF_WIN_RUN_RET_MSG, true).toBool();
    config.endGroup();

    if (showDialog && currentStatus() == Defs::CurrStatusReady)
    {
        qDebug() << "create dialog";
        InfoMessage runRetrieverDialog(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Cancel, this);
        runRetrieverDialog.setTitle(tr("Running"));
        runRetrieverDialog.setType(InfoMessage::RUN_RETRIEVER);
        runRetrieverDialog.setMessage(tr("The metadata retriever is a running mode. "
                                        "It will assess the metadata in all GHG "
                                        "files and provide the metadata in a single "
                                        "alternative metadata file that you can "
                                        "examine and edit.<br />"
                                        "Do you really want to run the Metadata "
                                        "Retriever?"));
        runRetrieverDialog.refresh();

        if (runRetrieverDialog.exec() == QMessageBox::Cancel)
        {
            return;
        }
        emit updateMetadataReadRequest();
    }
    runRetriever();
}

void MainWindow::runRetriever()
{
    Defs::CurrStatus status = currentStatus();
    retrieverClicked_ = true;

    if (status == Defs::CurrStatusReady)
    {
        if (testBeforeRunningPassed(0) == QMessageBox::Yes)
        {
            changePage(Defs::CurrPageRun);
            QString workingDir = QCoreApplication::applicationDirPath()
                                 + QStringLiteral("/")
                                 + Defs::BIN_FILE_DIR
                                 + QStringLiteral("/");
            QString engineFilePath(workingDir + Defs::ENGINE_1_BIN);

            ecProject_->setGeneralRunMode(Defs::CurrRunModeRet);
            emit checkMetadataOutputRequest();
            fileSave();

            QStringList args;
            args << QStringLiteral("-s");
            args << Defs::HOST_OS;
            args << QStringLiteral("-e");
            args << appEnvPath_;
            engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

            qDebug() << "engineFilePath" << engineFilePath;
            if (engineProcess_->isRunning())
            {
                setCurrentStatus(Defs::CurrStatusRun);
                setCurrentRunStatus(Defs::CurrRunStatusRet);
                mainWidget_->runPage()->startRun(Defs::CurrRunStatusRet);
                setRunRetIcon2Pause();
            }
        }
        // case QMessageBox::Cancel
        else
        {
            return;
        }
    }
    else if (currentStatus() == Defs::CurrStatusRun)
    {
        if (pauseEngine(Defs::CurrRunStatusRet))
        {
            setCurrentStatus(Defs::CurrStatusPause);
            engineResumableFlag_ = true;
            setRunRetIcon2Resume();
        }
    }
    else if (currentStatus() == Defs::CurrStatusPause)
    {
        if (resumeEngine(Defs::CurrRunStatusRet))
        {
            setCurrentStatus(Defs::CurrStatusRun);
            engineResumableFlag_ = false;
            setRunRetIcon2Pause();
        }
    }
}

bool MainWindow::pauseEngine(Defs::CurrRunStatus mode)
{
    if (mainWidget_->runPage()->pauseRun(mode))
    {
        engineProcess_->processPause(mode);
        return true;
    }
    return false;
}

bool MainWindow::resumeEngine(Defs::CurrRunStatus mode)
{
    if (mainWidget_->runPage()->resumeRun(mode))
    {
        engineProcess_->processResume(mode);
        return true;
    }
    return false;
}

void MainWindow::stopEngine()
{
    if (engineProcess_->isRunning())
    {
        if (queryIfStopRun() == QMessageBox::Yes)
        {
            stopEngineProcess();
        }
    }
}

void MainWindow::stopEngineProcess()
{
    engineProcess_->processStop();

    ecProject_->setGeneralRunMode(Defs::CurrRunModeAdv);

    displayExitDialog();
    fileSave();

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;
}

void MainWindow::resetRunIcons()
{
    runExpressAction->setIcon(QIcon(QStringLiteral(":/icons/run-exp")));
    runAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/run-adv")));
    runRetrieverAction->setIcon(QIcon(QStringLiteral(":/icons/run-ret")));

    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop")));
}

void MainWindow::setRunExpIcon2Resume()
{
    runExpressAction->setIcon(QIcon(QStringLiteral(":/icons/resume-exp")));
    runAdvancedAction->setEnabled(false);
    runRetrieverAction->setEnabled(false);
}

void MainWindow::setRunExpIcon2Pause()
{
    runExpressAction->setIcon(QIcon(QStringLiteral(":/icons/pause-exp")));
    runAdvancedAction->setEnabled(false);
    runRetrieverAction->setEnabled(false);
    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop-red")));
}

void MainWindow::setRunAdvIcon2Resume()
{
    runExpressAction->setEnabled(false);
    runAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/resume-adv")));
    runRetrieverAction->setEnabled(false);
}

void MainWindow::setRunAdvIcon2Pause()
{
    runExpressAction->setEnabled(false);
    runAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/pause-adv")));
    runRetrieverAction->setEnabled(false);
    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop-red")));
}

void MainWindow::setRunRetIcon2Resume()
{
    runExpressAction->setEnabled(false);
    runAdvancedAction->setEnabled(false);
    runRetrieverAction->setIcon(QIcon(QStringLiteral(":/icons/resume-ret")));
}

void MainWindow::setRunRetIcon2Pause()
{
    runExpressAction->setEnabled(false);
    runAdvancedAction->setEnabled(false);
    runRetrieverAction->setIcon(QIcon(QStringLiteral(":/icons/pause-ret")));
    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop-red")));
}


void MainWindow::displayExitDialog()
{
    updateConsoleReceived();
    setCurrentStatus(Defs::CurrStatusReady);
    setCurrentRunStatus(Defs::CurrRunStatusExp);
    mainWidget_->runPage()->stopRun();

    if (!neededEngineStep2_)
    {
        resetRunIcons();
        updateMenuActionStatus(currentPage());
        qDebug() << "engineProcess_->processExit()" << engineProcess_->processExit();
        displayExitMsg(engineProcess_->processExit());
    }
    else
    {
        displayExitMsg2(engineProcess_->processExit());
    }
}

void MainWindow::displayExitMsg(Process::ExitStatus exitReason)
{
    DEBUG_FUNC_NAME
    QMessageBox msgBox(this);

    // remove the context help button
    Qt::WindowFlags winFflags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint;
    msgBox.setWindowFlags(winFflags);
    winFflags = msgBox.windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    msgBox.setWindowFlags(winFflags);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setWindowTitle(tr("EddyPro Results"));

    QPushButton *openOutDirButton = new QPushButton(tr("Open the output folder"));
    QPushButton *questionMark_1 = new QPushButton();
    questionMark_1->setIcon(QIcon(QStringLiteral(":/icons/qm-enabled")));
    questionMark_1->setFixedSize(77, 25);

    connect(questionMark_1, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_1()));

    QProcess::ProcessError last_error = engineProcess_->processError();

    switch (exitReason)
    {
    case Process::exitSuccess:
        msgBox.setText(tr("<b>Data processed successfully!"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.addButton(openOutDirButton, QMessageBox::ActionRole);
        msgBox.setDefaultButton(openOutDirButton);
        break;
    case Process::exitFailureToStart:
        msgBox.setText(tr("<b>Engine (eddypro_rp) not found!&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        break;
    case Process::exitFailure:
        msgBox.setText(tr("<b>Oops, an error has occurred.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        msgBox.setInformativeText(tr("<p>Double check the project parameters and check "
                                     "the output console for more information.</p>"));
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    case Process::exitError:
    {
        qDebug() << engineProcess_->processErrorString();
        msgBox.setText(tr("<b>Oops, an error has occurred.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        QString infoText;
        switch (last_error)
        {
            case QProcess::FailedToStart:
                infoText = tr("<p>Computations failed to start. Possible reasons are:"
                              "<ul>"
                              "<li>You may have insufficient (virtual) memory</li>"
                              "<li>You may have insufficient permissions</li>"
                              // should be already detected
                              // "<li>You miss the engine executable</li>"
                              "</ul>"
                              "</p>");
                break;
            case QProcess::Crashed:
                infoText = tr("<p>Computations crashed.</p>");
                break;
            case QProcess::Timedout:
                infoText = tr("<p>Computations timed out.</p>");
                break;
            case QProcess::WriteError:
                infoText = tr("<p>Computations write error.</p>");
                break;
            case QProcess::ReadError:
                infoText = tr("<p>Computations read error.</p>");
                break;
            case QProcess::UnknownError:
                infoText = tr("<p>Computations unknown error.</p>");
                break;
        }
        infoText += tr("<p>Double check the project parameters and check "
                       "the output console for more information.</p>");
        msgBox.setInformativeText(infoText);
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    }
    case Process::exitStopped:
        msgBox.setText(tr("<b>Program stopped!&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><br /><br />"
                          "Preliminary results have been stored "
                          "in the selected output folder. Depending on your "
                          "processing settings, these results may be fully "
                          "valid for analysis. You can remove the temporary "
                          "extension \".tmp\" and use them. However, the "
                          "format of these files does not allow EddyPro to "
                          "use them in subsequent runs.</p>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        break;
    }
    msgBox.exec();

    if (msgBox.clickedButton() == openOutDirButton)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(ecProject_->generalOutPath()));
    }
    if (openOutDirButton)
    {
        qDebug() << "delete openOutDirButton";
        delete openOutDirButton;
    }
    if (questionMark_1)
    {
        qDebug() << "delete questionMark_1";
        delete questionMark_1;
    }

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;
}

void MainWindow::displayExitMsg2(Process::ExitStatus exitReason)
{
    DEBUG_FUNC_NAME
    QMessageBox msgBox(this);

    // remove the context help button
    Qt::WindowFlags winFflags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint;
    msgBox.setWindowFlags(winFflags);
    winFflags = msgBox.windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    msgBox.setWindowFlags(winFflags);
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setWindowTitle(tr("EddyPro Results"));

    QPushButton *questionMark_1 = new QPushButton();
    questionMark_1->setIcon(QIcon(QStringLiteral(":/icons/qm-enabled")));
    questionMark_1->setFixedSize(77, 25);

    connect(questionMark_1, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_1()));

    QProcess::ProcessError last_error = engineProcess_->processError();

    switch (exitReason)
    {
    case Process::exitSuccess:
        updateSpectraPaths();
        runAdvancedStep_2();
        break;
    case Process::exitFailureToStart:
        msgBox.setText(tr("<b>Engine (eddypro_fcc) not found!&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        break;
    case Process::exitFailure:
        msgBox.setText(tr("<b>Oops, an error has occurred.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        msgBox.setInformativeText(tr("<p>Double check the project parameters and check "
                                     "the output console for more information.</p>"));
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    case Process::exitError:
    {
        qDebug() << engineProcess_->processErrorString();
        msgBox.setText(tr("<b>Oops, an error has occurred.&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                          "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b>"));
        QString infoText;
        switch (last_error)
        {
            case QProcess::FailedToStart:
                infoText = tr("<p>Computations failed to start. Possible reasons are:"
                              "<ul>"
                              "<li>You may have insufficient (virtual) memory</li>"
                              "<li>You may have insufficient permissions</li>"
                              // should be already detected
                              // "<li>You miss the engine executable</li>"
                              "</ul>"
                              "</p>");
                break;
            case QProcess::Crashed:
                infoText = tr("<p>Computations crashed.</p>");
                break;
            case QProcess::Timedout:
                infoText = tr("<p>Computations timed out.</p>");
                break;
            case QProcess::WriteError:
                infoText = tr("<p>Computations write error.</p>");
                break;
            case QProcess::ReadError:
                infoText = tr("<p>Computations read error.</p>");
                break;
            case QProcess::UnknownError:
                infoText = tr("<p>Computations unknown error.</p>");
                break;
        }
        infoText += tr("<p>Double check the project parameters and check "
                       "the output console for more information.</p>");
        msgBox.setInformativeText(infoText);
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    }
    case Process::exitStopped:
        msgBox.setText(tr("<b>Program stopped!&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</b><br />"
                          "Preliminary results have been stored "
                          "in the selected output folder. Depending on your "
                          "processing settings, these results may be fully "
                          "valid for analysis. You can remove the temporary "
                          "extension \".tmp\" and use them. However, the "
                          "format of these files does not allow EddyPro to "
                          "use them in subsequent runs.</p>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        break;
    }

    if (exitReason != Process::exitSuccess)
    {
        resetRunIcons();
        updateMenuActionStatus(currentPage());
        msgBox.exec();
    }
    if (questionMark_1)
    {
        qDebug() << "delete questionMark_1";
        delete questionMark_1;
    }

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;
}

void MainWindow::onlineHelpTrigger_1()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Error_Codes.htm")));
}

// qt5
void MainWindow::updateConsole(QByteArray &data)
{
    consoleOutput->appendPlainText(QLatin1String(data.trimmed().constData()));
}

void MainWindow::updateConsoleReceived()
{
    QByteArray newData = engineProcess_->readAllStdOut();

    if (!newData.isEmpty())
    {
        mainWidget_->runPage()->bufferData(newData);
    }
}

void MainWindow::updateConsoleError()
{
    DEBUG_FUNC_NAME
    QByteArray newData = engineProcess_->readAllStdErr();

    if (!newData.isEmpty())
    {
        updateConsole(newData);
    }
}

int MainWindow::queryIfStopRun()
{
    return QMessageBox::question(0,
                        tr("Stop Data Processing"),
                        tr("<p>Do you really want to "
                           "stop data processing?</p>"),
                        QMessageBox::Yes,
                        QMessageBox::Cancel);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    DEBUG_FUNC_NAME

    QSize widgetSize = event->size();

    qDebug() << widgetSize.width() << " x " << widgetSize.height();

    if (widgetSize.width() < 1200 || widgetSize.height() < 630)
    {
        fileToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        viewToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        runToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

        mainWidget_->startPage()->updateWelcomePage(true);

        configState_.general.recentnum = 2;
    }
    else
    {
        fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        viewToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        runToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        mainWidget_->startPage()->updateWelcomePage(false);

        updateMenuActionStatus(currentPage());

        configState_.general.recentnum = 4;
    }

    updateStatusBar();

    event->accept();
}

QString MainWindow::grabEssentialPath(const QString& fileName)
{
    DEBUG_FUNC_NAME

    qDebug() << "fileName" << fileName;

    // open ini file
    QFile dataFile(fileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // error opening file
        qDebug() << "Error: Cannot open" << fileName << "file";
        return QString();
    }

    // read ini file
    QSettings logIni(fileName, QSettings::IniFormat);
    QString essentialPath = logIni.value(QStringLiteral("ex_file")).toString();
    dataFile.close();

    QString canonicalPath = QDir::fromNativeSeparators(essentialPath);
    qDebug() << canonicalPath;

    if (canonicalPath.endsWith(QStringLiteral(".tmp")))
        canonicalPath.chop(4);

    return canonicalPath;
}

void MainWindow::updateSpectraPaths()
{
    DEBUG_FUNC_NAME

    QString logPath = appEnvPath_ + QStringLiteral("/") + Defs::ENGINE_RP_LOGFILE;
    QString exFilePath = grabEssentialPath(logPath);
    ecProject_->setSpectraExFile(exFilePath);

    if (ecProject_->generalBinSpectraAvail())
    {
        ecProject_->setSpectraBinSpectra(ecProject_->spectraBinSpectra());
    }
    else
    {
        ecProject_->setSpectraBinSpectra(ecProject_->generalOutPath() + QStringLiteral("/eddypro_binned_cospectra"));
    }

    if (ecProject_->generalFullSpectraAvail())
    {
        ecProject_->setSpectraFullSpectra(ecProject_->spectraFullSpectra());
    }
    else
    {
        ecProject_->setSpectraFullSpectra(ecProject_->generalOutPath() + QStringLiteral("/eddypro_full_cospectra"));
    }

    fileSave();
}

void MainWindow::updateSpectraPathFromPreviousData(const QString& exFilePath)
{
    DEBUG_FUNC_NAME

    qDebug() << exFilePath;
    ecProject_->setSpectraExFile(exFilePath);

    if (ecProject_->generalHfMethod() == 2
        || ecProject_->generalHfMethod() == 3
        || ecProject_->generalHfMethod() == 4)
    {
        if (ecProject_->generalBinSpectraAvail())
        {
            ecProject_->setSpectraBinSpectra(ecProject_->spectraBinSpectra());
        }

        if (ecProject_->generalHfMethod() == 4)
        {
            if (ecProject_->generalFullSpectraAvail())
            {
                ecProject_->setSpectraFullSpectra(ecProject_->spectraFullSpectra());
            }
        }
    }
}

void MainWindow::showUpdateDialog()
{
    DEBUG_FUNC_NAME

    if (!updateDialog)
    {
        updateDialog = new UpdateDialog(this);
    }
    updateDialog->initialize();
    updateDialog->show();
    updateDialog->raise();
    updateDialog->activateWindow();
    updateDialog->checkUpdate();
}

void MainWindow::showAutoUpdateDialog()
{
    DEBUG_FUNC_NAME

    if (!updateDialog)
    {
        updateDialog = new UpdateDialog(this);
    }
    updateDialog->initialize();
    updateDialog->checkUpdate();

    QTimer::singleShot(60000, this, SLOT(showAutoUpdateResults()));
}

void MainWindow::showAutoUpdateResults()
{
    DEBUG_FUNC_NAME
    if (updateDialog->hasNewVersion())
    {
        qDebug() << "yes NEW VERSION";
        updateDialog->show();
        updateDialog->raise();
        updateDialog->activateWindow();
    }
    else
    {
        qDebug() << "no NEW VERSION";
    }
}

bool MainWindow::testForPreviousData()
{
    DEBUG_FUNC_NAME

    bool test = false;

    // first preliminary test
    if ((ecProject_->generalHfMethod() == 2 || ecProject_->generalHfMethod() == 3)
        && (ecProject_->spectraMode() == 1 && ecProject_->generalBinSpectraAvail() == 0))
    {
        qDebug() << "failed first preliminary test";
        return test;
    }

    // second preliminary test
    if (ecProject_->generalHfMethod() == 4
        && ((ecProject_->spectraMode() == 1 && ecProject_->generalBinSpectraAvail() == 0)
        || ecProject_->generalFullSpectraAvail() == 0))
    {
        qDebug() << "second first preliminary test";
        return test;
    }

    QString epFormat = QStringLiteral("*.") + Defs::APP_NAME_LCASE;
    QString csvFormat = QStringLiteral("*.") + Defs::CSV_NATIVE_DATA_FILE_EXT;

    QStringList previousRunList(FileUtils::getFiles(ecProject_->spectraExDir(),
                                         epFormat,
                                         true));
    qDebug() << "previousRunList" << previousRunList << previousRunList.count();

    QStringList previousEssentialList(FileUtils::getFiles(ecProject_->spectraExDir(),
                                         csvFormat,
                                         true));
    qDebug() << previousEssentialList.count();

    previousEssentialList = previousEssentialList.filter(QStringLiteral("essentials"));
    qDebug() << "previousEssentialList" << previousEssentialList << previousEssentialList.count();

    // prunes the previousRunList if there are no corresponding essential files
    foreach (const QString& processingFile, previousRunList)
    {
        QFileInfo info(processingFile);
        QString filenameDate = info.fileName().mid(11, 17);

        qDebug() << "info.fileName()" << info.fileName();
        qDebug() << "filenameDate" << filenameDate;

        if (!StringUtils::isISODateTimeString(filenameDate)
            || previousEssentialList.filter(filenameDate).isEmpty())
        {
            qDebug() << "essential fail";
            qDebug() << "remove processingFile" << processingFile;
            previousRunList.removeOne(processingFile);
        }
    }
    qDebug() << "previousRunList" << previousRunList << previousRunList.count();

    // load and compare each old project found with the current project
    ConfigState currConfigState;

    QScopedPointer<EcProject> currEcProject(new EcProject(this, currConfigState.project));
    QScopedPointer<EcProject> prevEcProject(new EcProject(this, currConfigState.project));
    int i = 0;
    foreach (const QString& processingFile, previousRunList)
    {
        bool modified; // not necessary in this case

        qDebug() << "begin iteration --------------------";
        if (currEcProject->loadEcProject(ecProject_->generalFileName(), false, &modified)
            && prevEcProject->loadEcProject(processingFile, false, &modified)
            && prevEcProject->generalRunMode() == Defs::CurrRunModeAdv)
        {
            qDebug() << "loading";
            if (currEcProject->fuzzyCompare(*prevEcProject))
            {
                qDebug() << "fuzzyCompare: OK";
                test = true;
                QFileInfo info(processingFile);
                qDebug() << "previousEssentialList.filter(info.fileName().mid(11, 17))"
                         << previousEssentialList.filter(info.fileName().mid(11, 17));
                qDebug() << "info.fileName().mid(11, 17)" << info.fileName().mid(11, 17);
                QString exFilePath = previousEssentialList.filter(info.fileName().mid(11, 17)).first();

                qDebug() << "exFilePath" << exFilePath;
                updateSpectraPathFromPreviousData(exFilePath);
                qDebug() << "end of the successful iteration --------------------";
                break;
            }
            else
            {
                qDebug() << "fuzzyCompare: FAIL";
            }
        }
        else
        {
            qDebug() << "loading FAIL";
        }

        ++i;
        qDebug() << "end iteration --------------------" << i;
    }

    return test;
}

void MainWindow::openLicorSite()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.licor.com/env/")));
}

void MainWindow::checkInternetConnection()
{
    DEBUG_FUNC_NAME
    qDebug() << Alia::isOnline_2();
}

// NOTE: hack to prevent that WindowStateChange events can change visibility of
// the console. Indeed those events are able to fire a toggled signals on the
// toggleConsoleOutputAct action and so hide the console
bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == this)
    {
        bool previousVisible = consoleDock->isVisible();

        if (e->type() == QEvent::WindowStateChange)
        {
            toggleConsoleOutputAct->setChecked(previousVisible);
            return true;
        }
    }

    return QMainWindow::eventFilter(o, e);
}

void MainWindow::connectBinarySettingsDialog()
{
    connect(mainWidget_->projectPage()->getBinarySettingsDialog(),
            SIGNAL(saveRequest()),
            this, SLOT(fileSave()));
}

void MainWindow::connectPlanarFitDialog()
{
    DEBUG_FUNC_NAME
    connect(mainWidget_->advancedPage()->advancedSettingPages()
            ->processingOptions()->getPlanarFitSettingsDialog(),
            SIGNAL(saveRequest()),
            this, SLOT(fileSave()));

    connect(mainWidget_->processingPage(),
            SIGNAL(setDateRangeRequest(QPair<QDateTime, QDateTime>)),
            mainWidget_->advancedPage()->advancedSettingPages()
            ->processingOptions()->getPlanarFitSettingsDialog(),
            SLOT(setDateRange(QPair<QDateTime, QDateTime>)));
}

void MainWindow::connectTimeLagDialog()
{
    DEBUG_FUNC_NAME
    connect(mainWidget_->advancedPage()->advancedSettingPages()
            ->processingOptions()->getTimeLagSettingsDialog(),
            SIGNAL(saveRequest()),
            this, SLOT(fileSave()));

    connect(mainWidget_->processingPage(),
            SIGNAL(setDateRangeRequest(QPair<QDateTime, QDateTime>)),
            mainWidget_->advancedPage()->advancedSettingPages()
            ->processingOptions()->getTimeLagSettingsDialog(),
            SLOT(setDateRange(QPair<QDateTime, QDateTime>)));
}

void MainWindow::wheelEvent(QWheelEvent * event)
{
    if (Qt::ControlModifier == QApplication::keyboardModifiers())
    {
        if (event->delta() > 0)
        {
            resize(width() + width() / 10, height() + height() / 10);
        }
        else
        {
            resize(width() - width() / 10, height() - height() / 10);
        }
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

// NOTE: to implement
void MainWindow::updateDockBorder(Qt::DockWidgetArea)
{
    DEBUG_FUNC_NAME

}
