/***************************************************************************
  rawfilesettingsdialog.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2017, LI-COR Biosciences
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

#ifndef RAWFILESETTINGSDIALOG_H
#define RAWFILESETTINGSDIALOG_H

#include <QDialog>

////////////////////////////////////////////////////////////////////////////////
/// \file src/rawfilesettingsdialog.h
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

class QComboBox;
class QLineEdit;
class QSpinBox;

class ClickLabel;
class CustomClearLineEdit;
class DlProject;

/// \class RawFileSettingsDialog
/// \brief Class representing the raw files settings dialog
class RawFileSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RawFileSettingsDialog(QWidget* parent, DlProject *dlProject);
    ~RawFileSettingsDialog();

public slots:
    void refresh();

private slots:
    void onClickFieldSepLabel();
    void updateFieldSep(const QString& s);
    void onClickHeaderRowsLabel();
    void updateHeaderRows(int n);
    void onClickDataRecLabel();
    void updateDataRecLabel(const QString& s);
    void onlineHelpTrigger_1();

private:
    ClickLabel* fieldSepLabel;
    QComboBox* fieldSepCombo;
    ClickLabel* headerRowsLabel;
    QSpinBox* headerRowsSpin;
    ClickLabel* dataRecLabel;
    CustomClearLineEdit* dataRecLabelEdit;
    QPushButton *questionMark_1;

    DlProject *dlProject_;
};

#endif // RAWFILESETTINGSDIALOG_H
