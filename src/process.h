/***************************************************************************
  process.h
  -------------------
  A process management class
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

#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>
#include "defs.h"

class QByteArray;

class Process: public QObject
{
Q_OBJECT

public:
    enum ExitStatus { exitSuccess, exitFailureToStart, exitFailure, exitError, exitStopped};

    Process(QWidget* parent = 0, const QString &fullPath = QString());
    ~Process();

    bool engineProcessStart(const QString& fullPath, const QString& workingDir, const QStringList& argList);

    bool zipProcessAddStart(const QString& fileName, const QString &toArchive, const QString &workingDir = QString(), const QString& fileType = QString());
    bool zipProcessExtMdStart(const QString& fileName, const QString& outDir);
    bool zipContainsFiletype(const QString& fileName, const QString& fileExt);

    void setChannelsMode(QProcess::ProcessChannelMode mode);
    void setReadChannels(QProcess::ProcessChannel channel);

    QByteArray readAllStdOut();
    QByteArray readAllStdErr();

    QProcess *process();
    inline QProcess::ProcessState processState() { return process_->state(); }
    inline QProcess::ProcessError processError() { return process_->error(); }
    inline QString processErrorString() { return process_->errorString(); }
    inline QString processPath() { return fullPath_; }

    void setEnv(const QStringList &envList);

    void processPause(Defs::CurrRunStatus mode);
    void processResume(Defs::CurrRunStatus mode);
    void processStop();
    inline ExitStatus processExit() { return processExit_; }
    bool isRunning();

private slots:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processPause_2();
    void bufferFreezerOutput();

private:
    QProcess* process_;
    QString fullPath_;
    ExitStatus processExit_;
    Q_PID processPid_;
    QProcess* freezerUtility_;
    QString pid_;
    QByteArray rxBuffer_;

    void parseFreezerPid(const QByteArray& data);

signals:
    void readyReadStdOut();
    void readyReadStdErr();
    void processSuccess();
    void processFailure();
};

#endif // PROCESS_H
