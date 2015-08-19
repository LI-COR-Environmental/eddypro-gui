/***************************************************************************
  main.cpp
  -------------------
  EddyPro is an evolution of the ECO2S Eddy Covariance programs suite
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

// TODO: general
// - use QApplication::aboutToQuit() to control closing after shutdown request
// - use QSignalMapper (ex. for browse buttons in the same page to use the same slot)
// - QStandardPaths (qt5), QStandardPaths::enableTestMode
// - use Q_SET_OBJECT_NAME
// - refactor buffer*() functions, factoring them in a utiliy function
// - refactor and check all the list access to guard at(), first(), last(), etc,
//   testing in advance if list.isEmpty()
// - add constant to all the relevant (member) functions
// - use a custom icon theme, especially for the standard dialog icons
// (see QIcon::fromTheme() and QIcon::themeSearchPaths())
// - create a custom QLabel class with plain text as default text format
// for security and performance
// (http://labs.qt.nokia.com/2011/10/04/security-considerations-regarding-qlabel-and-friends/
// https://bugreports.qt-project.org/browse/QTBUG-21858)
// - use QStringRef
// - change all the static msgbox
// - cleanup old tooltip window stuff (toggleTooltipOutputAct, ...)
// - cleanup all the not necessary blockSignals() calls
// + get rid of QT_TR_NOOP wherever possible

// TODO: ep5
// - create button disabled until ready to go (loading at least a ghg)
// - review guided mode messages in smartflux mode
// - review metadata fields with mike
// - control chmod 755 with mike
// - review delay issue entering the basic settings page
// + update changelog
// - fix procini file copy
// - review dll dependancy
// - qwidget::repaint: Recursive repaint detected

// - integrate ancillary files testing
// - remove setStyleSheet (or try to change order) when use setToolTip on the same object

// - review ftp version retrieval in case of connection and other errors
// - hover effect on smartflux checkbox labels
// - clean up ftpmanager error conditions
// - avoid flickers changing full/normal screen mode
// - show engine version/date in about
// - fix double msg when selecting a raw data with no ghg files
// - fix run buttons available when selecting a raw data with no ghg files
// - review tooltip bg color

// + check project file line endings on windows
// + update guided info messages and run btton availability when leaving smartflux
// + control that run buttons are disable in smartflux mode
// + notice overriding smartflux pkg within create
// + changed policy for deleting files extracted from ghg files
// + include data and biomet metadata in the .smartflux
// + fix full screen toggle
// + change show result msg on create
// + fix phone numbers
// + manage smartflux mode in case of untitled
// + correct hr in create pkg dialog
// + remove file name from welcome page
// + smartfluxbar handling
// + remove number of files to merge
// + create copy of a loaded project selecting smartflux
// + create a message to notice the user of changes to the loaded project in smartflux mode(do not show again checkbox)
// + open recent projects in smartflux mode
// + integrate changes from EP4
// + complete QListWidget css (no hover in welcome page)
// + fixed enabled/disabled status of Spectral assessment file availability radio buttons when reloading with Moncrieff
// + fixed enabled/disabled status of Set suggested range in spectral corrections page
// + mailto link in aboutdialog
// + remove height from Irga
// + check if height removal creates issues with old files
// + smartflux UI
// + smartflux 'setup mode' -> 'configuration'
// + test removed irga height with old files
// + cleanup disabled fields in smartflux mode
// + disable console page in smartflux mode
// + disable the output dir and disable preset the id
// + check biomet embedded
// + disable detect dataset dates button
// + uncheck and disable search in subfolder
// + uncheck and disable select subperiod
// + control in smartflux default project that search in subfolder is unchecked
// + control in the smartflux default project that biomet embedded is unchecked
// + create msg after completing package creation (success/failure)
// + output: disable time series and full lenght spactra and cospectra and unchecked
// + spatral corr: disable fratini
// + output: ameriflux and ghg-europe unchecked and disabled
// + output: biomet checked and disabled
// + output: steady-state unchecked and enabled
// + output: metadata unchecked and disabled
// + change crosswind in the smartflux express project
// + set averaging interval to 30

// FIXME: Qt5
// - Defs::CONF_WIN_SMARTFLUX_CONFIG_MSG not working
// - dock max/close buttons look
// - refine and cleanup eddypro.pro file
// + use QStringLiteral where needed
// + update unicode string literals
// - double dropdown animation on combo inside tables
// - use new connect syntax
// - planar fit pie update
// - dialog about changing the project during a run looks bad
// + border of md tables not complete
// + full screen toggled action does not works

// FIXME: macosx
// + create package dialog showing again when focus is back on the main window
// - layout misalignement
// + toolbar look&feel
// + standard menu (about, quit, ...)
// + about menu appearance
// + correct some spinbox width (ex. fft frequency bins)
// - use Qt::Drawer ?
// - QTextCursor::setPosition position out of range warnings in RunPage::errorEdit_
// - use QAction::setMenuRole() consistently

// - change all modal dialogs in sheet mode (qdialog and qmessagebox)
// + remove 7zip usage and port zip/unzip to quazip
// - find equivalent for pausep
// - add do not show again on all the annoying msg dialogs

// COMPLETE:
// + complete update_dialog/ftpmanager
// - complete dock hr/vr border update, see updateDockBorder()
// - configState_.general.recentnum usage in MainWindow::resizeEvent()

// CLEANUP:
// - "not used" functions/vars
// - rename variables, functions and classes
// - refactor repetitive code
// - refactor argument management using QCommandLineParser/QCommandLineOption
// - refactor logging in main()
// - refactor Alia
// - QErrorMessage

// PATTERNS:
// - create factory in Process (to rename ProcessManager)

// TESTING:
// - introduce automatic unit testing
// - introduce functional testing

// EXCEPTIONS:
// - introduce use of exceptions
// - use assertions

// - evaluate usage of Q_COREAPP_STARTUP_FUNCTION(QtStartUpFunction ptr)
// - evaluate usage of qAddPostRoutine(QtCleanUpFunction ptr)

// C++11:
// - use auto
// - use enum class where appropriate
// - use range-based for (but evaluate differences with Qt foreach)
// - use lambdas
// - nullptr
// - use std::unique_ptr:
//       #include <memory>
//       Q_DECLARE_SMART_POINTER_METATYPE(std::unique_ptr)
// - use std containers:
//       #include <deque>
//       Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::deque)
//       #include <unordered_list>
//       Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(std::unordered_map)

// REFACTORING:
// - express ownership using smart pointers (std::unqiue_ptr or QScopedPointer)
// - use unnamed namespaces for helper functions local to a class
// - evaluate use of Q_GADGET
//    http://stackoverflow.com/questions/6798575/c-qt-reflection-with-copy-and-assignment
//    http://www.qtcentre.org/threads/56539-Q_ENUMS-outside-of-the-class
//    http://qt-project.org/forums/viewthread/42188

#include <QApplication>
#include <QBitmap>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QtGlobal>
#include <QSettings>
#include <QTextCodec>
#include <QTextStream>
#include <QTranslator>

#include "customsplashscreen.h"
#include "dbghelper.h"
#include "defs.h"
#include "fileutils.h"
#include "mainwindow.h"
#include "mystyle.h"
#include "stringutils.h"
#include "widget_utils.h"

// in debug mode
//#ifndef QT_NO_DEBUG_OUTPUT
//#ifdef QT_DEBUG
#if 1
QTextStream *outputStream = nullptr;

void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString debugTimestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy.MM.dd hh:mm:ss"));
    QString localMsg;
    switch (type)
    {
    case QtDebugMsg:
        localMsg = debugTimestamp + QStringLiteral("[D]");
        break;
    case QtWarningMsg:
        localMsg = debugTimestamp + QStringLiteral("[W]");
        break;
    case QtCriticalMsg:
        localMsg = debugTimestamp + QStringLiteral("[C]");
        break;
    case QtFatalMsg:
        localMsg = debugTimestamp + QStringLiteral("[F]");
        break;
    }
    (*outputStream) << localMsg << " " << msg << "\n";

    if (QtFatalMsg == type)
    {
        abort();
    }
}
#endif

#if 0
//#ifdef QT_DEBUG

#include <QMessageBox>
#include <QThread>
#include <iostream>

// By default, fairly big problems like QObject::connect not working due to not being able
// to find a signal or slot goes to the debug output.  There can be a lot of spew which
// makes that easy to miss.  While perhaps the release build would want to try and
// keep going, it helps debugging to get told this ASAP.
//
// Would be nice to chain to the default Qt platform error handler
// However, this is not feasible as there is no "default error handler" function
// The default error handling is merely what runs in qt_message_output
// 	http://qt.gitorious.org/qt/qt/blobs/4.5/src/corelib/global/qglobal.cpp#line2004
//
void noisyFailureMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

//    QString msg (msgAsCstring);
//    std::cerr << msgAsCstring;
//    std::cerr.flush();

    // Why on earth didn't Qt want to make failed signal/slot connections qWarning?
    if ((type == QtDebugMsg)
            && msg.contains(QStringLiteral("::connect")))
    {
        type = QtWarningMsg;
    }

    // this is another one that doesn't make sense as just a debug message.  pretty serious
    // sign of a problem
    // http://www.developer.nokia.com/Community/Wiki/QPainter::begin:Paint_device_returned_engine_%3D%3D_0_(Known_Issue)
    if ((type == QtDebugMsg)
            && msg.contains(QStringLiteral("QPainter::begin"))
            && msg.contains(QStringLiteral("Paint device returned engine")))
    {
        type = QtWarningMsg;
    }

    // This qWarning about "Cowardly refusing to send clipboard message to hung application..."
    // is something that can easily happen if you are debugging and the application is paused.
    // As it is so common, not worth popping up a dialog.
    if ((type == QtWarningMsg)
            && QString(msg).contains(QStringLiteral("QClipboard::event"))
            && QString(msg).contains(QStringLiteral("Cowardly refusing")))
    {
        type = QtDebugMsg;
    }

    // only the GUI thread should display message boxes.  If you are
    // writing a multithreaded application and the error happens on
    // a non-GUI thread, you'll have to queue the message to the GUI
    QCoreApplication * instance = QCoreApplication::instance();
    const bool isGuiThread =
        instance && (QThread::currentThread() == instance->thread());

    if (isGuiThread)
    {
        QMessageBox messageBox;
        switch (type)
        {
        case QtDebugMsg:
            return;
        case QtWarningMsg:
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            break;
        case QtCriticalMsg:
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            break;
        case QtFatalMsg:
            messageBox.setIcon(QMessageBox::Critical);
            messageBox.setInformativeText(msg);
            messageBox.setStandardButtons(QMessageBox::Cancel);
            break;
        }

        int ret = messageBox.exec();
        if (ret == QMessageBox::Cancel)
            abort();
    }
    else
    {
        if (type != QtDebugMsg)
            abort(); // be NOISY unless overridden!
    }
}
#endif

/// \file main.cpp
/// \brief Entry point for the GUI application
///
/// This file contains main().

/// \fn void doVersion(QTextStream &stream)
/// \brief Print outputStream copyright and version information to stdout
/// \param[outputStream] stream
void doVersion(QTextStream &stream)
{
    stream << endl;
    stream << Defs::APP_NAME << ' ' << Defs::APP_VERSION_STR << '\n';
    stream << endl;
}

/// \fn void doHelp(QTextStream &stream)
/// \brief Print outputStream help info to stdout
/// \param[outputStream] stream
void doHelp(QTextStream &stream)
{
    stream << endl;
    stream << Defs::ABOUT_DESC;
    stream << endl << endl;
    stream << QObject::tr("Usage: %1 [options] [file]").arg(Defs::APP_NAME);
    stream << endl << endl;
    // doHelp() arguments
    stream << QObject::tr("Arguments [file]:");
    stream << endl;
    stream << QObject::tr("    file                  File to open");
    stream << endl << endl;
    // general options
    stream << QObject::tr("Options [options]:");
    stream << endl;
//    stream << QObject::tr("    --lang       xx       Start loading a specific language at start-up,\n");
//    stream << QObject::tr("                          where \"xx\" is a valid locale (ex. \"it\").\n");
    stream << QObject::tr("    --help                Print the command line options.");
    stream << endl;
    stream << QObject::tr("    --version             Print the application version.");
    stream << endl;
}

// NOTE: not used
//void doLog(const QString& appEnvPath)
//{
//    DEBUG_FUNC_NAME

//// in debug mode
////#ifndef QT_NO_DEBUG_OUTPUT
////#ifdef QT_DEBUG
//#if 1
//    QFile *logFile = new QFile(appEnvPath
//                           + QStringLiteral("/")
//                           + Defs::LOG_FILE_DIR
//                           + QStringLiteral("/")
//                           + Defs::APP_NAME_LCASE
//                           + QStringLiteral("_gui.")
//                           + Defs::LOG_FILE_EXT);

//    if (logFile->size() > 1048576)
//        logFile->remove();

//    if (logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
//    {
//        outputStream = new QTextStream(logFile);
//        QtMessageHandler oldMsgHandler(qInstallMessageHandler(logOutput));
//        Q_UNUSED(oldMsgHandler);
//    }
//    else
//    {
//        qDebug() << "Error opening logFile file '" << Defs::APP_NAME_LCASE
//                 << "'. All debug output redirected to console.";
//    }
//#endif

//#if 0
////#ifdef QT_DEBUG
//    // Because our "noisy" message handler uses the GUI subsystem for message
//    // boxes, we can't install it until after the QApplication is constructed.  But it
//    // is good to be the very next thing to run, to start catching warnings ASAP.
//    {
//        QtMessageHandler oldMsgHandler (qInstallMessageHandler(noisyFailureMsgHandler));
//        Q_UNUSED(oldMsgHandler); // squash "didn't use" compiler warning
//    }
//#endif
//}

/// \fn QString doArgs(QStringList arguments, QTextStream &stream)
/// \brief Process the arguments that QApplication doesn't take care of
/// \param[in] arguments
/// \param[outputStream] stream
/// \return A file name string
QString doArgs(const QStringList& arguments, QTextStream &stream, bool *getLogFile = nullptr)
{
    QString arg;
    for (int n = 1; n < arguments.count(); ++n)
    {
        arg = arguments.at(n);
        if ((arg == QLatin1String("-h"))
                || (arg == QLatin1String("-help"))
                || (arg == QLatin1String("--help")))
        {
            doHelp(stream);
            exit(0);
        }
        else if ((arg == QLatin1String("-v"))
                    || (arg == QLatin1String("-version"))
                    || (arg == QLatin1String("--version")))
        {
            doVersion(stream);
            exit(0);
        }
        else if ((arg == QLatin1String("-d"))
                    || (arg == QLatin1String("-debug"))
                    || (arg == QLatin1String("--debug")))
        {
            *getLogFile = true;
        }
//        else if ((arg == QLatin1String("-l"))
//                    || (arg == QLatin1String("-lang"))
//                    || (arg == QLatin1String("--lang")))
//        {
//            doLocale(stream);
//        }
        else if (arg[0] == QLatin1Char('-'))
        {
            // no other valid options
            stream << QObject::tr("Invalid parameter \"%1\"\n") .arg(arg);
            doHelp(stream);
            exit(1);
        }
        else
        {
            // it must be a filename
            return arg;
        }
    }
    return Defs::DEFAULT_PROJECT_FILENAME;
}

bool showSplash()
{
    QSettings config;
    bool showSplash;

    // read general config
    config.beginGroup(Defs::CONFGROUP_GENERAL);
    if (config.contains(Defs::CONF_GEN_SHOW_SPLASH))
    {
        showSplash = config.value(Defs::CONF_GEN_SHOW_SPLASH).toBool();
    }
    else
    {
        config.setValue(Defs::CONF_GEN_SHOW_SPLASH, true);
        showSplash = true;
    }
    config.endGroup();
    config.sync();

    return showSplash;
}

/// \fn int main(int argc, char *argv[])
/// \brief Entry point of the application.
int main(int argc, char *argv[])
{
    // initialize resources at startup (qrc file l    oading)
    Q_INIT_RESOURCE(eddypro);

    // NOTE: https://bugreports.qt-project.org/browse/QTBUG-28816
    // https://bugreports.qt-project.org/browse/QTBUG-32221
    // https://bugreports.qt-project.org/browse/QTBUG-32914
#if defined(Q_OS_MAC)
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
//     qApp->setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#endif

    QApplication::setColorSpec(QApplication::ManyColor);
//    QApplication::setDesktopSettingsAware(false);

    QApplication::setEffectEnabled(Qt::UI_AnimateMenu);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip);

    QApplication app(argc, argv);
    app.setApplicationName(Defs::APP_NAME);
    app.setOrganizationName(Defs::ORG_NAME);
    app.setOrganizationDomain(Defs::ORG_DOMAIN);

    qDebug() << "currentUnicodeVersion" << QChar::currentUnicodeVersion();

    // TODO: complete custom ttf setup
    int fontId_1 = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fonts/OpenSans-Regular.ttf"));
    Q_ASSERT(fontId_1 != -1);
    qDebug() << QFontDatabase::applicationFontFamilies(fontId_1);
    int fontId_2 = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fonts/OpenSans-Italic.ttf"));
    Q_ASSERT(fontId_2 != -1);
    qDebug() << QFontDatabase::applicationFontFamilies(fontId_2);
    int fontId_3 = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fonts/OpenSans-Bold.ttf"));
    Q_ASSERT(fontId_3 != -1);
    qDebug() << QFontDatabase::applicationFontFamilies(fontId_3);
    int fontId_4 = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fonts/OpenSans-Semibold.ttf"));
    Q_ASSERT(fontId_4 != -1);
    qDebug() << QFontDatabase::applicationFontFamilies(fontId_4);
    int fontId_5 = QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/fonts/OpenSans-BoldItalic.ttf"));
    Q_ASSERT(fontId_5 != -1);
    qDebug() << QFontDatabase::applicationFontFamilies(fontId_5);

    // load translation file embedded in resources
    QTranslator appTranslator;
    bool ok = appTranslator.load(QStringLiteral(":/tra/en"));
    qDebug() << "loading translation:" << ok;
    app.installTranslator(&appTranslator);

    QString currentWorkingDir = QDir::currentPath();
    QString installationDir = qApp->applicationDirPath();
    qDebug() << "currentWorkingDir" << currentWorkingDir;
    qDebug() << "installationDir" << installationDir;
    if (currentWorkingDir != installationDir)
        qDebug() << QDir::setCurrent(installationDir);
    qDebug() << "currentWorkingDir" << QDir::currentPath();

    qDebug() << "------------------------------------------------------------";
    qDebug() << "Default Style: " << app.style()->metaObject()->className();

    app.setStyle(new MyStyle(app.style()->objectName()));

#if defined(Q_OS_WIN)
    FileUtils::loadStyleSheetFile(QStringLiteral(":/css/winstyle"));
#elif defined(Q_OS_MAC)
    FileUtils::loadStyleSheetFile(QStringLiteral(":/css/macstyle"));
#endif

    QString appEnvPath = FileUtils::setupEnv();
    if (appEnvPath.isEmpty())
    {
        WidgetUtils::critical(nullptr,
                              QObject::tr("Cannot proceed"),
                              QObject::tr("Home Path not available."));
        return 1000;
    }

    // check for additional command line arguments
    QTextStream stream(stdout);
    bool getLogFile = false;
    QString filename = doArgs(app.arguments(), stream, &getLogFile);
    qDebug() << "filename:" << filename;
    qDebug() << "getLogFile:" << getLogFile;

    QFile logFile(appEnvPath
                  + QStringLiteral("/")
                  + Defs::LOG_FILE_DIR
                  + QStringLiteral("/")
                  + Defs::APP_NAME_LCASE
                  + QStringLiteral("_gui.")
                  + Defs::LOG_FILE_EXT);
    if (getLogFile)
    {
        if (logFile.size() > 1048576)
            logFile.remove();

        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            outputStream = new QTextStream(&logFile);
            QtMessageHandler oldMsgHandler(qInstallMessageHandler(logOutput));
            Q_UNUSED(oldMsgHandler);
        }
        else
        {
            qDebug() << "Error opening logFile file '" << Defs::APP_NAME_LCASE
                     << "'. All debug output redirected to console.";
        }
    }

    QPixmap pixmap(QStringLiteral(":/icons/splash-img"));
    QBitmap bm(pixmap.createHeuristicMask());
    CustomSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

    if (showSplash())
    {
        splash.setMask(bm);

        QFont splashFont;
        splashFont.setFamily(QStringLiteral("Open Sans"));
        splashFont.setBold(true);
        splashFont.setPixelSize(10);

        splash.setFont(splashFont);
        splash.setMessageRect(QRect(0, 440, 600, 14), Qt::AlignCenter); // Setting the message position.
        splash.show();
        splash.showStatusMessage(QObject::tr("Initializing..."));
    }

    QLocale::setDefault(QLocale::C);

    if (showSplash())
    {
        splash.showStatusMessage(QObject::tr("Loading..."));
        splash.setProgressValue(20);
        splash.setProgressValue(30);
    }

    MainWindow mainWin(filename, appEnvPath);
    if (showSplash())
    {
        splash.setProgressValue(40);
        splash.setProgressValue(50);
        splash.setProgressValue(60);
    }
    mainWin.show();

    if (showSplash())
    {
        splash.setProgressValue(70);
        splash.setProgressValue(80);
        splash.clearMessage();
        splash.showStatusMessage(QObject::tr("Loading complete."));
        splash.setProgressValue(90);
        splash.setProgressValue(100);
    }

    qDebug() << "applicationDisplayName" << qApp->applicationDisplayName();

    const int returnVal = app.exec();

    // cleanup
//    if (logFile)
//    {
//        logFile->close();
//        delete logFile;
//    }

    if (outputStream)
    {
        delete outputStream;
    }

    return returnVal;
}
