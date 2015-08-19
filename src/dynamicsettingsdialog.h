#ifndef DYNAMICSETTINGSDIALOG_H
#define DYNAMICSETTINGSDIALOG_H

/***************************************************************************
  dynamicsettingsdialog.cpp
  -------------------
  A dialog for the generic dynamic file settings
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011, LI-COR Biosciences

  This file is part of EddyPro (TM).

  EddyPro (TM) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (TM) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (TM). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include <QDialog>

////////////////////////////////////////////////////////////////////////////////
/// \file src/dynamicsettingsdialog.h
/// \brief Declares the DynamicSettingsDialog class
/// \version     1.6.3
/// \date        2010-12-01
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QLineEdit;
class QCheckBox;
class QPushButton;
class EcProject;

/// \class DynamicSettingsDialog
/// \brief Class representing the Dynamic settings dialog
class DynamicSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DynamicSettingsDialog(QWidget* parent, EcProject *ecProject);
    ~DynamicSettingsDialog();

public slots:
    void close();
    void reset();
    void refresh();

private slots:    
    void timelineFileLoad_clicked();
    void onTimelineFileCheckBoxClicked(bool b);
    void updateUseTimelineFile(const bool&);
    void updateTimelineFile(const QString& fp);

    void meteoFileLoad_clicked();
    void onMeteoFileCheckBoxClicked(bool b);
    void updateUseMeteoFile(const bool&);
    void updateMeteoFile(const QString& fp);

private:    
    QCheckBox *timelineFileCheckBox;
    QLineEdit *timelineFileEdit;
    QPushButton *timelineFileLoad;

    QCheckBox *meteoFileCheckBox;
    QLineEdit *meteoFileEdit;
    QPushButton *meteoFileLoad;

    EcProject *ecProject_;

};

#endif // DYNAMICSETTINGSDIALOG_H
