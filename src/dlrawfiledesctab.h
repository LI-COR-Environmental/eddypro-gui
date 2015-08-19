/***************************************************************************
  dlrawfiledesctab.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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
 ***************************************************************************/

#ifndef DLRAWFILEDESCTAB_H
#define DLRAWFILEDESCTAB_H

#include <QWidget>

class QPushButton;

class DlProject;
class RawFileSettingsDialog;
class VariableDelegate;
class VariableModel;
class VariableView;

class DlRawfileDescTab : public QWidget {
    Q_OBJECT
public:
    DlRawfileDescTab(QWidget* parent, DlProject* dlProject);
    virtual ~DlRawfileDescTab();

    void reset();
    void refresh();
    void rawfileViewRefresh();

    VariableView* variableView_;
    QPushButton* rawSettingsButton;

private slots:
    void modelModified();
    void updateModels();
    void showRawSettingsDialog();
    void clearInstrModels();
    void onlineHelpTrigger_1();

private:
    VariableModel* variableModel_;
    VariableDelegate* variableDelegate_;

    DlProject* dlProject_;

    RawFileSettingsDialog* rawSettingsDialog;

    bool fieldSepTOB1Flag_;

    QPushButton* clearCustomVarsButton;
};

#endif // DLRAWFILEDESCTAB_H
