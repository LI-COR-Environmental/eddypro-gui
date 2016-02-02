/***************************************************************************
  timelagsettingsdialog.h
  -------------------
  Copyright (C) 2013-2016, LI-COR Biosciences
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

#ifndef TIMELAGSETTINGSDIALOG_H
#define TIMELAGSETTINGSDIALOG_H

#include <QDateTime>
#include <QDialog>

////////////////////////////////////////////////////////////////////////////////
/// \file src/timelagsettingsdialog.h
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

class QButtonGroup;
class QCheckBox;
class QDate;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QRadioButton;
class QSpinBox;
class QTimeEdit;

class AncillaryFileTest;
class ClickLabel;
struct ConfigState;
class EcProject;
class FileBrowseWidget;

class TimeLagSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TimeLagSettingsDialog(QWidget *parent, EcProject *ecProject, ConfigState* config);
    ~TimeLagSettingsDialog();

    void setSmartfluxUI();
signals:
    void saveRequest();

public slots:
    void close();
    void reset();
    void refresh();
    void partialRefresh();
    void setDateRange(QPair<QDateTime, QDateTime> dates);

private slots:
    void updateTlMode(int radioButton);
    void radioClicked(int radioButton);
    void testSelectedFile(const QString &fp);

    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate &d);
    void updateEndDate(const QDate &d);
    void updateStartTime(const QTime &t);
    void updateEndTime(const QTime &t);

    void onRhClassClicked();
    void updateRhClass(int n);

    void onCo2MinFluxClicked();
    void updateCo2MinFlux(double d);
    void onCh4MinFluxClicked();
    void updateCh4MinFlux(double d);
    void onGas4MinFluxClicked();
    void updateGas4MinFlux(double d);
    void onLeMinFluxClicked();
    void updateLeMinFlux(double d);
    void onPgRangeLabelClicked();
    void updatePgRange(double d);

    void onCo2LabelClicked();
    void updateMinCo2Tl(double d);
    void updateMaxCo2Tl(double d);
    void onH2oLabelClicked();
    void updateMinH2oTl(double d);
    void updateMaxH2oTl(double d);
    void onCh4LabelClicked();
    void updateMinCh4Tl(double d);
    void updateMaxCh4Tl(double d);
    void onGas4LabelClicked();
    void updateMinGas4Tl(double d);
    void updateMaxGas4Tl(double d);

    void updateSubsetSelection(bool b);

private:
    void updateFile(const QString& fp);
    void forceEndDatePolicy();
    void forceEndTimePolicy();

    QRadioButton* existingRadio;
    QRadioButton* nonExistingRadio;
    QButtonGroup* radioGroup;
    FileBrowseWidget * fileBrowse;
    QCheckBox* subsetCheckBox;
    ClickLabel* startDateLabel;
    QDateEdit* startDateEdit;
    QTimeEdit* startTimeEdit;
    QLabel* lockedIcon;
    ClickLabel* endDateLabel;
    QDateEdit* endDateEdit;
    QTimeEdit* endTimeEdit;
    ClickLabel* pgRangeLabel;
    QLabel* pgRangeLabel_2;
    QDoubleSpinBox* pgRangeSpin;

    QLabel* h2oTitleLabel;
    ClickLabel* rhClassLabel;
    QSpinBox* rhClassSpin;
    ClickLabel* leMinFluxLabel;
    QDoubleSpinBox* leMinFluxSpin;

    QLabel* gasTitleLabel;
    ClickLabel* co2MinFluxLabel;
    QDoubleSpinBox* co2MinFluxSpin;
    ClickLabel* ch4MinFluxLabel;
    QDoubleSpinBox* ch4MinFluxSpin;
    ClickLabel* gas4MinFluxLabel;
    QDoubleSpinBox* gas4MinFluxSpin;

    QLabel* searchWindowLabel;
    QLabel* minLabel;
    QLabel* maxLabel;
    ClickLabel* co2Label;
    ClickLabel* h2oLabel;
    ClickLabel* ch4Label;
    ClickLabel* gas4Label;
    QDoubleSpinBox* minCo2TlSpin;
    QDoubleSpinBox* maxCo2TlSpin;
    QDoubleSpinBox* minH2oTlSpin;
    QDoubleSpinBox* maxH2oTlSpin;
    QDoubleSpinBox* minCh4TlSpin;
    QDoubleSpinBox* maxCh4TlSpin;
    QDoubleSpinBox* minGas4TlSpin;
    QDoubleSpinBox* maxGas4TlSpin;

    EcProject *ecProject_;
    ConfigState* configState_;
};

#endif // TIMELAGSETTINGSDIALOG_H
