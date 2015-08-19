/***************************************************************************
  planarfitsettingsdialog.h
  -------------------
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#ifndef PLANARFITSETTINGSDIALOG_H
#define PLANARFITSETTINGSDIALOG_H

#include <QDateTime>
#include <QDialog>

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
class QLineEdit;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QSpinBox;
class QToolButton;

class QwwButtonLineEdit;

class AnglesView;
class AngleTableModel;
class AngleTableView;
class ClickLabel;
struct ConfigState;
class EcProject;

class PlanarFitSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlanarFitSettingsDialog(QWidget* parent, EcProject *ecProject, ConfigState* config);
    ~PlanarFitSettingsDialog();

    void setSmartfluxUI();
signals:
    void saveRequest();

public slots:
    void close();
    void reset();
    void refresh();
    void setDateRange(QPair<QDateTime, QDateTime> dates);

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void updatePfMode(int radioButton);
    void radioClicked(int radioButton);
    void updateFile(const QString& fp);
    void fileLoad_clicked();
    void onStartDateLabelClicked();
    void onEndDateLabelClicked();
    void updateStartDate(const QDate &d);
    void updateEndDate(const QDate &d);
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
    void clearFileEdit();

private:
    QRadioButton* existingRadio;
    QRadioButton* nonExistingRadio;
    QCheckBox *subsetCheckBox;
    QLabel* lockedIcon;
    ClickLabel* startDateLabel;
    QDateEdit* startDateEdit;
    ClickLabel* endDateLabel;
    QDateEdit* endDateEdit;
    QwwButtonLineEdit* fileEdit;
    QPushButton* fileLoad;
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

    void forceEndDatePolicy();

    void setupModel();
    void setupViews();

    void insertAngleAt(int row);
    void removeAngleAt(int row);

    void resizeRows();
};

#endif // PLANARFITSETTINGSDIALOG_H
