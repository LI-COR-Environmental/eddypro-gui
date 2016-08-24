/***************************************************************************
  process.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#include "process.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

#if defined(Q_OS_WIN)
#include <windows.h>
#include <psapi.h>
#endif

#include "dbghelper.h"

Process::Process(QWidget* parent, const QString &fullPath) :
    QObject(parent),
    process_(0),
    fullPath_(fullPath),
    processExit_(ExitStatus::Success),
    processPid_(0),
    winPid_(QString()),
    freezerUtility_(0),
    rxBuffer_(QByteArray())
{
    process_ = new QProcess(this);
    connect(process_, &QProcess::readyReadStandardOutput,
             this, &Process::readyReadStdOut);
    connect(process_, &QProcess::readyReadStandardError,
             this, &Process::readyReadStdErr);

    freezerUtility_ = new QProcess(this);
}

Process::~Process()
{
    DEBUG_FUNC_NAME
}

bool Process::engineProcessStart(const QString& fullPath, const QString& workingDir, const QStringList& argList)
{
    DEBUG_FUNC_NAME

    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->setWorkingDirectory(workingDir);

    // NOTE: start() function without args not parse correctly filepath with spaces, Qt bug?
    process_->start(fullPath, argList, QProcess::Unbuffered | QProcess::ReadOnly);
    processPid_ = process_->processId();

    return true;
}

// add file to an archive fileName
// using an external helper (7z)
// NOTE: never used
bool Process::zipProcessAddStart(const QString &fileName,
                                 const QString &toArchive,
                                 const QString &workingDir,
                                 const QString &fileType)
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
            + QLatin1Char('/')
            + Defs::BIN_FILE_DIR
            + QLatin1Char('/')
            + Defs::COMPRESSOR_BIN);
    qDebug() << "fp" << fp;
    qDebug() << "filename" << fileName;
    qDebug() << "toArchive" << toArchive;
    qDebug() << "args" << args;
    qDebug() << "workDir" << workDir;

    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->setWorkingDirectory(workDir);

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished(2000))
        return false;

    return true;
}

// extract int the outDir all the metadata files in the fileName archive
// using an external helper (7z)
// NOTE: never used
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
            + QLatin1Char('/')
            + Defs::BIN_FILE_DIR
            + QLatin1Char('/')
            + Defs::COMPRESSOR_BIN);
    qDebug() << fp;
    qDebug() << args;

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished(2000))
        return false;

    return process_->exitCode();
}

// return if a specific file type is present in the archive
// using an external helper (7z)
// NOTE: never used
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

    // file path of the 7z utility
    QString fp(qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR + QLatin1Char('/') + Defs::COMPRESSOR_BIN);

    connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->start(fp, args, QProcess::Unbuffered | QProcess::ReadWrite);

    if (!process_->waitForFinished())
        return false;

    QByteArray dataList = process_->readAllStandardOutput();
//    qDebug() << "dataList" << dataList;
    return dataList.contains(filePattern.mid(1).toLatin1());
}

void Process::processPause(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME
    Q_UNUSED(mode)

    // file path of the utility to pause the engine
    QString fp;

    // arguments
    QStringList args;

#if defined(Q_OS_WIN)
    // On Windows we pause using a third-party utility, namely 'pausep.exe'.
    // Pausing the engine requires two runs of it,
    // one for detecting the pid with no args and one for tha actual pausing.

    fp = qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR + QLatin1Char('/') + Defs::FREEZER_BIN;
    connect(freezerUtility_, &QProcess::readyReadStandardOutput,
             this, &Process::bufferFreezerOutput);

#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    // on mac anc linux we use the standard kill
    // calling, 'kill -STOP processPid_'

    fp = QStringLiteral("kill");

    args << QStringLiteral("-STOP");
    args << QString::number(processPid_);
#endif

    qDebug() << "fp" << fp << "args" << args;

    freezerUtility_->setWorkingDirectory(qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR);
    freezerUtility_->start(fp, args);
}

void Process::processResume(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME
    Q_UNUSED(mode)

    // file path of the utility to resume the engine
    QString fp;

    // arguments
    QStringList args;

#if defined(Q_OS_WIN)
    // file path of the utility to resume the engine
    fp = qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR + QLatin1Char('/') + Defs::FREEZER_BIN;

    args << winPid_;
    args << QStringLiteral("/r");

#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    // on mac anc linux we use the standard kill
    // calling, 'kill -CONT processPid_'

    fp = QStringLiteral("kill");

    args << QStringLiteral("-CONT");
    args << QString::number(processPid_);
#endif

    qDebug() << "fp" << fp << "args" << args;

    freezerUtility_->setWorkingDirectory(qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR);
    freezerUtility_->start(fp, args);
}

void Process::processStop()
{
    DEBUG_FUNC_NAME

    // to avoid crash message error in windows
    disconnect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(processFinished(int, QProcess::ExitStatus)));
    disconnect(process_, SIGNAL(error(QProcess::ProcessError)),
             this, SLOT(processError(QProcess::ProcessError)));

    process_->kill();
    processExit_ = ExitStatus::Stopped;
}

void Process::processError(QProcess::ProcessError error)
{
    DEBUG_FUNC_NAME
    if (error == QProcess::FailedToStart)
    {
        qWarning() << tr("program not found.");
        processExit_ = ExitStatus::FailureToStart;
    }
    else
    {
        processExit_ = ExitStatus::Error;
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
    disconnect(process_, SIGNAL(finished(int, QProcess::ExitStatus)),
             this, SLOT(processFinished(int, QProcess::ExitStatus)));

    if (exitStatus == QProcess::CrashExit)
    {
        processExit_ = ExitStatus::Error;
        qWarning() << tr("process crashed.");
    }
    else if (exitCode != 0)
    {
        processExit_ = ExitStatus::Failure;
        qWarning() << tr("process failed.");
    }
    else
    {
        processExit_ = ExitStatus::Success;
        qWarning() << tr("process ok.");
        emit processSuccess();
        return;
    }
    emit processFailure();
}

QByteArray Process::readAllStdOut() const
{
    DEBUG_FUNC_NAME
    return process_->readAllStandardOutput();
}

QByteArray Process::readAllStdErr() const
{
    DEBUG_FUNC_NAME
    return process_->readAllStandardError();
}

QProcess *Process::process() const
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

// analyze the pausep output
void Process::bufferFreezerOutput()
{
    DEBUG_FUNC_NAME
#if defined(Q_OS_WIN)
    QByteArray data = freezerUtility_->readAllStandardOutput();

    rxBuffer_.append(data);
    QByteArray line(rxBuffer_);
    QByteArrayList lineList(line.split('\n'));

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
#endif
}

// detect the engine pid under Windows
void Process::parseFreezerPid(const QByteArray& data)
{
#if defined(Q_OS_WIN)
    QByteArray cleanLine;
    cleanLine.append(data.simplified());

    if (cleanLine.contains(Defs::ENGINE_RP.toLatin1()))
    {
        QByteArrayList columnList(cleanLine.split(' '));
        QByteArray col = columnList.at(1);
        const char* colData = col.constData();
        winPid_ = QString::fromUtf8(colData);
        qDebug() << "pid" << winPid_;

        freezerUtility_->kill();

        disconnect(freezerUtility_, &QProcess::readyReadStandardOutput,
                 this, &Process::bufferFreezerOutput);
        QTimer::singleShot(1000, this, SLOT(processPause_2()));
    }
#endif
    Q_UNUSED(data)
}

// Second run of 'pausep.exe' on Windows
void Process::processPause_2()
{
    DEBUG_FUNC_NAME

#if defined(Q_OS_WIN)
    // file path of the program
    QString fp(qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR + QLatin1Char('/') + Defs::FREEZER_BIN);

    QStringList args;
    args << winPid_;

    freezerUtility_->setWorkingDirectory(qApp->applicationDirPath() + QLatin1Char('/') + Defs::BIN_FILE_DIR);
    freezerUtility_->start(fp, args);
#endif
}

#if 0
// from http://www.qtcentre.org/threads/44489-Get-Process-ID-for-a-running-application
unsigned int Process::getProcessIdsByProcessName(const QString& processName, QStringList &listOfPids)
{
    DEBUG_FUNC_NAME
    // Clear content of returned list of PIDS
    listOfPids.clear();

#if defined(Q_OS_WIN)
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 0;
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Search for a matching name for each process
    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            char szProcessName[MAX_PATH] = {0};

            DWORD processID = aProcesses[i];

            // Get a handle to the process.
            HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                PROCESS_VM_READ,
                FALSE, processID);

            // Get the process name
            if (NULL != hProcess)
            {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
                {
                    GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(char));
                }

                // Release the handle to the process.
                CloseHandle(hProcess);

                QByteArray ba = processName.toLocal8Bit();
                const char *processName_str = ba.data();

                if (*szProcessName != 0 && strcmp(processName_str, szProcessName) == 0)
                {
                    listOfPids.append(QString::number(processID));
                }
            }
        }
    }

    return listOfPids.count();

#else

    // Run pgrep, which looks through the currently running processses and lists the process IDs
    // which match the selection criteria to stdout.
    QProcess process;
    process.start(QStringLiteral("pgrep"),  QStringList() << processName);
    process.waitForReadyRead();

    QByteArray bytes = process.readAllStandardOutput();

    process.terminate();
    process.waitForFinished();
    process.kill();

    // Output is something like "2472\n2323" for multiple instances
    if (bytes.isEmpty())
        return 0;

    // Remove trailing CR
    if (bytes.endsWith("\n"))
        bytes.resize(bytes.size() - 1);

    qDebug() << bytes;

    listOfPids = QString(QLatin1String(bytes)).split(QStringLiteral("\n"));
    return listOfPids.count();

#endif
}
#endif
