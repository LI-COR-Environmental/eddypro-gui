/***************************************************************************
  process.h
  ---------
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
