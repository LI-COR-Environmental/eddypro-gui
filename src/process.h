/***************************************************************************
  process.h
  -------------------
  A process management class
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

#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>

#include "defs.h"

////////////////////////////////////////////////////////////////////////////////
/// \file src/process.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QByteArray;

/// \class Process
/// \brief Class representing external processes
class Process: public QObject
{
Q_OBJECT

public:
    enum class ExitStatus {
        Success,
        FailureToStart,
        Failure,
        Error,
        Stopped
    };

    Process(QObject* parent = nullptr, QString  fullPath = QString());

    bool engineProcessStart(const QString& fullPath, const QString& workingDir, const QStringList& argList);

    // NOTE: never used (7z based)
    bool zipProcessAddStart(const QString& fileName,
                            const QString &toArchive,
                            const QString &workingDir = QString(),
                            const QString& fileType = QString());

    // NOTE: never used (7z based)
    bool zipProcessExtMdStart(const QString& fileName,
                              const QString& outDir);
    // NOTE: never used (7z based)
    bool zipContainsFiletype(const QString& fileName,
                             const QString& fileExt);

    void setChannelsMode(QProcess::ProcessChannelMode mode);
    void setReadChannels(QProcess::ProcessChannel channel);

    QByteArray readAllStdOut() const;
    QByteArray readAllStdErr() const;

    QProcess *process() const;
    inline QProcess::ProcessState processState() const { return process_->state(); }
    inline QProcess::ProcessError processError() const { return process_->error(); }
    inline QString processErrorString() const { return process_->errorString(); }
    inline QString processPath() const { return fullPath_; }

    void setEnv(const QStringList &envList);

    void processPause(Defs::CurrRunStatus mode);
    void processResume(Defs::CurrRunStatus mode);
    void processStop();
    inline ExitStatus processExit() const { return processExit_; }
    bool isRunning() const { return (process_->state() == QProcess::Running); }

#if 0
    static unsigned int getProcessIdsByProcessName(const QString &processName, QStringList &listOfPids);
#endif

private slots:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processPause_2();
    void bufferFreezerOutput();

private:
    QProcess* process_;
    QString fullPath_;
    ExitStatus processExit_;
    qint64 processPid_;
    QString winPid_;
    QProcess* freezerUtility_;
    QByteArray rxBuffer_;

    void parseFreezerPid(const QByteArray& data);

signals:
    void readyReadStdOut();
    void readyReadStdErr();
    void processSuccess();
    void processFailure();
};

#endif // PROCESS_H
