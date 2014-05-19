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

#include <QDebug>
#include <QGridLayout>
#include <QProgressBar>
#include <QTimer>
#include <QFile>
#include <QTextEdit>

#include "dbghelper.h"
#include "alia.h"
#include "clicklabel.h"
#include "QProgressIndicator.h"
#include "ecproject.h"
#include "smartfluxbar.h"
#include "runpage.h"

RunPage::RunPage(QWidget *parent, EcProject *ecProject, ConfigState* config)
    : QWidget(parent),
      runMode_(Defs::CurrRunStatusExp),
      ecProject_(ecProject),
      configState_(config),
      progressValue_(0),
      delayTimer_(0),
      elapsedTimer_(0),
      errorEdit_(0)
{
    progressWidget_ = new QProgressIndicator();
    progressWidget_->setAnimationDelay(40);
    progressWidget_->setDisplayedWhenStopped(false);
    progressWidget_->setFixedSize(30, 30);
    progressWidget_->setColor(QColor(46, 98, 152));

    runModeIcon_ = new ClickLabel();

    runModeLabel_ = new QLabel();
    runModeLabel_->setObjectName(QStringLiteral("welcomeLabel"));

    progressLabel_ = new QLabel();
    progressLabel_->setObjectName(QStringLiteral("welcomeLabel"));

    fileLabel_ = new QLabel();
    fileLabel_->setObjectName(QStringLiteral("welcomeLabel"));

    fileProgressLabel_ = new QLabel();
    fileProgressLabel_->setObjectName(QStringLiteral("welcomeLabel"));

    timeEstimateLabels_ = new QLabel();
    timeEstimateLabels_->setText(QStringLiteral("Elapsed time: 00:00:00 - Estimated time to completion: --:--:--"));
    timeEstimateLabels_->setObjectName(QStringLiteral("welcomeLabel"));

    pauseResumeLabel_ = new QLabel();
    pauseResumeLabel_->setObjectName(QStringLiteral("welcomeLabel"));

    progressBar_ = new QProgressBar();
    progressBar_->setMinimum(0);
    progressBar_->setMaximum(10);

    miniProgressBar_ = new QProgressBar();
    miniProgressBar_->setMinimum(0);
    miniProgressBar_->setMaximum(25);
    miniProgressBar_->setObjectName(QStringLiteral("miniProgress"));
    miniProgressBar_->setTextVisible(false);

    delayTimer_ = new QTimer(this);
    delayTimer_->setInterval(1500);
    delayTimer_->setSingleShot(true);

    elapsedTimer_ = new QTimer(this);
    elapsedTimer_->setInterval(1000);
    elapsedTimer_->setTimerType(Qt::CoarseTimer);

    errorEdit_ = new QTextEdit;
    errorEdit_->setReadOnly(true);
    errorEdit_->setCursorWidth(0);
    errorEdit_->setText(QStringLiteral("No errors"));

    QGridLayout *progressLayout = new QGridLayout();
    progressLayout->addWidget(runModeIcon_, 0, 0, 1, 1);
    progressLayout->addWidget(runModeLabel_, 0, 1);
    progressLayout->addWidget(progressWidget_, 1, 0, 1, 1, Qt::AlignCenter);
    progressLayout->addWidget(progressLabel_, 1, 1);
    progressLayout->addWidget(timeEstimateLabels_, 2, 1);
    progressLayout->addWidget(progressBar_, 3, 1, 1, -1);
    progressLayout->addWidget(fileLabel_, 4, 1);
    progressLayout->addWidget(miniProgressBar_, 5, 1, 1, -1);
    progressLayout->addWidget(fileProgressLabel_, 6, 1, Qt::AlignTop);
    progressLayout->addWidget(errorEdit_, 7, 1, 1, 2);
    progressLayout->addWidget(pauseResumeLabel_, 8, 1);
    progressLayout->setColumnStretch(2, 2);
    progressLayout->setRowStretch(9, 2);
    progressLayout->setRowMinimumHeight(0, 42);   // = runModeIcon_.width()
    progressLayout->setColumnMinimumWidth(0, 42); // > runModeIcon_.height()
    progressLayout->setHorizontalSpacing(6);

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);
    smartfluxBar_->setVisible(false);
    smartfluxBar_->setToolTip(tr("EddyPro is in SMARTFlux configuration mode (Ctrl+F to exit)"));

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(smartfluxBar_, 0, 0);
    mainLayout->addLayout(progressLayout, 1, 0);
    mainLayout->setContentsMargins(20, 15, 30, 0);
    mainLayout->setRowStretch(1, 0);
    setLayout(mainLayout);

    connect(runModeIcon_, SIGNAL(clicked()),
            this, SLOT(runModeIconClicked()));

    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));

    connect(elapsedTimer_, SIGNAL(timeout()), this, SLOT(updateElapsedTime()));

    QList<Alia::PropertyList> progressBarProp;
    progressBarProp << Alia::PropertyList("expRun", false)
                    << Alia::PropertyList("advRun", false)
                    << Alia::PropertyList("retRun", false);
    Alia::updatePropertyListAndStyle(miniProgressBar_, progressBarProp);
}

