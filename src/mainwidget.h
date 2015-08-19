/***************************************************************************
  mainwidget.h
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
****************************************************************************/

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QPointer>
#include <QWidget>

#include "defs.h"
#include "faderwidget.h"

////////////////////////////////////////////////////////////////////////////////
/// \file src/mainwidget.h
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

class QStackedLayout;

class DlProject;
class EcProject;
class WelcomePage;
class ProjectPage;
class FullProjectPage;
class BasicSettingsPage;
class AdvancedSettingsPage;
class RunPage;
struct ConfigState;

/// \class StartDialog
/// \brief Class representing the start page of the application
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* configState);
    ~MainWidget();

    inline WelcomePage* startPage() { return welcomePage_; }
    inline ProjectPage* projectPage() { return projectPage_; }
    inline BasicSettingsPage* processingPage() { return basicSettingsPage_; }
    inline AdvancedSettingsPage* advancedPage() { return advancedSettingsPage_; }
    inline RunPage* runPage() { return runPage_; }

    Defs::CurrPage currentPage();

public slots:
    void setCurrentPage(Defs::CurrPage page);
    void updateSmartfluxBarStatus();
//    void routeSmartfluxBarRequests();

private:
    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;

    QStackedLayout* mainWidgetLayout;

    WelcomePage* welcomePage_;
    ProjectPage* projectPage_;
    BasicSettingsPage* basicSettingsPage_;
    AdvancedSettingsPage* advancedSettingsPage_;
    RunPage* runPage_;

    QPointer<FaderWidget> faderWidget;
    bool fadingOn;

private slots:
    void fadeInWidget(int);

signals:
    void openProjectRequest(QString);
    void newProjectRequest();
    void updateMetadataReadResult(bool);
    void recentUpdated();
    void checkUpdatesRequest();
    void showSmartfluxBarRequest(bool on);
    void saveSilentlyRequest();
    void saveRequest();
};

#endif // MAINDIALOG_H
