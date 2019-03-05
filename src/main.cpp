/***************************************************************************
  main.cpp
  -------------------
  EddyPro is an evolution of the ECO2S Eddy Covariance programs suite
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
***************************************************************************/

#include <QApplication>
#include <QBitmap>
#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QFontDatabase>
#include <QtGlobal>
#include <QTextCodec>
#include <QTextStream>
#include <QTranslator>

#include "customsplashscreen.h"
#include "defs.h"
#include "fileutils.h"
#include "globalsettings.h"
#include "JlCompress.h"
#include "mainwindow.h"
#include "mystyle.h"
#include "openfilefilter.h"
#include "qt_helpers.h"
#include "stringutils.h"
#include "widget_utils.h"

// in debug mode
//#ifndef QT_NO_DEBUG_OUTPUT
//#ifdef QT_DEBUG
#if 1
static QTextStream *outputStream = nullptr;
#endif

/// \brief logOutput
/// \param type
/// \param context
/// \param msg
///
void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

/// \fn void doHelp(QTextStream &stream)
/// \brief Print outputStream help info to stdout
/// \param[outputStream] stream
void doHelp(QTextStream &stream);

/// \fn void doVersion(QTextStream &stream)
/// \brief Print outputStream copyright and version information to stdout
/// \param[outputStream] stream
void doVersion(QTextStream &stream);

/// \fn QString doArgs(QStringList arguments, QTextStream &stream)
/// \brief Process the arguments that QApplication doesn't take care of
/// \param[in] arguments
/// \param[outputStream] stream
/// \return A file name string
QString doArgs(const QStringList& arguments, QTextStream &stream, bool *getLogFile = nullptr);

///
/// \brief Extract docs.zip shipped inside the Mac bundle.
/// \brief Necessary because digital signature on Mac fails if docs
/// \brief are not compressed in one file
/// \param[in] currentDir: the application working directory
/// \return True if success
bool extractDocs(const QString& currentDir);

/// \fn int main(int argc, char *argv[])
/// \brief Entry point of the application.
///
/// \file main.cpp
/// \brief Entry point for the GUI application
///
/// This file contains main().
///////////////////
/// \brief main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
#if QT_DEBUG
    // logger creation
#endif

    // initialize resources at startup (qrc file loading)
#if defined(Q_OS_MACOS)
    Q_INIT_RESOURCE(eddypro_mac);
#elif defined(Q_OS_WIN)
    Q_INIT_RESOURCE(eddypro_win);
#else
    Q_INIT_RESOURCE(eddypro_lin);
#endif

    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#if defined(Q_OS_MACOS)
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);

    // workaround necessary in case of widget painting issues
//    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#endif

//    QApplication::setColorSpec(QApplication::ManyColor);
//    QApplication::setDesktopSettingsAware(false);

    QApplication::setEffectEnabled(Qt::UI_AnimateMenu);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip);

    ///
    /// A set of flags to workaroud issues in specific cases
    ///
    // QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    // QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    // QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    // QCoreApplication::setAttribute(Qt::AA_ForceRasterWidgets);
    // QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    // QCoreApplication::setAttribute(Qt::AA_SetPalette);
    // QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);
    QApplication::setApplicationName(Defs::APP_NAME);
    QApplication::setApplicationDisplayName(Defs::APP_NAME);
    QApplication::setOrganizationName(Defs::ORG_NAME);
    QApplication::setOrganizationDomain(Defs::ORG_DOMAIN);

    qDebug() << "currentUnicodeVersion" << QChar::currentUnicodeVersion();

    // fix order of library paths
    // see https://bugreports.qt-project.org/browse/QTBUG-40738
    QString executable = QLatin1String(argv[0]);
    QtHelper::prependApplicationPathToLibraryPaths(executable);
    qDebug() << "library paths" << QApplication::libraryPaths();