RunPage::~RunPage()
{
    if (delayTimer_)
        delete delayTimer_;
}

void RunPage::startRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    QList<Alia::PropertyList> iconModeProp;
    QList<Alia::PropertyList> progressBarProp;

    runMode_ = mode;
    switch (runMode_)
    {
        case Defs::CurrRunStatusExp:
        {
            iconModeProp << Alia::PropertyList("pauseExpButton", true)
                         << Alia::PropertyList("pauseAdvButton", false)
                         << Alia::PropertyList("pauseRetButton", false)
                         << Alia::PropertyList("resumeExpButton", false)
                         << Alia::PropertyList("resumeAdvButton", false)
                         << Alia::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running in Express mode"));

            progressBarProp << Alia::PropertyList("expRun", true)
                            << Alia::PropertyList("advRun", false)
                            << Alia::PropertyList("retRun", false);
            break;
        }
        case Defs::CurrRunStatusAdv:
        {
            iconModeProp << Alia::PropertyList("pauseExpButton", false)
                         << Alia::PropertyList("pauseAdvButton", true)
                         << Alia::PropertyList("pauseRetButton", false)
                         << Alia::PropertyList("resumeExpButton", false)
                         << Alia::PropertyList("resumeAdvButton", false)
                         << Alia::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running in Advanced mode"));

            progressBarProp << Alia::PropertyList("expRun", false)
                            << Alia::PropertyList("advRun", true)
                            << Alia::PropertyList("retRun", false);
            break;
        }
        case Defs::CurrRunStatusRet:
        {
            iconModeProp << Alia::PropertyList("pauseExpButton", false)
                         << Alia::PropertyList("pauseAdvButton", false)
                         << Alia::PropertyList("pauseRetButton", true)
                         << Alia::PropertyList("resumeExpButton", false)
                         << Alia::PropertyList("resumeAdvButton", false)
                         << Alia::PropertyList("resumeRetButton", false);

            runModeLabel_->setText(tr("Running Metadata Retriever"));

            progressBarProp << Alia::PropertyList("expRun", false)
                            << Alia::PropertyList("advRun", false)
                            << Alia::PropertyList("retRun", true);
            break;
        }
        default:
            break;
    }

    Alia::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
    Alia::updatePropertyListAndStyle(progressBar_, progressBarProp);
    Alia::updatePropertyListAndStyle(miniProgressBar_, progressBarProp);

    runModeIcon_->setVisible(true);
    progressLabel_->setText(tr("Initializing raw data processing..."));
    progressWidget_->startAnimation();
    errorEdit_->clear();
    errorEdit_->setText(QStringLiteral("No errors"));
}

