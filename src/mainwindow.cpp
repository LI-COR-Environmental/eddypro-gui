/***************************************************************************
  mainwindow.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

#include "mainwindow.h"

#include <QApplication>
#include <QtConcurrent>
#include <QDebug>
#include <QDesktopServices>
#include <QDockWidget>
#include <QErrorMessage>
#include <QFileDialog>
#include <QFuture>
#include <QGridLayout>
#include <QMenuBar>
#include <QNetworkProxyFactory>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QScreen>
#include <QScrollBar>
#include <QStatusBar>
#include <QTextDocumentFragment>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QWhatsThis>

#include "aboutdialog.h"
#include "network_helpers.h"
#include "advancedsettingspage.h"
#include "advprocessingoptions.h"
#include "advspectraloptions.h"
#include "advsettingscontainer.h"
#include "basicsettingspage.h"
#include "binarysettingsdialog.h"
#include "clicklabel.h"
#include "customsplashscreen.h"
#include "detectdaterangedialog.h"
#include "dbghelper.h"
#include "dlproject.h"
#include "ecproject.h"
#include "globalsettings.h"
#include "infomessage.h"
#include "mainwidget.h"
#include "mymenu.h"
#include "planarfitsettingsdialog.h"
#include "projectpage.h"
#include "runpage.h"
#include "stringutils.h"
#include "timelagsettingsdialog.h"
#include "tooltipfilter.h"
#include "updatedialog.h"
#include "welcomepage.h"
#include "wheeleventfilter.h"
#include "widget_utils.h"

MainWindow::MainWindow(QString filename,
                       QString appEnvPath,
                       CustomSplashScreen* splashscreen,
                       QWidget* parent,
                       Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    aboutDialog(nullptr),
    splash_screen_(splashscreen),
    mainWidget_(nullptr),
    configState_(ConfigState()),
    dlProject_(nullptr),
    ecProject_(nullptr),
    currEcProjectFilename_(std::move(filename)),
    appEnvPath_(std::move(appEnvPath)),
    notificationTimer_(nullptr),
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
    previousPage_(Defs::CurrPage::Welcome),
    currentPage_(Defs::CurrPage::Welcome),
    currentStatus_(Defs::CurrStatus::Ready),
    currentRunStatus_(Defs::CurrRunStatus::Express),
    expressClicked_(false),
    advancedClicked_(false),
    retrieverClicked_(false),
    engineProcess_(nullptr),
    updateDialog(nullptr),
    argFilename_(false),
    scheduledSilentMdCleanup_(false)
{
    readSettings();

    saveEnvSettings(appEnvPath_);

    // setup tooltip filter
    tooltipFilter_ = new TooltipFilter(configState_.window.tooltips, this);
    qApp->installEventFilter(tooltipFilter_);

    auto wheelFilter_ = new WheelEventFilter(this);
    qApp->installEventFilter(wheelFilter_);

    // create metadata file
    dlProject_ = new DlProject(this, configState_.project);

    // create project file
    ecProject_ = new EcProject(this, configState_.project);

    // set main window components
    setWindowTitle(Defs::APP_NAME + QLatin1String(" ") + Defs::REGISTERED_TRADEMARK_SYMBOL);

#if defined(Q_OS_WIN)
    // NOTE: inserted fake (inexistent) icon to prevent icon in the menu bar
    setWindowIcon(QIcon(QStringLiteral(":/win_files/app_ico.png")));
#elif defined(Q_OS_MACOS)
    setWindowIcon(QIcon(QStringLiteral(":/mac_files/app.icns")));
#elif defined(Q_OS_LINUX)
    setWindowIcon(QIcon(QStringLiteral(":/lin_files/app.png")));
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
    setMinimumSize(800, 666);

    connectActions();

    createEngineProcess();

    // connections
    connect(ecProject_, &EcProject::ecProjectNew,
            this, &MainWindow::updateInfoMessages);
    connect(ecProject_, &EcProject::ecProjectModified,
            this, &MainWindow::setEcProjectModified);
    connect(ecProject_, &EcProject::ecProjectChanged,
            this, &MainWindow::updateInfoMessages);
    connect(ecProject_, &EcProject::updateInfo,
            this, &MainWindow::updateInfoMessages);
    connect(ecProject_, &EcProject::updateInfo,
            this, &MainWindow::updateRunButtonsAvailability);
    connect(fileMenuOpenRecent, &QMenu::aboutToShow,
            this, &MainWindow::recentMenuShow);

    connect(dlProject_, &DlProject::projectModified,
            this, &MainWindow::updateInfoMessages);
    connect(dlProject_, &DlProject::projectChanged,
            this, &MainWindow::updateInfoMessages);

    // from MainWidget
    connect(mainWidget_->runPage(), &RunPage::updateConsoleLineRequest,
            this, &MainWindow::updateConsoleLine);
    connect(mainWidget_->runPage(), &RunPage::updateConsoleCharRequest,
            this, &MainWindow::updateConsoleChar);
    connect(mainWidget_->runPage(), &RunPage::pauseRequest,
            this, &MainWindow::pauseResumeComputations);
    connect(mainWidget_, &MainWidget::showSmartfluxBarRequest,
            this, &MainWindow::setSmartfluxMode);
    connect(mainWidget_, &MainWidget::saveSilentlyRequest,
            this, &MainWindow::fileSaveSilently);
    connect(mainWidget_, &MainWidget::mdCleanupRequest,
            this, &MainWindow::scheduleMdCleanup);
    connect(mainWidget_, SIGNAL(saveRequest()),
            this, SLOT(fileSave()));

    // dialogs connections
    connect(mainWidget_->projectPage(), &ProjectPage::connectBinarySettingsRequest,
            this, &MainWindow::connectBinarySettingsDialog);

    // from BasicSettingsPage
    connect(mainWidget_, &MainWidget::updateMetadataReadResult,
            this, &MainWindow::setMetadataRead);

    connect(this, &MainWindow::recentUpdated,
            mainWidget_, &MainWidget::recentUpdated);

    connect(mainWidget_, &MainWidget::openProjectRequest,
            this, &MainWindow::fileOpenRequest);
    connect(mainWidget_, &MainWidget::newProjectRequest,
            this, &MainWindow::fileNewRequest);
    connect(mainWidget_, &MainWidget::checkUpdatesRequest,
            this, &MainWindow::showUpdateDialog);

    connectPlanarFitDialog();
    connectTimeLagDialog();

    // restore window state
    QTimer::singleShot(0, this, SLOT(restorePreviousStatus()));

    QTimer::singleShot(0, this, SLOT(initialize()));

    QTimer::singleShot(0, this, SLOT(checkInternetConnection()));

    QTimer::singleShot(0, this, SLOT(showAutoUpdateDialog()));

    // notify every 24 hrs from the last start
    notificationTimer_ = new QTimer(this);
    connect(notificationTimer_, &QTimer::timeout,
            this, &MainWindow::showAutoUpdateDialog);
    notificationTimer_->start(1000 * 60 * 60 * 24);

    // NOTE: for testing only
//    notificationTimer_->start(5000);

    // NOTE: for testing only
//    QStringList list;
//    Process::getProcessIdsByProcessName(QStringLiteral("eddypro_debug"), list);
//    qDebug() << "eddypro_debug" << list;
}

MainWindow::~MainWindow()
{
    delete notificationTimer_;
    delete aboutDialog;

    if (engineProcess_->isRunning())
    {
        engineProcess_->processStop();
    }
    delete engineProcess_;
}

void MainWindow::initialize()
{
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
        updatePage(Defs::CurrPage::Welcome);
        showGuidedModeMessages_1();

        QNetworkProxyFactory::setUseSystemConfiguration(true);

        // open filename argument
        if (argFilename_)
        {
            fileOpen(currentProjectFile());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();

    // cancel the operation
    if (!continueBeforeClose())
    {
        event->ignore();
        return;
    }

    // continue the operation
    // NOTE: hack prevent non interactive changes
    disconnect(toggleInfoOutputAct, &QAction::toggled,
            this, &MainWindow::viewInfoOutput);
    disconnect(toggleTooltipOutputAct, &QAction::toggled,
            this, &MainWindow::viewTooltipOutput);
    disconnect(toggleConsoleOutputAct, &QAction::toggled,
            this, &MainWindow::viewConsoleOutput);

    event->accept();
}

void MainWindow::setCurrentProjectFile(const QString& fileName, bool modified)
{
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
    QString shownName;
    if (filename.isEmpty())
    {
        shownName = Defs::DEFAULT_PROJECT_FILENAME;
    }
    else
    {
        shownName = QFileInfo(filename).fileName();
    }

    setWindowTitle(QStringLiteral("%1 %2 - [%3[*]]")
                   .arg(Defs::APP_NAME)
                   .arg(Defs::REGISTERED_TRADEMARK_SYMBOL)
                   .arg(shownName));

    if (clearStar)
    {
        setWindowModified(false); // clear the star from the window title
    }
}

void MainWindow::fileNew()
{
    if (!configState_.project.smartfluxMode)
    {
        if (!continueBeforeClose()) { return; }
    }

    // continue the operation
    newFile();
    if (configState_.project.smartfluxMode)
    {
        changePage(Defs::CurrPage::BasicSettings, false);
        mainWidget_->updateSmartfluxBarStatus();
    }
    else
    {
        changePage(Defs::CurrPage::ProjectCreation, false);
    }
    showStatusTip(tr("New project created"));
    updateInfoDock(false);
    updateMenuActionStatus(currentPage());
    newClicked_ = true;
}

/// \fn void MainWindow::fileOpen(const QString &filename)
/// \param[in] filename
/// \return void
void MainWindow::fileOpen(const QString &fileName)
{
    // cancel the operation
    if (!continueBeforeClose()) { return; }

    QString fileStr;

    // continue the operation
    showStatusTip(tr("Opening..."));

    FileUtils::cleanSmfDirRecursively(configState_.general.env);

    // action triggered by user interaction
    if (fileName.isEmpty())
    {
        fileStr = QFileDialog::getOpenFileName(this,
                        tr("Select an %1 Project File").arg(Defs::APP_NAME),
                        WidgetUtils::getSearchPathHint(),
                        tr("%1 Project Files (*.%2);;All Files (*.*)").arg(Defs::APP_NAME, Defs::PROJECT_FILE_EXT),
                        nullptr
                        // , QFileDialog::DontUseNativeDialog
                        );
        if (fileStr.isEmpty()) { return; }
    }
    // programmatically
    else
    {
        fileStr = fileName;
    }

    QFileInfo projectDir(fileStr);
    auto projectPath = projectDir.canonicalPath();
    configState_.window.last_data_path = projectPath;
    GlobalSettings::updateLastDatapath(projectPath);

    if (openFile(fileStr))
    {
        showStatusTip(tr("Project loaded"));
        consoleOutput->clear();

        if (currentPage() != Defs::CurrPage::ProjectCreation)
        {
            if (configState_.project.smartfluxMode)
            {
                if (currentPage() == Defs::CurrPage::Welcome)
                {
                    changePage(Defs::CurrPage::BasicSettings);
                }
                mainWidget_->updateSmartfluxBarStatus();
            }
            else
            {
                changePage(Defs::CurrPage::ProjectCreation);
            }
        }
        updateInfoDock(true);
    }
    else
    {
        showStatusTip(tr("Project loading failed"));
    }
    openingFlag_ = false;

    silentMdClenaup();
}

//
bool MainWindow::openFile(const QString& filename)
{
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
                        setCurrentProjectFile(filename, modified);
                        newFlag_ = false;
                        addRecent(currentProjectFile());
                        saveAction->setEnabled(false);

                        if (modified)
                        {
                            // load was successful
                            WidgetUtils::warning(this,
                                tr("Load Project"),
                                tr("The project was successfully imported "
                                   "and updated."),
                                tr("Save the project to complete the update."));
                        }
                        return true;
                    }
                    // load was unsuccessful
                    WidgetUtils::warning(this,
                                         tr("Load Project Error"),
                                         tr("Unable to load the project."));

                    // close the current open project to prevent partial loading
                    // of ec project settings (currently there is no roll-back
                    // of the loadEcProject() function)
                    fileClose();
                    return false;
                }
                // file not in native format
                return false;
            }
            // file does not exist
            return false;
        }
    }
    else
    {
#ifdef QT_DEBUG
//    auto msgBox = new QMessageBox;
//    msgBox->setWindowTitle(QStringLiteral("Troppi click, Ger :-)"));
//    msgBox->setText(QStringLiteral("Detected Double Open"));
//    msgBox->show();
#endif
    }
    // empty file name
    return false;
}

void MainWindow::fileRecent()
{
    auto action = qobject_cast<QAction *>(sender());
    if (!action) { return; }

    QString fname = action->text();

    if (configState_.project.smartfluxMode)
    {
        loadSmartfluxProjectCopy(fname);
    }
    else
    {
        // cancel the operation
        if (!continueBeforeClose()) { return; }

        // continue the operation
        if (openFile(fname))
        {
            showStatusTip(tr("Recent project loaded"));
            consoleOutput->clear();

            if (currentPage() != Defs::CurrPage::ProjectCreation)
            {
                changePage(Defs::CurrPage::ProjectCreation);
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

// Return true if it saved successfully, false if the operation was canceled
// or if an error occurred.
bool MainWindow::fileSave(bool quiet)
{
    closeOpenDialogs();

    if (newFlag_)
    {
        return fileSaveAs();
    }
    if (!quiet)
    {
        if (alertChangesWhileRunning())
        {
            showStatusTip(tr("Canceled..."));
            return false;
        }
    }
    if (saveFile(currentProjectFile()))
    {
        // successful in saving file
        newFlag_ = false;
        saveAction->setEnabled(false);
        setCurrentProjectFile(currentProjectFile());
        showStatusTip(tr("Project saved"));
        return true;
    }
    // error in saving
    WidgetUtils::warning(this,
                         tr("Save Project Error"),
                         tr("%1 was unable to save to %2")
                         .arg(Defs::APP_NAME, currentProjectFile()));
    showStatusTip(tr("Error in saving project"));
    return false;
}

// Return true if it saved successfully, false if the operation was canceled
// or if an error occurred.
bool MainWindow::fileSaveAs(const QString& fileName)
{
    closeOpenDialogs();

    auto searchPath = WidgetUtils::getSearchPathHint();

    auto filenameHint = QString();
    if ((ecProject_->generalFileName() == Defs::DEFAULT_PROJECT_FILENAME)
        && (!ecProject_->generalTitle().isEmpty()))
    {
        filenameHint = searchPath + QLatin1Char('/') + ecProject_->generalTitle();
    }
    else if ((ecProject_->generalFileName() == Defs::DEFAULT_PROJECT_FILENAME)
        && (ecProject_->generalTitle().isEmpty()))
    {
        filenameHint = searchPath;
    }
    else if (FileUtils::existsPath(QFileInfo(ecProject_->generalFileName()).canonicalPath()))
    {
        filenameHint = ecProject_->generalFileName();
    }
    else
    {
        filenameHint = searchPath;
    }

    showStatusTip(tr("Saving project..."));

    QString fileToSave;
    if (fileName.isEmpty())
    {
        fileToSave = QFileDialog::getSaveFileName(this,
                tr("Save the Project File Name As..."),
                filenameHint,
                tr("%1 Project Files (*.%2)").arg(Defs::APP_NAME, Defs::PROJECT_FILE_EXT),
                nullptr
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
        if (fileToSave.indexOf(epExt) == -1) { fileToSave += epExt; }

// see before, alternative if DontConfirmOverwrite
        // overwrite?
        if (QFile::exists(fileToSave))
        {
            if (alertChangesWhileRunning())
            {
                showStatusTip(tr("Canceled..."));
                return false;
            }

            if (!WidgetUtils::okToOverwrite(this, fileToSave))
            {
                showStatusTip(tr("Canceled..."));
                return false;
            }
        }

        if (saveFile(fileToSave))
        {
            // successfully saved
            newFlag_ = false;
            saveAction->setEnabled(false);
            setCurrentProjectFile(fileToSave);
            showStatusTip(tr("Project saved"));
            return true;
        }
        // error in saving
        WidgetUtils::warning(QApplication::activeWindow(),
                             tr("Save Project Error"),
                             tr("%1 was unable to save to %2")
                                 .arg(Defs::APP_NAME, QFileInfo(fileToSave).fileName()),
                             tr("Make sure the file is not in use "
                                "by another application."
                                "If the problem persists, contact "
                                "envsupport@licor.com ."));
        showStatusTip(tr("Error in saving project"));
        return false;
    }
    // fileToSave.isEmpty(), i.e. no file chosen
    showStatusTip(tr("Saving aborted"));
    return false;
}

// return true if it saved successfully, false if an error occurred
// the only message being in case of error
bool MainWindow::fileSaveSilently()
{
    if (newFlag_ && modifiedFlag_)
    {
        return fileSave();
    }

    // hack: in case of new flag (it should not be possible with old files,
    // for which we shall make a silent saving) delete the flag
    // TODO: prevent it
    if (newFlag_)
    {
        newFlag_ = false;
    }

    if (modifiedFlag_)
    {
        auto quiet = true;
        return fileSave(quiet);
    }

//    auto quiet = true;
//    return fileSave(quiet);

//    QMessageBox::warning(nullptr, QLatin1String(Q_FUNC_INFO),
//    tr("New 2: %1\nModified: %2").arg(newFlag_, modifiedFlag_));
    return true;
}

void MainWindow::scheduleMdCleanup()
{
    scheduledSilentMdCleanup_ = true;
}

void MainWindow::fileClose()
{
    closeOpenDialogs();

    // cancel the operation
    if (!continueBeforeClose()) { return; }

    newFile();
    newClicked_ = false;
    changePage(Defs::CurrPage::Welcome, false);
    showStatusTip(tr("Project closed"));
    updateInfoDock(false);
    updateRunButtonsAvailability();
    FileUtils::cleanSmfDirRecursively(configState_.general.env);
}

void MainWindow::newFile()
{
    // create a new file
    ecProject_->newEcProject(configState_.project);
    newFlag_ = true;
    saveAction->setEnabled(false);
    setCurrentProjectFile(QString());
    runExpressAvailable_ = false;
    runAdvancedAvailable_ = false;
}

bool MainWindow::alertChangesWhileRunning()
{
    if (currentStatus() == Defs::CurrStatus::Run)
    {
        WidgetUtils::warning(this,
                             tr("Changes During Run"),
                             tr("You can not save changes "
                                "while EddyPro is running. "),
                             tr("Wait until run has finished "
                                "before saving."),
                             QStringLiteral("saveDuringRunMessage"));
        return true;
    }
    return false;
}

bool MainWindow::continueBeforeClose()
{
    // stop if processing
    if (currentStatus() != Defs::CurrStatus::Ready)
    {
        if (WidgetUtils::yesNoQuestion(this,
                                  tr("Close EddyPro"),
                                  tr("EddyPro is processing (running or in pause)."),
                                  tr("Do you want to stop the computations "
                                     "before proceeding?"),
                                  QStringLiteral("stopMessage")))
        {
            stopEngineProcess();
        }
        else
        {
            showStatusTip(tr("Canceled..."));
            return false;
        }
    }

    // save if project modified
    if (!modifiedFlag_) return true;

    int buttonRole = WidgetUtils::requestToSave(this,
                           tr("Save Project"),
                           tr("Current project has been modified."),
                           tr("Do you want to save your changes?"));

    switch (buttonRole)
    {
    case QMessageBox::NoRole:
        showStatusTip(tr("Canceled..."));
        return false;
    case QMessageBox::AcceptRole:
        return fileSave();
    case QMessageBox::RejectRole:
        break;
    }
    return true;
}

bool MainWindow::saveFile(const QString& fileName)
{
    bool saved = ecProject_->saveEcProject(fileName);

    if (saved)
    {
        // new
        currEcProjectFilename_ = fileName;
        addRecent(currentProjectFile());
    }
    return saved;
}

void MainWindow::about()
{
    if (!aboutDialog)
    {
        aboutDialog = new AboutDialog(this);
    }

    aboutDialog->show();
    aboutDialog->raise();
    aboutDialog->activateWindow();
}

void MainWindow::setEcProjectModified()
{
    saveAction->setEnabled(true);
    setWindowModified(true);
    modifiedFlag_ = true;
}

void MainWindow::updateInfoMessages()
{
    if (currentPage() == Defs::CurrPage::ProjectCreation)
    {
        showGuidedModeMessages_1();
    }
    else if (currentPage() == Defs::CurrPage::BasicSettings)
    {
        showGuidedModeMessages_2();
    }
    else if (currentPage() == Defs::CurrPage::AdvancedSettings)
    {
        showGuidedModeMessages_3();
    }
}

void MainWindow::createActions()
{
    newAction = new QAction(this);
    newAction->setText(tr("&New\nProject"));
    newAction->setIcon(QIcon(QStringLiteral(":/icons/new")));
    newAction->setShortcut(QKeySequence(QKeySequence::New));
    newAction->setToolTip(tr("Start a new project. (%1)")
                          .arg((newAction->shortcut().toString())));

    openAction = new QAction(this);
    openAction->setText(tr("&Open\nProject..."));
    openAction->setIcon(QIcon(QStringLiteral(":/icons/open")));
    openAction->setShortcut(QKeySequence(QKeySequence::Open));
    openAction->setToolTip(tr("Open an existing project. (%1)")
                           .arg((openAction->shortcut().toString())));

    closeAction = new QAction(this);
    closeAction->setText(tr("&Close"));
    closeAction->setIcon(QIcon(QStringLiteral(":/icons/close")));
    closeAction->setShortcut(QKeySequence(tr("Ctrl+W")));
    closeAction->setShortcutContext(Qt::ApplicationShortcut);
    closeAction->setToolTip(tr("Close the current project. (%1)")
                            .arg((closeAction->shortcut().toString())));

    recentOpenAction = new QAction(this);
    recentOpenAction->setText(tr("Open &Recent..."));
    recentOpenAction->setIcon(QIcon(QStringLiteral(":/icons/open")));
    recentOpenAction->setToolTip(tr("Open a recent project."));

    saveAction = new QAction(this);
    saveAction->setText(tr("&Save"));
    saveAction->setIcon(QIcon(QStringLiteral(":/icons/save")));
    saveAction->setShortcut(QKeySequence(QKeySequence::Save));
    saveAction->setToolTip(tr("Save the current project. (%1)")
                           .arg((saveAction->shortcut().toString())));

    saveAsAction = new QAction(this);
    saveAsAction->setText(tr("Save\n&As..."));
    saveAsAction->setIcon(QIcon(QStringLiteral(":/icons/save-as")));
    // Extend to Windows the other platforms' standard shortcut
    saveAsAction->setShortcut(tr("Ctrl+Shift+S"));
    saveAsAction->setToolTip(tr("Save the current project "
                                "with a new name. (%1)")
                             .arg((saveAsAction->shortcut().toString())));

    smartfluxAction = new QAction(this);
    smartfluxAction->setCheckable(true);
    smartfluxAction->setText(tr("SmartFlux Configuration"));
    smartfluxAction->setShortcut(tr("Ctrl+F"));
    smartfluxAction->setToolTip(tr("<b>SmartFlux Configuration</b>. (%1)")
                                .arg((smartfluxAction->shortcut().toString())));

    quitAction = new QAction(this);
    quitAction->setText(tr("&Quit"));
    // Extend to Windows the other platforms' standard shortcut
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setMenuRole(QAction::QuitRole);
    quitAction->setToolTip(tr("<b>Quit:</b> Quit the application. (%1)")
                           .arg((quitAction->shortcut().toString())));

    // view start menu action
    viewWelcomeAction = new QAction(this);
    viewWelcomeAction->setText(tr("Welcome"));

//    auto welcome_pixmap_2x = QPixmap(QStringLiteral(":/icons/welcome"));
//#if defined(Q_OS_MACOS)
//    welcome_pixmap_2x.setDevicePixelRatio(2.0);
//#endif
//    viewWelcomeAction->setIcon(welcome_pixmap_2x);

    viewWelcomeAction->setIcon(QIcon(QStringLiteral(":/icons/welcome")));
    viewWelcomeAction->setShortcut(tr("Alt+1"));
    viewWelcomeAction->setToolTip(tr("Go to the <i>Welcome page</i>. (%1)")
                                  .arg((viewWelcomeAction->shortcut().toString())));

    viewProjectCreationAction = new QAction(this);
    viewProjectCreationAction->setText(tr("Project\nCreation"));
    viewProjectCreationAction->setIcon(QIcon(QStringLiteral(":/icons/project")));
    viewProjectCreationAction->setShortcut(tr("Alt+2"));
    viewProjectCreationAction->setToolTip(tr("Go to the <i>Project Creation Page</i>. (%1)")
                                          .arg((viewProjectCreationAction
                                                ->shortcut().toString())));

    viewBasicSettingsAction = new QAction(this);
    viewBasicSettingsAction->setText(tr("Basic\nSettings"));
    viewBasicSettingsAction->setIcon(QIcon(QStringLiteral(":/icons/basic-settings")));
    viewBasicSettingsAction->setShortcut(tr("Alt+3"));
    viewBasicSettingsAction->setToolTip(tr("Go to the <i>Basic Settings Page</i>. (%1)")
                                        .arg((viewBasicSettingsAction->shortcut().toString())));

    viewAdvancedAction = new QAction(this);
    viewAdvancedAction->setText(tr("Advanced\nSettings"));
    viewAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/advanced-settings")));
    viewAdvancedAction->setShortcut(tr("Alt+4"));
    viewAdvancedAction->setToolTip(tr("Go to the <i>Advanced Settings Page</i>. (%1)")
                                   .arg((viewAdvancedAction->shortcut().toString())));

    viewRunPageAction = new QAction(this);
    viewRunPageAction->setObjectName(QStringLiteral("view_run_action"));
    viewRunPageAction->setText(tr("Run\nOutput"));
    viewRunPageAction->setIcon(QIcon(QStringLiteral(":/icons/console")));
    viewRunPageAction->setShortcut(tr("Alt+5"));
    viewRunPageAction->setToolTip(tr("Go to the <i>Output Console Page</i>. (%1)")
                                  .arg((viewRunPageAction->shortcut().toString())));

    viewActionGroup = new QActionGroup(this);
    viewActionGroup->addAction(viewWelcomeAction);
    viewActionGroup->addAction(viewProjectCreationAction);
    viewActionGroup->addAction(viewBasicSettingsAction);
    viewActionGroup->addAction(viewAdvancedAction);
    viewActionGroup->addAction(viewRunPageAction);

    runExpressAction = new QAction(this);
    runExpressAction->setText(tr("Express\nMode"));
    runExpressAction->setIcon(QIcon(QStringLiteral(":/icons/run-exp")));
    runExpressAction->setPriority(QAction::HighPriority);
    runExpressAction->setShortcut(tr("Ctrl+E"));
    runExpressAction->setProperty("show-tooltip", false);
    runExpressAction->setToolTip(tr("Process in <i>Express mode</i>. (%1)")
                                 .arg((runExpressAction->shortcut().toString())));

    runAdvancedAction = new QAction(this);
    runAdvancedAction->setText(tr("Advanced\nMode"));
    runAdvancedAction->setIcon(QIcon(QStringLiteral(":/icons/run-adv")));
    runAdvancedAction->setPriority(QAction::HighPriority);
    runAdvancedAction->setShortcut(tr("Ctrl+A"));
    runAdvancedAction->setProperty("show-tooltip", false);
    runAdvancedAction->setToolTip(tr("Process in <i>Advanced mode</i>. (%1)")
                                  .arg((runAdvancedAction->shortcut().toString())));

    runRetrieverAction = new QAction(this);
    runRetrieverAction->setText(tr("Metadata\nRetriever"));
    runRetrieverAction->setIcon(QIcon(QStringLiteral(":/icons/run-ret")));
    runRetrieverAction->setPriority(QAction::HighPriority);
    runRetrieverAction->setShortcut(tr("Ctrl+R"));
    runRetrieverAction->setProperty("show-tooltip", true);
    runRetrieverAction->setToolTip(tr("<b>Metadata retriever:</b> "
                                      "Only available for GHG files, this tool "
                                      "retrieves metadata from GHG files and "
                                      "organizes them into a unique file that "
                                      "can be examined (e.g., to create a "
                                      "time series of canopy heights at the "
                                      "site) and can also be provided to "
                                      "EddyPro as a dynamic metadata file. (%1)")
                                      .arg((runRetrieverAction->shortcut().toString())));

    stopAction = new QAction(this);
    stopAction->setText(tr("Stop"));
    stopAction->setIcon(QIcon(QStringLiteral(":/icons/stop")));
    stopAction->setPriority(QAction::HighPriority);
    stopAction->setShortcut(tr("Ctrl+T"));
    stopAction->setProperty("show-tooltip", false);
    stopAction->setToolTip(tr("Stop processing. (%1)")
                           .arg((stopAction->shortcut().toString())));

//#if !defined(Q_OS_MACOS)
    // Full Screen Action
    toggleFullScreenAction = new QAction(this);
    toggleFullScreenAction->setText(tr("Full Screen"));
    toggleFullScreenAction->setCheckable(true);
    toggleFullScreenAction->setShortcut(QKeySequence(QKeySequence::FullScreen));
    toggleFullScreenAction->setStatusTip(tr("Full screen mode view"));
//#endif

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
    helpAction->setShortcut(QKeySequence(tr("Ctrl+F1")));

    pdfHelpAction = new QAction(this);
    pdfHelpAction->setText(tr("Instruction Manual (PDF)"));
    pdfHelpAction->setIcon(QIcon(QStringLiteral(":/icons/img/menu-help.png")));
    pdfHelpAction->setShortcut(tr("Ctrl+M"));

    starterPdfHelpAction = new QAction(this);
    starterPdfHelpAction->setText(tr("Getting Started (PDF)"));
    starterPdfHelpAction->setIcon(QIcon(QStringLiteral(":/icons/img/menu-help.png")));

    toggleOfflineHelpAct = new QAction(this);
    toggleOfflineHelpAct->setText(tr("Use Offline Help"));
    toggleOfflineHelpAct->setCheckable(true);

    appWebpageAction = new QAction(this);
    appWebpageAction->setText(tr("EddyPro Web Page"));

    forumWebpageAction = new QAction(this);
    forumWebpageAction->setText(tr("EddyPro Forum"));

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
    // File actions
    connect(newAction, &QAction::triggered, this, &MainWindow::fileNew);
    connect(openAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(closeAction, &QAction::triggered, this, &MainWindow::fileClose);
    connect(saveAction, &QAction::triggered, this, &MainWindow::fileSave);
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    connect(quitAction, &QAction::triggered, this, &MainWindow::closeMainWindow);

    connect(smartfluxAction, &QAction::triggered,
            this, &MainWindow::requestSmartFluxMode);

    // View actions
    connect(viewWelcomeAction, &QAction::triggered,
            this, &MainWindow::viewWelcomePage);
    connect(viewProjectCreationAction, &QAction::triggered,
            this, &MainWindow::viewProjectPage);
    connect(viewBasicSettingsAction, &QAction::triggered,
            this, &MainWindow::viewBasicSettingsPage);
    connect(viewAdvancedAction, &QAction::triggered,
            this, &MainWindow::viewAdvancedSettingsPage);
    connect(viewRunPageAction, &QAction::triggered,
            this, &MainWindow::viewRunPage);

    connect(runExpressAction, &QAction::triggered,
            this, &MainWindow::getRunExpress);
    connect(runAdvancedAction, &QAction::triggered,
            this, &MainWindow::getRunAdvanced);
    connect(runRetrieverAction, &QAction::triggered,
            this, &MainWindow::getRunRetriever);
    connect(stopAction, &QAction::triggered,
            this, &MainWindow::stopEngine);

//#if !defined(Q_OS_MACOS)
    connect(toggleFullScreenAction, &QAction::toggled,
            this, &MainWindow::setFullScreen);
//#endif

    connect(toggleStatusbarAct, &QAction::triggered,
            this, &MainWindow::toggleStatusbar);

    // Tools actions
    connect(toggleInfoOutputAct, &QAction::triggered,
            this, &MainWindow::viewInfoOutput);
    connect(toggleTooltipOutputAct, &QAction::triggered,
            this, &MainWindow::viewTooltipOutput);
    connect(toggleConsoleOutputAct, &QAction::triggered,
            this, &MainWindow::viewConsoleOutput);

    // test only
//    connect(toggleConsoleOutputAct, SIGNAL(toggled(bool)),
//            this, SLOT(dbgViewConsoleOutputToggled(bool)));
//    connect(toggleConsoleOutputAct, SIGNAL(triggered(bool)),
//            this, SLOT(dbgViewConsoleOutputTriggered(bool)));

    connect(toggleInfoOutputAct, &QAction::toggled,
            this, &MainWindow::viewInfoOutput);
    connect(toggleTooltipOutputAct, &QAction::toggled,
            this, &MainWindow::viewTooltipOutput);
    connect(toggleConsoleOutputAct, &QAction::toggled,
            this, &MainWindow::viewConsoleOutput);

    // Help actions
    connect(helpAction, &QAction::triggered,
            this, &MainWindow::showHelp);
    connect(pdfHelpAction, &QAction::triggered,
            this, &MainWindow::showPdfHelp);
    connect(starterPdfHelpAction, &QAction::triggered,
            this, &MainWindow::showStarterPdfHelp);
    connect(toggleOfflineHelpAct, &QAction::triggered,
            this, &MainWindow::setOfflineHelp);
    connect(appWebpageAction, &QAction::triggered,
            this, &WidgetUtils::openAppWebsite);
    connect(forumWebpageAction, &QAction::triggered,
            this, &WidgetUtils::openForumWebsite);
    connect(checkUpdateAction, &QAction::triggered,
            this, &MainWindow::showUpdateDialog);
    connect(aboutAction, &QAction::triggered,
            this, &MainWindow::about);
    connect(aboutQtAction, &QAction::triggered,
            qApp, &QApplication::aboutQt);
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
//#if !defined(Q_OS_MACOS)
    viewMenu->addAction(toggleFullScreenAction);
//#endif

    toolsMenu = new MyMenu(this);
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
    helpMenu->addAction(appWebpageAction);
    helpMenu->addAction(forumWebpageAction);
    helpMenu->addAction(checkUpdateAction);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("&File ToolBar"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);
    fileToolBar->addAction(saveAsAction);
    fileToolBar->addAction(closeAction);
    fileToolBar->setObjectName(QStringLiteral("fileToolBar"));
    fileToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    sep1 = new ClickLabel;
    auto sep_pixmap = QPixmap(QStringLiteral(":/icons/sep"));
#if defined(Q_OS_MACOS)
    sep_pixmap.setDevicePixelRatio(2.0);
#endif
    sep1->setPixmap(sep_pixmap);
    sep2 = new ClickLabel;
    sep2->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
    sep3 = new ClickLabel;
    sep3->setPixmap(QPixmap(QStringLiteral(":/icons/sep")));
    sep4 = new ClickLabel;
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
    viewToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

    runToolBar = addToolBar(tr("&Tools ToolBar"));

    auto runLabel = new QLabel(tr("  Run:  "));
    runLabel->setObjectName(QStringLiteral("runLabel"));

    auto tSep = new QAction(this);
    tSep->setSeparator(true);
    tSep->setIcon(QIcon(QStringLiteral(":/icons/sep")));

    auto sep5 = new QLabel;
    sep5->setPixmap(QPixmap(QStringLiteral(":/icons/tsep")));
    auto sep6 = new QLabel;
    sep6->setPixmap(QPixmap(QStringLiteral(":/icons/tsep")));
    auto sep7 = new QLabel;
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

    fileToolBar->setIconSize(QSize(26, 40));
    viewToolBar->setIconSize(QSize(42, 40));
    runToolBar->setIconSize(QSize(32, 40));

    // to add small licor logo right aligned
    auto spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto licorLogo = new ClickLabel;
    licorLogo->setObjectName(QStringLiteral("licorLogo"));

    runToolBar->addWidget(spacer);
    runToolBar->addWidget(licorLogo);
    connect(licorLogo, &ClickLabel::clicked, this, &MainWindow::openLicorSite);
}

void MainWindow::createStatusBar()
{
    showStatusTip(tr("Ready"));

    currentProjectLabel = new QLabel(this);
    currentProjectLabel->setAlignment(Qt::AlignCenter);
    currentProjectLabel->setText(currentProjectFile());

    statusBar()->addPermanentWidget(currentProjectLabel);
    statusBar()->setToolTip(currentProjectFile());
}

void MainWindow::updateStatusBar()
{
    WidgetUtils::setElidedTextToLabel(currentProjectLabel, currentProjectFile(), Qt::ElideMiddle, this->size().width());
    statusBar()->setToolTip(currentProjectFile());
}

void MainWindow::createConsoleDockWin()
{
    consoleOutput = new QPlainTextEdit;
    consoleOutput->setObjectName(QStringLiteral("consoleDockEdit"));
    consoleOutput->setReadOnly(true);
    consoleOutput->setLineWrapMode(QPlainTextEdit::NoWrap);
    consoleOutput->setMaximumBlockCount(10000);
    consoleOutput->setUndoRedoEnabled(false);
    consoleOutput->setMinimumWidth(this->width() / 2);

    auto clearConsoleButton = new QPushButton(tr("Clear"));
    clearConsoleButton->setProperty("mdButton", true);
    clearConsoleButton->setStyleSheet(QStringLiteral("QPushButton { margin-right: 4px; }"));
    clearConsoleButton->setMaximumWidth(clearConsoleButton->sizeHint().width());

    connect(clearConsoleButton, &QPushButton::clicked,
            consoleOutput, &QPlainTextEdit::clear);

    auto consoleLayout = new QVBoxLayout;
    consoleLayout->addWidget(consoleOutput);
    consoleLayout->addWidget(clearConsoleButton, 0, Qt::AlignBottom | Qt::AlignRight);

    auto consoleFrame = new QWidget(this);
    consoleFrame->setLayout(consoleLayout);

    consoleDock = new QDockWidget(tr("Output Console"), this);
    consoleDock->setObjectName(QStringLiteral("consoleDock"));
    consoleDock->setWidget(consoleFrame);
    consoleDock->setVisible(false);

    addDockWidget(Qt::RightDockWidgetArea, consoleDock, Qt::Horizontal);
}

void MainWindow::createInfoDockWin()
{
    infoOutput = new QPlainTextEdit;
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
    connect(infoDock, &QDockWidget::dockLocationChanged,
            this, &MainWindow::updateDockBorder);
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

void MainWindow::closeMainWindow()
{
    closeOpenDialogs();
    close();
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
            settings.value(Defs::CONF_GEN_RECENTNUM, configState_.general.recentnum).toInt();
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
            configState_.project.default_data_path.clear();
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
        settings.setValue(Defs::CONF_GEN_RECENTNUM, configState_.general.recentnum);
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

    // write project config
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_GENERAL,
                                             Defs::CONF_GEN_ENV,
                                             configState_.general.env);
}

void MainWindow::restorePreviousStatus()
{
    // must be before restoreGeometry(), which restore the possible full screen
    // state, because otherwise setFullScreen() reset restoreGeometry()
//#if !defined(Q_OS_MACOS)
    toggleFullScreenAction->setChecked(configState_.window.fullScreen);
//#endif

    // the dockwidgets checked state is restored by restoreState()
//    toggleTooltipOutputAct->setChecked(config_state_.window.tooltipDock);
//    toggleConsoleOutputAct->setChecked(config_state_.window.consoleDock);

    restoreGeometry(configState_.window.mainwin_geometry);
    restoreState(configState_.window.mainwin_state);

    // show or hide statusbar depending on initial setting
    // note: QMainWindow::restoreState() acts on toolbars and dockwidgets
    // but not on statusbar
    statusBar()->setVisible(configState_.window.statusBar);
    toggleStatusbarAct->setChecked(configState_.window.statusBar);
    toggleOfflineHelpAct->setChecked(configState_.window.offlineHelp);
}

// remove splash screen, for example before going full screen
void MainWindow::removeSplashScreen()
{
    if (splash_screen_)
    {
        splash_screen_->close();
    }
}

// Used only on Windows
void MainWindow::setFullScreen(bool on)
{
    removeSplashScreen();

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
    configState_.window.tooltips = on;
    writeSettings();
    tooltipFilter_->setTooltipAvailable(on);
}

void MainWindow::viewConsoleOutput(bool on)
{
    configState_.window.consoleDock = on;
    writeSettings();
}

void MainWindow::viewInfoOutput(bool on)
{
    configState_.window.infoDock = on;
    writeSettings();
}

void MainWindow::showHelp()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/EddyPro_Home.html"), QUrl::StrictMode));
}

void MainWindow::showPdfHelp()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("https://boxenterprise.net/s/1ium2zmwm6hl36yz9bu4"), QUrl::StrictMode));
}

void MainWindow::showStarterPdfHelp()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("https://boxenterprise.net/s/qmhucid6g0hdvd3d13tk"), QUrl::StrictMode));
}

void MainWindow::setOfflineHelp(bool yes)
{
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                             Defs::CONF_WIN_OFFLINEHELP,
                                             yes);
    toggleOfflineHelpAct->setChecked(yes);
}

void MainWindow::requestSmartFluxMode(bool on)
{
    // ask for closing in case of smartflux mode previously on

    if (!on)
    {
        if (!mainWidget_->smartFluxCloseRequest())
        {
            // undo checking of the menu action
            smartfluxAction->setChecked(true);

            return;
        }
    }
    setSmartfluxMode(on);
}

void MainWindow::setSmartfluxMode(bool on)
{
    configState_.project.smartfluxMode = on;
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                             Defs::CONF_PROJ_SMARTFLUX,
                                             on);
    if (on)
    {
        bool showDialog
            = GlobalSettings::getAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG,
                true).toBool();

        if (showDialog)
        {
            // info message
            InfoMessage smartfluxConfigDialog(QDialogButtonBox::Ok, this);
            smartfluxConfigDialog.setTitle(tr("SmartFlux Configuration"));
            smartfluxConfigDialog.setType(InfoMessage::Type::SMARTFLUX_CONFIG);
            smartfluxConfigDialog.setMessage(tr("The SmartFlux configuration will use "
                "a renamed copy of your current project file. <br />"));
            smartfluxConfigDialog.refresh();
            smartfluxConfigDialog.exec();
        }
        if (!currentProjectFile().contains(Defs::DEFAULT_SMARTFLUX_SUFFIX))
        {
            // laod a renamed copy of the previous loaded project
            loadSmartfluxProjectCopy(currentProjectFile());
        }
    }

    smartfluxAction->setChecked(on);
    mainWidget_->updateSmartfluxBarStatus();

    updateInfoMessages();
    updateRunButtonsAvailability();

    viewProjectCreationAction->setEnabled(!on);
    viewProjectCreationAction->setCheckable(!on);

    if (currentPage() == Defs::CurrPage::Welcome)
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
    else if (currentPage() == Defs::CurrPage::ProjectCreation
             || currentPage() == Defs::CurrPage::Run)
    {
        changePage(Defs::CurrPage::BasicSettings, false);
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
    // laod a renamed copy of the previous loaded project
    const QString epExt = QStringLiteral(".") + Defs::PROJECT_FILE_EXT;

    QString filenameCopy = filename;
    if (filenameCopy.indexOf(Defs::DEFAULT_SMARTFLUX_SUFFIX) == -1)
    {
        filenameCopy.insert(filenameCopy.indexOf(epExt), Defs::DEFAULT_SMARTFLUX_SUFFIX);
    }

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
    if (configState_.general.recentnum == 0) { return; }

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

    for (const auto &recentfile : configState_.general.recentfiles)
    {
        if (QFile::exists(recentfile))
        {
            auto action = new QAction(recentfile, fileMenuOpenRecent);
            connect(action, &QAction::triggered, this, &MainWindow::fileRecent);
            fileMenuOpenRecent->addAction(action);
        }
    }
}

void MainWindow::viewWelcomePage()
{
    if (currentPage() != Defs::CurrPage::Welcome)
    {
        changePage(Defs::CurrPage::Welcome);
        writeSettings();
        emit recentUpdated();
    }
}

void MainWindow::viewProjectPage()
{
    if (currentPage() != Defs::CurrPage::ProjectCreation)
    {
        if (configState_.project.smartfluxMode)
        {
            changePage(Defs::CurrPage::BasicSettings);
            showGuidedModeMessages_2();
        }
        else
        {
            changePage(Defs::CurrPage::ProjectCreation);
            showGuidedModeMessages_1();
        }
    }
}

void MainWindow::viewBasicSettingsPage()
{
    if (basicSettingsPageAvailable_)
    {
        if (currentPage() != Defs::CurrPage::BasicSettings)
        {
            changePage(Defs::CurrPage::BasicSettings);
            showGuidedModeMessages_2();
        }
    }
}

void MainWindow::viewAdvancedSettingsPage()
{
    if (currentPage() != Defs::CurrPage::AdvancedSettings)
    {
        showGuidedModeMessages_3();
        changePage(Defs::CurrPage::AdvancedSettings);
    }
}

void MainWindow::viewRunPage()
{
    if (currentPage() != Defs::CurrPage::Run)
    {
        if (configState_.project.smartfluxMode)
        {
            changePage(Defs::CurrPage::AdvancedSettings);
        }
        else
        {
            changePage(Defs::CurrPage::Run);
        }
    }
}

Defs::CurrPage MainWindow::currentPage() const
{
    return currentPage_;
}

void MainWindow::setCurrentPage(Defs::CurrPage page)
{
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
    if (currentRunStatus_ != state)
    {
        currentRunStatus_ = state;
    }
}

// restore menus and dockwidget state
void MainWindow::updateMenuActionStatus(Defs::CurrPage page)
{
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

    // show or hide objects depending on previous settings and current page status
    if (page == Defs::CurrPage::Welcome)
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

        // NOTE: hack to prevent temporarly the effect of non interactive
        // change of visibility
        disconnect(toggleInfoOutputAct, &QAction::toggled,
                this, &MainWindow::viewInfoOutput);
        disconnect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleInfoOutputAct->setEnabled(false);
        infoDock->setVisible(false);

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleInfoOutputAct, &QAction::toggled,
                this, &MainWindow::viewInfoOutput);
        connect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleStatusbarAct->setEnabled(false);
        statusBar()->setVisible(false);
    }
    else if (page == Defs::CurrPage::ProjectCreation)
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

        // NOTE: hack to prevent temporarly the effect of non interactive
        // change of visibility
        disconnect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPage::BasicSettings)
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

        // NOTE: hack to prevent temporarly the effect of non interactive
        // change of visibility
        disconnect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPage::AdvancedSettings)
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

        // NOTE: hack to prevent temporarly the effect of non interactive
        // change of visibility
        disconnect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleConsoleOutputAct->setEnabled(false);
        consoleDock->setVisible(false);

        // NOTE: hack to permit non interactive changes
        connect(toggleConsoleOutputAct, &QAction::toggled,
                this, &MainWindow::viewConsoleOutput);

        toggleStatusbarAct->setEnabled(true);
        toggleStatusbar(configState_.window.statusBar);
    }
    else if (page == Defs::CurrPage::Run)
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

void MainWindow::showStatusTip(const QString &text) const
{
    statusBar()->showMessage(text, 2000);
}

void MainWindow::windowTitleUpdate(Defs::CurrPage page)
{
    switch (page)
    {
        case Defs::CurrPage::Welcome:
            setWindowTitle(Defs::APP_NAME + QLatin1String(" ") + Defs::REGISTERED_TRADEMARK_SYMBOL);
            break;
        case Defs::CurrPage::ProjectCreation:
        case Defs::CurrPage::BasicSettings:
        case Defs::CurrPage::AdvancedSettings:
        case Defs::CurrPage::Run:
            setFileCaption(currentProjectFile(), false);
            break;
//        default:
//            setFileCaption(QString());
//            break;
    }
}

void MainWindow::updateInfoDock(bool yes)
{
    if (yes)
    {
        if (!infoDock->isVisible())
            infoDock->toggleViewAction()->trigger();

        infoDock->raise();

        showGuidedModeMessages_2();
        showGuidedModeMessages_3();
        showGuidedModeMessages_1();
    }
    else
    {
        infoOutput->clear();
    }
}

void MainWindow::showGuidedModeMessages_1()
{
    QString red_intro;
    QString orange_intro;
    QString red_msg;
    QString orange_msg;
    auto doRedFix = true;
    auto doOrangeFix = true;

    if (doRedFix)
    {
        red_intro = tr("<p>Some information in the <b>Project Creation</b> "
                       "page is incomplete or erroneous. Please address the "
                       "following issues:</p>");
        red_msg = tr("<ul>");
        doRedFix = false;
    }

    if (doOrangeFix)
    {
        orange_intro = tr("<p>Some information in the <b>Project Creation</b> "
                          "page might be incomplete or erroneous. Please "
                          "address the following issues:</p>");
        orange_msg = tr("<ul>");
        doOrangeFix = false;
    }

    if (ecProject_->generalUseAltMdFile())
    {
        if (ecProject_->generalMdFilepath().isEmpty())
        {
            red_msg += tr("<li><span style=\"color: red;\">Metadata file"
                          ":</span> Load or create one using the Metadata File "
                          "Editor.</li>");
            doRedFix = true;
        }

        if (dlProject_->fileDuration() == 0)
        {
            red_msg += tr("<li><span style=\"color: red;\">Metadata File "
                          "Editor - File Duration:</span> A file duration "
                          "greater than zero is required.</li>");
            doRedFix = true;
        }

        if ((ecProject_->generalFileType() == Defs::RawFileType::GHG
            || ecProject_->generalFileType() == Defs::RawFileType::ASCII)
            && dlProject_->fieldSep().isEmpty())
        {
            red_msg += tr("<li><span style=\"color: red;\">Metadata File "
                          "Editor - Raw File Description:</span> Field "
                          "separator required. Click the \"Raw File "
                          "Settings...\" button.</li>");
            doRedFix = true;
        }

        if ((ecProject_->generalFileType() == Defs::RawFileType::GHG
            || ecProject_->generalFileType() == Defs::RawFileType::ASCII)
            && dlProject_->headerRows() < 0)
        {
            red_msg += tr("<li><span style=\"color: red;\">Metadata File "
                          "Editor - Raw File Description:</span> Number of "
                          "header rows required. Click the \"Raw File "
                          "Settings...\" button.</li>");
            doRedFix = true;
        }

        // anemometer tests
        if (!dlProject_->hasOneGoodAnemometer())
        {
            red_msg += tr("<li><span style=\"color: red;\">Metadata File "
                          "Editor - Instruments:</span> The complete "
                          "description of at least one anemometer is "
                          "required.</li>");
            doRedFix = true;
        }
        else if (dlProject_->hasOneGoodAnemometer())
        {
            if (!dlProject_->hasGoodWindComponentsAndTemperature())
            {
                red_msg += tr("<li><span style=\"color: red;\">Metadata File "
                          "Editor - Raw File Description:</span> The complete "
                          "description of 3-d wind components and at least one "
                          "fast ambient temperature or speed-of-sound "
                          "measurement is required.</li>");
                doRedFix = true;
            }
        }

        if (!dlProject_->masterAnemHasFwVersion()
            && !dlProject_->masterAnemContainsGillWindmaster())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Instruments "
                      "Editor - Raw File Description:</span> "
                      "We suggest to enter the anemometer firmware version "
                      "for sake of record tracking and future implementations.</li>");
            doOrangeFix = true;
        }

        if (!dlProject_->masterAnemHasGoodWindmasterFwVersion())
        {
            red_msg += tr("<li><span style=\"color: red;\">Instruments "
                      "Editor - Raw File Description:</span> "
                      "Enter the Gill Windmaster/Pro "
                      "firmware version in the typical form: 2329.600.01. "
                      "Not filling this field will affect the application of "
                      "the Angle of Attack correction.</li>");
            doRedFix = true;
        }

        // irga tests
        if (!dlProject_->hasGoodIrgaNames())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Metadata File "
                             "Editor - Instruments:</span> One or more closed "
                             "gas analyzers are not well described, "
                             "because of incomplete manufacturer or "
                             "model.</li>");
            doOrangeFix = true;
        }

        if (!dlProject_->hasGoodIrgaSeparations())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Metadata File "
                             "Editor - Instruments:</span> One or more gas "
                             "analyzers do not have at least one separation "
                             "different from zero.</li>");
            doOrangeFix = true;
        }

        if (!dlProject_->hasGoodIrgaClosedPath())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Metadata File "
                             "Editor - Instruments:</span> One or more closed "
                             "path gas analyzers are not well described, "
                             "not having tube length and diameter "
                             "all greater than zero.</li>");
            doOrangeFix = true;
        }

        if (!dlProject_->hasGoodIrgaFlowRate())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Metadata File "
                             "Editor - Instruments:</span> One or more closed "
                             "path gas analyzers are not well described, "
                             "not having flow rate greater than zero.</li>");
            doOrangeFix = true;
        }

        if (!dlProject_->hasGoodIrgaGeneric())
        {
            orange_msg += tr("<li><span style=\"color: orange;\">Metadata File "
                             "Editor - Instruments:</span> One or more generic "
                             "gas analyzers are not well described, "
                             "not having path lengths and time response "
                             "all greater than zero.</li>");
            doOrangeFix = true;
        }

        // other tests
        if (ecProject_->generalFileType() == Defs::RawFileType::BIN)
        {
            if (ecProject_->generalBinaryHNLines() < 0)
            {
                red_msg += tr("<li><span style=\"color: red;\">Generic Binary:"
                          "</span> Number of ASCII header lines required. "
                          "Click the \"Settings...\" button.</li>");
                doRedFix = true;
            }
            if (ecProject_->generalBinaryEol() < 0)
            {
                red_msg += tr("<li><span style=\"color: red;\">Generic Binary:"
                          "</span> ASCII header end of line required. Click "
                          "the \"Settings...\" button.</li>");
                doRedFix = true;
            }
            if (ecProject_->generalBinaryNBytes() <= 0)
            {
                red_msg += tr("<li><span style=\"color: red;\">Generic Binary:"
                          "</span> Number of bytes per variable required. "
                          "Click the \"Settings...\" button.</li>");
                doRedFix = true;
            }
            if (ecProject_->generalBinaryLittleEnd() < 0)
            {
                red_msg += tr("<li><span style=\"color: red;\">Generic Binary:"
                          "</span> Endianess required. Click the \"Settings..."
                          "\" button.</li>");
                doRedFix = true;
            }
        }
    }

    if (ecProject_->generalUseTimelineFile())
    {
        if (ecProject_->generalTimelineFilepath().isEmpty())
        {
            red_msg += tr("<li><span style=\"color: red;\">Dynamic metadata "
                          "file: </span>Select a file using the \"Load...\" "
                          "button or uncheck this option.</li>");
            doRedFix = true;
        }
    }

    if (ecProject_->generalUseBiomet() == 2)
    {
        if (ecProject_->generalBiomFile().isEmpty())
        {
            red_msg += tr("<li><span style=\"color: red;\">Biomet file: </span>"
                      "Select a file using the \"Load...\" button.</li>");
            doRedFix = true;
        }
    }
    else if (ecProject_->generalUseBiomet() == 3)
    {
        if (ecProject_->generalBiomDir().isEmpty())
        {
            red_msg += tr("<li><span style=\"color: red;\">Biomet directory: "
                      "</span>Select a directory using the \"Browse...\" "
                      "button.</li>");
            doRedFix = true;
        }
    }

    if (!doRedFix)
    {
        red_intro.clear();
        red_msg.clear();
    }

    if (!doOrangeFix)
    {
        orange_intro.clear();
        orange_msg.clear();
    }

    basicSettingsPageAvailable_ = red_intro.isEmpty();

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, red_intro + red_msg +
                               orange_intro + orange_msg);
}

void MainWindow::showGuidedModeMessages_2()
{
    QString intro;
    QString msg;
    bool doFix = true;

    if (doFix)
    {
        intro = tr("<p>Some information in the <b>Basic Settings</b> page "
                   "is incomplete. Please address the "
                   "following issues:</p>");
        msg = tr("<ul>");
        doFix = false;
    }

    if (ecProject_->screenDataPath().isEmpty())
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose a Raw data directory.</li>");
        doFix = true;
    }

    if (ecProject_->generalFileType() != Defs::RawFileType::GHG
        && !ecProject_->isGoodRawFilePrototype(ecProject_->generalFilePrototype()))
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Raw file name format not well described.</li>");
        doFix = true;
    }

    if (ecProject_->generalFilePrototype().isEmpty())
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose a Raw file name format.</li>");
        doFix = true;
    }

    if (ecProject_->generalOutPath().isEmpty() && !configState_.project.smartfluxMode)
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose an Output directory.</li>");
        doFix = true;
    }

    if (ecProject_->generalId().isEmpty() && !configState_.project.smartfluxMode)
    {
        msg += tr("<li><span style=\"color: red;\">Files Info:</span> Choose an Output ID.</li>");
        doFix = true;
    }

    AnemDescList *adl = dlProject_->anems();
    if (!adl->isEmpty())
    {
        QString anemModel = ecProject_->generalColMasterSonic();
        if (!anemModel.isEmpty())
        {
            int anemIndex = anemModel.midRef(anemModel.lastIndexOf(QLatin1Char('_')) + 1).toInt();

            // check if valid index position in the list (i.e., 0 <= i < size())
            int i = anemIndex - 1;
            if (i >= 0 && i < adl->size())
            {
                const AnemDesc& anem = adl->at(i);
                if (!anem.hasGoodTemp()
                    && dlProject_->hasOneFastTemperature()
                    && ecProject_->generalColTs() == 0)
                {
                    if (!configState_.project.smartfluxMode)
                    {
                        msg += tr("<li><span style=\"color: red;\">Select Items:</span> Select a Fast temperature reading (alternative to sonic temp), or go back to the Project Creation page and describe either a Sonic Temperature or a Speed-of-Sound measurement for the selected anemometer.</li>");
                        doFix = true;
                    }
                    else
                    {
                        msg += tr("<li><span style=\"color: red;\">Select Items:</span> Select a Fast temperature reading (alternative to sonic temp).</li>");
                        doFix = true;
                    }
                }
            }
        }
    }

    if (!dlProject_->masterAnemHasGoodWindmasterFwVersion())
    {
        msg += tr("<li><span style=\"color: red;\">Missing anemometer firmware version:</span> "
                  "Select <em>Use alternative file</em> in the <em>Project creation page</em> "
                  "and fill the section \"Instruments Editor - Raw File Description\". "
                  "Enter the Gill Windmaster/Pro firmware version in the typical form: 2329.600.01. "
                  "Not filling this field will affect the application of "
                  "the Angle of Attack correction.</li>");
        doFix = true;
    }

    if (!doFix && runAdvancedAvailable_ && !configState_.project.smartfluxMode)
    {
        intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings or <span style=\"color: #2986f5; \">Advanced Mode</span> using Advanced Settings.<br />"
                    "Please note that running in <span style=\"color: #52893c; \">Express Mode</span> means EddyPro will ignore all your entries in the Advanced Settings pages. In this case, your settings will not be overridden. You will be able to retrieve them at any time, but they will not be used for the computations.");
        msg = tr("<ul>");
        doFix = false;
    }
    else if (!doFix && !runAdvancedAvailable_ && !configState_.project.smartfluxMode)
    {
        intro = tr("You are ready to run in <span style=\"color: #52893c; \">Express Mode</span> using express default settings.<br />");
        msg = tr("<ul>");
        doFix = false;
    }

    if (!doFix)
    {
        msg += tr("</ul>");
    }

    runExpressAvailable_ = intro.contains(tr("You are ready"));

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, intro + msg);
}

void MainWindow::showGuidedModeMessages_3()
{
    QString intro;
    QString msg;
    bool doFix = true;

    if (doFix)
    {
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
    }

    if (ecProject_->screenRotMethod() >= 3)
    {
        if (ecProject_->planarFitMode())
        {
            if (!ecProject_->hasPlanarFitFullAngle())
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Complete wind sectors configuration.</li>");
                doFix = true;
            }

            if (ecProject_->planarFitItemPerSector() == 0)
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Minimum number of elements per sector.</li>");
                doFix = true;
            }

            if (qFuzzyCompare(ecProject_->planarFitWmax(), 0.099))
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Maximum mean vertical wind component.</li>");
                doFix = true;
            }

            if (qFuzzyCompare(ecProject_->planarFitUmin(), -0.001))
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Set Minimum mean horizontal wind component.</li>");
                doFix = true;
            }
        }
        else
        {
            if (ecProject_->planarFitFile().isEmpty())
            {
                msg += tr("<li><span style=\"color: red;\">Planar Fit Settings:</span> Load a Planar fit file.</li>");
                doFix = true;
            }
        }
    }
    else
    {
        if (!doFix)
            doFix = false;
    }

    if (ecProject_->screenTlagMeth() == 4)
    {
        if (ecProject_->timelagOptMode())
        {
        }
        else
        {
            if (ecProject_->timelagOptFile().isEmpty())
            {
                msg += tr("<li><span style=\"color: red;\">Timelag Optimization Settings:</span> Load a Time lag file.</li>");
                doFix = true;
            }
        }
    }
    else
    {
        if (!doFix)
            doFix = false;
    }

    if (ecProject_->generalHfMethod() > 1 && ecProject_->generalHfMethod() < 5)
    {
        if (ecProject_->spectraMode() == 0)
        {
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
                if (ecProject_->spectraBinSpectra().isEmpty())
                {
                    msg += tr("<li><span style=\"color: red;\">Spectral Correction Settings:</span> Choose a valid path, where 'Binned (co)spectra' files are stored.</li>");
                    doFix = true;
                }
            }

            if (ecProject_->generalFullSpectraAvail())
            {
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
    }

    if (!doFix)
    {
        msg += tr("</ul>");
    }
    runAdvancedAvailable_ = !intro.contains(tr("Additional settings")) && runExpressAvailable_;

    updateMenuActionStatus(currentPage());

    updateHtmlDock(infoOutput, intro + msg);
}

void MainWindow::updateRunButtonsAvailability()
{
    if (currentPage() == Defs::CurrPage::Welcome
        || configState_.project.smartfluxMode)
    {
        runExpressAction->setEnabled(false);
        runAdvancedAction->setEnabled(false);
        runRetrieverAction->setEnabled(false);
        stopAction->setEnabled(false);
    }
    else
    {
        Defs::CurrStatus status = currentStatus();
        Defs::CurrRunStatus run_status = currentRunStatus();

        runExpressAction->setEnabled(basicSettingsPageAvailable_
            && runExpressAvailable_
            && (status == Defs::CurrStatus::Ready
             || (status == Defs::CurrStatus::Run
                 && run_status == Defs::CurrRunStatus::Express)
             || (status == Defs::CurrStatus::Pause
                 && run_status == Defs::CurrRunStatus::Express)));
        runAdvancedAction->setEnabled(basicSettingsPageAvailable_
            && runExpressAvailable_
            && runAdvancedAvailable_
            && (status == Defs::CurrStatus::Ready
                || (status == Defs::CurrStatus::Run
                    && run_status == Defs::CurrRunStatus::Advanced_RP)
                || (status == Defs::CurrStatus::Pause
                    && run_status == Defs::CurrRunStatus::Advanced_RP)));
        runRetrieverAction->setEnabled(runExpressAction->isEnabled()
            && !ecProject_->generalUseAltMdFile()
            && (status == Defs::CurrStatus::Ready
                || (status == Defs::CurrStatus::Run
                    && run_status == Defs::CurrRunStatus::Retriever)
                || (status == Defs::CurrStatus::Pause
                    && run_status
                    == Defs::CurrRunStatus::Retriever)));
        stopAction->setEnabled(basicSettingsPageAvailable_
            && (runExpressAvailable_ || runAdvancedAvailable_));
    }
}

void MainWindow::changePage(Defs::CurrPage page, bool testCurrentPage)
{
    if (testCurrentPage)
    {
        if (mainWidget_->currentPage() != page)
        {
            mainWidget_->setCurrentPage(page);
            updatePage(page);
            windowTitleUpdate(page);

            if (page == Defs::CurrPage::Run)
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
    if (page == Defs::CurrPage::BasicSettings)
    {
        emit updateMetadataReadRequest();

        // to ease the migration from 5.2.1
        // for existing GHG projects with rawe data dir set,
        // but no raw file name format set
        if (!newFlag_
            && !ecProject_->screenDataPath().isEmpty()
            && ecProject_->generalFileType() == Defs::RawFileType::GHG
            && ecProject_->generalFilePrototype().isEmpty())
        {
            emit mainWidget_->showSetPrototypeRequest();
        }
    }
    previousPage_ = currentPage();
    setCurrentPage(page);
    updateMenuActionStatus(page);
    changeViewToolbarSeparators(page);
    togglePageButton(page);
}

void MainWindow::togglePageButton(Defs::CurrPage page)
{
    switch (page)
    {
        case Defs::CurrPage::Welcome:
            if (!viewWelcomeAction->isChecked())
                viewWelcomeAction->setChecked(true);
            break;
        case Defs::CurrPage::ProjectCreation:
            if (!viewProjectCreationAction->isChecked())
                viewProjectCreationAction->setChecked(true);
            break;
        case Defs::CurrPage::BasicSettings:
            if (!viewBasicSettingsAction->isChecked())
                viewBasicSettingsAction->setChecked(true);
            break;
        case Defs::CurrPage::AdvancedSettings:
            if (!viewAdvancedAction->isChecked())
                viewAdvancedAction->setChecked(true);
            break;
        case Defs::CurrPage::Run:
            if (!viewRunPageAction->isChecked())
                viewRunPageAction->setChecked(true);
            break;
//        default:
//            break;
    }
}

void MainWindow::changeViewToolbarSeparators(Defs::CurrPage page)
{
    auto sep_left_selected_2x = QPixmap(QStringLiteral(":/icons/sepleftsel"));
    auto sep_normal_2x = QPixmap(QStringLiteral(":/icons/sep"));
    auto sep_right_selected_2x = QPixmap(QStringLiteral(":/icons/seprightsel"));

#if defined(Q_OS_MACOS)
    sep_left_selected_2x.setDevicePixelRatio(2.0);
    sep_normal_2x.setDevicePixelRatio(2.0);
    sep_right_selected_2x.setDevicePixelRatio(2.0);
#endif

    switch (page)
    {
        case Defs::CurrPage::Welcome:
            sep1->setPixmap(sep_left_selected_2x);
            sep2->setPixmap(sep_normal_2x);
            sep3->setPixmap(sep_normal_2x);
            sep4->setPixmap(sep_normal_2x);
            connect(sep1, &ClickLabel::clicked,
                    viewProjectCreationAction, &QAction::trigger);
            disconnect(sep2);
            disconnect(sep3);
            disconnect(sep4);
            break;
        case Defs::CurrPage::ProjectCreation:
            sep1->setPixmap(sep_right_selected_2x);
            sep2->setPixmap(sep_left_selected_2x);
            sep3->setPixmap(sep_normal_2x);
            sep4->setPixmap(sep_normal_2x);
            disconnect(sep1);
            if (basicSettingsPageAvailable_)
            {
                connect(sep2, &ClickLabel::clicked,
                        viewBasicSettingsAction, &QAction::trigger);
                connect(sep3, &ClickLabel::clicked,
                        viewAdvancedAction, &QAction::trigger);
            }
            else
            {
                disconnect(sep2);
                disconnect(sep3);
            }
            connect(sep4, &ClickLabel::clicked,
                    viewRunPageAction, &QAction::trigger);
            break;
        case Defs::CurrPage::BasicSettings:
            sep1->setPixmap(sep_normal_2x);
            sep2->setPixmap(sep_right_selected_2x);
            sep3->setPixmap(sep_left_selected_2x);
            sep4->setPixmap(sep_normal_2x);
            connect(sep1, &ClickLabel::clicked,
                    viewBasicSettingsAction, &QAction::trigger);
            disconnect(sep2);
            connect(sep3, &ClickLabel::clicked,
                    viewAdvancedAction, &QAction::trigger);
            connect(sep4, &ClickLabel::clicked,
                    viewRunPageAction, &QAction::trigger);
            break;
        case Defs::CurrPage::AdvancedSettings:
            sep1->setPixmap(sep_normal_2x);
            sep2->setPixmap(sep_normal_2x);
            sep3->setPixmap(sep_right_selected_2x);
            sep4->setPixmap(sep_left_selected_2x);
            connect(sep1, &ClickLabel::clicked,
                    viewProjectCreationAction, &QAction::trigger);
            connect(sep2, &ClickLabel::clicked,
                    viewBasicSettingsAction, &QAction::trigger);
            disconnect(sep3);
            connect(sep4, &ClickLabel::clicked,
                    viewRunPageAction, &QAction::trigger);
            break;
        case Defs::CurrPage::Run:
            sep1->setPixmap(sep_normal_2x);
            sep2->setPixmap(sep_normal_2x);
            sep3->setPixmap(sep_normal_2x);
            sep4->setPixmap(sep_right_selected_2x);
            connect(sep1, &ClickLabel::clicked,
                    viewProjectCreationAction, &QAction::trigger);
            connect(sep2, &ClickLabel::clicked,
                    viewBasicSettingsAction, &QAction::trigger);
            connect(sep3, &ClickLabel::clicked,
                    viewAdvancedAction, &QAction::trigger);
            disconnect(sep4);
            break;
//        default:
//            break;
    }
}

void MainWindow::fileOpenRequest(const QString& file)
{
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

    connect(engineProcess_, &Process::processFailure,
            this, &MainWindow::displayExitDialog);
    connect(engineProcess_, &Process::processSuccess,
            this, &MainWindow::displayExitDialog);

    connect(engineProcess_, &Process::processFailure,
            mainWidget_->runPage(), &RunPage::resetBuffer);
    connect(engineProcess_,&Process::processSuccess,
            mainWidget_->runPage(), &RunPage::resetBuffer);

    connect(engineProcess_, &Process::readyReadStdOut,
            this, &MainWindow::updateConsoleReceived);
    connect(engineProcess_, &Process::readyReadStdErr,
            this, &MainWindow::updateConsoleError);
}

void MainWindow::setMetadataRead(bool b)
{
    metadataReadFlag_ = b;
}

int MainWindow::testBeforeRunningPassed()
{
    int returnValue = QMessageBox::Yes;

    if (!metadataReadFlag_)
    {
        changePage(Defs::CurrPage::BasicSettings);
    }

    if (!fileSaveSilently())
    {
        return QMessageBox::Cancel;
    }
    return returnValue;
}

void MainWindow::closeOpenDialogs()
{
    QList<QDialog *> childDialogs = findChildren<QDialog *>(QString());

    for (auto dialog : childDialogs)
    {
        if (dialog->objectName() != QStringLiteral("DlIniDialog")
            && dialog->objectName() != QStringLiteral("RawFilenameDialog"))
        {
            dialog->accept();
        }
    }
}

FileUtils::DateRange MainWindow::getCurrentDateRange()
{
    auto recursion = ecProject_->screenRecurse();
    auto ghgFormat = QStringLiteral("*.") + Defs::GHG_NATIVE_DATA_FILE_EXT;
    auto currentRawDataList = QStringList();
    FileUtils::DateRange dates = qMakePair(QDateTime(), QDateTime());

    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        currentRawDataList = FileUtils::getFiles(ecProject_->screenDataPath(), ghgFormat, recursion);
    }
    else
    {
        auto extensionIndex = ecProject_->generalFilePrototype().lastIndexOf(QLatin1String(".")) + 1;
        auto extension = QStringLiteral("*.") + ecProject_->generalFilePrototype().mid(extensionIndex);
        currentRawDataList = FileUtils::getFiles(ecProject_->screenDataPath(), extension, recursion);
    }

    if (!currentRawDataList.isEmpty())
    {
        QFuture<FileUtils::DateRange> future = QtConcurrent::run(&FileUtils::getDateRangeFromFileList,
                                                                        currentRawDataList,
                                                                        ecProject_->generalFilePrototype());
        while (!future.isFinished())
        {
            QCoreApplication::processEvents();
        }
        dates = future.result();

        // correct the start/end date accounting for file duration
        if (dlProject_->timestampEnd() == 0)
        {
            dates.second = dates.second.addSecs(dlProject_->fileDuration() * 60);
        }
        else
        {
            dates.first = dates.first.addSecs(-dlProject_->fileDuration() * 60);
        }
    }
    return dates;
}

bool MainWindow::showDatesRangeDialog(Defs::CurrRunMode mode)
{
    // TODO: complete management of this persistent setting
    auto showDialog
        = GlobalSettings::getAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                                   Defs::CONF_WIN_DATES_CHECK_MSG,
                                                   true).toBool();
    if (!showDialog) { return true; }

    DetectDateRangeDialog detectDateRangeDialog(this, ecProject_, dlProject_);

    connect(&detectDateRangeDialog, &DetectDateRangeDialog::alignDeclinationRequest,
            mainWidget_->basicPage(), &BasicSettingsPage::alignDeclinationDate);
    connect(&detectDateRangeDialog, &DetectDateRangeDialog::refreshRdRequest,
            mainWidget_->basicPage(), &BasicSettingsPage::partialRefresh);
    connect(&detectDateRangeDialog, &DetectDateRangeDialog::refreshPfRequest,
            mainWidget_->pfDialog(), &PlanarFitSettingsDialog::partialRefresh);
    connect(&detectDateRangeDialog, &DetectDateRangeDialog::refreshTlRequest,
            mainWidget_->tlDialog(), &TimeLagSettingsDialog::partialRefresh);
    connect(&detectDateRangeDialog, &DetectDateRangeDialog::refreshSpRequest,
            mainWidget_->spectralOptions(), &AdvSpectralOptions::partialRefresh);

    // detect the actual range and compare with the subsets
    detectDateRangeDialog.setCurrentRange(getCurrentDateRange());

    if (mode == Defs::CurrRunMode::Express)
    {
        if (ecProject_->generalSubset())
        {
            detectDateRangeDialog.showDateRange(DetectDateRangeDialog::DateRangeType::RawData);
        }
    }
    else
    {
        detectDateRangeDialog.showDateRange(DetectDateRangeDialog::DateRangeType::RawData);
        detectDateRangeDialog.showDateRange(DetectDateRangeDialog::DateRangeType::Spectra);
        detectDateRangeDialog.showDateRange(DetectDateRangeDialog::DateRangeType::PlanarFit);
        detectDateRangeDialog.showDateRange(DetectDateRangeDialog::DateRangeType::TimeLag);
    }

    return detectDateRangeDialog.exec();
}

bool MainWindow::getDatesRangeDialog(Defs::CurrRunMode mode)
{
    // verify if there are subperiods set

    // raw data subperiod
    auto isSubperiodSet = ecProject_->generalSubset();

    if (mode == Defs::CurrRunMode::Express)
    {
        if (!isSubperiodSet) return true;
    }
    else
    {
        isSubperiodSet = isSubperiodSet
                         || ecProject_->spectraSubset()
                         || ecProject_->planarFitSubset()
                         || ecProject_->timelagOptSubset();
    }

    // if there are subperiods and we are not in paused run,
    // notify with a blocking dialog
    if (isSubperiodSet && currentStatus() == Defs::CurrStatus::Ready)
    {
        auto dialogAccepted = showDatesRangeDialog(mode);
        return dialogAccepted;
    }
    return true;
}

void MainWindow::getRunExpress()
{
    auto status = currentStatus();
    auto runStatus = currentRunStatus();

    if (status == Defs::CurrStatus::Run
        || status == Defs::CurrStatus::Pause)
    {
        pauseResumeComputations(runStatus);
        return;
    }

    closeOpenDialogs();

    auto showDialog = GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_WINDOW,
                                        Defs::CONF_WIN_RUN_EXP_MSG,
                                        true).toBool();

    if (showDialog && status == Defs::CurrStatus::Ready)
    {
        InfoMessage runExpressDialog(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel, this);
        runExpressDialog.setTitle(tr("Running"));
        runExpressDialog.setType(InfoMessage::Type::RUN_EXPRESS);
        runExpressDialog.setIcon(QPixmap(QStringLiteral(":/icons/msg-question")));
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

        if (runExpressDialog.exec() == QMessageBox::Cancel) { return; }
    }
    emit updateMetadataReadRequest();

    // detect date range and verify date subset intersection
    if (!getDatesRangeDialog(Defs::CurrRunMode::Express)) return;

    runExpress();
}

void MainWindow::runExpress()
{
    Defs::CurrStatus status = currentStatus();
    expressClicked_ = true;

    if (status == Defs::CurrStatus::Ready)
    {
        if (testBeforeRunningPassed() == QMessageBox::Cancel) { return; }

        changePage(Defs::CurrPage::Run);
        QString workingDir = QApplication::applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR;
        QString engineFilePath(workingDir + QLatin1Char('/') + Defs::ENGINE_RP);

        ecProject_->setGeneralRunMode(Defs::CurrRunMode::Express);
        if (!fileSaveSilently()) { return; }

        QStringList args;

        args << QStringLiteral("-c");
        args << QStringLiteral("gui");
        args << QStringLiteral("-s");
        args << Defs::HOST_OS;
        args << QStringLiteral("-e");
        args << appEnvPath_;
        args << ecProject_->generalFileName();

        engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

        // block until the process truly start to ensure reliable behavior
        // of the gui
        engineProcess_->process()->waitForStarted();

        if (engineProcess_->isRunning())
        {
            setCurrentStatus(Defs::CurrStatus::Run);
            setCurrentRunStatus(Defs::CurrRunStatus::Express);
            mainWidget_->runPage()->startRun(Defs::CurrRunStatus::Express);

            setRunExpIcon2Pause();
        }
    }
}

void MainWindow::getRunAdvanced()
{
    auto status = currentStatus();
    auto runStatus = currentRunStatus();

    if (status == Defs::CurrStatus::Run
        || status == Defs::CurrStatus::Pause)
    {
        pauseResumeComputations(runStatus);
        return;
    }

    closeOpenDialogs();

    bool showDialog = GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_WINDOW,
                                        Defs::CONF_WIN_RUN_ADV_MSG,
                                        true).toBool();

    if (showDialog && currentStatus() == Defs::CurrStatus::Ready)
    {
        InfoMessage runAdvancedDialog(QDialogButtonBox::Ok
                                       | QDialogButtonBox::Cancel, this);
        runAdvancedDialog.setTitle(tr("Running"));
        runAdvancedDialog.setType(InfoMessage::Type::RUN_ADVANCED);
        runAdvancedDialog.setIcon(QPixmap(QStringLiteral(":/icons/msg-question")));
        runAdvancedDialog.setMessage(tr("Running in Advanced mode means you "
                                       "run using the <br />"
                                       "advanced settings of EddyPro"));
        runAdvancedDialog.refresh();

        if (runAdvancedDialog.exec() == QMessageBox::Cancel)
        {
            return;
        }
    }
    emit updateMetadataReadRequest();

    // detect date range and verify date subset intersection
    if (!getDatesRangeDialog(Defs::CurrRunMode::Advanced)) return;

    runAdvancedStep_1();
}

void MainWindow::runAdvancedStep_1()
{
    Defs::CurrStatus status = currentStatus();

    advancedClicked_ = true;

    if (status == Defs::CurrStatus::Ready)
    {
        int ret = testBeforeRunningPassed();
        if (ret == QMessageBox::Cancel) { return; }

        // start from step 1
        if (ret == QMessageBox::Yes)
        {
            changePage(Defs::CurrPage::Run);
            QString workingDir = QApplication::applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR;
            QString engine1FilePath(workingDir + QLatin1Char('/') + Defs::ENGINE_RP);

            ecProject_->setGeneralRunMode(Defs::CurrRunMode::Advanced);
            if (!fileSaveSilently()) { return; }

            QStringList args;
            args << QStringLiteral("-c");
            args << QStringLiteral("gui");
            args << QStringLiteral("-s");
            args << Defs::HOST_OS;
            args << QStringLiteral("-e");
            args << appEnvPath_;
            args << ecProject_->generalFileName();
            engineProcess_->engineProcessStart(engine1FilePath, workingDir, args);

            // block until the process truly start to ensure reliable behavior
            // of the gui
            engineProcess_->process()->waitForStarted();

            if (engineProcess_->isRunning())
            {
                setCurrentStatus(Defs::CurrStatus::Run);
                setCurrentRunStatus(Defs::CurrRunStatus::Advanced_RP);
                mainWidget_->runPage()->startRun(Defs::CurrRunStatus::Advanced_RP);
                setRunAdvIcon2Pause();

                neededEngineStep2_ = ecProject_->isEngineStep2Needed();
            }
        }
        // jump to step 2
        else if (ret == QMessageBox::No)
        {
            runAdvancedStep_2();
            return;
        }
    }
}

void MainWindow::runAdvancedStep_2()
{
    Defs::CurrStatus status = currentStatus();

    advancedClicked_ = true;

    if (status == Defs::CurrStatus::Ready)
    {
        updateSpectraPaths();

        if (testBeforeRunningPassed() == QMessageBox::Cancel) { return; }

        changePage(Defs::CurrPage::Run);
        QString workingDir = QApplication::applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR;
        QString engineFilePath(workingDir + QLatin1Char('/') + Defs::ENGINE_FCC);

        ecProject_->setGeneralRunMode(Defs::CurrRunMode::Advanced);

        if (!fileSaveSilently()) { return; }

        QStringList args;
        args << QStringLiteral("-c");
        args << QStringLiteral("gui");
        args << QStringLiteral("-s");
        args << Defs::HOST_OS;
        args << QStringLiteral("-e");
        args << appEnvPath_;
        args << ecProject_->generalFileName();
        engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

        // block until the process truly start to ensure reliable behavior
        // of the gui
        engineProcess_->process()->waitForStarted();

        if (engineProcess_->isRunning())
        {
            setCurrentStatus(Defs::CurrStatus::Run);
            setCurrentRunStatus(Defs::CurrRunStatus::Advanced_RP);
            mainWidget_->runPage()->startRun(Defs::CurrRunStatus::Advanced_FCC);
            setRunAdvIcon2Pause();
            neededEngineStep2_ = false;
        }
    }
}

void MainWindow::getRunRetriever()
{
    auto status = currentStatus();
    auto runStatus = currentRunStatus();

    if (status == Defs::CurrStatus::Run
        || status == Defs::CurrStatus::Pause)
    {
        pauseResumeComputations(runStatus);
        return;
    }

    closeOpenDialogs();

    bool showDialog = GlobalSettings::getAppPersistentSettings(
                                        Defs::CONFGROUP_WINDOW,
                                        Defs::CONF_WIN_RUN_RET_MSG,
                                        true).toBool();

    if (showDialog && currentStatus() == Defs::CurrStatus::Ready)
    {
        InfoMessage runRetrieverDialog(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Cancel, this);
        runRetrieverDialog.setTitle(tr("Running"));
        runRetrieverDialog.setType(InfoMessage::Type::RUN_RETRIEVER);
        runRetrieverDialog.setIcon(QPixmap(QStringLiteral(":/icons/msg-question")));
        runRetrieverDialog.setMessage(tr("The metadata retriever is a running mode. "
                                        "It will assess the metadata in all GHG "
                                        "files and provide the metadata in a single "
                                        "alternative metadata file that you can "
                                        "examine and edit.<br />"
                                        "Do you really want to run the Metadata "
                                        "Retriever?"));
        runRetrieverDialog.refresh();

        if (runRetrieverDialog.exec() == QMessageBox::Cancel) { return; }
        emit updateMetadataReadRequest();
    }
    runRetriever();
}

void MainWindow::runRetriever()
{
    Defs::CurrStatus status = currentStatus();
    retrieverClicked_ = true;

    if (status == Defs::CurrStatus::Ready)
    {
        if (testBeforeRunningPassed() == QMessageBox::Cancel) { return; }

        changePage(Defs::CurrPage::Run);
        QString workingDir = QApplication::applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR;
        QString engineFilePath(workingDir + QLatin1Char('/') + Defs::ENGINE_RP);

        ecProject_->setGeneralRunMode(Defs::CurrRunMode::Retriever);
        emit checkMetadataOutputRequest();
        if (!fileSaveSilently()) { return; }

        QStringList args;
        args << QStringLiteral("-c");
        args << QStringLiteral("gui");
        args << QStringLiteral("-s");
        args << Defs::HOST_OS;
        args << QStringLiteral("-e");
        args << appEnvPath_;
        args << ecProject_->generalFileName();
        engineProcess_->engineProcessStart(engineFilePath, workingDir, args);

        // block until the process truly start to ensure reliable behavior
        // of the gui
        engineProcess_->process()->waitForStarted();

        if (engineProcess_->isRunning())
        {
            setCurrentStatus(Defs::CurrStatus::Run);
            setCurrentRunStatus(Defs::CurrRunStatus::Retriever);
            mainWidget_->runPage()->startRun(Defs::CurrRunStatus::Retriever);
            setRunRetIcon2Pause();
        }
    }
}

void MainWindow::pauseResumeComputations(Defs::CurrRunStatus mode)
{
    Defs::CurrStatus status = currentStatus();

    if (status == Defs::CurrStatus::Run)
    {
        if (pauseEngine(mode))
        {
            setCurrentStatus(Defs::CurrStatus::Pause);
            engineResumableFlag_ = true;

            if (mode == Defs::CurrRunStatus::Express)
            {
                setRunExpIcon2Resume();
            }
            else if (mode == Defs::CurrRunStatus::Advanced_RP
                     || mode == Defs::CurrRunStatus::Advanced_FCC)
            {
                setRunAdvIcon2Resume();

            }
            else if (mode == Defs::CurrRunStatus::Retriever)
            {
                setRunRetIcon2Resume();
            }
        }
    }
    else if (status == Defs::CurrStatus::Pause)
    {
        if (resumeEngine(mode))
        {
            setCurrentStatus(Defs::CurrStatus::Run);
            engineResumableFlag_ = false;

            if (mode == Defs::CurrRunStatus::Express)
            {
                setRunExpIcon2Pause();
            }
            else if (mode == Defs::CurrRunStatus::Advanced_RP
                     || mode == Defs::CurrRunStatus::Advanced_FCC)
            {
                setRunAdvIcon2Pause();

            }
            else if (mode == Defs::CurrRunStatus::Retriever)
            {
                setRunRetIcon2Pause();
            }
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
        if (okToStopRun())
        {
            stopEngineProcess();
        }
    }
}

void MainWindow::stopEngineProcess()
{
    engineProcess_->processStop();

    // return to default running mode
    ecProject_->setGeneralRunMode(Defs::CurrRunMode::Advanced);

    displayExitDialog();

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;

    fileSave();

    cleanEnvTmpDir();
}

void MainWindow::cleanEnvTmpDir()
{
    FileUtils::cleanDirRecursively(appEnvPath_
                                   + QLatin1Char('/')
                                   + Defs::TMP_FILE_DIR);
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

// useful to close old message box after completion of a run
void MainWindow::cleanOverdueMessageBox(const QString& messageBoxName)
{
    auto childMessageBox = findChildren<QMessageBox *>(messageBoxName);
    for (auto child : childMessageBox)
    {
        child->close();
    }
}

void MainWindow::displayExitDialog()
{
    cleanOverdueMessageBox(QStringLiteral("saveDuringRunMessage"));
    cleanOverdueMessageBox(QStringLiteral("stopMessage"));

    updateConsoleReceived();
    setCurrentStatus(Defs::CurrStatus::Ready);
    setCurrentRunStatus(Defs::CurrRunStatus::Express);
    mainWidget_->runPage()->stopRun();

    if (!neededEngineStep2_)
    {
        resetRunIcons();
        updateMenuActionStatus(currentPage());
        displayExitMsg(engineProcess_->processExit());
    }
    else
    {
        displayExitMsg2(engineProcess_->processExit());
    }
}

void MainWindow::displayExitMsg(Process::ExitStatus exitReason)
{
    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);

    // set dialog flags
    Qt::WindowFlags winFflags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint;
    msgBox.setWindowFlags(winFflags);

    WidgetUtils::removeContextHelpButton(&msgBox);

    msgBox.setTextFormat(Qt::RichText);
    msgBox.setWindowTitle(tr("EddyPro Results"));

    auto openOutDirButton = new QPushButton(tr("Open the output folder"));
    auto questionMark_1 = new QPushButton;
    auto pixmap_2x = QPixmap(QStringLiteral(":/icons/qm-enabled"));
#if defined(Q_OS_MACOS)
    pixmap_2x.setDevicePixelRatio(2.0);
#endif
    questionMark_1->setIcon(pixmap_2x);
    questionMark_1->setFixedSize(77, 25);

    connect(questionMark_1, &QPushButton::clicked,
            this, &MainWindow::onlineHelpTrigger_1);

    QProcess::ProcessError last_error = engineProcess_->processError();

    switch (exitReason)
    {
    case Process::ExitStatus::Success:
        msgBox.setText(tr("<h3>Data processed successfully!</h3>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.addButton(openOutDirButton, QMessageBox::ActionRole);
        msgBox.setDefaultButton(openOutDirButton);
        msgBox.setEscapeButton(QMessageBox::Ok);
        break;
    case Process::ExitStatus::FailureToStart:
        msgBox.setText(tr("<h3>Engine (eddypro_rp) not found</h3>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        break;
    case Process::ExitStatus::Failure:
        msgBox.setText(tr("<h3>Oops, an error has occurred.</h3>"));
        msgBox.setInformativeText(tr("<p>Double check the project parameters and check "
                                     "the output console for more information.</p>"));
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    case Process::ExitStatus::Error:
    {
        msgBox.setText(tr("<h3>Oops, an error has occurred.</h3>"));
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
        msgBox.setEscapeButton(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    }
    case Process::ExitStatus::Stopped:
        msgBox.setText(tr("<h3>Program stopped!</h3>"
                          "<p style=\"font-style: normal; font-weight: normal;\">Preliminary results have been stored "
                          "in the selected output folder. Depending on your "
                          "processing settings, these results may be fully "
                          "valid for analysis. You can remove the temporary "
                          "extension \".tmp\" and use them. However, the "
                          "format of these files does not allow EddyPro to "
                          "use them in subsequent runs.</p>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        break;
    }

    msgBox.exec();

    if (msgBox.clickedButton() == openOutDirButton)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(ecProject_->generalOutPath()));
    }
    delete openOutDirButton;
    delete questionMark_1;

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;
}

void MainWindow::displayExitMsg2(Process::ExitStatus exitReason)
{
    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);

    // set dialog flags
    Qt::WindowFlags winFflags = Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint;
    msgBox.setWindowFlags(winFflags);

    WidgetUtils::removeContextHelpButton(&msgBox);

    msgBox.setTextFormat(Qt::RichText);
    msgBox.setWindowTitle(tr("EddyPro Results"));

    auto questionMark_1 = new QPushButton;
    auto pixmap_2x = QPixmap(QStringLiteral(":/icons/qm-enabled"));
#if defined(Q_OS_MACOS)
    pixmap_2x.setDevicePixelRatio(2.0);
#endif
    questionMark_1->setIcon(pixmap_2x);
    questionMark_1->setFixedSize(77, 25);

    connect(questionMark_1, &QPushButton::clicked,
            this, &MainWindow::onlineHelpTrigger_1);

    QProcess::ProcessError last_error = engineProcess_->processError();

    switch (exitReason)
    {
    case Process::ExitStatus::Success:
        runAdvancedStep_2();
        break;
    case Process::ExitStatus::FailureToStart:
        msgBox.setText(tr("<h3>Engine (eddypro_fcc) not found!</h3>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        break;
    case Process::ExitStatus::Failure:
        msgBox.setText(tr("<h3>Oops, an error has occurred.<h3>"));
        msgBox.setInformativeText(tr("<p>Double check the project parameters and check "
                                     "the output console for more information.</p>"));
        msgBox.addButton(questionMark_1, QMessageBox::ActionRole);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    case Process::ExitStatus::Error:
    {
        msgBox.setText(tr("<h3>Oops, an error has occurred.</h3>"));
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
        msgBox.setEscapeButton(QMessageBox::Ok);
        msgBox.setDefaultButton(questionMark_1);
        break;
    }
    case Process::ExitStatus::Stopped:
        msgBox.setText(tr("<h3>Program stopped!</h3>"
                          "<p style=\"font-style: normal; font-weight: normal;\">Preliminary results have been stored "
                          "in the selected output folder. Depending on your "
                          "processing settings, these results may be fully "
                          "valid for analysis. You can remove the temporary "
                          "extension \".tmp\" and use them. However, the "
                          "format of these files does not allow EddyPro to "
                          "use them in subsequent runs.</p>"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setEscapeButton(QMessageBox::Ok);
        break;
    }

    if (exitReason != Process::ExitStatus::Success)
    {
        resetRunIcons();
        updateMenuActionStatus(currentPage());
        msgBox.exec();
    }
    delete questionMark_1;

    expressClicked_ = false;
    advancedClicked_ = false;
    retrieverClicked_ = false;
}

void MainWindow::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Error_Codes.html")));
}

// qt5
void MainWindow::updateConsoleLine(QByteArray &data)
{
    consoleOutput->appendPlainText(QLatin1String(data.trimmed().constData()));
}

void MainWindow::updateConsoleChar(QByteArray &data)
{
    consoleOutput->insertPlainText(QLatin1String(data.trimmed().constData()));
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
    QByteArray newData = engineProcess_->readAllStdErr();

    if (!newData.isEmpty())
    {
        updateConsoleLine(newData);
    }
}

bool MainWindow::okToStopRun()
{
    return WidgetUtils::yesNoQuestion(this,
                                tr("Stop Data Processing"),
                                tr("EddyPro is processing (running or in pause)."),
                                tr("Do you really want to "
                                   "stop the computations?"),
                                QStringLiteral("stopMessage"));
}

// NOTE: hack to prevent that WindowStateChange events can change visibility of
// the console. Indeed those events are able to fire a toggled signals on the
// toggleConsoleOutputAct action and so hide the console
bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::WindowStateChange)
    {
        if (windowState() == Qt::WindowFullScreen)
        {
//            removeSplashScreen();
            configState_.window.fullScreen = true;
        }
        else if (windowState() == Qt::WindowNoState)
        {
            configState_.window.fullScreen = false;
        }
        writeSettings();
    }

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

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QSize widgetSize = event->size();
    QSize widgetOldSize = event->oldSize();

    qDebug() << "size" << widgetSize;
    qDebug() << "old size" << widgetOldSize;

    if (widgetSize.width() <= 1200 || widgetSize.height() <= 900)
    {
        minimizeGui();
    }
    else
    {
        maximizeGui();
    }

    if (windowState() == Qt::WindowState::WindowMaximized)
    {
        maximizeGui();
    }

    updateStatusBar();

    event->accept();
}

void MainWindow::minimizeGui()
{
    fileToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    viewToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    runToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    mainWidget_->welcomePage()->updateWelcomePage(true);
    mainWidget_->runPage()->updateRunPage(true);

    configState_.general.recentnum = 2;
}

void MainWindow::maximizeGui()
{
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    viewToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    runToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mainWidget_->welcomePage()->updateWelcomePage(false);
    mainWidget_->runPage()->updateRunPage(false);

    updateMenuActionStatus(currentPage());

    configState_.general.recentnum = 4;
}

void MainWindow::updateSpectraPaths()
{
    // reload after engine_rp updates the ex_file path, i.e. when it finishes
    bool modified = false;
    if (ecProject_->loadEcProject(ecProject_->generalFileName(), false, &modified))
    {
        if (ecProject_->generalBinSpectraAvail())
        {
            ecProject_->setSpectraBinSpectra(ecProject_->spectraBinSpectra());
        }
        else
        {
            ecProject_->setSpectraBinSpectra(ecProject_->generalOutPath() + Defs::OUT_BINNED_COSPECTRA_DIR);
        }

        if (ecProject_->generalFullSpectraAvail())
        {
            ecProject_->setSpectraFullSpectra(ecProject_->spectraFullSpectra());
        }
        else
        {
            ecProject_->setSpectraFullSpectra(ecProject_->generalOutPath() + QStringLiteral("/eddypro_full_cospectra"));
        }

        fileSaveSilently();
    }
}

void MainWindow::showUpdateDialog()
{
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
    if (updateDialog->hasNewVersion())
    {
        updateDialog->show();
        updateDialog->raise();
        updateDialog->activateWindow();
    }
    else
    {
        qDebug() << "no NEW VERSION";
    }
}

void MainWindow::openLicorSite() const
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.licor.com/env/")));
}

void MainWindow::checkInternetConnection()
{
#if 0
    DEBUG_FUNC_MSG(QStringLiteral("online: %1")
                   .arg(Networking::isOnline() ? QStringLiteral("true")
                                               : QStringLiteral("false")))
#endif
}

void MainWindow::connectBinarySettingsDialog()
{
    BinarySettingsDialog* binary_settings_dialog =
            mainWidget_->projectPage()->getBinarySettingsDialog();

    connect(binary_settings_dialog, SIGNAL(saveRequest()),
            this, SLOT(fileSave()));
}

void MainWindow::connectPlanarFitDialog()
{
    PlanarFitSettingsDialog* planar_fit_dialog = mainWidget_->pfDialog();

    bool c1 = connect(planar_fit_dialog, SIGNAL(saveRequest()),
                      this, SLOT(fileSave()));
    Q_ASSERT(c1);

    bool c2 = connect(mainWidget_->basicPage(), &BasicSettingsPage::setDateRangeRequest,
                      planar_fit_dialog, &PlanarFitSettingsDialog::setDateRange);
    Q_ASSERT(c2);
}

void MainWindow::connectTimeLagDialog()
{
    TimeLagSettingsDialog* time_lag_dialog = mainWidget_->tlDialog();

    bool c1 = connect(time_lag_dialog, SIGNAL(saveRequest()),
                      this, SLOT(fileSave()));
    Q_ASSERT(c1);

    bool c2 = connect(mainWidget_->basicPage(), &BasicSettingsPage::setDateRangeRequest,
                      time_lag_dialog, &TimeLagSettingsDialog::setDateRange);
    Q_ASSERT(c2);
}

// Reimplement wheel event handler to support resizing with Ctl + Wheel
void MainWindow::wheelEvent(QWheelEvent* event)
{
    if (Qt::ControlModifier == QApplication::keyboardModifiers())
    {
        if (event->angleDelta().y() > 0)
        {
            // limit the maximum resize
            auto screen = QGuiApplication::screens().at(0);
            auto maxDesktopWidth = screen->availableVirtualSize().width();

            if (this->size().width() > maxDesktopWidth - 100)
            {
                QWidget::wheelEvent(event);
                return;
            }
            resize(width() + width() / 10, height() + height() / 10);
        }
        else
        {
            resize(width() - width() / 10, height() - height() / 10);
        }
    }
}

// NOTE: to implement
void MainWindow::updateDockBorder(Qt::DockWidgetArea)
{
}

void MainWindow::silentMdClenaup()
{
    if (scheduledSilentMdCleanup_)
    {
        ecProject_->setGeneralMdFilepath(QString());

        const bool quiet = true;
        fileSave(quiet);
        scheduledSilentMdCleanup_ = false;
    }
}

bool MainWindow::queryEcProjectImport(const QString& filename)
{
    return WidgetUtils::yesNoQuestion(this,
        tr("Import Project"),
        tr("<p>Your project file has to be imported "
           "and updated to a new version. "
           "If you proceed, you will "
           "lose your file and the "
           "compatibility with previous versions of EddyPro "
           "but you will have a smooth "
           "transition to the new EddyPro version. "
           "If you are unsure, "
           "select 'No' and create a backup copy of your "
           "project file before proceeding.</p>"),
        tr("<p>Are you sure you want to "
           "import the following file?<br>"
           "<b>\"%1\"</b></p>").arg(filename));
}

bool MainWindow::queryDlProjectImport()
{
    return WidgetUtils::information(this,
        tr("Import Metadata"),
        tr("<p>Your metadata file has to be imported "
           "to a new version for compatibility. </p>"),
        tr("<p>Save the metadata file with a new file name "
           "to create a backup copy "
           "or simply overwrite it.</p>"
           "<p>To cancel the import operation, simply close "
           "without pushing 'Ok'.</p>"));
}
