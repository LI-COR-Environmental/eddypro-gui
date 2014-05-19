/***************************************************************************
  runpage.h
  -------------------
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

#ifndef RUNPAGE_H
#define RUNPAGE_H

#include <QWidget>
#include <QTime>

#include "defs.h"
#include "configstate.h"

class QProgressIndicator;
class QProgressBar;
class QLabel;
class ClickLabel;
class QTextEdit;

class EcProject;
class SmartFluxBar;

class RunPage : public QWidget
{
    Q_OBJECT

public:
    explicit RunPage(QWidget *parent, EcProject *ecProject, ConfigState *config);
    ~RunPage();

    void startRun(Defs::CurrRunStatus mode);
    bool pauseRun(Defs::CurrRunStatus mode);
    bool resumeRun(Defs::CurrRunStatus mode);
    void stopRun();
    void updateSmartfluxBar();

private:
    Defs::CurrRunStatus runMode_;
    QProgressIndicator *progressWidget_;
    QProgressBar* progressBar_;
    QProgressBar* miniProgressBar_;
    ClickLabel* runModeIcon_;
    QLabel* runModeLabel_;
    QLabel* progressLabel_;
    QLabel* fileLabel_;
    QLabel* fileProgressLabel_;
    QLabel* timeEstimateLabels_;
    QLabel* pauseResumeLabel_;

    EcProject *ecProject_;
    ConfigState* configState_;
    int progressValue_;
    QByteArray rxBuffer_;
    QTimer* delayTimer_;
    QTimer* elapsedTimer_;
    QTextEdit *errorEdit_;

    QTime overallTime_;

    SmartFluxBar* smartfluxBar_;

    QByteArray cleanupEngineOutput(QByteArray data);
    void parseEngineOutput(const QByteArray& data);
    void resetProgressSoft();
    void resetProgressHard();
    void doneLabel();
    void resetLabels();
    void resetFileLabels();
    void resetTimeEstimateLabels();

private slots:
    void pauseLabel();
    void resumeLabel();
    void runModeIconClicked();

    void updateElapsedTime();
signals:
    void updateConsoleRequest(QByteArray &data);
    void runExpRequest();
    void runAdvRequest();
    void runRetRequest();

public slots:
    void resetBuffer();
    void bufferData(QByteArray &data);
};

#endif // RUNPAGE_H
