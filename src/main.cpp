/***************************************************************************
  main.cpp
  -------------------
  EddyPro is an evolution of the ECO2S Eddy Covariance programs suite
  -------------------
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
#include <QLoggingCategory>

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
#include "logging.h"

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
QString doArgs(const QStringList& arguments, QTextStream &stream);

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
    // handle logs
    qInstallMessageHandler(Logging::messageHandler);
    // activate useful diagnostics
    QLoggingCategory::setFilterRules(QStringLiteral(
                             "qtc.clang*=true\n"
                             "qt.scenegraph.general=true\n"));

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

    QString filename = doArgs(QApplication::arguments(), stream);
    qDebug() << "filename:" << filename;

#if defined(Q_OS_MACOS)
    if (!fileToOpen.isEmpty())
    {
        filename = fileToOpen;
    }
#endif

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

    MainWindow mainWin(filename, appEnvPath, &splash);

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

    delete outputStream;
    return returnVal;
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
/// \return
///
QString doArgs(const QStringList& arguments, QTextStream &stream)
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
