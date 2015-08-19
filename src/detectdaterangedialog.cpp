/***************************************************************************
  detectdaterangedialog.cpp
  -------------------
  Copyright (C) 2015, LI-COR Biosciences
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

#include "detectdaterangedialog.h"

#include <QDebug>
#include <QCheckBox>
#include <QDateEdit>
#include <QDesktopServices>
#include <QFuture>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimeEdit>
#include <QTimer>
#include <QtConcurrent>

#include "clicklabel.h"
#include "dbghelper.h"
#include "defs.h"
#include "ecproject.h"
#include "dlproject.h"
#include "stringutils.h"
#include "widget_utils.h"

DetectDateRangeDialog::DetectDateRangeDialog(QWidget *parent, EcProject *ecProject, DlProject *dlProject) :
    QDialog(parent),
    ecProject_(ecProject),
    dlProject_(dlProject),
    isRangeOverlapping_(false)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Detect dates"));
    WidgetUtils::removeContextHelpButton(this);

    auto groupTitle = new QLabel;
    groupTitle->setText(tr("Checking date range of the dataset and it's subsets..."));

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QLatin1String("hrLabel"));
    hrLabel->setMinimumWidth(400);

//    msgLabel = new QLabel(tr("Detect and compare..."));
//    msgLabel->setStyleSheet(QStringLiteral("QLabel { margin-bottom: 15px; }"));

    setAsCurrentRangeButton = new QPushButton(tr("Set all to the available range"));
    setAsCurrentRangeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAsCurrentRangeButton->setProperty("commonButton2", true);
    setAsCurrentRangeButton->setStyleSheet(QStringLiteral("QPushButton { margin-bottom: 10px; }"));

    okButton = new QPushButton(tr("Ok"));
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okButton->setProperty("commonButton", true);
    okButton->setDefault(true);

    dialogLayout = new QGridLayout(this);
    dialogLayout->addWidget(groupTitle, 0, 0, 1, -1);
    dialogLayout->addWidget(hrLabel, 1, 0, 1, -1);
//    dialogLayout->addWidget(msgLabel, 2, 0, 1, -1);
    dialogLayout->addWidget(setAsCurrentRangeButton, 7, 0, 1, 2, Qt::AlignCenter);
    dialogLayout->addWidget(okButton, 8, 0, 1, 2, Qt::AlignCenter);
    dialogLayout->setVerticalSpacing(10);
    dialogLayout->setContentsMargins(30, 30, 0, 30);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(dialogLayout);

    connect(setAsCurrentRangeButton, &QPushButton::clicked,
            this, &DetectDateRangeDialog::setToAvailableDates);
    connect(okButton, &QPushButton::clicked,
            [=](){ this->close(); this->setResult(QDialog::Accepted);});

//    QTimer::singleShot(0, this, SLOT(initialize()));
}

DetectDateRangeDialog::~DetectDateRangeDialog()
{
    DEBUG_FUNC_NAME
}

// void DetectDateRangeDialog::setLabel(const QString &label)
// {
//     msgLabel->setText(label);
// }

void DetectDateRangeDialog::createDateSelectionWidget(DateRangeType type,
                                                      FileUtils::DateRange subrange,
                                                      DateRangeOverlapping overlap)
{
#if 0
    auto typeStr = QString();
    switch (type) {
    case DateRangeType::RawData:
        typeStr = QStringLiteral("rd");
        break;
    case DateRangeType::PlanarFit:
        typeStr = QStringLiteral("pf");
        break;
    case DateRangeType::TimeLag:
        typeStr = QStringLiteral("tl");
        break;
    case DateRangeType::Spectra:
        typeStr = QStringLiteral("sd");
        break;
    default:
        break;
    }

    DEBUG_FUNC_MSG(tr("Type Subset: %1\n"
                      "Start: %2\n"
                      "End: %3\n\n").arg(typeStr)
                                    .arg(subrange.first.toString(Qt::ISODate))
                                    .arg(subrange.second.toString(Qt::ISODate)));
#endif

    auto rangeTypeLabel = new QLabel;

    auto overlapStr = QString();
    if (overlap == DateRangeOverlapping::Overlap)
    {
        overlapStr = QStringLiteral("<font color=\"#C0C0C0\">________________________________________</font>");
    }
    else
    {
        overlapStr = tr("<font color=\"#FF3300\">Not overlapping with available dataset</font>");
    }

    auto overlapLabel = new QLabel;
    overlapLabel->setText(overlapStr);

    auto lockedIcon = new QLabel;
    auto pixmap = QPixmap(QStringLiteral(":/icons/vlink-locked"));
#if defined(Q_OS_MAC)
    pixmap.setDevicePixelRatio(2.0);
#endif
    lockedIcon->setPixmap(pixmap);

    auto startDateLabel = new ClickLabel;
    startDateLabel->setText(tr("Start :"));
    startDateLabel->setToolTip(tr("<b>Start:</b> Starting date of the dataset to process. "
                                  "This may or may not coincide with the date of the first "
                                  "raw file. It is used to select a  of available raw data for processing."));
    startDateLabel->setStyleSheet(QStringLiteral("ClickLabel { margin-left: 30px; }"));

    auto startDateEdit = new QDateEdit;
    startDateEdit->setToolTip(startDateLabel->toolTip());
    startDateEdit->setCalendarPopup(true);
    WidgetUtils::customizeCalendar(startDateEdit->calendarWidget());
    startDateEdit->setDate(subrange.first.date());

    auto startTimeEdit = new QTimeEdit;
    startTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    startTimeEdit->setAccelerated(true);
    startTimeEdit->setTime(subrange.first.time());

    auto startDateContainer = new QHBoxLayout;
    startDateContainer->addWidget(startDateEdit);
    startDateContainer->insertSpacing(1, 10);
    startDateContainer->addWidget(startTimeEdit);
    startDateContainer->addStretch();
    startDateContainer->setContentsMargins(0, 0, 0, 0);

    auto endDateLabel = new ClickLabel;
    endDateLabel->setText(tr("End :"));
    endDateLabel->setToolTip(tr("<b>End:</b> Ending date of the dataset to process. "
                                "This may or may not coincide with the date of the last "
                                "raw file. It is used to select a subset of available raw data for processing."));
    auto endDateEdit = new QDateEdit;
    endDateEdit->setToolTip(endDateLabel->toolTip());
    endDateEdit->setCalendarPopup(true);
    WidgetUtils::customizeCalendar(endDateEdit->calendarWidget());
    endDateEdit->setDate(subrange.second.date());

    auto endTimeEdit = new QTimeEdit;
    endTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    endTimeEdit->setAccelerated(true);
    endTimeEdit->setTime(subrange.second.time());

    auto endDateContainer = new QHBoxLayout;
    endDateContainer->addWidget(endDateEdit);
    endDateContainer->insertSpacing(1, 10);
    endDateContainer->addWidget(endTimeEdit);
    endDateContainer->addStretch();
    endDateContainer->setContentsMargins(0, 0, 0, 0);

    auto subsetCheckBox = new QCheckBox;
    subsetCheckBox->setText(tr("Select a different period"));
    subsetCheckBox->setToolTip(tr("<b>Select a different period:</b> Select this option "
                                  "if you only want to process a subset of data in the "
                                  "raw data directory. Leave it blank to process all the "
                                  "raw data in the directory."));
    subsetCheckBox->setStyleSheet(QStringLiteral("QCheckBox {margin-left: 13px}"));

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(rangeTypeLabel, 0, 0);
    gridLayout->addWidget(overlapLabel, 0, 1, 1, 4, Qt::AlignRight);
    gridLayout->addWidget(subsetCheckBox, 1, 0);
    gridLayout->addWidget(startDateLabel, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(lockedIcon, 1, 2, 2, 1, Qt::AlignCenter);
    gridLayout->addLayout(startDateContainer, 1, 3, 1, 2);
    gridLayout->addWidget(endDateLabel, 2, 1, Qt::AlignRight);
    gridLayout->addLayout(endDateContainer, 2, 3, 1, 2);
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 0);
    gridLayout->setColumnStretch(3, 1);
    gridLayout->setColumnStretch(4, 1);
    gridLayout->setColumnStretch(5, 2);
    gridLayout->setVerticalSpacing(3);
    gridLayout->setContentsMargins(0, 0, 27, 0);

    switch (type) {
        case DateRangeType::RawData:
            startDateEdit->setObjectName(QStringLiteral("rawStartDate"));
            startTimeEdit->setObjectName(QStringLiteral("rawStartTime"));
            endDateEdit->setObjectName(QStringLiteral("rawEndDate"));
            endTimeEdit->setObjectName(QStringLiteral("rawEndTime"));

            rangeTypeLabel->setText(QStringLiteral("<b>Raw data</b>"));
            subsetCheckBox->setChecked(ecProject_->generalSubset());
            dialogLayout->addLayout(gridLayout, 3, 0);

            connect(subsetCheckBox, &QCheckBox::toggled, [=](bool checked)
                { ecProject_->setGeneralSubset(checked);
                  emit refreshRdRequest(); });

            connect(startDateLabel, &ClickLabel::clicked, [=]()
                { startDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(startDateEdit); });
            connect(startDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setGeneralStartDate(d.toString(Qt::ISODate));
                  endDateEdit->setMinimumDate(d);
                  updateOverlap(overlapLabel, d, startTimeEdit->time(),
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshRdRequest(); });

            connect(endDateLabel, &ClickLabel::clicked, [=]()
                { endDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(endDateEdit); });
            connect(endDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setGeneralEndDate(d.toString(Qt::ISODate));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     d, endTimeEdit->time());
                  emit alignDeclinationRequest(d);
                  emit refreshRdRequest(); });

            connect(startTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setGeneralStartTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), t,
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshRdRequest(); });
            connect(endTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setGeneralEndTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     endDateEdit->date(), t);
                  emit refreshRdRequest(); });
            break;
        case DateRangeType::PlanarFit:
            startDateEdit->setObjectName(QStringLiteral("pfStartDate"));
            startTimeEdit->setObjectName(QStringLiteral("pfStartTime"));
            endDateEdit->setObjectName(QStringLiteral("pfEndDate"));
            endTimeEdit->setObjectName(QStringLiteral("pfEndTime"));

            rangeTypeLabel->setText(QStringLiteral("<b>Planar fit</b>"));
            subsetCheckBox->setChecked(ecProject_->planarFitSubset());
            dialogLayout->addLayout(gridLayout, 4, 0);

            connect(subsetCheckBox, &QCheckBox::toggled, [=](bool checked)
                { ecProject_->setPlanarFitSubset(checked);
                  emit refreshPfRequest(); });

            connect(startDateLabel, &ClickLabel::clicked, [=]()
                { startDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(startDateEdit); });
            connect(startDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setPlanarFitStartDate(d.toString(Qt::ISODate));
                  endDateEdit->setMinimumDate(d);
                  updateOverlap(overlapLabel, d, startTimeEdit->time(),
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshPfRequest(); });

            connect(endDateLabel, &ClickLabel::clicked, [=]()
                { endDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(endDateEdit); });
            connect(endDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setPlanarFitEndDate(d.toString(Qt::ISODate));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     d, endTimeEdit->time());
                  emit refreshPfRequest(); });

            connect(startTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setPlanarFitStartTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), t,
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshPfRequest(); });
            connect(endTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setPlanarFitEndTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     endDateEdit->date(), t);
                  emit refreshPfRequest(); });
            break;
        case DateRangeType::TimeLag:
            startDateEdit->setObjectName(QStringLiteral("tlStartDate"));
            startTimeEdit->setObjectName(QStringLiteral("tlStartTime"));
            endDateEdit->setObjectName(QStringLiteral("tlEndDate"));
            endTimeEdit->setObjectName(QStringLiteral("tlEndTime"));

            rangeTypeLabel->setText(QStringLiteral("<b>Time lag</b>"));
            subsetCheckBox->setChecked(ecProject_->timelagOptSubset());
            dialogLayout->addLayout(gridLayout, 5, 0);

            connect(subsetCheckBox, &QCheckBox::toggled, [=](bool checked)
                { ecProject_->setTimelagOptSubset(checked);
                  emit refreshTlRequest(); });

            connect(startDateLabel, &ClickLabel::clicked, [=]()
                { startDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(startDateEdit); });
            connect(startDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setTimelagOptStartDate(d.toString(Qt::ISODate));
                  endDateEdit->setMinimumDate(d);
                  updateOverlap(overlapLabel, d, startTimeEdit->time(),
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshTlRequest(); });

            connect(endDateLabel, &ClickLabel::clicked, [=]()
                { endDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(endDateEdit); });
            connect(endDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setTimelagOptEndDate(d.toString(Qt::ISODate));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     d, endTimeEdit->time());
                  emit refreshTlRequest(); });

            connect(startTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setTimelagOptStartTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), t,
                                     endDateEdit->date(), endTimeEdit->time());
                  emit refreshTlRequest(); });
            connect(endTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setTimelagOptEndTime(t.toString(QStringLiteral("hh:mm")));
                  updateOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                     endDateEdit->date(), t);
                  emit refreshTlRequest(); });
            break;
        case DateRangeType::Spectra:
            startDateEdit->setObjectName(QStringLiteral("spStartDate"));
            startTimeEdit->setObjectName(QStringLiteral("spStartTime"));
            endDateEdit->setObjectName(QStringLiteral("spEndDate"));
            endTimeEdit->setObjectName(QStringLiteral("spEndTime"));

            rangeTypeLabel->setText(QStringLiteral("<b>Spectra</b>"));
            subsetCheckBox->setChecked(ecProject_->spectraSubset());
            dialogLayout->addLayout(gridLayout, 6, 0);

            connect(subsetCheckBox, &QCheckBox::toggled, [=](bool checked)
                { ecProject_->setSpectraSubset(checked);
                  emit refreshSpRequest(); });

            connect(startDateLabel, &ClickLabel::clicked, [=]()
                { startDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(startDateEdit); });
            connect(startDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setSpectraStartDate(d.toString(Qt::ISODate));
                  endDateEdit->setMinimumDate(d);
                  updateSpectraOverlap(overlapLabel, d, startTimeEdit->time(),
                                       endDateEdit->date(), endTimeEdit->time());
                  emit refreshSpRequest(); });

            connect(endDateLabel, &ClickLabel::clicked, [=]()
                { endDateEdit->setFocus();
                  WidgetUtils::showCalendarOf(endDateEdit); });
            connect(endDateEdit, &QDateEdit::dateChanged, [=](const QDate& d)
                { ecProject_->setSpectraEndDate(d.toString(Qt::ISODate));
                  updateSpectraOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                       d, endTimeEdit->time());
                  emit refreshSpRequest(); });

            connect(startTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setSpectraStartTime(t.toString(QStringLiteral("hh:mm")));
                  updateSpectraOverlap(overlapLabel, startDateEdit->date(), t,
                                       endDateEdit->date(), endTimeEdit->time());
                  emit refreshSpRequest(); });
            connect(endTimeEdit, &QTimeEdit::timeChanged, [=](const QTime& t)
                { ecProject_->setSpectraEndTime(t.toString(QStringLiteral("hh:mm")));
                  updateSpectraOverlap(overlapLabel, startDateEdit->date(), startTimeEdit->time(),
                                       endDateEdit->date(), t);
                  emit refreshSpRequest(); });
            break;
    }
}

void DetectDateRangeDialog::createCurrentRange()
{
    auto highlightColor = QStringLiteral("{ background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
                           "stop:0 white, stop:1 #67c2ee); }");

    auto currentRangeLabel = new QLabel;
    currentRangeLabel->setText(tr("<b>Available dataset</b>"));

    auto startDateLabel = new ClickLabel;
    startDateLabel->setText(tr("Start :"));
    startDateLabel->setToolTip(tr("<b>Start:</b> Starting date of the dataset to process. "
                                  "This may or may not coincide with the date of the first "
                                  "raw file. It is used to select a  of available raw data for processing."));
    auto startDateEdit = new QDateEdit;
    startDateEdit->setToolTip(startDateLabel->toolTip());
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(availableDataRange_.first.date());
    startDateEdit->setReadOnly(true);
    startDateEdit->setStyleSheet(QStringLiteral("QDateEdit ")
                                 + highlightColor);

    auto startTimeEdit = new QTimeEdit;
    startTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    startTimeEdit->setTime(availableDataRange_.first.time());
    startTimeEdit->setReadOnly(true);
    startTimeEdit->setStyleSheet(QStringLiteral("QTimeEdit ")
                                 + highlightColor);

    auto startDateContainer = new QHBoxLayout;
    startDateContainer->addWidget(startDateEdit);
    startDateContainer->insertSpacing(1, 10);
    startDateContainer->addWidget(startTimeEdit);
    startDateContainer->addStretch();
    startDateContainer->setContentsMargins(0, 0, 0, 0);

    auto endDateLabel = new ClickLabel;
    endDateLabel->setText(tr("End :"));
    endDateLabel->setToolTip(tr("<b>End:</b> Ending date of the dataset to process. This may "
                                "or may not coincide with the date of the last raw file. It is "
                                "used to select a subset of available raw data for processing."));
    auto endDateEdit = new QDateEdit;
    endDateEdit->setToolTip(endDateLabel->toolTip());
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(availableDataRange_.second.date());
    endDateEdit->setReadOnly(true);
    endDateEdit->setStyleSheet(QStringLiteral("QDateEdit ")
                               + highlightColor);

    auto endTimeEdit = new QTimeEdit;
    endTimeEdit->setDisplayFormat(QStringLiteral("hh:mm"));
    endTimeEdit->setTime(availableDataRange_.second.time());
    endTimeEdit->setReadOnly(true);
    endTimeEdit->setStyleSheet(QStringLiteral("QTimeEdit ")
                               + highlightColor);

    auto endDateContainer = new QHBoxLayout;
    endDateContainer->addWidget(endDateEdit);
    endDateContainer->insertSpacing(1, 10);
    endDateContainer->addWidget(endTimeEdit);
    endDateContainer->addStretch();
    endDateContainer->setContentsMargins(0, 0, 0, 0);

    auto lockedIcon = new QLabel;
    auto pixmap = QPixmap(QStringLiteral(":/icons/vlink-locked"));
#if defined(Q_OS_MAC)
    pixmap.setDevicePixelRatio(2.0);
#endif
    lockedIcon->setPixmap(pixmap);
    lockedIcon->setStyleSheet(QStringLiteral("QLabel { margin-right: 0px; }"));

    auto gridLayout = new QGridLayout;
    gridLayout->addWidget(currentRangeLabel, 0, 0, 1, -1);
    gridLayout->addWidget(startDateLabel, 1, 2, Qt::AlignRight);
    gridLayout->addLayout(startDateContainer, 1, 4, 1, 2);
    gridLayout->addWidget(lockedIcon, 1, 3, 2, 1, Qt::AlignCenter);
    gridLayout->addWidget(endDateLabel, 2, 2, Qt::AlignRight);
    gridLayout->addLayout(endDateContainer, 2, 4, 1, 2);
    gridLayout->setColumnStretch(0, 5);
    gridLayout->setColumnStretch(1, 1);
    gridLayout->setColumnStretch(2, 1);
    gridLayout->setColumnStretch(3, 0);
    gridLayout->setColumnStretch(4, 1);
    gridLayout->setColumnStretch(5, 1);
    gridLayout->setColumnStretch(6, 1);
    gridLayout->setVerticalSpacing(3);

    dialogLayout->addLayout(gridLayout, 2, 0);
}

void DetectDateRangeDialog::showDateRange(DateRangeType type)
{
    switch (type) {
        case DateRangeType::RawData:
        {
            auto dataStartStr = ecProject_->generalStartDate()
                                + QStringLiteral("T")
                                + ecProject_->generalStartTime();
            auto dataEndStr = ecProject_->generalEndDate()
                              + QStringLiteral("T")
                              + ecProject_->generalEndTime();
            auto dataStart = QDateTime::fromString(dataStartStr, Qt::ISODate);
            auto dataEnd = QDateTime::fromString(dataEndStr, Qt::ISODate);
            auto rawDataSubset = qMakePair(dataStart, dataEnd);

            if (FileUtils::dateRangesOverlap(availableDataRange_, rawDataSubset))
            {
                createDateSelectionWidget(type, rawDataSubset, DateRangeOverlapping::Overlap);
            }
            else
            {
                createDateSelectionWidget(type, rawDataSubset, DateRangeOverlapping::NoOverlap);
            }
        }
        break;
        case DateRangeType::PlanarFit:
        {
            if (ecProject_->screenRotMethod() == 3
                || ecProject_->screenRotMethod() == 4)
            {
                auto pfStartStr = ecProject_->planarFitStartDate()
                                  + QStringLiteral("T")
                                  + ecProject_->planarFitStartTime();
                auto pfEndStr = ecProject_->planarFitEndDate()
                                + QStringLiteral("T")
                                + ecProject_->planarFitEndTime();
                auto pfStart = QDateTime::fromString(pfStartStr, Qt::ISODate);
                auto pfEnd = QDateTime::fromString(pfEndStr, Qt::ISODate);
                auto pfSubset = qMakePair(pfStart, pfEnd);

                if (FileUtils::dateRangesOverlap(availableDataRange_, pfSubset))
                {
                    createDateSelectionWidget(type, pfSubset, DateRangeOverlapping::Overlap);
                }
                else
                {
                    createDateSelectionWidget(type, pfSubset, DateRangeOverlapping::NoOverlap);
                }
            }
        }
        break;
        case DateRangeType::TimeLag:
        {
            if (ecProject_->screenTlagMeth() == 4)
            {
                auto tlStartStr = ecProject_->timelagOptStartDate()
                                  + QStringLiteral("T")
                                  + ecProject_->timelagOptStartTime();
                auto tlEndStr = ecProject_->timelagOptEndDate()
                                + QStringLiteral("T")
                                + ecProject_->timelagOptEndTime();
                auto tlStart = QDateTime::fromString(tlStartStr, Qt::ISODate);
                auto tlEnd = QDateTime::fromString(tlEndStr, Qt::ISODate);
                auto tlSubset = qMakePair(tlStart, tlEnd);

                if (FileUtils::dateRangesOverlap(availableDataRange_, tlSubset))
                {
                    createDateSelectionWidget(type, tlSubset, DateRangeOverlapping::Overlap);
                }
                else
                {
                    createDateSelectionWidget(type, tlSubset, DateRangeOverlapping::NoOverlap);
                }
            }
        }
        break;
        case DateRangeType::Spectra:
        {
            if (isSpectraSubsetPosssible())
            {
                auto spStartStr = ecProject_->spectraStartDate()
                                  + QStringLiteral("T")
                                  + ecProject_->spectraStartTime();
                auto spEndStr = ecProject_->spectraEndDate()
                                + QStringLiteral("T")
                                + ecProject_->spectraEndTime();
                auto spStart = QDateTime::fromString(spStartStr, Qt::ISODate);
                auto spEnd = QDateTime::fromString(spEndStr, Qt::ISODate);
                auto spSubset = qMakePair(spStart, spEnd);

                if (FileUtils::dateRangesOverlap(availableDataRange_, spSubset))
                {
                    createDateSelectionWidget(type, spSubset, DateRangeOverlapping::Overlap);
                }
                else
                {
                    createDateSelectionWidget(type, spSubset, DateRangeOverlapping::NoOverlap);
                }
            }
        }
        break;
    }
}

void DetectDateRangeDialog::setCurrentRange(FileUtils::DateRange currentRange)
{
    availableDataRange_ = currentRange;
    createCurrentRange();
}

bool DetectDateRangeDialog::dateRangesOverlap(FileUtils::DateRange availableDataset,
                                              const QDate& start_date,
                                              const QTime& start_time,
                                              const QDate& end_date,
                                              const QTime& end_time)
{
    auto dateStart = QDateTime();
    dateStart.setDate(start_date);
    dateStart.setTime(start_time);

    auto dateEnd = QDateTime();
    dateEnd.setDate(end_date);
    dateEnd.setTime(end_time);

    auto dateSubset = qMakePair(dateStart, dateEnd);

    qDebug() << dateStart;
    qDebug() << dateEnd;

    return FileUtils::dateRangesOverlap(availableDataset, dateSubset);
}

void DetectDateRangeDialog::updateOverlap(QLabel *label,
                                               const QDate& start_date,
                                               const QTime& start_time,
                                               const QDate& end_date,
                                               const QTime& end_time)
{
    qDebug() << start_date;
    qDebug() << start_time;
    qDebug() << end_date;
    qDebug() << end_time;
    qDebug() << availableDataRange_.first.date();
    qDebug() << availableDataRange_.first.time();
    qDebug() << availableDataRange_.second.date();
    qDebug() << availableDataRange_.second.time();
    qDebug() << "overlapLabel" << label->text();

    auto overlapStr = QString();
    if (!dateRangesOverlap(availableDataRange_,
                           start_date,
                           start_time,
                           end_date,
                           end_time))
    {
        overlapStr = tr("<font color=\"#FF3300\">Not overlapping with available dataset</font>");
    }
    else
    {
        overlapStr = QStringLiteral("<font color=\"#C0C0C0\">________________________________________</font>");
    }

    label->setText(overlapStr);
    update();
}

QPair<QDateTime, QDateTime> DetectDateRangeDialog::getBinnedCospectraDateRange()
{
    FileUtils::DateRange dates = qMakePair(QDateTime(), QDateTime());
    auto binnedCospectraDataList = QStringList();

    auto csvFormat = QStringLiteral("*.") + Defs::CSV_NATIVE_DATA_FILE_EXT;
    auto binnedCospectraDir = ecProject_->generalOutPath() + Defs::OUT_BINNED_COSPECTRA_DIR;
    auto noRecursion = false;
    binnedCospectraDataList = FileUtils::getFiles(binnedCospectraDir,
                                                  csvFormat,
                                                  noRecursion);

    if (!binnedCospectraDataList.isEmpty())
    {
        QFuture<FileUtils::DateRange> future
            = QtConcurrent::run(&FileUtils::getDateRangeFromFileList,
                                binnedCospectraDataList,
                                QStringLiteral("yyyymmdd-HHMM_binned_cospectra_2015-03-16T094426_adv.csv"));
        while (!future.isFinished())
        {
            QCoreApplication::processEvents();
        }
        dates = future.result();

        // correct the start/end date accounting for file duration
        if (dlProject_->timestampEnd() == 0)
        {
            dates.second = dates.second.addSecs(dlProject_->fileDuration() * 60);
        }
        else
        {
            dates.first = dates.first.addSecs(-dlProject_->fileDuration() * 60);
        }
    }
    return dates;
}

bool DetectDateRangeDialog::isSpectraSubsetPosssible()
{
    // if ( (an in-situ spectra correction method is selected [Horst, Ibrom, Fratini]
    //       AND spectral assessment file is not available)
    // OR Ensemble averaged spectra output is selected
    // OR Ensemble averaged cospectra output is selected )
    return (((ecProject_->generalHfMethod() == 2
                     || ecProject_->generalHfMethod() == 3
                     || ecProject_->generalHfMethod() == 4) && ecProject_->spectraMode())
                  || ecProject_->generalOutMeanSpectra()
                  || ecProject_->generalOutMeanCosp());
}

void DetectDateRangeDialog::updateSpectraOverlap(QLabel *label,
                                               const QDate& start_date,
                                               const QTime& start_time,
                                               const QDate& end_date,
                                               const QTime& end_time)
{
    auto overlapStr = QString();

    if (isSpectraSubsetPosssible())
    {
        if (!ecProject_->generalBinSpectraAvail())
        {
            if (ecProject_->generalSubset())
            {
                auto dateStart = QDateTime();
                dateStart.setDate(QDate::fromString(ecProject_->spectraStartDate(), Qt::ISODate));
                dateStart.setTime(QTime::fromString(ecProject_->spectraStartTime(), QStringLiteral("hh:mm")));

                auto dateEnd = QDateTime();
                dateEnd.setDate(QDate::fromString(ecProject_->spectraEndDate(), Qt::ISODate));
                dateEnd.setTime(QTime::fromString(ecProject_->spectraEndTime(), QStringLiteral("hh:mm")));

                auto rawDataSubset = qMakePair(dateStart, dateEnd);

                if (!dateRangesOverlap(rawDataSubset,
                                       start_date,
                                       start_time,
                                       end_date,
                                       end_time))
                {
                    overlapStr = tr("<font color=\"#FF3300\">Not overlapping with available dataset</font>");
                }
                else
                {
                    overlapStr
                            = QStringLiteral("<font color=\"#C0C0C0\">________________________________________</font>");
                }
            }
            else
            {
                if (!dateRangesOverlap(availableDataRange_,
                                       start_date,
                                       start_time,
                                       end_date,
                                       end_time))
                {
                    overlapStr = tr("<font color=\"#FF3300\">Not overlapping with available dataset</font>");
                }
                else
                {
                    overlapStr
                            = QStringLiteral("<font color=\"#C0C0C0\">________________________________________</font>");
                }
            }
        }
        else
        {
            FileUtils::DateRange binnedCospectraDateRange = getBinnedCospectraDateRange();

            if (!dateRangesOverlap(binnedCospectraDateRange,
                                   start_date,
                                   start_time,
                                   end_date,
                                   end_time))
            {
                overlapStr = tr("<font color=\"#FF3300\">Not overlapping with available dataset</font>");
            }
            else
            {
                overlapStr = QStringLiteral("<font color=\"#C0C0C0\">________________________________________</font>");
            }
        }
    }

    label->setText(overlapStr);
}

void DetectDateRangeDialog::setToAvailableDates()
{
    DEBUG_FUNC_NAME

    auto firstAvailableDate = availableDataRange_.first.date();
    auto firstAvailableTime = availableDataRange_.first.time();
    auto lastAvailableDate = availableDataRange_.second.date();
    auto lastAvailableTime = availableDataRange_.second.time();

    auto rawStartEdit = this->findChild<QDateEdit *>(QStringLiteral("rawStartDate"));
    if (rawStartEdit)
    {
        rawStartEdit->setDate(firstAvailableDate);
    }
    auto rawStartTime = this->findChild<QTimeEdit *>(QStringLiteral("rawStartTime"));
    if (rawStartTime)
    {
        rawStartTime->setTime(firstAvailableTime);
    }
    auto rawEndEdit = this->findChild<QDateEdit *>(QStringLiteral("rawEndDate"));
    if (rawEndEdit)
    {
        rawEndEdit->setDate(lastAvailableDate);
    }
    auto rawEndTime = this->findChild<QTimeEdit *>(QStringLiteral("rawEndTime"));
    if (rawEndTime)
    {
        rawEndTime->setTime(lastAvailableTime);
    }

    auto spectraStartEdit = this->findChild<QDateEdit *>(QStringLiteral("spStartDate"));
    if (spectraStartEdit)
    {
        spectraStartEdit->setDate(firstAvailableDate);
    }
    auto spectraStartTime = this->findChild<QTimeEdit *>(QStringLiteral("spStartTime"));
    if (spectraStartTime)
    {
        spectraStartTime->setTime(firstAvailableTime);
    }
    auto spectraEndEdit = this->findChild<QDateEdit *>(QStringLiteral("spEndDate"));
    if (spectraEndEdit)
    {
        spectraEndEdit->setDate(lastAvailableDate);
    }
    auto spectraEndTime = this->findChild<QTimeEdit *>(QStringLiteral("spEndTime"));
    if (spectraEndTime)
    {
        spectraEndTime->setTime(lastAvailableTime);
    }

    auto pfStartEdit = this->findChild<QDateEdit *>(QStringLiteral("pfStartDate"));
    if (pfStartEdit)
    {
        pfStartEdit->setDate(firstAvailableDate);
    }
    auto pfStartTime = this->findChild<QTimeEdit *>(QStringLiteral("pfStartTime"));
    if (pfStartTime)
    {
        pfStartTime->setTime(firstAvailableTime);
    }
    auto pfEndEdit = this->findChild<QDateEdit *>(QStringLiteral("pfEndDate"));
    if (pfEndEdit)
    {
        pfEndEdit->setDate(lastAvailableDate);
    }
    auto pfEndTime = this->findChild<QTimeEdit *>(QStringLiteral("pfEndTime"));
    if (pfEndTime)
    {
        pfEndTime->setTime(lastAvailableTime);
    }

    auto tlStartEdit = this->findChild<QDateEdit *>(QStringLiteral("tlStartDate"));
    if (tlStartEdit)
    {
        tlStartEdit->setDate(firstAvailableDate);
    }
    auto tlStartTime = this->findChild<QTimeEdit *>(QStringLiteral("tlStartTime"));
    if (tlStartTime)
    {
        tlStartTime->setTime(firstAvailableTime);
    }
    auto tlEndEdit = this->findChild<QDateEdit *>(QStringLiteral("tlEndDate"));
    if (tlEndEdit)
    {
        tlEndEdit->setDate(lastAvailableDate);
    }
    auto tlEndTime = this->findChild<QTimeEdit *>(QStringLiteral("tlEndTime"));
    if (tlEndTime)
    {
        tlEndTime->setTime(lastAvailableTime);
    }
}
