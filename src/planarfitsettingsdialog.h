/***************************************************************************
  planarfitsettingsdialog.h
  -------------------------
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

#ifndef PLANARFITSETTINGSDIALOG_H
#define PLANARFITSETTINGSDIALOG_H

#include <QDateTime>
#include <QDialog>

#include "fileutils.h"

////////////////////////////////////////////////////////////////////////////////
/// \file src/planarfitsettingsdialog.h
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
class QComboBox;
class QDate;
class QDateEdit;
class QDoubleSpinBox;
class QItemSelectionModel;
class QLabel;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSpinBox;
class QTimeEdit;
class QToolButton;

class AncillaryFileTest;
class AnglesView;
class AngleTableModel;
class AngleTableView;
class ClickLabel;
struct ConfigState;
class EcProject;
class FileBrowseWidget;

class PlanarFitSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    PlanarFitSettingsDialog(QWidget* parent, EcProject *ecProject, ConfigState* config);
    void setSmartfluxUI();

signals:
    void saveRequest();

public slots:
    void close();
    void reset();
    void refresh();
    void partialRefresh();
    void setDateRange(FileUtils::DateRange dates);

private slots:
    void updatePfMode(int radioButton);
    void radioClicked(int radioButton);
    void testSelectedFile(const QString& fp);

    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate &d);
    void updateStartTime(const QTime &t);
    void updateEndDate(const QDate &d);
    void updateEndTime(const QTime &t);

    void onOffsetLabelClicked();
    void onItemPerSectorLabelClicked();
    void onMaxAvgWLabelClicked();
    void onMinAvgULabelClicked();
    void updateItemPerSector(int i);
    void updateMaxAvgW(double d);
    void updateMinAvgU(double d);
    void onFixPolicyLabelClicked();
    void updateFixPolicy(int n);
    void addAngle();
    void removeAngle();
    void setEquallySpaced();
    void setNorthOffset(double angle);
    void fillPie();
    void modelModified();
    void updateModel();
    void updateSubsetSelection(bool b);

private:
    void forceEndDatePolicy();
    void forceEndTimePolicy();

    void setupModel();
    void setupViews();

    void insertAngleAt(int row);
    void removeAngleAt(int row);

    void resizeRows();
    void updateFile(const QString& fp);

    QRadioButton* existingRadio;
    QRadioButton* nonExistingRadio;
    QCheckBox *subsetCheckBox;
    QLabel* lockedIcon;
    ClickLabel* startDateLabel;
    QDateEdit* startDateEdit;
    QTimeEdit* startTimeEdit;
    ClickLabel* endDateLabel;
    QDateEdit* endDateEdit;
    QTimeEdit* endTimeEdit;
    FileBrowseWidget * fileBrowse;
    QButtonGroup* radioGroup;

    QScrollArea* propScrollArea;
    QLabel *hrLabel_2;

    ClickLabel* itemPerSectorLabel;
    QSpinBox* itemPerSectorSpin;
    ClickLabel* maxAvgWLabel;
    QDoubleSpinBox* maxAvgWSpin;
    ClickLabel* minAvgULabel;
    QDoubleSpinBox* minAvgUSpin;
    ClickLabel* fixPolicyLabel;
    QComboBox* fixPolicyCombo;

    QToolButton *addButton;
    QToolButton *removeButton;

    QLabel* sectorConfigTitle;
    QWidget *sectorConfigFrame;
    ClickLabel* offsetLabel;
    QDoubleSpinBox* offsetSpin;
    QPushButton* setEquallySpacedButton;

    EcProject *ecProject_;
    ConfigState* configState_;

    AngleTableModel *angleTableModel_;
    AngleTableView *angleTableView_;
    AnglesView *anglesView_;
    QItemSelectionModel *angleSelectionModel_;
};

#endif // PLANARFITSETTINGSDIALOG_H
