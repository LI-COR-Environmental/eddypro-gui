/***************************************************************************
  runpage.cpp
  -------------------
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

#include <cmath>

#include "clicklabel.h"
#include "dbghelper.h"
#include "ecproject.h"
#include "smartfluxbar.h"
#include "widget_utils.h"
#include "spinner.h"

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
    progressWidget_ = new Spinner;
    progressWidget_->setFixedSize(31, 31);
    progressWidget_->stop();
//    progressWidget_->setColor(QColor(46, 98, 152));

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
    main_progress_bar->setObjectName(QStringLiteral("mainProgress"));

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
    errorEdit_->setLineWrapMode(QTextEdit::NoWrap);

    open_output_dir = new QPushButton;
    open_output_dir->setText(tr("Open output directory"));
    open_output_dir->setProperty("commonButton2", true);
    open_output_dir->setMaximumWidth(200);
    open_output_dir->setStyleSheet(QStringLiteral("QPushButton {margin-left: 0px; }"));

    auto clearErrorEditButton = new QPushButton(tr("Clear"));
    clearErrorEditButton->setProperty("mdButton", true);
    clearErrorEditButton->setMaximumWidth(clearErrorEditButton->sizeHint().width());

    toviLabel = new ClickLabel;
    toviLabel->setProperty("toviLabel", true);
    toviLabel->setProperty("toviThinLabel", false);
    toviLabel->setStyleSheet(QStringLiteral("ClickLabel {margin-left: -15px; }"));
    connect(toviLabel, &ClickLabel::clicked,
            this, &RunPage::openToviHomepage);

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
    progressLayout->addWidget(clearErrorEditButton, 9, 2, Qt::AlignRight);
    progressLayout->addWidget(open_output_dir, 10, 1);
    progressLayout->addWidget(toviLabel, 11, 1, Qt::AlignLeft);
    progressLayout->setColumnStretch(2, 2);
    progressLayout->setRowStretch(8, 2);
    progressLayout->setRowStretch(12, 2);
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

    setLayout(mainLayout);

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

    auto open_output_lambda = [=]() {
        open_output_dir->setDisabled(ecProject_->generalOutPath().isEmpty());
    };

    connect(ecProject_, &EcProject::ecProjectNew,
            this, open_output_lambda);

    connect(ecProject_, &EcProject::ecProjectModified,
            this, open_output_lambda);

    connect(ecProject_, &EcProject::ecProjectChanged,
            this, open_output_lambda);

    connect(clearErrorEditButton, &QPushButton::clicked,
            errorEdit_, &QTextEdit::clear);

    // other init
    QList<WidgetUtils::PropertyList> progressBarProp;
    progressBarProp << WidgetUtils::PropertyList("expRun", false)
                    << WidgetUtils::PropertyList("advRun", false)
                    << WidgetUtils::PropertyList("retRun", false);
    WidgetUtils::updatePropertyListAndStyle(main_progress_bar, progressBarProp);
    WidgetUtils::updatePropertyListAndStyle(mini_progress_bar_, progressBarProp);
}

RunPage::~RunPage()
{
    delete pauseResumeDelayTimer_;
}

void RunPage::startRun(Defs::CurrRunStatus mode)
{
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

            break;
        }
//    default:
//        break;
    }

    WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
    WidgetUtils::updatePropertyListAndStyle(main_progress_bar, progressBarProp);
    WidgetUtils::updatePropertyListAndStyle(mini_progress_bar_, progressBarProp);

    runModeIcon_->setVisible(true);
    progressLabel_->setText(progressText);
    progressWidget_->start();
}

bool RunPage::pauseRun(Defs::CurrRunStatus mode)
{
    if (!pauseResumeDelayTimer_->isActive())
    {
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
//    default:
//        break;
        }

        WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Pausing computations..."));
        progressWidget_->stop();
        total_elapsed_update_timer_->stop();
        main_progress_timer_.invalidate();
        QTimer::singleShot(1000, this, SLOT(pauseLabel()));
        return true;
    }
    return false;
}

bool RunPage::resumeRun(Defs::CurrRunStatus mode)
{
    if (!pauseResumeDelayTimer_->isActive())
    {
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
//    default:
//        break;
        }

        WidgetUtils::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Resuming computations..."));
        progressWidget_->start();
        total_elapsed_update_timer_->start();
        main_progress_timer_.restart();
        QTimer::singleShot(1000, this, SLOT(resumeLabel()));
        return true;
    }
    return false;
}

void RunPage::stopRun()
{
    progressWidget_->stop();
    resetBuffer();
    resetProgressHard();
    total_elapsed_update_timer_->stop();
    main_progress_timer_.invalidate();
}

void RunPage::pauseLabel()
{
    pauseResumeDelayTimer_->stop();
    pauseResumeLabel_->setText(tr("Computations paused"));
}

void RunPage::resumeLabel()
{
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
    main_progress_bar->setValue(main_progress_bar->maximum());
    progressLabel_->setText(tr("Done"));
}

void RunPage::resetFileLabels()
{
    avgPeriodLabel_->clear();
    fileListLabel_->clear();
    fileProgressLabel_->clear();
}

void RunPage::resetTimeEstimateLabels()
{
    // commented for debug only
//    timeEstimateLabels_->clear();
}

void RunPage::resetLabels()
{
    progressLabel_->clear();
    resetFileLabels();
    resetTimeEstimateLabels();
}

bool RunPage::filterData(const QByteArray& data)
{
    if (inPlanarFit_ or inTimeLag_)
    {
        return data.contains(QByteArrayLiteral("another small step"));
    }
    return false;
}

void RunPage::bufferData(QByteArray &data)
{
    rxBuffer_.append(data);
    QByteArray line(rxBuffer_);
    QByteArrayList lineList(line.split('\n'));

    // newline found
    if (lineList.at(0) != rxBuffer_)
    {
        for (const auto& tempData : lineList)
        {
            data = cleanupEngineOutput(tempData);
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
    if (data.contains(QByteArrayLiteral("of file Z:")))
    {
        // cleanup file path in case of fortan runtime error
        data = data.mid(0, data.indexOf("Z:")) + data.mid(data.lastIndexOf("\\") + 1);
    }
    else if (data == QByteArrayLiteral(" ")
             || data.contains(QByteArrayLiteral("STOP"))
             || data.contains(QByteArrayLiteral("Note: The following floating-point exceptions")))
    {
        data = QByteArray();
    }

    return data;
}

void RunPage::parseEngineOutput(const QByteArray &data)
{
    QByteArray cleanLine;
    cleanLine.append(data);

    static int averagingPeriodIndex = 0;
    static int totalAveragingPeriods = 0;
    static int processingTimeMSec = 0;
    static int meanProcessingTimeMSec = 0;
    static int elapsedTimeMSec = 0;
    static int estimatedTimeToCompletionMSec = 0;

    static qint64 previous_elapsed_time = 0;
    qint64 current_step_elapsed_time = 0;

    static QString fromStr;
    static QString toStr;

#ifdef QT_DEBUG
    QString elapsedTimeMSecStr;
#endif

    QStringList currentFileList;

    static QDate currentPlanarFitDate;
    static QDate currentTimeLagDate;

#ifdef QT_DEBUG
    QFile outfile(QStringLiteral("parse-engine-output.txt"));
    outfile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream out(&outfile);
#endif

    // start preamble
    if (cleanLine.contains(QByteArrayLiteral("Executing EddyPro")))
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

        fromStr.clear();
        toStr.clear();

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
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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
    if (cleanLine.contains(QByteArrayLiteral("Reading EddyPro project file")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Retrieving file")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("names from directory")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Retrieving timestamps")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("from file names")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Arranging raw files")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("in chronological order")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Creating master time series")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());

#ifdef QT_DEBUG
        out << "Creating master time series";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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
    if (cleanLine.contains(QByteArrayLiteral("Performing planar-fit assessment")))
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
        fromStr.clear();
        toStr.clear();

        main_progress_timer_.restart(); // restart to measure planar fit run time

        errorEdit_->append(QStringLiteral("Performing planar-fit assessment"));

        return;
    }
    // valid for both planar fit and time lag
    if (cleanLine.contains(QByteArrayLiteral("Maximum number of flux averaging periods available")))
    {
        QString numStr = QLatin1String(cleanLine.trimmed().split(':').last().trimmed().constData());
        totalAveragingPeriods = numStr.toInt();
        main_progress_bar->setMaximum(totalAveragingPeriods);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Importing wind data for")))
    {
        QString dateStr = QLatin1String(cleanLine.mid(25).constData());
        currentPlanarFitDate = QDate::fromString(dateStr.trimmed(),
                                                 QStringLiteral("dd MMMM yyyy"));
        fileProgressLabel_->setText(QStringLiteral("Importing wind data"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("another small step to the planar-fit")))
    {
        auto timeStr = QLatin1String(cleanLine.trimmed().split(' ')
                                     .last().trimmed().constData());
        auto currentPlanarFitTime = QTime::fromString(timeStr, QStringLiteral("hh:mm"));
        QDateTime fromDate(currentPlanarFitDate, currentPlanarFitTime
                           .addSecs(-ecProject_->screenAvrgLen() * 60));
        fromStr = fromDate.toString(Qt::ISODate).replace(QLatin1String("T"), QLatin1String(" "));
        QDateTime toDate(currentPlanarFitDate, currentPlanarFitTime);
        toStr = toDate.toString(Qt::ISODate);
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr, toStr));
        auto avgPeriod = avgPeriodLabel_->text();
        errorEdit_->append(avgPeriod.prepend(QStringLiteral("<font color=\"#A6D7F2\">"))
                                    .append(QStringLiteral("</font>")));

        ++averagingPeriodIndex;

        mini_progress_bar_->setValue(0);
        mini_progress_bar_->reset();
        main_progress_bar->setValue(++progressValue_);

        // ETC computation
        current_step_elapsed_time = main_progress_timer_.elapsed();
        processingTimeMSec = static_cast<int>(current_step_elapsed_time - previous_elapsed_time);

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
    if (cleanLine.contains(QByteArrayLiteral("Sorting wind data by sector")))
    {
        fileProgressLabel_->setText(QStringLiteral("Sorting wind data by sector"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating planar fit rotation matrices")))
    {
        fileProgressLabel_->setText(QStringLiteral("Calculating planar fit rotation matrices"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Planar Fit session terminated")))
    {
        inPlanarFit_ = false;
        main_progress_bar->setValue(main_progress_bar->maximum());
        previous_elapsed_time = 0;
        return;
    }
    // end planar fit

    // start time lag
    if (cleanLine.contains(QByteArrayLiteral("Performing time-lag optimization")))
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
        fromStr.clear();
        toStr.clear();

        main_progress_timer_.restart(); // restart to measure time lag run time

        errorEdit_->append(QStringLiteral("Performing time-lag optimization"));

        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Importing data for")))
    {
        QString dateStr = QLatin1String(cleanLine.mid(21).constData());
        currentTimeLagDate = QDate::fromString(dateStr.trimmed(),
                                                 QStringLiteral("dd MMMM yyyy"));
        fileProgressLabel_->setText(QStringLiteral("Importing data"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("another small step to the time-lag")))
    {
        auto timeStr = QLatin1String(cleanLine.trimmed().split(' ')
                                     .last().trimmed().constData());
        auto currentTimeLagTime = QTime::fromString(timeStr, QStringLiteral("hh:mm"));

        QDateTime fromDate(currentTimeLagDate, currentTimeLagTime
                           .addSecs(-ecProject_->screenAvrgLen() * 60));
        fromStr = fromDate.toString(Qt::ISODate).replace(QLatin1String("T"), QLatin1String(" "));
        QDateTime toDate(currentTimeLagDate, currentTimeLagTime);
        toStr = toDate.toString(Qt::ISODate);

        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr, toStr));
        auto avgPeriod = avgPeriodLabel_->text();
        errorEdit_->append(avgPeriod.prepend(QStringLiteral("<font color=\"#A6D7F2\">"))
                                    .append(QStringLiteral("</font>")));

        ++averagingPeriodIndex;

        mini_progress_bar_->setValue(0);
        mini_progress_bar_->reset();
        main_progress_bar->setValue(++progressValue_);

        // ETC computation
        current_step_elapsed_time = main_progress_timer_.elapsed();
        processingTimeMSec = static_cast<int>(current_step_elapsed_time - previous_elapsed_time);

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
    if (cleanLine.contains(QByteArrayLiteral("Time lag optimization session terminated")))
    {
        inTimeLag_ = false;
        main_progress_bar->setValue(main_progress_bar->maximum());
        previous_elapsed_time = 0;
        return;
    }
    // end time lag

    // start raw data processing
    if (cleanLine.contains(QByteArrayLiteral("Start raw data processing")))
    {
        progressLabel_->setText(tr("Processing raw data..."));

        main_progress_timer_.restart(); // restart to measure main cycle run time

        errorEdit_->append(QStringLiteral("Start raw data processing"));

        // re-init
        averagingPeriodIndex = 0;
        totalAveragingPeriods = 0;
        processingTimeMSec = 0;
        meanProcessingTimeMSec = 0;
        elapsedTimeMSec = 0;
        estimatedTimeToCompletionMSec = 0;
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        fromStr.clear();
        toStr.clear();

#ifdef QT_DEBUG
        out << "Start raw data processing";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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
    if (cleanLine.contains(QByteArrayLiteral("From:")))
    {
        fromStr = QLatin1String(cleanLine.mid(7, 16).constData());
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("To:")))
    {
        toStr = QLatin1String(cleanLine.mid(7, 16).constData());
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr, toStr));
        auto avgPeriod = avgPeriodLabel_->text();
        errorEdit_->append(avgPeriod.prepend(QStringLiteral("<font color=\"#A6D7F2\">"))
                                    .append(QStringLiteral("</font>")));

        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Total number of flux averaging periods")))
    {
        totalAveragingPeriods = cleanLine.trimmed().trimmed().split(':').last().trimmed().toInt();
        main_progress_bar->setMaximum(totalAveragingPeriods * 8 + 1);
        return;
    }
    // start processing cycle
    if (cleanLine.contains(QByteArrayLiteral("processing new flux averaging period")))
    {
        ++averagingPeriodIndex;
        main_progress_bar->setValue(++progressValue_);

#ifdef QT_DEBUG
        out << ">> Processing new flux averaging period";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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

    if (cleanLine.contains(QByteArrayLiteral("File(s): ..")))
    {
        mini_progress_bar_->setValue(1);
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr, toStr));

        fileProgressLabel_->setText(tr("Parsing file"));

        currentFileList.append(QLatin1String(cleanLine.trimmed().split('\\')
                                             .last().trimmed().constData()));
        fileListLabel_->setText(QStringLiteral("File(s): %1")
                                .arg(currentFileList.join(QLatin1Char('\n'))));
        return;
    }

    if (cleanLine.contains((QByteArrayLiteral("Skipping to next averaging period"))))
    {
        main_progress_bar->setValue(++progressValue_);
        avgPeriodLabel_->setText(tr("Averaging interval, From: %1, To: %2")
                            .arg(fromStr, toStr));

#ifdef QT_DEBUG
        out << "Skipping to next averaging period";
        endl(out);
#endif
        return;
    }

    if (cleanLine.contains(QByteArrayLiteral("..\\")))
    {
        currentFileList.append(QLatin1String(cleanLine.trimmed().split('\\')
                                             .last().trimmed().constData()));
        return;
    }

    if (cleanLine.contains(QByteArrayLiteral("Number of samples")))
    {
        mini_progress_bar_->setValue(2);
        fileListLabel_->setText(QStringLiteral("File(s): %1")
                                .arg(currentFileList.join(QLatin1Char('\n'))));
        return;
    }

    if (cleanLine.contains(QByteArrayLiteral("Calculating statistics..")))
    {
        mini_progress_bar_->setValue(3);
        fileProgressLabel_->setText(tr("Calculating statistics"));

        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Raw level statistical screening")))
    {
        mini_progress_bar_->setValue(4);
        fileProgressLabel_->setText(tr("Raw level statistical screening"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Spike detection/removal test")))
    {
        mini_progress_bar_->setValue(5);
        fileProgressLabel_->setText(tr("Spike detection/removal"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Spike detection/removal test")))
    {
        mini_progress_bar_->setValue(6);
        fileProgressLabel_->setText(tr("Spike detection/removal test"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Absolute limits test")))
    {
        mini_progress_bar_->setValue(7);
        fileProgressLabel_->setText(tr("Absolute limits test"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Skewness & kurtosis test")))
    {
        main_progress_bar->setValue(++progressValue_);
        mini_progress_bar_->setValue(8);
        fileProgressLabel_->setText(tr("Skewness & kurtosis test"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Despiking user set")))
    {
        mini_progress_bar_->setValue(9);
        fileProgressLabel_->setText(tr("Despiking user set"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Cross-wind correction")))
    {
        mini_progress_bar_->setValue(10);
        fileProgressLabel_->setText(tr("Cross-wind correction"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Converting into mixing ratio")))
    {
        mini_progress_bar_->setValue(11);
        fileProgressLabel_->setText(tr("Converting into mixing ratio"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Performing tilt correction")))
    {
        mini_progress_bar_->setValue(12);
        fileProgressLabel_->setText(tr("Performing tilt correction"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Compensating time lags")))
    {
        mini_progress_bar_->setValue(13);
        fileProgressLabel_->setText(tr("Compensating time lags"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Compensating user variables")))
    {
        mini_progress_bar_->setValue(14);
        fileProgressLabel_->setText(tr("Compensating user variables"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Performing stationarity test")))
    {
        mini_progress_bar_->setValue(15);
        fileProgressLabel_->setText(tr("Performing stationarity test"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Detrending")))
    {
        main_progress_bar->setValue(++progressValue_);
        mini_progress_bar_->setValue(16);
        fileProgressLabel_->setText(tr("Detrending"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating (co)spectra")))
    {
        mini_progress_bar_->setValue(17);
        fileProgressLabel_->setText(tr("Calculating (co)spectra"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Tapering timeseries")))
    {
        mini_progress_bar_->setValue(18);
        fileProgressLabel_->setText(tr("Tapering timeseries"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("FFT-ing")))
    {
        mini_progress_bar_->setValue(19);
        fileProgressLabel_->setText(tr("FFT-ing"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Cospectral densities")))
    {
        mini_progress_bar_->setValue(20);
        fileProgressLabel_->setText(tr("Cospectral densities"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating fluxes Level 0")))
    {
        mini_progress_bar_->setValue(21);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 0"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating fluxes Level 1")))
    {
        mini_progress_bar_->setValue(22);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 1"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating fluxes Level 2")))
    {
        mini_progress_bar_->setValue(23);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 2 and 3"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Estimating footprint")))
    {
        mini_progress_bar_->setValue(24);
        fileProgressLabel_->setText(tr("Estimating footprint"));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating quality flags")))
    {
        mini_progress_bar_->setValue(mini_progress_bar_->maximum());
        fileProgressLabel_->setText(tr("Calculating quality flags"));
        return;
    }

    // end processing cycle
    if (cleanLine.contains(QByteArrayLiteral("Flux averaging period processing time")))
    {
        // ETC computation
        auto procTimeString = QLatin1String(cleanLine.trimmed().split(' ')
                                            .last().trimmed().constData());
        auto procTime = QTime::fromString(procTimeString, QStringLiteral("h:mm:ss.zzz"));
        processingTimeMSec = QTime(0, 0).msecsTo(procTime);

        // prevent division by zero
        if (averagingPeriodIndex == 0) ++averagingPeriodIndex;

        estimatedTimeToCompletionMSec = updateETC(&meanProcessingTimeMSec,
                                                  processingTimeMSec,
                                                  averagingPeriodIndex,
                                                  totalAveragingPeriods);

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

        mini_progress_bar_->reset();
        mini_progress_bar_->setValue(0);
        resetFileLabels();
        return;
    }
    // end raw data processing
    if (cleanLine.contains(QByteArrayLiteral("Raw data processing terminated")))
    {
        errorEdit_->append(QStringLiteral("Raw data processing terminated"));

        return;
    }
    // get essential file path
    if (cleanLine.contains(QByteArrayLiteral("Essentials file path:")))
    {
        auto ex_file_path_ = QString();
        QByteArrayList path_components = cleanLine.trimmed().split(':');

        // one semicolon only (mac)
        if (path_components.size() < 3)
        {
            ex_file_path_ = QLatin1String(path_components.last().trimmed().constData());
        }
        // two semicolons (windows)
        else
        {
            // preserve the drive letter
            auto full_path_ = path_components.mid(1).join(':');
            ex_file_path_ = QLatin1String(full_path_.trimmed().constData());
        }
        ecProject_->setSpectraExFile(ex_file_path_);
        return;
    }

    // start flux computation
    if (cleanLine.contains(QByteArrayLiteral("Starting flux computation and correction")))
    {
        progressLabel_->setText(tr("Starting flux computation and correction..."));
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Initializing retrieval of EddyPro-RP results")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("File found, importing content")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    // end flux computation

    // start spectral corrections
    if (cleanLine.contains(QByteArrayLiteral("Starting Spectral Assessment")))
    {
//        resetTimeEstimateLabels();
        progressLabel_->setText(tr("Performing spectral assessment..."));

        QDate dStart(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
        QDate dEnd(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        main_progress_bar->setMaximum(static_cast<int>(dStart.daysTo(dEnd)) + 1);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Importing binned (co)spectra for")))
    {
        main_progress_bar->setValue(++progressValue_);
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Fitting model")))
    {
        main_progress_bar->setValue(++progressValue_);
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Sorting")))
    {
        main_progress_bar->setValue(++progressValue_);
        avgPeriodLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Spectral Assessment session terminated")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Calculating fluxes for:")))
    {
        resetProgressSoft();
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    // start spectral corrections

    // start finalizing output files
    if (cleanLine.contains(QByteArrayLiteral("Raw data processing terminated. "
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

#ifdef QT_DEBUG
        out << "Raw data processing terminated";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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
    if (cleanLine.contains(QByteArrayLiteral("Creating Full Output dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Creating GHG-EUROPE-style dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Creating Metadata dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Creating Level")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Creating Biomet dataset")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Closing COMMON output files")))
    {
        main_progress_bar->setValue(++progressValue_);
        return;
    }
    if (cleanLine.contains(QByteArrayLiteral("Closing RP output files")))
    {
        main_progress_bar->setValue(++progressValue_);

#ifdef QT_DEBUG
        out << "Closing RP output files";
        endl(out);
        elapsedTimeMSecStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
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
    if (cleanLine.contains(QByteArrayLiteral("gracefully")))
    {
        main_progress_bar->setValue(main_progress_bar->maximum());
        progressWidget_->stop();
        averagingPeriodIndex = 0;
        total_elapsed_update_timer_->stop();
        main_progress_timer_.invalidate();
        return;
    }

    if (cleanLine.contains(QByteArrayLiteral("At line"))
        || cleanLine.contains(QByteArrayLiteral("Fortran runtime error")))
    {
        cleanLine.prepend("Critical> ");
    }

    if (cleanLine.contains(QByteArrayLiteral("Alert"))
        || cleanLine.contains(QByteArrayLiteral("Warning"))
        || cleanLine.contains(QByteArrayLiteral("Error"))
        // runtime errors
        || cleanLine.contains(QByteArrayLiteral("Critical"))
        || cleanLine.contains(QByteArrayLiteral("Fatal error")))
    {
        // color the labels
        auto tag = cleanLine.mid(0, cleanLine.indexOf('>') + 1);

        auto htmlTag = tag;
        htmlTag.append(QByteArrayLiteral("</font>"));

        if (cleanLine.contains(QByteArrayLiteral("Alert"))
            && cleanLine.contains(QByteArrayLiteral("Error")))
        {
            // blue
            cleanLine.replace(tag, htmlTag.prepend(QByteArrayLiteral("<font color=\"#0066FF\">")));

            goto SKIP;
        }
        if (cleanLine.contains(QByteArrayLiteral("Alert")))
        {
            // blue
            cleanLine.replace(tag, htmlTag.prepend(QByteArrayLiteral("<font color=\"#0066FF\">")));
        }
        if (cleanLine.contains(QByteArrayLiteral("Warning")))
        {
            if (cleanLine.contains(QByteArrayLiteral("(80)"))
                || cleanLine.contains(QByteArrayLiteral("(81)"))
                || cleanLine.contains(QByteArrayLiteral("(82)"))
                || cleanLine.contains(QByteArrayLiteral("(83)"))
                || cleanLine.contains(QByteArrayLiteral("(84)"))
                || cleanLine.contains(QByteArrayLiteral("(85)")))
            {
                cleanLine.clear();
            }

            // yellow
            cleanLine.replace(tag, htmlTag.prepend(QByteArrayLiteral("<font color=\"#FFFF00\">")));
        }
        if (cleanLine.contains(QByteArrayLiteral("Error")))
        {
            // orange
            cleanLine.replace(tag, htmlTag.prepend(QByteArrayLiteral("<font color=\"#FF9900\">")));
        }
        if (cleanLine.contains(QByteArrayLiteral("Critical"))
            || cleanLine.contains(QByteArrayLiteral("Fatal error"))
            || cleanLine.contains(QByteArrayLiteral("At line"))
            || cleanLine.contains(QByteArrayLiteral("Fortran runtime error")))
        {
            // red
            cleanLine.replace(tag, htmlTag.prepend(QByteArrayLiteral("<font color=\"#FF3300\">")));
        }
SKIP:
        QString clearedStr = QLatin1String(cleanLine.trimmed().constData());
        if (!clearedStr.isEmpty())
        {
            errorEdit_->append(clearedStr);
        }

        if (cleanLine.contains(QByteArrayLiteral("Fatal"))
            || cleanLine.contains(QByteArrayLiteral("aborted")))
        {
            stopRun();
            averagingPeriodIndex = 0;
        }

        return;
    }
}

void RunPage::runModeIconClicked()
{
    emit pauseRequest(runMode_);
}

void RunPage::updateSmartfluxBar()
{
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}

void RunPage::updateElapsedTime()
{
    QString elapsedTimeStr = QTime(0, 0).addMSecs(static_cast<int>(overall_progress_timer_.elapsed()))
                                        .toString(QStringLiteral("hh:mm:ss"));
    timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(20, 8, elapsedTimeStr));
}

// Update mini progress bar every second, scaling the speed of progress in respect of the
// standard 30 minutes. The progression is arbitrary.
void RunPage::updateMiniProgress()
{
    if (inPlanarFit_ or inTimeLag_)
    {
        // prevent division by zero in case of 'File as is' averaging period
        auto avrgPeriod = ecProject_->screenAvrgLen();
        if (avrgPeriod == 0) ++avrgPeriod;

        // factor to scale speed of progression in respect of the standard 30 minutes
        auto progressFactor = static_cast<int>(lround(7.0 * 30.0 / avrgPeriod));

        WidgetUtils::setProgressValue(mini_progress_bar_,
                                      mini_progress_bar_->value() + progressFactor);
    }
}

// Return ETC (Estimated Time to Completion) in msec and update the running average
// of the processing time using a Simple Moving Average
int RunPage::updateETC(int* mean_processing_time,
                       int current_processing_time,
                       int index,
                       int num_steps)
{
    Q_ASSERT(index != 0);

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

void RunPage::openToviHomepage()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://tovi.io/?utm_source=EddyPro%20Software&utm_medium=Tovi%20Ads&utm_campaign=EP_Tovi_ads")));
}

void RunPage::updateRunPage(bool small)
{
    QList<WidgetUtils::PropertyList> toviAdProp;

    if (small)
    {
        toviAdProp << WidgetUtils::PropertyList("toviLabel", false)
                   << WidgetUtils::PropertyList("toviThinLabel", true);
    }
    else
    {
        toviAdProp << WidgetUtils::PropertyList("toviLabel", true)
                   << WidgetUtils::PropertyList("toviThinLabel", false);
    }

    WidgetUtils::updatePropertyListAndStyle(toviLabel, toviAdProp);
}
