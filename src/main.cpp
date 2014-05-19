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

#include <QDebug>
#include <QSettings>
#include <QTextStream>
#include <QTextCodec>
#include <QTranslator>
#include <QApplication>
#include <QBitmap>
#include <QFontDatabase>
#include <QFile>
#include <QtGlobal>

#include "dbghelper.h"
#include "mainwindow.h"
#include "stringutils.h"
#include "fileutils.h"
#include "customsplashscreen.h"
#include "defs.h"
#include "mystyle.h"

#if 1
QTextStream *outputStream = 0;

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

void doVersion(QTextStream &stream)
{
    stream << endl;
    stream << Defs::APP_NAME << ' ' << Defs::APP_VERSION_STR << '\n';
    stream << endl;
}

void doHelp(QTextStream &stream)
{
    stream << endl;
    stream << Defs::ABOUT_DESC;
    stream << endl << endl;
    stream << QObject::tr("Usage: %1 [options] [file]").arg(Defs::APP_NAME);
    stream << endl << endl;
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

QString doArgs(const QStringList& arguments, QTextStream &stream, bool *getLogFile = 0)
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

int main(int argc, char *argv[])
{
    // initialize resources at startup (qrc file loading)
    Q_INIT_RESOURCE(eddypro);

#ifdef Q_OS_MAC
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication::setColorSpec(QApplication::ManyColor);

    QApplication::setEffectEnabled(Qt::UI_AnimateMenu);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip);
    QApplication::setEffectEnabled(Qt::UI_AnimateToolBox);

    QApplication app(argc, argv);
    app.setApplicationName(Defs::APP_NAME);
    app.setOrganizationName(Defs::ORG_NAME);
    app.setOrganizationDomain(Defs::ORG_DOMAIN);

    qDebug() << "currentUnicodeVersion" << QChar::currentUnicodeVersion();

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
    QString installationDir = QCoreApplication::applicationDirPath();
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
        QMessageBox::critical(0,
                              QT_TR_NOOP(QStringLiteral("Cannot proceed")),
                              QT_TR_NOOP(QStringLiteral("Home Path not available.")));
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

    if (outputStream)
    {
        delete outputStream;
    }

    return returnVal;
}