#if defined(Q_OS_MACOS)
    // file to open at start
    auto fileToOpen = QString();
    // install event filter to open clicked files in macOS
    OpenFileFilter openFileFilter;
    app.installEventFilter(&openFileFilter);
    QApplication::processEvents();
    auto requestedFile = openFileFilter.fileRequested();
    if (requestedFile.endsWith(QStringLiteral(".eddypro")))
    {
        fileToOpen = requestedFile;
    }
#endif

    // custom ttf setup
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
    QApplication::installTranslator(&appTranslator);

    // working dir
    QDir dir = QDir::current();
    qDebug() << "current dir" << dir.absolutePath();

    QString currentWorkingDir = QDir::currentPath();
    QString installationDir = QApplication::applicationDirPath();
    qDebug() << "currentWorkingDir" << currentWorkingDir;
    qDebug() << "installationDir" << installationDir;
    if (currentWorkingDir != installationDir)
    {
        QDir::setCurrent(installationDir);
    }
    qDebug() << "currentWorkingDir" << QDir::currentPath();
    qDebug() << "currentWorkingDir" << QApplication::applicationDirPath();

    // styles
    qDebug() << "------------------------------------------------------------";
    qDebug() << "Default Style: " << QApplication::style()->metaObject()->className();

    MyStyle myStyle(QApplication::style()->objectName());
    QApplication::setStyle(&myStyle);

#if defined(Q_OS_WIN)
    FileUtils::loadStyleSheetFile(QStringLiteral(":/css/winstyle"));
#elif defined(Q_OS_MACOS)
    FileUtils::loadStyleSheetFile(QStringLiteral(":/css/macstyle"));
#elif defined(Q_OS_LINUX)
    FileUtils::loadStyleSheetFile(QStringLiteral(":/css/linstyle"));
#endif

    QString appEnvPath = FileUtils::setupEnv();
    if (appEnvPath.isEmpty())
    {
        WidgetUtils::critical(nullptr,
                              QObject::tr("Cannot proceed"),
                              QObject::tr("Home Path not available."));
        return 1000;
    }
    qDebug() << "appEnvPath" << appEnvPath;

    // check for command line arguments
    QTextStream stream(stdout);

//#ifdef QT_DEBUG
//    bool getLogFile = true;
//#else
    bool getLogFile = false;
//#endif
    QString filename = doArgs(QApplication::arguments(), stream, &getLogFile);
    qDebug() << "filename:" << filename;
    qDebug() << "getLogFile:" << getLogFile;

#if defined(Q_OS_MACOS)
    if (!fileToOpen.isEmpty())
    {
        filename = fileToOpen;
    }
