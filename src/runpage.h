/***************************************************************************
  runpage.h
  ---------
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

#ifndef RUNPAGE_H
#define RUNPAGE_H

#include <QElapsedTimer>
#include <QWidget>

#include "configstate.h"
#include "defs.h"

class QLabel;
class QProgressBar;
class QPushButton;
class QTextEdit;
class QTimer;

class QProgressIndicator;

class ClickLabel;
class EcProject;
class SmartFluxBar;

class RunPage : public QWidget
{
    Q_OBJECT

public:
    RunPage(QWidget *parent, EcProject *ecProject, ConfigState *config);
    ~RunPage() override;

    void startRun(Defs::CurrRunStatus mode);
    bool pauseRun(Defs::CurrRunStatus mode);
    bool resumeRun(Defs::CurrRunStatus mode);
    void stopRun();
    void updateSmartfluxBar();
    void updateRunPage(bool small);

public slots:
    void resetBuffer();
    void bufferData(QByteArray &data);

signals:
    void updateConsoleLineRequest(QByteArray &data);
    void updateConsoleCharRequest(QByteArray &data);
    void pauseRequest(Defs::CurrRunStatus mode);

private slots:
    void pauseLabel();
    void resumeLabel();
    void runModeIconClicked();
    void updateElapsedTime();
    void updateMiniProgress();
    void openOutputDir();
    void openToviHomepage();

    void init();

private:
    bool filterData(const QByteArray &data);
    QByteArray cleanupEngineOutput(QByteArray data);
    void parseEngineOutput(const QByteArray& data);
    void resetProgressSoft();
    void resetProgressHard();
    void doneLabel();
    void resetLabels();
    void resetFileLabels();
    void resetTimeEstimateLabels();
    int updateETC(int *mean_processing_time,
                  int current_processing_time,
                  int index,
                  int num_steps);

    Defs::CurrRunStatus runMode_;
    QProgressIndicator* progressWidget_;
    QProgressBar* main_progress_bar;
    QProgressBar* mini_progress_bar_;
    ClickLabel* runModeIcon_;
    QLabel* runModeLabel_;
    QLabel* progressLabel_;
    QLabel* avgPeriodLabel_;
    QLabel* fileListLabel_;
    QLabel* fileProgressLabel_;
    QLabel* timeEstimateLabels_;
    QLabel* pauseResumeLabel_;
    QPushButton* open_output_dir;
    ClickLabel *toviLabel;

    EcProject* ecProject_;
    ConfigState* configState_;
    int progressValue_;
    QByteArray rxBuffer_;
    QTextEdit* errorEdit_;

    QTimer* pauseResumeDelayTimer_;        // delay and control the pause/resume operations
    QTimer* total_elapsed_update_timer_;   // update the overall elapsed time shown during a run
    QElapsedTimer overall_progress_timer_; // measure the total run time
    QElapsedTimer main_progress_timer_;    // measure the main steps run time

    SmartFluxBar* smartfluxBar_;

    bool inPlanarFit_ = false;
    bool inTimeLag_ = false;
};

#endif // RUNPAGE_H