bool RunPage::pauseRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    if (!delayTimer_->isActive())
    {
        qDebug() << "delayTimer_->isActive()" << delayTimer_->isActive();

        QList<Alia::PropertyList> iconModeProp;
        delayTimer_->start();
        runMode_ = mode;
        switch (runMode_)
        {
            case Defs::CurrRunStatusExp:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", false)
                             << Alia::PropertyList("pauseAdvButton", false)
                             << Alia::PropertyList("pauseRetButton", false)
                             << Alia::PropertyList("resumeExpButton", true)
                             << Alia::PropertyList("resumeAdvButton", false)
                             << Alia::PropertyList("resumeRetButton", false);
                break;
            }
            case Defs::CurrRunStatusAdv:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", false)
                             << Alia::PropertyList("pauseAdvButton", false)
                             << Alia::PropertyList("pauseRetButton", false)
                             << Alia::PropertyList("resumeExpButton", false)
                             << Alia::PropertyList("resumeAdvButton", true)
                             << Alia::PropertyList("resumeRetButton", false);
                break;
            }
            case Defs::CurrRunStatusRet:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", false)
                             << Alia::PropertyList("pauseAdvButton", false)
                             << Alia::PropertyList("pauseRetButton", false)
                             << Alia::PropertyList("resumeExpButton", false)
                             << Alia::PropertyList("resumeAdvButton", false)
                             << Alia::PropertyList("resumeRetButton", true);
                break;
            }
            default:
                break;
        }

        Alia::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Pausing computations..."));
        progressWidget_->setDisplayedWhenStopped(true);
        progressWidget_->stopAnimation();
        elapsedTimer_->stop();
        QTimer::singleShot(1000, this, SLOT(pauseLabel()));
        return true;
    }
    else
    {
        qDebug() << "delayTimer_->isActive()" << delayTimer_->isActive();
        return false;
    }
}

bool RunPage::resumeRun(Defs::CurrRunStatus mode)
{
    DEBUG_FUNC_NAME

    if (!delayTimer_->isActive())
    {
        qDebug() << "delayTimer_->isActive()" << delayTimer_->isActive();

        QList<Alia::PropertyList> iconModeProp;
        delayTimer_->start();
        runMode_ = mode;
        switch (runMode_)
        {
            case Defs::CurrRunStatusExp:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", true)
                             << Alia::PropertyList("pauseAdvButton", false)
                             << Alia::PropertyList("pauseRetButton", false)
                             << Alia::PropertyList("resumeExpButton", false)
                             << Alia::PropertyList("resumeAdvButton", false)
                             << Alia::PropertyList("resumeRetButton", false);
                break;
            }
            case Defs::CurrRunStatusAdv:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", false)
                             << Alia::PropertyList("pauseAdvButton", true)
                             << Alia::PropertyList("pauseRetButton", false)
                             << Alia::PropertyList("resumeExpButton", false)
                             << Alia::PropertyList("resumeAdvButton", false)
                             << Alia::PropertyList("resumeRetButton", false);
                break;
            }
            case Defs::CurrRunStatusRet:
            {
                iconModeProp << Alia::PropertyList("pauseExpButton", false)
                             << Alia::PropertyList("pauseAdvButton", false)
                             << Alia::PropertyList("pauseRetButton", true)
                             << Alia::PropertyList("resumeExpButton", false)
                             << Alia::PropertyList("resumeAdvButton", false)
                             << Alia::PropertyList("resumeRetButton", false);
                break;
            }
            default:
                break;
        }

        Alia::updatePropertyListAndStyle(runModeIcon_, iconModeProp);
        runModeIcon_->setVisible(true);
        pauseResumeLabel_->setText(tr("Resuming computations..."));
        progressWidget_->setDisplayedWhenStopped(true);
        progressWidget_->startAnimation();
        elapsedTimer_->start();
        QTimer::singleShot(1000, this, SLOT(resumeLabel()));
        return true;
    }
    else
    {
        qDebug() << "delayTimer_->isActive()" << delayTimer_->isActive();
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
    elapsedTimer_->stop();
}

void RunPage::pauseLabel()
{
    DEBUG_FUNC_NAME
    delayTimer_->stop();
    pauseResumeLabel_->setText(tr("Computations paused"));
}

void RunPage::resumeLabel()
{
    DEBUG_FUNC_NAME
    delayTimer_->stop();
    pauseResumeLabel_->clear();
}

void RunPage::resetBuffer()
{
    rxBuffer_.resize(0);
}

void RunPage::resetProgressSoft()
{
    progressValue_ = 0;
    progressBar_->setValue(progressValue_);
    progressBar_->setMaximum(10);
    progressBar_->reset();
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
    miniProgressBar_->setValue(0);
    miniProgressBar_->reset();
}

void RunPage::doneLabel()
{
    DEBUG_FUNC_NAME
    progressBar_->setValue(progressBar_->maximum());
    progressLabel_->setText(tr("Done"));
}