#endif

    // log file
    QFile logFile(appEnvPath
                  + QLatin1Char('/')
                  + Defs::LOG_FILE_DIR
                  + QLatin1Char('/')
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

    // create and show splash screen
    QPixmap pixmap(QStringLiteral(":/icons/splash-img"));
#if defined(Q_OS_MACOS)
    pixmap.setDevicePixelRatio(2.0);
#endif
    CustomSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
    auto show_splash =
        GlobalSettings::getFirstAppPersistentSettings(Defs::CONFGROUP_GENERAL,
                                                      Defs::CONF_GEN_SHOW_SPLASH,
                                                      true).toBool();

#if defined(Q_OS_MACOS)
    auto is_full_screen_set =
        GlobalSettings::getFirstAppPersistentSettings(Defs::CONFGROUP_WINDOW,
                                                      Defs::CONF_WIN_FULLSCREEN,
                                                      true).toBool();
    if (is_full_screen_set)
    {
        show_splash = false;
    }
#endif
    qDebug() << "show_splash:" << show_splash;

    if (show_splash)
    {
        QFont splashFont;
        splashFont.setFamily(QStringLiteral("Open Sans"));
        splashFont.setBold(false);
        splashFont.setPixelSize(12);

        splash.setFont(splashFont);
        splash.setMessageRect(QRect(0, 427, 600, 25), Qt::AlignCenter); // Setting the message position.
        splash.show();
        splash.showStatusMessage(QObject::tr("Initializing..."));
    }
    QApplication::processEvents();

    QLocale::setDefault(QLocale::C);

    if (show_splash)
    {
        splash.showStatusMessage(QObject::tr("Loading..."));
        splash.setProgressValue(20);
        splash.setProgressValue(30);
        splash.repaint();
    }
    QApplication::processEvents();

//#if defined(Q_OS_MACOS)
//    MainWindow mainWin(filename, appEnvPath, &splash, 0,
//                       Qt::WindowFlags()
//                       Window|WindowTitleHint|WindowSystemMenuHint|WindowMinMaxButtonsHint|WindowCloseButtonHint|WindowFullscreenButtonHint
//                       Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint
//                       );
//#elif defined(Q_OS_WIN)
    MainWindow mainWin(filename, appEnvPath, &splash);
//#endif

    if (show_splash)
    {
        splash.setProgressValue(40);
        splash.setProgressValue(50);
        splash.setProgressValue(60);
    }
    mainWin.show();

    if (show_splash)
    {
        splash.setProgressValue(70);
        splash.setProgressValue(80);
        splash.clearMessage();
        splash.showStatusMessage(QObject::tr("Loading complete."));
        splash.setProgressValue(90);
        splash.setProgressValue(100);
        splash.repaint();
    }

    qDebug() << "applicationDisplayName" << QApplication::applicationDisplayName();

#if defined(Q_OS_MACOS)
    qDebug() << "____________________________________________________";
    auto docExtraction = extractDocs(installationDir);
    qDebug() << "docs.zip extraction:" << docExtraction;
    qDebug() << "____________________________________________________";
#endif
    qDebug() << "++++++++++++++++++++++++++++++++++++++++++++++++++++";

    const int returnVal = QApplication::exec();

    // cleanup
//    if (logFile)
//    {
//        logFile->close();
//        delete logFile;
//    }

    delete outputStream;
    return returnVal;
}
///////////////////////////////////////////////////////////////////////////////
////////////////////////
/// \brief logOutput
/// \param type
/// \param context
/// \param msg
///
void logOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)
    QString debugTimestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy.MM.dd hh:mm:ss"));
    QString localMsg;
    switch (type)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        localMsg = debugTimestamp + QStringLiteral("[I]");
        break;
#endif
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

////////////////////
/// \brief doHelp
/// \param stream
///
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

///////////////////////
/// \brief doVersion
/// \param stream
///
void doVersion(QTextStream &stream)
{
    stream << endl;
    stream << Defs::APP_NAME << ' ' << Defs::APP_VERSION_STR << '\n';
    stream << endl;
}

/////////////////////
/// \brief doArgs
/// \param arguments
/// \param stream
/// \param getLogFile
/// \return
///
QString doArgs(const QStringList& arguments, QTextStream &stream, bool *getLogFile)
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

// http://blog.hostilefork.com/qt-essential-noisy-debug-hook/
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

// NOTE: not used
//void doLog(const QString& appEnvPath)
//{
//// in debug mode
////#ifndef QT_NO_DEBUG_OUTPUT
////#ifdef QT_DEBUG
//#if 1
//    QFile *logFile = new QFile(appEnvPath
//                           + QLatin1Char('/')
//                           + Defs::LOG_FILE_DIR
//                           + QLatin1Char('/')
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

bool extractDocs(const QString& currentDir)
{
    QString docDir = currentDir
                  + QStringLiteral("/")
                  + Defs::DOC_DIR;
    qDebug() << "docDir" << docDir;

    // skip if already extracted
    if (FileUtils::existsPath(docDir))
    {
        return false;
    }

    QString docArchive = docDir
                      + QStringLiteral(".")
                      + Defs::ZIP_NATIVE_DATA_FILE_EXT;
    qDebug() << "docArchive" << docArchive;

    if (!QFile::exists(docArchive))
    {
        return false;
    }

    auto extractedFiles = JlCompress::extractDir(docArchive, currentDir);
    return (!extractedFiles.isEmpty());
}
