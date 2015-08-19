/***************************************************************************
  runpage.cpp
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

#include "runpage.h"

#include <QDebug>
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QFile>
#include <QGridLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QTime>
#include <QTimer>

#include "QProgressIndicator.h"

#include "clicklabel.h"
#include "dbghelper.h"
#include "ecproject.h"
#include "smartfluxbar.h"
#include "widget_utils.h"

RunPage::RunPage(QWidget *parent, EcProject *ecProject, ConfigState* config)
    : QWidget(parent),
      runMode_(Defs::CurrRunStatus::Express),
      ecProject_(ecProject),
      configState_(config),
      progressValue_(0),
      errorEdit_(nullptr),
      pauseResumeDelayTimer_(nullptr),
      total_elapsed_update_timer_(nullptr)
{
    progressWidget_ = new QProgressIndicator;
    progressWidget_->setAnimationDelay(40);
    progressWidget_->setDisplayedWhenStopped(false);
    progressWidget_->setFixedSize(30, 30);
    progressWidget_->setColor(QColor(46, 98, 152));

    runModeIcon_ = new ClickLabel;

    runModeLabel_ = new QLabel;
    runModeLabel_->setObjectName(QStringLiteral("fileLabel"));

    progressLabel_ = new QLabel;
    progressLabel_->setObjectName(QStringLiteral("fileLabel"));

    avgPeriodLabel_ = new QLabel;
    avgPeriodLabel_->setObjectName(QStringLiteral("fileLabel"));

    fileListLabel_  = new QLabel;
    fileListLabel_->setObjectName(QStringLiteral("fileLabel"));

    fileProgressLabel_ = new QLabel;
    fileProgressLabel_->setObjectName(QStringLiteral("fileLabel"));

    timeEstimateLabels_ = new QLabel;
    timeEstimateLabels_->setText(QStringLiteral("Total elapsed time: 00:00:00 - Estimated time "
                                                "to completion: --:--:--.---"));
    timeEstimateLabels_->setObjectName(QStringLiteral("fileLabel"));

    pauseResumeLabel_ = new QLabel;
    pauseResumeLabel_->setObjectName(QStringLiteral("fileLabel"));

    main_progress_bar = new QProgressBar;
    main_progress_bar->setMinimum(0);
    main_progress_bar->setMaximum(10);

    mini_progress_bar_ = new QProgressBar;
    mini_progress_bar_->setMinimum(0);
    mini_progress_bar_->setMaximum(25);
    mini_progress_bar_->setObjectName(QStringLiteral("miniProgress"));
    mini_progress_bar_->setTextVisible(false);

    pauseResumeDelayTimer_ = new QTimer(this);
    pauseResumeDelayTimer_->setInterval(1500);
    pauseResumeDelayTimer_->setSingleShot(true);

    total_elapsed_update_timer_ = new QTimer(this);
    total_elapsed_update_timer_->setInterval(1000);
    total_elapsed_update_timer_->setTimerType(Qt::PreciseTimer);

    errorEdit_ = new QTextEdit;
    errorEdit_->setObjectName(QStringLiteral("ErrorEdit"));
    errorEdit_->setReadOnly(true);
    errorEdit_->setCursorWidth(0);
    errorEdit_->setText(QStringLiteral("No errors"));
    errorEdit_->setLineWrapMode(QTextEdit::NoWrap);

    open_output_dir = new QPushButton;
    open_output_dir->setText(tr("Open output directory"));
    open_output_dir->setProperty("commonButton2", true);
    open_output_dir->setMaximumWidth(200);

    auto progressLayout = new QGridLayout;
    progressLayout->addWidget(runModeIcon_, 0, 0, 1, 1);
    progressLayout->addWidget(runModeLabel_, 0, 1);
    progressLayout->addWidget(progressWidget_, 1, 0, 1, 1, Qt::AlignCenter);
    progressLayout->addWidget(progressLabel_, 1, 1);
    progressLayout->addWidget(timeEstimateLabels_, 2, 1);
    progressLayout->addWidget(main_progress_bar, 3, 1, 1, -1);
    progressLayout->addWidget(avgPeriodLabel_, 4, 1);
    progressLayout->addWidget(mini_progress_bar_, 5, 1, 1, 2);
    progressLayout->addWidget(fileListLabel_, 6, 1, Qt::AlignTop);
    progressLayout->addWidget(fileProgressLabel_, 7, 1, Qt::AlignTop);
    progressLayout->addWidget(errorEdit_, 8, 1, 1, 2);
    progressLayout->addWidget(pauseResumeLabel_, 9, 1);
    progressLayout->addWidget(open_output_dir, 10, 1);
    progressLayout->setColumnStretch(2, 2);
    progressLayout->setRowStretch(8, 2);
    progressLayout->setRowStretch(11, 2);
    progressLayout->setRowMinimumHeight(0, 42);   // = runModeIcon_.width()
    progressLayout->setColumnMinimumWidth(0, 42); // > runModeIcon_.height()
    progressLayout->setHorizontalSpacing(6);
    progressLayout->setContentsMargins(20, 15, 60, 0);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);

    auto mainLayout = new QGridLayout(this);
    mainLayout->addWidget(smartfluxBar_, 0, 0);
    mainLayout->addLayout(progressLayout, 1, 0);
    mainLayout->setContentsMargins(20, 15, 30, 0);
    mainLayout->setRowStretch(1, 0);

    setLayout(progressLayout);

    connect(runModeIcon_, &ClickLabel::clicked,
            this, &RunPage::runModeIconClicked);

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));

    connect(total_elapsed_update_timer_, &QTimer::timeout,
            this, &RunPage::updateElapsedTime);

    connect(total_elapsed_update_timer_, &QTimer::timeout,
            this, &RunPage::updateMiniProgress);
    connect(open_output_dir, &QPushButton::clicked,
            this, &RunPage::openOutputDir);

    auto open_output = [=]() {
        open_output_dir->setDisabled(ecProject_->generalOutPath().isEmpty());
    };

    connect(ecProject_, &EcProject::ecProjectNew,
            this, open_output);

    connect(ecProject_, &EcProject::ecProjectModified,
            this, open_output);

    connect(ecProject_, &EcProject::ecProjectChanged,
            this, open_output);

    QList<WidgetUtils::PropertyList> progressBarProp;
    progressBarProp << WidgetUtils::PropertyList("expRun", false)
                    << WidgetUtils::PropertyList("advRun", false)
                    << WidgetUtils::PropertyList("retRun", false);
    WidgetUtils::updatePropertyListAndStyle(main_progress_bar, progressBarProp);
    WidgetUtils::updatePropertyListAndStyle(mini_progress_bar_, progressBarProp);
}

RunPage::~RunPage()
{
    if (pauseResumeDelayTimer_)
        delete pauseResumeDelayTimer_;
}

void RunPage::startRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    QList<WidgetUtils::PropertyList> iconModeProp;
    QList<WidgetUtils::PropertyList> progressBarProp;
    auto progressText = QString();

    runMode_ = mode;

    switch (runMode_)
    {
        case Defs::CurrRunStatus::Express:
        {
            iconModeProp << WidgetUtils::PropertyList("pauseExpButton", true)
                         << WidgetUtils::PropertyList("pauseAdvButton", false)
                         << WidgetUtils::PropertyList("pauseRetButton", false)
                         << WidgetUtils::PropertyList("resumeExpButton", false)
                         << WidgetUtils::PropertyList("resumeAdvButton", false)
                         << WidgetUtils::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running in Express mode"));

            progressBarProp << WidgetUtils::PropertyList("expRun", true)
                            << WidgetUtils::PropertyList("advRun", false)
                            << WidgetUtils::PropertyList("retRun", false);

            progressText = tr("Initializing raw data processing...");

            errorEdit_->clear();
            errorEdit_->setText(QStringLiteral("No errors."));

            break;
        }
    case Defs::CurrRunStatus::Advanced_RP:
        {
            iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                         << WidgetUtils::PropertyList("pauseAdvButton", true)
                         << WidgetUtils::PropertyList("pauseRetButton", false)
                         << WidgetUtils::PropertyList("resumeExpButton", false)
                         << WidgetUtils::PropertyList("resumeAdvButton", false)
                         << WidgetUtils::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running in Advanced mode"));

            progressBarProp << WidgetUtils::PropertyList("expRun", false)
                            << WidgetUtils::PropertyList("advRun", true)
                            << WidgetUtils::PropertyList("retRun", false);

            progressText = tr("Initializing raw data processing...");

            errorEdit_->clear();
            errorEdit_->setText(QStringLiteral("No errors."));

            break;
        }
    case Defs::CurrRunStatus::Advanced_FCC:
        {
            iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                         << WidgetUtils::PropertyList("pauseAdvButton", true)
                         << WidgetUtils::PropertyList("pauseRetButton", false)
                         << WidgetUtils::PropertyList("resumeExpButton", false)
                         << WidgetUtils::PropertyList("resumeAdvButton", false)
                         << WidgetUtils::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running in Advanced mode"));

            progressBarProp << WidgetUtils::PropertyList("expRun", false)
                            << WidgetUtils::PropertyList("advRun", true)
                            << WidgetUtils::PropertyList("retRun", false);

            progressText = tr("Initializing flux computation processing...");
            errorEdit_->append(QStringLiteral("\n"));
            break;
        }
    case Defs::CurrRunStatus::Retriever:
        {
            iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                         << WidgetUtils::PropertyList("pauseAdvButton", false)
                         << WidgetUtils::PropertyList("pauseRetButton", true)
                         << WidgetUtils::PropertyList("resumeExpButton", false)
                         << WidgetUtils::PropertyList("resumeAdvButton", false)
                         << WidgetUtils::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running Metadata Retriever"));

            progressBarProp << WidgetUtils::PropertyList("expRun", false)
                            << WidgetUtils::PropertyList("advRun", false)
                            << WidgetUtils::PropertyList("retRun", true);

            progressText = tr("Initializing metadata retriever...");

            errorEdit_->clear();
            errorEdit_->setText(QStringLiteral("No errors."));

            break;
        }
        default:
            break;
    }

    WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
    WidgetUtils::updatePropertyListAndStyle(main_progress_bar, progressBarProp);
    WidgetUtils::updatePropertyListAndStyle(mini_progress_bar_, progressBarProp);

    runModeIcon_->setVisible(true);
    progressLabel_->setText(progressText);
    progressWidget_->startAnimation();
}

bool RunPage::pauseRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    if (!pauseResumeDelayTimer_->isActive())
    {
        qDebug() << "delayTimer_->isActive()" << pauseResumeDelayTimer_->isActive();

        QList<WidgetUtils::PropertyList> iconModeProp;
        pauseResumeDelayTimer_->start();
        runMode_ = mode;
        switch (runMode_)
        {
            case Defs::CurrRunStatus::Express:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                             << WidgetUtils::PropertyList("pauseAdvButton", false)
                             << WidgetUtils::PropertyList("pauseRetButton", false)
                             << WidgetUtils::PropertyList("resumeExpButton", true)
                             << WidgetUtils::PropertyList("resumeAdvButton", false)
                             << WidgetUtils::PropertyList("resumeRetButton", false);
                break;
            }
        case Defs::CurrRunStatus::Advanced_RP:
        case Defs::CurrRunStatus::Advanced_FCC:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                             << WidgetUtils::PropertyList("pauseAdvButton", false)
                             << WidgetUtils::PropertyList("pauseRetButton", false)
                             << WidgetUtils::PropertyList("resumeExpButton", false)
                             << WidgetUtils::PropertyList("resumeAdvButton", true)
                             << WidgetUtils::PropertyList("resumeRetButton", false);
                break;
            }
        case Defs::CurrRunStatus::Retriever:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                             << WidgetUtils::PropertyList("pauseAdvButton", false)
                             << WidgetUtils::PropertyList("pauseRetButton", false)
                             << WidgetUtils::PropertyList("resumeExpButton", false)
                             << WidgetUtils::PropertyList("resumeAdvButton", false)
                             << WidgetUtils::PropertyList("resumeRetButton", true);
                break;
            }
            default:
                break;
        }

        WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Pausing computations..."));
        progressWidget_->setDisplayedWhenStopped(true);
        progressWidget_->stopAnimation();
        total_elapsed_update_timer_->stop();
        main_progress_timer_.invalidate();
        QTimer::singleShot(1000, this, SLOT(pauseLabel()));
        return true;
    }
    else
    {
        qDebug() << "delayTimer_->isActive()" << pauseResumeDelayTimer_->isActive();
        return false;
    }
}

bool RunPage::resumeRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    if (!pauseResumeDelayTimer_->isActive())
    {
        qDebug() << "delayTimer_->isActive()" << pauseResumeDelayTimer_->isActive();

        QList<WidgetUtils::PropertyList> iconModeProp;
        pauseResumeDelayTimer_->start();
        runMode_ = mode;
        switch (runMode_)
        {
            case Defs::CurrRunStatus::Express:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", true)
                             << WidgetUtils::PropertyList("pauseAdvButton", false)
                             << WidgetUtils::PropertyList("pauseRetButton", false)
                             << WidgetUtils::PropertyList("resumeExpButton", false)
                             << WidgetUtils::PropertyList("resumeAdvButton", false)
                             << WidgetUtils::PropertyList("resumeRetButton", false);
                break;
            }
        case Defs::CurrRunStatus::Advanced_RP:
        case Defs::CurrRunStatus::Advanced_FCC:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                             << WidgetUtils::PropertyList("pauseAdvButton", true)
                             << WidgetUtils::PropertyList("pauseRetButton", false)
                             << WidgetUtils::PropertyList("resumeExpButton", false)
                             << WidgetUtils::PropertyList("resumeAdvButton", false)
                             << WidgetUtils::PropertyList("resumeRetButton", false);
                break;
            }
        case Defs::CurrRunStatus::Retriever:
            {
                iconModeProp << WidgetUtils::PropertyList("pauseExpButton", false)
                             << WidgetUtils::PropertyList("pauseAdvButton", false)
                             << WidgetUtils::PropertyList("pauseRetButton", true)
                             << WidgetUtils::PropertyList("resumeExpButton", false)
                             << WidgetUtils::PropertyList("resumeAdvButton", false)
                             << WidgetUtils::PropertyList("resumeRetButton", false);
                break;
            }
            default:
                break;
        }

        WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Resuming computations..."));
        progressWidget_->setDisplayedWhenStopped(true);
        progressWidget_->startAnimation();
        total_elapsed_update_timer_->start();
        main_progress_timer_.restart();
        QTimer::singleShot(1000, this, SLOT(resumeLabel()));
        return true;
    }
    else
    {
        qDebug() << "delayTimer_->isActive()" << pauseResumeDelayTimer_->isActive();
        return false;
    }
}

void RunPage::stopRun()
{
    DEBUG_FUNC_NAME
    progressWidget_->setDisplayedWhenStopped(false);
    progressWidget_->stopAnimation();
    resetBuffer();
    resetProgressHard();
    total_elapsed_update_timer_->stop();
    main_progress_timer_.invalidate();
}

void RunPage::pauseLabel()
{
    DEBUG_FUNC_NAME
    pauseResumeDelayTimer_->stop();
    pauseResumeLabel_->setText(tr("Computations paused"));
}

void RunPage::resumeLabel()
{
    DEBUG_FUNC_NAME
    pauseResumeDelayTimer_->stop();
    pauseResumeLabel_->clear();
}

void RunPage::resetBuffer()
{
    rxBuffer_.resize(0);
}

void RunPage::resetProgressSoft()
{
    progressValue_ = 0;
    main_progress_bar->setValue(0);
    main_progress_bar->setMaximum(10);
    main_progress_bar->reset();
}

void RunPage::resetProgressHard()
{
    runModeIcon_->setVisible(false);
    runModeLabel_->clear();
    pauseResumeLabel_->clear();
    doneLabel();
    resetLabels();
    resetFileLabels();
    resetTimeEstimateLabels();

    resetProgressSoft();
    mini_progress_bar_->setValue(0);
    mini_progress_bar_->reset();
}

void RunPage::doneLabel()
{
    DEBUG_FUNC_NAME
    main_progress_bar->setValue(main_progress_bar->maximum());
    progressLabel_->setText(tr("Done"));
}

void RunPage::resetFileLabels()
{
    DEBUG_FUNC_NAME
    avgPeriodLabel_->clear();
    fileListLabel_->clear();
    fileProgressLabel_->clear();
}

void RunPage::resetTimeEstimateLabels()
{
    DEBUG_FUNC_NAME
    // commented for debug only
//    timeEstimateLabels_->clear();
}

void RunPage::resetLabels()
{
    DEBUG_FUNC_NAME
    progressLabel_->clear();
    resetFileLabels();
    resetTimeEstimateLabels();
}

bool RunPage::filterData(const QByteArray& data)
{
    if (inPlanarFit_ or inTimeLag_)
    {
        return data.contains(QByteArray("another small step"));
    }
    return false;
}

void RunPage::bufferData(QByteArray &data)
{
    qDebug() << "data" << data;

    rxBuffer_.append(data);
    QByteArray line(rxBuffer_);
    QList<QByteArray> lineList(line.split('\n'));

    qDebug() << "rxBuffer_" << rxBuffer_;
    qDebug() << "lineList.at(0)" << lineList.at(0);

    // newline found
    if (lineList.at(0) != rxBuffer_)
    {
        for (int i = 0; i < lineList.size(); ++i)
        {
            qDebug() << "lineList.at(i)" << i << lineList.at(i);
            QByteArray tempData(lineList.at(i));
            data = cleanupEngineOutput(tempData);
            qDebug() << "data after cleanup" << data;
            if (!data.isEmpty())
            {
                parseEngineOutput(data);
                if (!filterData(data))
                {
                    emit updateConsoleLineRequest(data);
                }
            }
        }

        if (lineList.last().endsWith('\n'))
            resetBuffer();
        else
            rxBuffer_ = lineList.last();
    }
//    else
//    {
//        if (!data.isEmpty())
//        {
//            parseEngineOutput(data);
//            emit updateConsoleCharRequest(data);
//        }
////        rxBuffer_ = data;
//    }
}

QByteArray RunPage::cleanupEngineOutput(QByteArray data)
{
    DEBUG_FUNC_NAME
    if (data.contains(QByteArray("of file C:")))
    {
        // cleanup file path in case of fortan runtime error
        qDebug() << "data before" << data;
        data = data.mid(0, data.indexOf("C:")) + data.mid(data.lastIndexOf("\\") + 1);
        qDebug() << "data after" << data;
    }
    else if (data == QByteArray(" "))
    {
        data = QByteArray();
    }
    else if (data.contains(QByteArray("STOP")))
    {
        data = QByteArray();
    }
    return data;
}

void RunPage::parseEngineOutput(const QByteArray &data)
{
    DEBUG_FUNC_NAME

    QByteArray cleanLine;
    cleanLine.append(data);

    static int averagingPeriodIndex = 0;
    static int totalAveragingPeriods = 0;
    static int processingTimeMSec = 0;
    static int meanProcessingTimeMSec = 0;
    static int elapsedTimeMSec = 0;
    static int estimatedTimeToCompletionMSec = 0;
    static int previous_elapsed_time = 0;
    int current_step_elapsed_time = 0;

    static QString fromStr;
    static QString toStr;

    static QByteArray previousTag = QByteArray();

    QString elapsedTimeMSecStr; // just for debug

    QStringList currentFileList;

    static QDate currentPlanarFitDate;
    static QDate currentTimeLagDate;

    // NOTE: set but not used yet
    bool inCycle = false;

#ifdef QT_DEBUG
    QFile outfile(QStringLiteral("parse-engine-output.txt"));
    outfile.open(QFile::WriteOnly | QFile::Append);
    QTextStream out(&outfile);
#endif

    // start preamble
    if (cleanLine.contains(QByteArray("Executing EddyPro")))
    {
        total_elapsed_update_timer_->start();

        timeEstimateLabels_->setText(timeEstimateLabels_->text()
                                     .replace(20, 8, QStringLiteral("00:00:00")));
        timeEstimateLabels_->setText(timeEstimateLabels_->text()
                                     .replace(61, 12, QStringLiteral("--:--:--.---")));

        overall_progress_timer_.restart();
        main_progress_timer_.restart();

        // re-init numerical values
        averagingPeriodIndex = 0;
        totalAveragingPeriods = 0;
        processingTimeMSec = 0;
        meanProcessingTimeMSec = 0;
        elapsedTimeMSec = 0;
        estimatedTimeToCompletionMSec = 0;

        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        fromStr.clear();
        toStr.clear();
        previousTag.clear();

        inCycle = false;
        inPlanarFit_ = false;
        inTimeLag_ = false;
        currentFileList.clear();

#ifdef QT_DEBUG
        out << "file #, "
            << "procTimeMSec, " <<"meanProcTimeMSec, "
            << "elapsedMSec, " << "elapsedMSec_Str, "
            << "ETC, " << "ETC_Str";
        endl(out);
        out << "Executing EddyPro";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << processingTimeMSec << " "
            << meanProcessingTimeMSec << " "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    if (cleanLine.contains(QByteArray("Reading EddyPro project file")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Retrieving file")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("names from directory")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Retrieving timestamps")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("from file names")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Arranging raw files")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("in chronological order")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating master time series")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        out << "Creating master time series";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << "x "
            << "x "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    // end preamble

    // start planar fit
    if (cleanLine.contains(QByteArray("Performing planar-fit assessment")))
    {
        inPlanarFit_ = true;
        progressLabel_->setText(tr("Performing planar-fit assessment..."));

        // re-init
        averagingPeriodIndex = 0;
        totalAveragingPeriods = 0;
        processingTimeMSec = 0;
        meanProcessingTimeMSec = 0;
        elapsedTimeMSec = 0;
        estimatedTimeToCompletionMSec = 0;
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fromStr.clear();
        toStr.clear();

        main_progress_timer_.restart(); // restart to measure planar fit run time

        return;
    }
    // valid for both planar fit and time lag
    if (cleanLine.contains(QByteArray("Maximum number of flux averaging periods available")))
    {
        QString numStr = QLatin1String(cleanLine.trimmed().split(':').last().trimmed().constData());
        qDebug() << "numStr: " << numStr;
        totalAveragingPeriods = numStr.toInt();
        main_progress_bar->setMaximum(totalAveragingPeriods);
        return;
    }
    if (cleanLine.contains(QByteArray("Importing wind data for")))
    {
        QString dateStr = QLatin1String(cleanLine.mid(25).constData());
        currentPlanarFitDate = QDate::fromString(dateStr.trimmed(),
                                                 QStringLiteral("dd MMMM yyyy"));
        qDebug() << "currentPlanarFitDate: " << currentPlanarFitDate;

//        main_progress_bar->setValue(++progressValue_);
//        qDebug() << "progressValue_" << progressValue_;

        fileProgressLabel_->setText(QStringLiteral("Importing wind data"));
        return;
    }
    if (cleanLine.contains(QByteArray("another small step to the planar-fit")))
    {
        auto timeStr = QLatin1String(cleanLine.trimmed().split(' ')
                                     .last().trimmed().constData());
        qDebug() << "timeStr: " << timeStr;

        auto currentPlanarFitTime = QTime::fromString(timeStr, QStringLiteral("hh:mm"));
        qDebug() << "currentPlanarFitTime: " << currentPlanarFitTime;

        QDateTime fromDate(currentPlanarFitDate, currentPlanarFitTime
                           .addSecs(-ecProject_->screenAvrgLen() * 60));
        fromStr = fromDate.toString(Qt::ISODate).replace(QLatin1String("T"), QLatin1String(" "));
        QDateTime toDate(currentPlanarFitDate, currentPlanarFitTime);
        toStr = toDate.toString(Qt::ISODate);
        qDebug() << "fromStr: " << fromStr;
        qDebug() << "toStr: " << toStr;

        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr)
                            .arg(toStr));

        ++averagingPeriodIndex;

        mini_progress_bar_->setValue(0);
        mini_progress_bar_->reset();
        main_progress_bar->setValue(++progressValue_);

        // ETC computation
        current_step_elapsed_time = main_progress_timer_.elapsed();
        processingTimeMSec = current_step_elapsed_time - previous_elapsed_time;

        estimatedTimeToCompletionMSec = updateETC(&meanProcessingTimeMSec, processingTimeMSec,
                                                  averagingPeriodIndex, totalAveragingPeriods);

        auto estimatedTimeToCompletionMSecStr = QTime(0, 0)
                                .addMSecs(estimatedTimeToCompletionMSec)
                                .toString(QStringLiteral("hh:mm:ss.zzz"));
#ifdef QT_DEBUG
        out << "Planar-fit ETC computation";
        endl(out);
        out << "averagingPeriodIndex " << averagingPeriodIndex << " totalRuns " << totalAveragingPeriods;
        endl(out);
        out << "current_step_elapsed_time " << current_step_elapsed_time;
        out << " processingTimeMSec " << processingTimeMSec;
        out << " previous_elapsed_time " << previous_elapsed_time;
        endl(out);
        out << "estimatedTimeToCompletionMSec " << estimatedTimeToCompletionMSec;
        out << " estimatedTimeToCompletionMSecStr " << estimatedTimeToCompletionMSecStr;
        endl(out);
#endif
        previous_elapsed_time = current_step_elapsed_time;

        timeEstimateLabels_->setText(timeEstimateLabels_->text()
                                    .replace(61, 12, estimatedTimeToCompletionMSecStr));
        return;
    }
    if (cleanLine.contains(QByteArray("Sorting wind data by sector")))
    {
        fileProgressLabel_->setText(QStringLiteral("Sorting wind data by sector"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating planar fit rotation matrices")))
    {
        fileProgressLabel_->setText(QStringLiteral("Calculating planar fit rotation matrices"));
        return;
    }
    if (cleanLine.contains(QByteArray("Planar Fit session terminated")))
    {
        inPlanarFit_ = false;
        main_progress_bar->setValue(main_progress_bar->maximum());
        qDebug() << "progressValue_" << progressValue_;
        previous_elapsed_time = 0;
        return;
    }
    // end planar fit

    // start time lag
    if (cleanLine.contains(QByteArray("Performing time-lag optimization")))
    {
        inTimeLag_ = true;
        progressLabel_->setText(tr("Performing time-lag optimization..."));

        // re-init
        averagingPeriodIndex = 0;
        totalAveragingPeriods = 0;
        processingTimeMSec = 0;
        meanProcessingTimeMSec = 0;
        elapsedTimeMSec = 0;
        estimatedTimeToCompletionMSec = 0;
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fromStr.clear();
        toStr.clear();

        main_progress_timer_.restart(); // restart to measure time lag run time

        return;
    }
    if (cleanLine.contains(QByteArray("Importing data for")))
    {
        QString dateStr = QLatin1String(cleanLine.mid(21).constData());
        currentTimeLagDate = QDate::fromString(dateStr.trimmed(),
                                                 QStringLiteral("dd MMMM yyyy"));
        qDebug() << "currentTimeLagDate: " << currentTimeLagDate;

//        main_progress_bar->setValue(++progressValue_);
//        qDebug() << "progressValue_" << progressValue_;

        fileProgressLabel_->setText(QStringLiteral("Importing data"));
        return;
    }
    if (cleanLine.contains(QByteArray("another small step to the time-lag")))
    {
        auto timeStr = QLatin1String(cleanLine.trimmed().split(' ')
                                     .last().trimmed().constData());
        qDebug() << "timeStr: " << timeStr;

        auto currentTimeLagTime = QTime::fromString(timeStr, QStringLiteral("hh:mm"));
        qDebug() << "currentTimeLagTime: " << currentTimeLagTime;

        QDateTime fromDate(currentTimeLagDate, currentTimeLagTime
                      .addSecs(-ecProject_->screenAvrgLen() * 60));
        fromStr = fromDate.toString(Qt::ISODate).replace(QLatin1String("T"), QLatin1String(" "));
        QDateTime toDate(currentTimeLagDate, currentTimeLagTime);
        toStr = toDate.toString(Qt::ISODate);
        qDebug() << "fromStr: " << fromStr;
        qDebug() << "toStr: " << toStr;

        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr)
                            .arg(toStr));

        ++averagingPeriodIndex;

        mini_progress_bar_->setValue(0);
        mini_progress_bar_->reset();
        main_progress_bar->setValue(++progressValue_);

        // ETC computation
        current_step_elapsed_time = main_progress_timer_.elapsed();
        processingTimeMSec = current_step_elapsed_time - previous_elapsed_time;

        estimatedTimeToCompletionMSec = updateETC(&meanProcessingTimeMSec, processingTimeMSec,
                                                  averagingPeriodIndex, totalAveragingPeriods);

        auto estimatedTimeToCompletionMSecStr = QTime(0, 0)
                            .addMSecs(estimatedTimeToCompletionMSec)
                            .toString(QStringLiteral("hh:mm:ss.zzz"));
#ifdef QT_DEBUG
        out << "Time-lag ETC computation";
        endl(out);
        out << "averagingPeriodIndex " << averagingPeriodIndex << " totalRuns " << totalAveragingPeriods;
        endl(out);
        out << "current_step_elapsed_time " << current_step_elapsed_time;
        out << " processingTimeMSec " << processingTimeMSec;
        out << " previous_elapsed_time " << previous_elapsed_time;
        endl(out);
        out << "estimatedTimeToCompletionMSec " << estimatedTimeToCompletionMSec;
        out << " estimatedTimeToCompletionMSecStr " << estimatedTimeToCompletionMSecStr;
        endl(out);
#endif
        previous_elapsed_time = current_step_elapsed_time;

        timeEstimateLabels_->setText(timeEstimateLabels_->text()
                                    .replace(61, 12, estimatedTimeToCompletionMSecStr));
        return;
    }
    if (cleanLine.contains(QByteArray("Time lag optimization session terminated")))
    {
        inTimeLag_ = false;
        main_progress_bar->setValue(main_progress_bar->maximum());
        qDebug() << "progressValue_" << progressValue_;
        previous_elapsed_time = 0;
        return;
    }
    // end time lag

    // start raw data processing
    if (cleanLine.contains(QByteArray("Start raw data processing")))
    {
        inCycle = true;
        progressLabel_->setText(tr("Processing raw data..."));

        main_progress_timer_.restart(); // restart to measure main cycle run time

        // re-init
        averagingPeriodIndex = 0;
        totalAveragingPeriods = 0;
        processingTimeMSec = 0;
        meanProcessingTimeMSec = 0;
        elapsedTimeMSec = 0;
        estimatedTimeToCompletionMSec = 0;
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fromStr.clear();
        toStr.clear();

#ifdef QT_DEBUG
        out << "Start raw data processing";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << "x "
            << "x "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    if (cleanLine.contains(QByteArray("From:")))
    {
        fromStr = QLatin1String(cleanLine.mid(7, 16).constData());
        qDebug() << "fromStr: " << fromStr;
        return;
    }
    if (cleanLine.contains(QByteArray("To:")))
    {
        toStr = QLatin1String(cleanLine.mid(7, 16).constData());
        qDebug() << "toStr: " << toStr;
        return;
    }
    if (cleanLine.contains(QByteArray("Total number of flux averaging periods")))
    {
        totalAveragingPeriods = cleanLine.trimmed().trimmed().split(':').last().trimmed().toInt();
        qDebug() << "totalRuns" << totalAveragingPeriods;
        main_progress_bar->setMaximum(totalAveragingPeriods * 8 + 1);
        return;
    }
    // start processing cycle
    if (cleanLine.contains(QByteArray("processing new flux averaging period")))
    {
        ++averagingPeriodIndex;
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        out << ">> Processing new flux averaging period";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << "x "
            << "x "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif

        currentFileList.clear();

        return;
    }

    if (cleanLine.contains(QByteArray("File(s): ..")))
    {
        qDebug() << "fromToStr: " << fromStr << toStr;
        qDebug() << "averagingPeriodIndex: " << averagingPeriodIndex;
        mini_progress_bar_->setValue(1);
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr)
                            .arg(toStr));
        fileProgressLabel_->setText(tr("Parsing file"));

        currentFileList.append(QLatin1String(cleanLine.trimmed().split('\\')
                                             .last().trimmed().constData()));
        fileListLabel_->setText(QString(QStringLiteral("File(s): %1"))
                                .arg(currentFileList.join(QLatin1Char('\n'))));
        return;
    }

    if (cleanLine.contains((QByteArray("Skipping to next averaging period"))))
    {
        qDebug() << "fromToStr: " << fromStr << toStr;
        qDebug() << "averagingPeriodIndex: " << averagingPeriodIndex;
        main_progress_bar->setValue(++progressValue_);
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr)
                            .arg(toStr));
#ifdef QT_DEBUG
        out << "Skipping to next averaging period";
        endl(out);
#endif
        return;
    }

    if (cleanLine.contains(QByteArray("..\\")))
    {
        currentFileList.append(QLatin1String(cleanLine.trimmed().split('\\')
                                             .last().trimmed().constData()));
        return;
    }

    if (cleanLine.contains(QByteArray("Number of samples")))
    {
        qDebug() << "progressValue_" << progressValue_;
        mini_progress_bar_->setValue(2);

        fileListLabel_->setText(QString(QStringLiteral("File(s): %1"))
                                .arg(currentFileList.join(QLatin1Char('\n'))));
        return;
    }

    if (cleanLine.contains(QByteArray("Calculating statistics..")))
    {
        mini_progress_bar_->setValue(3);
        fileProgressLabel_->setText(tr("Calculating statistics"));

        return;
    }
    if (cleanLine.contains(QByteArray("Raw level statistical screening")))
    {
        mini_progress_bar_->setValue(4);
        fileProgressLabel_->setText(tr("Raw level statistical screening"));
        return;
    }
    if (cleanLine.contains(QByteArray("Spike detection/removal test")))
    {
        mini_progress_bar_->setValue(5);
        fileProgressLabel_->setText(tr("Spike detection/removal"));
        return;
    }
    if (cleanLine.contains(QByteArray("Spike detection/removal test")))
    {
        mini_progress_bar_->setValue(6);
        fileProgressLabel_->setText(tr("Spike detection/removal test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Absolute limits test")))
    {
        mini_progress_bar_->setValue(7);
        fileProgressLabel_->setText(tr("Absolute limits test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Skewness & kurtosis test")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        mini_progress_bar_->setValue(8);
        fileProgressLabel_->setText(tr("Skewness & kurtosis test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Despiking user set")))
    {
        mini_progress_bar_->setValue(9);
        fileProgressLabel_->setText(tr("Despiking user set"));
        return;
    }
    if (cleanLine.contains(QByteArray("Cross-wind correction")))
    {
        mini_progress_bar_->setValue(10);
        fileProgressLabel_->setText(tr("Cross-wind correction"));
        return;
    }
    if (cleanLine.contains(QByteArray("Converting into mixing ratio")))
    {
        mini_progress_bar_->setValue(11);
        fileProgressLabel_->setText(tr("Converting into mixing ratio"));
        return;
    }
    if (cleanLine.contains(QByteArray("Performing tilt correction")))
    {
        mini_progress_bar_->setValue(12);
        fileProgressLabel_->setText(tr("Performing tilt correction"));
        return;
    }
    if (cleanLine.contains(QByteArray("Compensating time lags")))
    {
        mini_progress_bar_->setValue(13);
        fileProgressLabel_->setText(tr("Compensating time lags"));
        return;
    }
    if (cleanLine.contains(QByteArray("Compensating user variables")))
    {
        mini_progress_bar_->setValue(14);
        fileProgressLabel_->setText(tr("Compensating user variables"));
        return;
    }
    if (cleanLine.contains(QByteArray("Performing stationarity test")))
    {
        mini_progress_bar_->setValue(15);
        fileProgressLabel_->setText(tr("Performing stationarity test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Detrending")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        mini_progress_bar_->setValue(16);
        fileProgressLabel_->setText(tr("Detrending"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating (co)spectra")))
    {
        mini_progress_bar_->setValue(17);
        fileProgressLabel_->setText(tr("Calculating (co)spectra"));
        return;
    }
    if (cleanLine.contains(QByteArray("Tapering timeseries")))
    {
        mini_progress_bar_->setValue(18);
        fileProgressLabel_->setText(tr("Tapering timeseries"));
        return;
    }
    if (cleanLine.contains(QByteArray("FFT-ing")))
    {
        mini_progress_bar_->setValue(19);
        fileProgressLabel_->setText(tr("FFT-ing"));
        return;
    }
    if (cleanLine.contains(QByteArray("Cospectral densities")))
    {
        mini_progress_bar_->setValue(20);
        fileProgressLabel_->setText(tr("Cospectral densities"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 0")))
    {
        mini_progress_bar_->setValue(21);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 0"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 1")))
    {
        mini_progress_bar_->setValue(22);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 1"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 2")))
    {
        mini_progress_bar_->setValue(23);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 2 and 3"));
        return;
    }
    if (cleanLine.contains(QByteArray("Estimating footprint")))
    {
        mini_progress_bar_->setValue(24);
        fileProgressLabel_->setText(tr("Estimating footprint"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating quality flags")))
    {
        mini_progress_bar_->setValue(mini_progress_bar_->maximum());
        fileProgressLabel_->setText(tr("Calculating quality flags"));
        return;
    }

    // end processing cycle
    if (cleanLine.contains(QByteArray("Flux averaging period processing time")))
    {
        // ETC computation
        auto procTimeString = QLatin1String(cleanLine.trimmed().split(' ')
                                            .last().trimmed().constData());
        qDebug() << "procTimeString" << procTimeString;
        auto procTime = QTime::fromString(procTimeString, QStringLiteral("h:mm:ss.zzz"));
        processingTimeMSec = QTime(0, 0).msecsTo(procTime);

        estimatedTimeToCompletionMSec = updateETC(&meanProcessingTimeMSec, processingTimeMSec,
                                                  averagingPeriodIndex, totalAveragingPeriods);

        auto estimatedTimeToCompletionMSecStr = QTime(0, 0)
                            .addMSecs(estimatedTimeToCompletionMSec)
                            .toString(QStringLiteral("hh:mm:ss.zzz"));

        timeEstimateLabels_->setText(timeEstimateLabels_->text()
                                    .replace(61, 12, estimatedTimeToCompletionMSecStr));

#ifdef QT_DEBUG
        out << "Flux averaging period processing time";
        endl(out);
        elapsedTimeMSec = averagingPeriodIndex * meanProcessingTimeMSec;
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(elapsedTimeMSec)
                                .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << processingTimeMSec << " "
            << meanProcessingTimeMSec << " "
            << elapsedTimeMSec << " "
            << elapsedTimeMSecStr << " "
            << estimatedTimeToCompletionMSec << " "
            << estimatedTimeToCompletionMSecStr;
        endl(out);
#endif

        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        mini_progress_bar_->reset();
        mini_progress_bar_->setValue(0);
        resetFileLabels();
        return;
    }
    // end raw data processing
    if (cleanLine.contains(QByteArray("Raw data processing terminated")))
    {
        inCycle = false;
        return;
    }

    // start flux computation
    if (cleanLine.contains(QByteArray("Starting flux computation and correction")))
    {
        progressLabel_->setText(tr("Starting flux computation and correction..."));
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Initializing retrieval of EddyPro-RP results")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("File found, importing content")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // end flux computation

    // start spectral corrections
    if (cleanLine.contains(QByteArray("Starting Spectral Assessment")))
    {
//        resetTimeEstimateLabels();
        progressLabel_->setText(tr("Performing spectral assessment..."));

        QDate dStart(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
        QDate dEnd(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        main_progress_bar->setMaximum(dStart.daysTo(dEnd) + 1);
        return;
    }
    if (cleanLine.contains(QByteArray("Importing binned spectra for")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Fitting model")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Sorting")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Spectral Assessment session terminated")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes for:")))
    {
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // start spectral corrections

    // start finalizing output files
    if (cleanLine.contains(QByteArray("Raw data processing terminated. "
                                      "Creating continuous datasets if necessary")))
    {
//        resetTimeEstimateLabels();
        progressLabel_->setText(tr("Finalizing output files..."));
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);

        int maxSteps = 0;

        if (ecProject_->generalRunMode() == Defs::CurrRunMode::Express)
        {
            maxSteps = 17;
        }
        else if (ecProject_->generalRunMode() == Defs::CurrRunMode::Advanced)
        {
            maxSteps = 2 + 2 * (ecProject_->screenOutSt1()
                                + ecProject_->screenOutSt2()
                                + ecProject_->screenOutSt3()
                                + ecProject_->screenOutSt4()
                                + ecProject_->screenOutSt5()
                                + ecProject_->screenOutSt6()
                                + ecProject_->screenOutSt7()
                                + ecProject_->screenOutRaw1()
                                + ecProject_->screenOutRaw2()
                                + ecProject_->screenOutRaw3()
                                + ecProject_->screenOutRaw4()
                                + ecProject_->screenOutRaw5()
                                + ecProject_->screenOutRaw6()
                                + ecProject_->screenOutRaw7()
                                )
                        + ecProject_->generalOutMd()
                        + ecProject_->screenOutDetails();
        }
        main_progress_bar->setMaximum(maxSteps);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        out << "Raw data processing terminated";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << "x "
            << "x "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Full Output dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating GHG-EUROPE-style dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Metadata dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Level")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Biomet dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Closing COMMON output files")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Closing RP output files")))
    {
        main_progress_bar->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        out << "Closing RP output files";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                    .toString(QStringLiteral("hh:mm:ss.zzz"));
        out << averagingPeriodIndex << " "
            << "x "
            << "x "
            << overall_progress_timer_.elapsed() << " "
            << elapsedTimeMSecStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    // end finalizing output files

    // engine run possible endings
    if (cleanLine.contains(QByteArray("gracefully")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());
        progressWidget_->stopAnimation();
        averagingPeriodIndex = 0;
        total_elapsed_update_timer_->stop();
        main_progress_timer_.invalidate();
        return;
    }

    if (cleanLine.contains(QByteArray("At line"))
        || cleanLine.contains(QByteArray("Fortran runtime error")))
    {
        cleanLine.prepend("Critical> ");
    }

    qDebug() << "cleanline" << cleanLine.trimmed().constData();
    if (cleanLine.contains(QByteArray("Alert"))
        || cleanLine.contains(QByteArray("Warning"))
        || cleanLine.contains(QByteArray("Error"))
        // runtime errors
        || cleanLine.contains(QByteArray("Critical"))
        || cleanLine.contains(QByteArray("Fatal error")))
    {
        // color the labels
        auto tag = cleanLine.mid(0, cleanLine.indexOf('>') + 1);
        qDebug() << "tag" << tag;

        auto htmlTag = tag;
        htmlTag.append(QByteArray("</font></b>"));
        qDebug() << "htmlTag" << htmlTag;

        if (cleanLine.contains(QByteArray("Alert"))
            && cleanLine.contains(QByteArray("Error")))
        {
            // blue
            cleanLine.replace(tag, htmlTag.prepend(QByteArray("<b><font color=\"#0066FF\">")));

            goto SKIP;
        }
        if (cleanLine.contains(QByteArray("Alert")))
        {
            // blue
            cleanLine.replace(tag, htmlTag.prepend(QByteArray("<b><font color=\"#0066FF\">")));
        }
        if (cleanLine.contains(QByteArray("Warning")))
        {
            // yellow
            cleanLine.replace(tag, htmlTag.prepend(QByteArray("<b><font color=\"#FFFF00\">")));
        }
        if (cleanLine.contains(QByteArray("Error")))
        {
            // orange
            cleanLine.replace(tag, htmlTag.prepend(QByteArray("<b><font color=\"#FF9900\">")));
        }
        if (cleanLine.contains(QByteArray("Critical"))
            || cleanLine.contains(QByteArray("Fatal error"))
            || cleanLine.contains(QByteArray("At line"))
            || cleanLine.contains(QByteArray("Fortran runtime error")))
        {
            // red
            cleanLine.replace(tag, htmlTag.prepend(QByteArray("<b><font color=\"#FF3300\">")));
            qDebug() << "replace tag" << tag;
        }
SKIP:
        // clear 'no error' string and append error msg
        if (errorEdit_->find(QStringLiteral("No errors")))
        {
            errorEdit_->clear();
        }

        qDebug() << "tags: current, previous" << tag << previousTag;
        if (tag != previousTag)
        {
            if (!previousTag.isEmpty())
            {
                auto avgPeriod = avgPeriodLabel_->text();
                if (avgPeriod.startsWith(tr("Averaging interval")))
                {
                    errorEdit_->append(QStringLiteral(" "));
                    errorEdit_->append(avgPeriodLabel_->text());
                    qDebug() << "avgPeriodLabel_->text()" << avgPeriodLabel_->text();
                    qDebug() << "append avgPeriod";
                }
            }

            previousTag = tag;
        }

        qDebug() << "tag" << tag;
        qDebug() << "cleanline to errorEdit" << cleanLine.trimmed().constData();
        errorEdit_->append(QLatin1String(cleanLine.trimmed().constData()));

        if (cleanLine.contains(QByteArray("Fatal"))
            || cleanLine.contains(QByteArray("aborted")))
        {
            stopRun();
            averagingPeriodIndex = 0;
        }

        return;
    }
}

void RunPage::runModeIconClicked()
{
    switch (runMode_)
    {
    case Defs::CurrRunStatus::Express:
        emit runExpRequest();
        break;
    case Defs::CurrRunStatus::Advanced_RP:
    case Defs::CurrRunStatus::Advanced_FCC:
        emit runAdvRequest();
        break;
    case Defs::CurrRunStatus::Retriever:
        emit runRetRequest();
        break;
    default:
        break;
    }
}

void RunPage::updateSmartfluxBar()
{
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}

void RunPage::updateElapsedTime()
{
    DEBUG_FUNC_NAME
    QString elapsedTimeStr = QTime(0, 0).addMSecs(overall_progress_timer_.elapsed())
                                        .toString(QStringLiteral("hh:mm:ss"));
    timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(20, 8, elapsedTimeStr));
}

// Update mini progress bar every second, scaling the speed of progress in respect of the
// standard 30 minutes. The progression is arbitrary.
void RunPage::updateMiniProgress()
{
    if (inPlanarFit_ or inTimeLag_)
    {
        // factor to scale speed of progression in respect of the standard 30 minutes
        int progressFactor = round(7 * 30 / ecProject_->screenAvrgLen());

        WidgetUtils::setProgressValue(mini_progress_bar_,
                                      mini_progress_bar_->value() + progressFactor);
    }
}

// Return ETC (Estimated Time to Completion) in msec and update the running average
// of the processing time using a Simple Moving Average
int RunPage::updateETC(int* mean_processing_time,
                       const int current_processing_time, const int index, const int num_steps)
{
    qDebug() << "mean_processing_time" << *mean_processing_time
             << "current_processing_time" << current_processing_time
             << "index" << index
             << "num_steps" << num_steps;

    Q_ASSERT(index != 0);
//    if (index == 0) ++index;

    if (index == 1)
    {
        *mean_processing_time = current_processing_time;
    }
    else
    {
        *mean_processing_time = (*mean_processing_time * (index - 1) + current_processing_time)
                                / index;
    }
    return (num_steps - index + 1) * (*mean_processing_time);
}

void RunPage::openOutputDir()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ecProject_->generalOutPath()));
}
