/***************************************************************************
  process.cpp
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
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTimer>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <psapi.h>
#endif

#include "dbghelper.h"
#include "process.h"

Process::Process(QWidget* parent, const QString &fullPath) :
    QObject(parent),
    process_(0),
    fullPath_(fullPath),
    processExit_(exitSuccess),
    processPid_(0),
    freezerUtility_(0),
    pid_(QString()),
    rxBuffer_(QByteArray())
{
    process_ = new QProcess(this);
    connect(process_, SIGNAL(readyReadStandardOutput()),
             this, SIGNAL(readyReadStdOut()));
    connect(process_, SIGNAL(readyReadStandardError()),
             this, SIGNAL(readyReadStdErr()));

    freezerUtility_ = new QProcess(this);
}

Process::~Process()
{
    DEBUG_FUNC_NAME
}

bool Process::engineProcessStart(const QString& fullPath, const QString& workingDir, const QStringList& argList)
{
    DEBUG_FUNC_NAME

    connect(process_, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->setWorkingDirectory(workingDir);

    // NOTE: start() function without args not parse correctly filepath with spaces, Qt bug?
    process_->start(fullPath, argList, QProcess::Unbuffered | QProcess::ReadOnly);
    processPid_ = process_->pid();

    return true;
}

// add file to an archive fileName
bool Process::zipProcessAddStart(const QString &fileName, const QString &toArchive, const QString &workingDir, const QString &fileType)
{
    QString zipFileName;
    QString workDir;
    QStringList args;

    QFileInfo fileInfo(fileName);

    if (fileType.isEmpty())
    {
        zipFileName = fileName;
    }
    else
    {
        zipFileName = fileInfo.completeBaseName() + QStringLiteral(".") + fileType;
    }

    if (workingDir.isEmpty())
    {
        workDir = fileInfo.absolutePath();
    }
    else
    {
        workDir = workingDir;
    }

    // switch 'a', i.e. add
    args << QStringLiteral("a");

    // assume zip if not present
    if (fileType.isEmpty())
    {
        args << QStringLiteral("-tzip");
    }
    else
    {
        args << QStringLiteral("-t") + fileType;
    }

    args << QStringLiteral("-mx=9");
    args << zipFileName;
    args << toArchive;

    // file path of the program
    QString fp(fullPath_
            + QStringLiteral("/")
            + Defs::BIN_FILE_DIR
            + QStringLiteral("/")
            + Defs::COMPRESSOR_BIN);
    qDebug() << "fp" << fp;
    qDebug() << "filename" << fileName;
    qDebug() << "toArchive" << toArchive;
    qDebug() << "args" << args;
    qDebug() << "workDir" << workDir;

    connect(process_, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->setWorkingDirectory(workDir);

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished(2000))
        return false;

    return true;
}

// extract int the outDir all the metadata files in the fileName archive
bool Process::zipProcessExtMdStart(const QString& fileName, const QString& outDir)
{
    QStringList args;

    args << QStringLiteral("e");
    if (!outDir.isEmpty())
        args << QStringLiteral("-o") + outDir;
    args << fileName;
    args << QStringLiteral("*.metadata");
    args << QStringLiteral("-y");

    // file path of the program
    QString fp(fullPath_
            + QStringLiteral("/")
            + Defs::BIN_FILE_DIR
            + QStringLiteral("/")
            + Defs::COMPRESSOR_BIN);
    qDebug() << fp;
    qDebug() << args;

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished(2000))
        return false;

    return process_->exitCode();
}

// return if a specific file type is present in the archive
bool Process::zipContainsFiletype(const QString& fileName, const QString& filePattern)
{
    DEBUG_FUNC_NAME
    QStringList args;

    qDebug() << "filePattern" << filePattern;

    args << QStringLiteral("l");
    args << fileName;
    if (!filePattern.isEmpty())
        args << QStringLiteral("-i!") + filePattern;
    else
        args << QStringLiteral("*");

    // file path of the program
    QString fp(qApp->applicationDirPath()
                + QStringLiteral("/")
                + Defs::BIN_FILE_DIR
                + QStringLiteral("/")
                + Defs::COMPRESSOR_BIN);

    connect(process_, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished())
        return false;

    QByteArray dataList = process_->readAllStandardOutput();
    return dataList.contains(filePattern.mid(1).toLatin1());
}

void Process::processPause(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    Q_UNUSED(mode)

    // file path of the program
    QString fp(QCoreApplication::applicationDirPath()
            + QStringLiteral("/")
            + Defs::BIN_FILE_DIR
            + QStringLiteral("/")
            + Defs::FREEZER_BIN);

    QStringList args;
    qDebug() << "fp" << fp << "args" << args;

    connect(freezerUtility_, SIGNAL(readyReadStandardOutput()),
             this, SLOT(bufferFreezerOutput()));

    freezerUtility_->setWorkingDirectory(QCoreApplication::applicationDirPath()
                                         + QStringLiteral("/")
                                         + Defs::BIN_FILE_DIR);
    freezerUtility_->start(fp, args);
}

void Process::processPause_2()
{
    DEBUG_FUNC_NAME

    // file path of the program
    QString fp(QCoreApplication::applicationDirPath()
            + QStringLiteral("/")
            + Defs::BIN_FILE_DIR
            + QStringLiteral("/")
            + Defs::FREEZER_BIN);

    QStringList args;
    args << pid_;
    freezerUtility_->start(fp, args);
}

void Process::processResume(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    Q_UNUSED(mode)

    // file path of the program
    QString fp(QCoreApplication::applicationDirPath()
            + QStringLiteral("/")
            + Defs::BIN_FILE_DIR
            + QStringLiteral("/")
            + Defs::FREEZER_BIN);

    QStringList args;
    args << pid_;
    args << QStringLiteral("/r");

    freezerUtility_->setWorkingDirectory(QCoreApplication::applicationDirPath()
                                         + QStringLiteral("/")
                                         + Defs::BIN_FILE_DIR);
    freezerUtility_->start(fp, args);
}

void Process::processStop()
{
    DEBUG_FUNC_NAME

    // to avoid crash message error in windows
    disconnect(process_, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(processFinished(int,QProcess::ExitStatus)));
    disconnect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->kill();
    processExit_ = exitStopped;
}

void Process::processError(QProcess::ProcessError error)
{
    DEBUG_FUNC_NAME
    if (error == QProcess::FailedToStart)
    {
        qWarning() << tr("program not found.");
        processExit_ = exitFailureToStart;
    }
    else
    {
        processExit_ = exitError;
    }
    // to avoid multiple call
    disconnect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));
    emit processFailure();
}

void Process::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    DEBUG_FUNC_NAME
    // to avoid multiple call
    disconnect(process_, SIGNAL(finished(int,QProcess::ExitStatus)),
             this, SLOT(processFinished(int,QProcess::ExitStatus)));

    if (exitStatus == QProcess::CrashExit)
    {
        processExit_ = exitError;
        qWarning() << tr("process crashed.");
    }
    else if (exitCode != 0)
    {
        processExit_ = exitFailure;
        qWarning() << tr("process failed.");
    }
    else
    {
        processExit_ = exitSuccess;
        qWarning() << tr("process ok.");
        emit processSuccess();
        return;
    }
    emit processFailure();
}

QByteArray Process::readAllStdOut()
{
    DEBUG_FUNC_NAME
    return process_->readAllStandardOutput();
}

QByteArray Process::readAllStdErr()
{
    DEBUG_FUNC_NAME
    return process_->readAllStandardError();
}

QProcess *Process::process()
{
    return process_;
}

void Process::setChannelsMode(QProcess::ProcessChannelMode mode)
{
    process_->setProcessChannelMode( mode);
}

void Process::setReadChannels(QProcess::ProcessChannel channel)
{
    process_->setReadChannel(channel);
}

void Process::setEnv(const QStringList &envList)
{
    QProcessEnvironment env;
    foreach (const QString& keyValue, envList)
    {
        env.insert(keyValue.split(QStringLiteral("=")).first(),
                   keyValue.split(QStringLiteral("=")).last());
    }

    process_->setProcessEnvironment(env);
}

void Process::bufferFreezerOutput()
{
    DEBUG_FUNC_NAME
    QByteArray data = freezerUtility_->readAllStandardOutput();

    rxBuffer_.append(data);
    QByteArray line(rxBuffer_);
    QList<QByteArray> lineList(line.split('\n'));

    // newline found
    if (lineList.at(0) != rxBuffer_)
    {
        for (int i = 0; i < lineList.size(); ++i)
        {
            parseFreezerPid(lineList.at(i));
        }

        if (lineList.last().endsWith('\n'))
            rxBuffer_.resize(0);
        else
            rxBuffer_ = lineList.last();
    }
}

void Process::parseFreezerPid(const QByteArray& data)
{
    QByteArray cleanLine;
    cleanLine.append(data.simplified());

    if (cleanLine.contains(Defs::ENGINE_1_BIN.toLatin1()))
    {
        QList<QByteArray> columnList(cleanLine.split(' '));
        QByteArray col = columnList.at(1);
        const char* colData = col.constData();
        pid_ = QString::fromUtf8(colData);
        qDebug() << "pid" << pid_;

        freezerUtility_->kill();

        disconnect(freezerUtility_, SIGNAL(readyReadStandardOutput()),
                 this, SLOT(bufferFreezerOutput()));
        QTimer::singleShot(1000, this, SLOT(processPause_2()));
    }
}

bool::Process::isRunning()
{
    return (process_->state() == QProcess::Running);
}
