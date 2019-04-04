/***************************************************************************
  detectdaterangedialog.h
  -----------------------
  Copyright © 2016-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#ifndef DETECTDATERANGEDIALOG_H
#define DETECTDATERANGEDIALOG_H

#include <QDialog>

#include "fileutils.h"

class QGridLayout;
class QLabel;

class EcProject;
class DlProject;

class DetectDateRangeDialog : public QDialog
{
    Q_OBJECT
public:
    DetectDateRangeDialog(QWidget* parent, EcProject *ecProject, DlProject *dlProject);

    enum class DateRangeType { RawData, Spectra, PlanarFit, TimeLag };
    enum class DateRangeOverlapping { Overlap, NoOverlap };

//    void setLabel(const QString& label);
    void showDateRange(DateRangeType type);
    void setCurrentRange(FileUtils::DateRange currentRange);

signals:
    void alignDeclinationRequest(const QDate& d);
    void refreshRdRequest();
    void refreshPfRequest();
    void refreshTlRequest();
    void refreshSpRequest();

public slots:

private slots:
    void setToAvailableDates();

private:
    void createDateSelectionWidget(DateRangeType type,
                                   FileUtils::DateRange subrange,
                                   DateRangeOverlapping overlap);
    bool dateRangesOverlap(FileUtils::DateRange availableDataset,
                           const QDate &start_date,
                           const QTime &start_time,
                           const QDate &end_date,
                           const QTime &end_time);
    void updateOverlap(QLabel *label,
                            const QDate &start_date,
                            const QTime &start_time,
                            const QDate &end_date,
                            const QTime &end_time);
    FileUtils::DateRange getBinnedCospectraDateRange();
    void updateSpectraOverlap(QLabel *label,
                            const QDate &start_date,
                            const QTime &start_time,
                            const QDate &end_date,
                            const QTime &end_time);
    void createCurrentRange();
    bool isSpectraSubsetPosssible();

    FileUtils::DateRange availableDataRange_;
    QPushButton *setAsCurrentRangeButton;
    QPushButton *okButton;
//    QLabel *msgLabel;
    QGridLayout *dialogLayout;

    EcProject *ecProject_;
    DlProject *dlProject_;
    bool isRangeOverlapping_;
};

#endif  // DETECTDATERANGEDIALOG_H