void RunPage::resetFileLabels()
{
    DEBUG_FUNC_NAME
    fileLabel_->clear();
    fileProgressLabel_->clear();
}

void RunPage::resetTimeEstimateLabels()
{
    DEBUG_FUNC_NAME
    timeEstimateLabels_->clear();
}

void RunPage::resetLabels()
{
    DEBUG_FUNC_NAME
    progressLabel_->clear();
    resetFileLabels();
    resetTimeEstimateLabels();
}

void RunPage::bufferData(QByteArray &data)
{
    rxBuffer_.append(data);
    QByteArray line(rxBuffer_);
    QList<QByteArray> lineList(line.split('\n'));

    // newline found
    if (lineList.at(0) != rxBuffer_)
    {
        for (int i = 0; i < lineList.size(); ++i)
        {
            qDebug() << "lineList.at(i)" << lineList.at(i);
            QByteArray tempData(lineList.at(i));
            data = cleanupEngineOutput(tempData);
            qDebug() << "data after cleanup" << data;
            if (!data.isEmpty())
            {
                parseEngineOutput(data);
                emit updateConsoleRequest(data);
            }
        }

        if (lineList.last().endsWith('\n'))
            resetBuffer();
        else
            rxBuffer_ = lineList.last();
    }
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
    QByteArray cleanLine;
    cleanLine.append(data);

    static int fileIndex = 0;
    static int totalRuns = 0;

    static int processingTimeSec = 0;
    static int meanProcessingTimeSec = 0;
    static int elapsedTimeSec = 0;
    static int estimatedTimeToCompletionSec = 0;

    static QString fromStr;
    static QString toStr;

    QString elapsedTimeStr; // just for debug

#ifdef QT_DEBUG
    QFile outfile(QStringLiteral("time-output.txt"));
    outfile.open(QFile::WriteOnly | QFile::Append);
    QTextStream out(&outfile);
#endif

    // start preamble
    if (cleanLine.contains(QByteArray("Executing EddyPro")))
    {
        overallTime_.restart();
        elapsedTimer_->start();
        timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(14, 8, QStringLiteral("00:00:00")));
        timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(55, 8, QStringLiteral("--:--:--")));

        // re-init numerical values
        fileIndex = 0;
        totalRuns = 0;
        processingTimeSec = 0;
        meanProcessingTimeSec = 0;
        elapsedTimeSec = 0;
        estimatedTimeToCompletionSec = 0;

        resetProgressSoft();
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        out << "# " << "meanProcTime " << "elapsed " << "elapsed-str " << "ETC " << "ETC-str";
        endl(out);
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));

        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    if (cleanLine.contains(QByteArray("Reading EddyPro project file")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Retrieving file")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("names from directory")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Retrieving timestamps")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("from file names")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Arranging raw files")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("in chronological order")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating master time series")))
    {
        progressBar_->setValue(progressBar_->maximum());
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
            << "x "
            << "x ";
#endif
        endl(out);
        return;
    }
    // end preamble

    // start planar fit
    if (cleanLine.contains(QByteArray("Starting Planar Fit")))
    {
        progressLabel_->setText(tr("Performing planar fit assessment..."));
        resetProgressSoft();
        return;
    }
    if (cleanLine.contains(QByteArray("Importing wind data")))
    {
        if (ecProject_->planarFitSubset() == 1)
        {
            QDate dStart(QDate::fromString(ecProject_->planarFitStartDate(), Qt::ISODate));
            QDate dEnd(QDate::fromString(ecProject_->planarFitEndDate(), Qt::ISODate));
            progressBar_->setMaximum(dStart.daysTo(dEnd) * 48);
            qDebug() << "dStart.daysTo(dEnd) + 3" << dStart.daysTo(dEnd) * 48;
        }
        else
        {
            progressBar_->setMaximum(progressBar_->maximum() + 48);
        }
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fileLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Verifying time series integrity")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating planar fit rotation")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Planar Fit session terminated")))
    {
        progressBar_->setValue(progressBar_->maximum());
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // end planar fit

    // start time lag
    if (cleanLine.contains(QByteArray("Starting time lag optimization session")))
    {
        progressLabel_->setText(tr("Performing time lag optimization..."));
        resetProgressSoft();
        return;
    }
    if (cleanLine.contains(QByteArray("Optimizing time lags")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Time lag optimization session terminated")))
    {
        progressBar_->setValue(progressBar_->maximum());
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // end time lag

    // start raw data processing
    if (cleanLine.contains(QByteArray("Start raw data processing")))
    {
        progressLabel_->setText(tr("Processing raw data..."));
        resetProgressSoft();

#ifdef QT_DEBUG
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
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
    if (cleanLine.contains(QByteArray("Total number of flux averaging periods: ")))
    {
        totalRuns = cleanLine.split(' ').at(8).trimmed().toInt();
        qDebug() << "totalRuns" << totalRuns;
        progressBar_->setMaximum(totalRuns * 8 + 1);
        return;
    }
    // start processing cycle
    if (cleanLine.contains(QByteArray("processing new flux averaging period")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }

    if (cleanLine.contains(QByteArray("File(s): ..\\")))
    {
        qDebug() << "fromToStr: " << fromStr << toStr;
        ++fileIndex;
        miniProgressBar_->setValue(1);
        fileLabel_->setText(tr("File: %1 - From: %2  To: %3")
                            .arg(QLatin1String(cleanLine.split('\\').at(1).trimmed().constData()))
                            .arg(fromStr)
                            .arg(toStr));
        fileProgressLabel_->setText(tr("Parsing file"));
        return;
    }
    if (cleanLine.contains(QByteArray("Number of samples")))
    {
        qDebug() << "progressValue_" << progressValue_;
        miniProgressBar_->setValue(2);
        return;
    }

    if (cleanLine.contains(QByteArray("Calculating statistics..")))
    {
        miniProgressBar_->setValue(3);
        fileProgressLabel_->setText(tr("Calculating statistics"));

        return;
    }
    if (cleanLine.contains(QByteArray("Raw level statistical screening")))
    {
        miniProgressBar_->setValue(4);
        fileProgressLabel_->setText(tr("Raw level statistical screening"));
        return;
    }
    if (cleanLine.contains(QByteArray("Spike detection/removal test")))
    {
        miniProgressBar_->setValue(5);
        fileProgressLabel_->setText(tr("Spike detection/removal"));
        return;
    }
    if (cleanLine.contains(QByteArray("Spike detection/removal test")))
    {
        miniProgressBar_->setValue(6);
        fileProgressLabel_->setText(tr("Spike detection/removal test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Absolute limits test")))
    {
        miniProgressBar_->setValue(7);
        fileProgressLabel_->setText(tr("Absolute limits test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Skewness & kurtosis test")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        miniProgressBar_->setValue(8);
        fileProgressLabel_->setText(tr("Skewness & kurtosis test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Despiking user set")))
    {
        miniProgressBar_->setValue(9);
        fileProgressLabel_->setText(tr("Despiking user set"));
        return;
    }
    if (cleanLine.contains(QByteArray("Cross-wind correction")))
    {
        miniProgressBar_->setValue(10);
        fileProgressLabel_->setText(tr("Cross-wind correction"));
        return;
    }
    if (cleanLine.contains(QByteArray("Converting into mixing ratio")))
    {
        miniProgressBar_->setValue(11);
        fileProgressLabel_->setText(tr("Converting into mixing ratio"));
        return;
    }
    if (cleanLine.contains(QByteArray("Performing tilt correction")))
    {
        miniProgressBar_->setValue(12);
        fileProgressLabel_->setText(tr("Performing tilt correction"));
        return;
    }
    if (cleanLine.contains(QByteArray("Compensating time lags")))
    {
        miniProgressBar_->setValue(13);
        fileProgressLabel_->setText(tr("Compensating time lags"));
        return;
    }
    if (cleanLine.contains(QByteArray("Compensating user variables")))
    {
        miniProgressBar_->setValue(14);
        fileProgressLabel_->setText(tr("Compensating user variables"));
        return;
    }
    if (cleanLine.contains(QByteArray("Performing stationarity test")))
    {
        miniProgressBar_->setValue(15);
        fileProgressLabel_->setText(tr("Performing stationarity test"));
        return;
    }
    if (cleanLine.contains(QByteArray("Detrending")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        miniProgressBar_->setValue(16);
        fileProgressLabel_->setText(tr("Detrending"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating (co)spectra")))
    {
        miniProgressBar_->setValue(17);
        fileProgressLabel_->setText(tr("Calculating (co)spectra"));
        return;
    }
    if (cleanLine.contains(QByteArray("Tapering timeseries")))
    {
        miniProgressBar_->setValue(18);
        fileProgressLabel_->setText(tr("Tapering timeseries"));
        return;
    }
    if (cleanLine.contains(QByteArray("FFT-ing")))
    {
        miniProgressBar_->setValue(19);
        fileProgressLabel_->setText(tr("FFT-ing"));
        return;
    }
    if (cleanLine.contains(QByteArray("Cospectral densities")))
    {
        miniProgressBar_->setValue(20);
        fileProgressLabel_->setText(tr("Cospectral densities"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 0")))
    {
        miniProgressBar_->setValue(21);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 0"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 1")))
    {
        miniProgressBar_->setValue(22);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 1"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes Level 2")))
    {
        miniProgressBar_->setValue(23);
        fileProgressLabel_->setText(tr("Calculating fluxes Level 2 and 3"));
        return;
    }
    if (cleanLine.contains(QByteArray("Estimating footprint")))
    {
        miniProgressBar_->setValue(24);
        fileProgressLabel_->setText(tr("Estimating footprint"));
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating quality flags")))
    {
        miniProgressBar_->setValue(miniProgressBar_->maximum());
        fileProgressLabel_->setText(tr("Calculating quality flags"));
        return;
    }

    // end processing cycle
    if (cleanLine.contains(QByteArray("Flux averaging period processing time")))
    {
        // ETC computation
        QString procTimeString = QLatin1String(cleanLine.split(' ').at(7).trimmed().constData());
        QTime procTime = QTime::fromString(procTimeString, QStringLiteral("h:mm:ss.zzz"));
        qDebug() << "procTime" << procTime << QTime(0, 0).secsTo(procTime);
        processingTimeSec = QTime(0, 0).secsTo(procTime);

        if (fileIndex == 1)
        {
            meanProcessingTimeSec = processingTimeSec;
        }
        else
        {
            meanProcessingTimeSec = (meanProcessingTimeSec * (fileIndex - 1)
                                     + processingTimeSec) / fileIndex;
        }
        estimatedTimeToCompletionSec = (totalRuns - fileIndex) * meanProcessingTimeSec;
        QString estimatedTimeToCompletionStr = QTime(0, 0)
                                .addSecs(estimatedTimeToCompletionSec)
                                .toString(QStringLiteral("hh:mm:ss"));

        timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(55, 8, estimatedTimeToCompletionStr));

#ifdef QT_DEBUG
        elapsedTimeSec = fileIndex * meanProcessingTimeSec;
        elapsedTimeStr = QTime(0, 0).addSecs(elapsedTimeSec)
                                .toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeSec << " "
            << elapsedTimeStr << " "
            << estimatedTimeToCompletionSec << " "
            << estimatedTimeToCompletionStr;
        endl(out);
#endif

        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

        miniProgressBar_->reset();
        miniProgressBar_->setValue(0);
        resetFileLabels();
        return;
    }

    // start flux computation
    if (cleanLine.contains(QByteArray("Starting flux computation and correction")))
    {
        progressLabel_->setText(tr("Starting flux computation and correction..."));
        resetProgressSoft();
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Initializing retrieval of EddyPro-RP results")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("File found, importing content")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // end flux computation

    // start spectral corrections
    if (cleanLine.contains(QByteArray("Starting Spectral Assessment")))
    {
        progressLabel_->setText(tr("Performing spectral assessment..."));

        QDate dStart(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
        QDate dEnd(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
        resetProgressSoft();
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        progressBar_->setMaximum(dStart.daysTo(dEnd) + 1);
        return;
    }
    if (cleanLine.contains(QByteArray("Importing")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fileLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Fitting model")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fileLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Sorting")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        fileLabel_->setText(QLatin1String(cleanLine.trimmed().constData()));
        return;
    }
    if (cleanLine.contains(QByteArray("Spectral Assessment session terminated")))
    {
        progressBar_->setValue(progressBar_->maximum());
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Calculating fluxes for:")))
    {
        resetProgressSoft();
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    // start spectral corrections

    // start finalizing output files
    if (cleanLine.contains(QByteArray("Raw data processing terminated. Creating continuous datasets if necessary")))
    {
        progressLabel_->setText(tr("Finalizing output files..."));
        resetProgressSoft();
        progressBar_->setValue(++progressValue_);

        int maxSteps = 0;

        if (ecProject_->generalRunMode() != Defs::CurrRunModeAdv)
        {
            maxSteps = 17;
        }
        else if (ecProject_->generalRunMode() == Defs::CurrRunModeAdv)
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
        progressBar_->setMaximum(maxSteps);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
            << "x "
            << "x ";
        endl(out);
#endif
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Full Output dataset")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating GHG-EUROPE-style dataset")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Metadata dataset")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Level")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Creating Biomet dataset")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Closing COMMON output files")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;
        return;
    }
    if (cleanLine.contains(QByteArray("Closing RP output files")))
    {
        progressBar_->setValue(++progressValue_);
        qDebug() << "progressValue_" << progressValue_;

#ifdef QT_DEBUG
        elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
        out << fileIndex << " "
            << meanProcessingTimeSec << " "
            << overallTime_.elapsed() / 1000 << " "
            << elapsedTimeStr << " "
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
        progressBar_->setValue(progressBar_->maximum());
        progressWidget_->stopAnimation();
        fileIndex = 0;
        elapsedTimer_->stop();
        return;
    }
    if (cleanLine.contains(QByteArray("error"))
        || cleanLine.contains(QByteArray("Error"))
        || cleanLine.contains(QByteArray("Invalid"))
        || cleanLine.contains(QByteArray("invalid"))
        || cleanLine.contains(QByteArray("corrupted"))
        || cleanLine.contains(QByteArray("warning"))
        || cleanLine.contains(QByteArray("Warning"))
        || cleanLine.contains(QByteArray("not created"))
        || cleanLine.contains(QByteArray("no files"))
        || cleanLine.contains(QByteArray("not found"))
        || cleanLine.contains(QByteArray("not able"))
        || cleanLine.contains(QByteArray("not included"))
        || cleanLine.contains(QByteArray("continues"))
        || cleanLine.contains(QByteArray("skipped"))
        || cleanLine.contains(QByteArray("switching"))
        || cleanLine.contains(QByteArray("Switching"))
        || cleanLine.contains(QByteArray("switched"))
        || cleanLine.contains(QByteArray("skipping"))
        || cleanLine.contains(QByteArray("Skipping"))
        || cleanLine.contains(QByteArray("will"))
        || cleanLine.contains(QByteArray("without"))
        || cleanLine.contains(QByteArray("check"))
        || cleanLine.contains(QByteArray("again"))
        || cleanLine.contains(QByteArray("set to -9999"))
        || cleanLine.contains(QByteArray("Forcing"))
        || cleanLine.contains(QByteArray("At line"))
        || cleanLine.contains(QByteArray("of file"))
        || cleanLine.contains(QByteArray("Fortran"))
        || cleanLine.contains(QByteArray("runtime"))
        || cleanLine.contains(QByteArray("aborted")))
    {
        if (errorEdit_->find(QStringLiteral("No errors")))
        {
            errorEdit_->clear();
        }
        errorEdit_->append(QLatin1String(cleanLine.trimmed().constData()));

        if (cleanLine.contains(QByteArray("Fatal"))
            || cleanLine.contains(QByteArray("aborted")))
        {
            stopRun();
            fileIndex = 0;
        }

        return;
    }
}

void RunPage::runModeIconClicked()
{
    switch (runMode_)
    {
        case Defs::CurrRunStatusExp:
            emit runExpRequest();
            break;
        case Defs::CurrRunStatusAdv:
            emit runAdvRequest();
            break;
        case Defs::CurrRunStatusRet:
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
    QString elapsedTimeStr = QTime(0, 0).addMSecs(overallTime_.elapsed()).toString(QStringLiteral("hh:mm:ss"));
    timeEstimateLabels_->setText(timeEstimateLabels_->text().replace(14, 8, elapsedTimeStr));
}
