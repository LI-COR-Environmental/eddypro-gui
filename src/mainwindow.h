/***************************************************************************
  mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "configstate.h"
#include "defs.h"
#include "process.h"

class QPlainTextEdit;
class QDockWidget;
class QActionGroup;
class QSignalMapper;
class QLabel;

class MainWidget;
class DlProject;
class EcProject;
class MdiChild;
class TooltipFilter;
class AboutDialog;
class ClickLabel;
class UpdateDialog;
class InfoMessage;

/// \class MainWindow
/// \brief Class representing the main window of the application
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& filename = QString(),
               const QString& appEnvPath = QString(),
               QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject* o, QEvent* e);
    void wheelEvent(QWheelEvent* event);
    void changeEvent(QEvent* event);

private slots:
    void dbgViewConsoleOutputToggled(bool on);
    void dbgViewConsoleOutputTriggered(bool on);

    void initialize();
    void fileNew();
    void fileOpen(const QString& fileName = QString());
    void fileRecent();
    bool fileSave(const bool quiet = false);
    bool fileSaveAs(const QString& fileName = QString());
    void fileClose();

    void fileOpenRequest(QString file);
    void fileNewRequest();

    bool fileSaveSilently();

    // add a file to recent file menu
    void addRecent(const QString& filename);

    // Windows
    void updateMenuActionStatus(Defs::CurrPage page);

    // View
    void setFullScreen(bool on);
    void toggleStatusbar(bool on);
    void viewTooltipOutput(bool on);
    void viewConsoleOutput(bool on);
    void viewInfoOutput(bool on);

    // displays help contents
    void showHelp();
    void showPdfHelp();
    void showStarterPdfHelp();
    void setOfflineHelp(bool yes);
    void setSmartfluxMode(bool on);
    void about();
    void whatsHelp();

    void setEcProjectModified();
    void recentMenuShow();

    void updateHtmlDock(QPlainTextEdit* dock, const QString& htmlMsg);
    void changePage(Defs::CurrPage page, bool testCurrentPage = true);
    void setCurrentPage(Defs::CurrPage page);
    void windowTitleUpdate(Defs::CurrPage page);
    void updatePage(Defs::CurrPage page);
    void updateRunButtonsAvailability();

    void setCurrentStatus(Defs::CurrStatus state);
    void setCurrentRunStatus(Defs::CurrRunStatus state);

    void viewWelcomePage();
    void viewProjectPage();
    void viewBasicSettingsPage();
    void viewAdvancedSettingsPage();
    void viewRunPage();

    void getRunExpress();
    void getRunAdvanced();
    void getRunRetriever();
    bool pauseEngine(Defs::CurrRunStatus mode);
    bool resumeEngine(Defs::CurrRunStatus mode);
    void stopEngine();
    void stopEngineProcess();

    void updateSpectraPaths();
    void updateSpectraPathFromPreviousData(const QString& exFilePath);

    void resetRunIcons();
    void setRunExpIcon2Resume();
    void setRunAdvIcon2Resume();
    void setRunRetIcon2Resume();
    void setRunExpIcon2Pause();
    void setRunAdvIcon2Pause();
    void setRunRetIcon2Pause();

    void displayExitDialog();
    void onlineHelpTrigger_1();

    void updateConsoleLine(QByteArray& data);
    void updateConsoleChar(QByteArray& data);
    void updateConsoleReceived();
    void updateConsoleError();

    void setMetadataRead(bool b);

    void showAutoUpdateDialog();
    void showAutoUpdateResults();
    void showUpdateDialog();

    void openLicorSite();

    void checkInternetConnection();
    void updateInfoMessages();
    void showInfoMessages_1();
    void showInfoMessages_2();
    void showInfoMessages_3();

    // restore application state
    void restorePreviousStatus();

    void connectBinarySettingsDialog();
    void connectPlanarFitDialog();
    void connectTimeLagDialog();

    void updateDockBorder(Qt::DockWidgetArea);

private:
    void runExpress();
    void runAdvancedStep_1();
    void runAdvancedStep_2();
    void runRetriever();
    void createEngineProcess();
    void createActions();
    void connectActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createInfoDockWin();
    void createConsoleDockWin();
    bool continueBeforeClose();
    void saveEnvSettings(const QString& env);

    void setCurrentProjectFile(const QString& fileName, bool modified = false);
    const QString &currentProjectFile() const;

    // set window caption from file name
    void setFileCaption(const QString& filename, bool clearStar = true);
    void updateStatusBar();

    void readSettings();
    void writeSettings();

    // save application state
    bool openFile(const QString& filename);
    void newFile();
    void loadFile(const QString& fileName);
    bool saveFile(const QString& fileName);

    void displayExitMsg(Process::ExitStatus exitReason);
    void displayExitMsg2(Process::ExitStatus exitReason);
    bool okToStopRun();
    int testBeforeRunningPassed(int step);
    bool testForPreviousData();
    bool alertChangesWhenRunning();
    void togglePageButton(Defs::CurrPage page);
    void changeViewToolbarSeparators(Defs::CurrPage page);

    void checkUpdate();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QMenu *fileMenuOpenRecent;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *runToolBar;

    ClickLabel* sep1;
    ClickLabel* sep2;
    ClickLabel* sep3;
    ClickLabel* sep4;

    QAction *newAction;
    QAction *openAction;
    QAction *closeAction;
    QAction *recentOpenAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *smartfluxAction;
    QAction *quitAction;
    QActionGroup* viewActionGroup;
    QAction *viewWelcomeAction;
    QAction *viewProjectCreationAction;
    QAction *viewBasicSettingsAction;
    QAction *viewAdvancedAction;
    QAction *viewRunPageAction;
    QAction *runExpressAction;
    QAction *runAdvancedAction;
    QAction *runRetrieverAction;
    QAction *stopAction;
#if 1 // !defined(Q_OS_MAC)
    QAction *toggleFullScreenAction;
#endif
    QAction *toggleStatusbarAct;
    QAction* toggleInfoOutputAct;
    QAction *toggleTooltipOutputAct;
    QAction *toggleConsoleOutputAct;
    QActionGroup *changeStyleAction;
    QAction *helpAction;
    QAction *pdfHelpAction;
    QAction *starterPdfHelpAction;
    QAction *toggleOfflineHelpAct;
    QAction *whatsHelpAction;
    QAction *videoTutorialsAction;
    QAction* swWebpageAction;
    QAction* checkUpdateAction;
    QAction *aboutAction;
    QAction *aboutQtAction;

    QDockWidget *infoDock;
    QPlainTextEdit *infoOutput;
    QDockWidget *consoleDock;
    QPlainTextEdit *consoleOutput;

    AboutDialog *aboutDialog;

private:
    TooltipFilter *tooltipFilter_;
    MainWidget *mainWidget_;

    ConfigState configState_;
    DlProject *dlProject_;

    EcProject *ecProject_;
    QString currEcProjectFilename_;
    QString appEnvPath_;
    QLabel *currentProjectLabel;

    int versionLabelWidth_;
    int currentProjectLabelWidth_;

    QTimer* notificationTimer_;

    // flags
    bool newFlag_;
    bool modifiedFlag_;
    bool openingFlag_;
    bool engineResumableFlag_;
    bool metadataReadFlag_;
    bool neededEngineStep2_;
    bool guidedModeOn_;
    bool basicSettingsPageAvailable_;
    bool runExpressAvailable_;
    bool runAdvancedAvailable_;
    bool newClicked_;

    Defs::CurrPage previousPage_;

    Defs::CurrPage currentPage_;
    Defs::CurrPage currentPage() const;

    Defs::CurrStatus currentStatus_;
    Defs::CurrStatus currentStatus() const;

    Defs::CurrRunStatus currentRunStatus_;
    Defs::CurrRunStatus currentRunStatus() const;

    bool expressClicked_;
    bool advancedClicked_;
    bool retrieverClicked_;

    void showStatusTip(const QString &text);

    Process* engineProcess_;
    int engineExit_;

    UpdateDialog* updateDialog;
    void updateInfoDock(bool yes);

    bool argFilename_;

    void loadSmartfluxProjectCopy(const QString &filename);
    void cleanEnvTmpDir();
signals:
    void updateMetadataReadRequest();
    void updateCrossWindRequest();
    void checkMetadataOutputRequest();
    void recentUpdated();
};

#endif // MAINWINDOW_H
