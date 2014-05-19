/***************************************************************************
  planarfitsettingsdialog.h
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

#ifndef PLANARFITSETTINGSDIALOG_H
#define PLANARFITSETTINGSDIALOG_H

#include <QDialog>
#include <QDateTime>

class QRadioButton;
class QButtonGroup;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QPushButton;
class QDate;
class QDateEdit;
class QComboBox;
class QItemSelectionModel;
class QScrollArea;

class QwwButtonLineEdit;
class QToolButton;

class TableView;
class PieView;
class TableModel;
class ClickLabel;
class QLabel;
class EcProject;
class QCheckBox;
struct ConfigState;

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

    TableModel *model_;
    TableView *table_;
    PieView *pieChart_;
    QItemSelectionModel *selectionModel_;

    void forceEndDatePolicy();

    void setupModel();
    void setupViews();

    void insertAngleAt(int row);
    void removeAngleAt(int row);

    void resizeRows();

    QList<bool> oldEnabled;
};

#endif // PLANARFITSETTINGSDIALOG_H
