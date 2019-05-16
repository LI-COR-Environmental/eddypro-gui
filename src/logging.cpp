#include "logging.h"

#include <QtCore/QtGlobal>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QUrl>
#include <QtCore/QDebug>

#include <iostream>
#include "defs.h"
#include "fileutils.h"

void Logging::messageHandler(QtMsgType type,
                             const QMessageLogContext& context,
                             const QString& message)
{
    // setup logging format
    auto QT_MESSAGE_PATTERN = QStringLiteral("[%{time yyyyMMdd h:mm:ss.zzz t}] "
                                             "%{if-debug}[Deb]%{endif}"
                                             "%{if-info}[Inf]%{endif}"
                                             "%{if-warning}[War]%{endif}"
                                             "%{if-critical}[Cri]%{endif}"
                                             "%{if-fatal}[Fat]%{endif} "
                                             "%{file}:%{line} - "
                                             "%{message}");
    qSetMessagePattern(QT_MESSAGE_PATTERN);
    auto text = qFormatLogMessage(type, context, message);

    // enable links in qtcreator application output panel
    // see https://github.com/qt-creator/qt-creator/blob/master/src/plugins/qtsupport/qtoutputformatter.cpp
    auto fileName = QLatin1String(context.file);

    if (!text.isEmpty() &&!fileName.isEmpty() && !text.contains(QStringLiteral("qrc")) ) {
        auto href = QUrl(fileName).fileName();
        auto link = QStringLiteral("file://") + href;
        text.replace(fileName, link);
    }

    // create the app local configs folder
    const auto appConfigDir = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation).first();
    FileUtils::createDir(appConfigDir);

    // output on log file
    // log file path:
    // - MAC: ~/Library/Preferences/LI-COR/eddypro/eddypro.log
    // - LIN: ~/.config/LI-COR/eddypro/eddypro.log
    // - WIN: C:/Users/<USER>/AppData/Local/LI-COR/eddypro/eddypro.log
    QFile logFile(appConfigDir
                  + QLatin1Char('/')
                  + Defs::APP_NAME_LCASE
                  + QLatin1Char('.')
                  + Defs::LOG_FILE_EXT);

    // trim the file above a certain size
    if (logFile.size() > Defs::LOG_FILE_MAX_SIZE)
    {
        logFile.remove();
    }

    if (!text.isEmpty()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream textStream(&logFile);
        textStream << text << endl;

        // output on console
        std::cerr << text.toStdString() << std::endl;
    }

    // to follow the pattern of the standard message handler, per documentation
    if (type == QtFatalMsg)
    {
        abort();
    }
}
