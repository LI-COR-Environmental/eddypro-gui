/***************************************************************************
  mainwidget.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

class AdvSpectralOptions;
class BasicSettingsPage;
class DlProject;
class EcProject;
class FullProjectPage;
class PlanarFitSettingsDialog;
class ProjectPage;
class RunPage;
class TimeLagSettingsDialog;
class WelcomePage;
struct ConfigState;

#include "advancedsettingspage.h"
#include "advsettingscontainer.h"
#include "advprocessingoptions.h"

/// \class StartDialog
/// \brief Widget representing the page container of the application
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent, DlProject *dlProject, EcProject *ecProject, ConfigState* configState);
    ~MainWidget();

    inline WelcomePage* welcomePage() { return welcomePage_; }
    inline ProjectPage* projectPage() { return projectPage_; }
    inline BasicSettingsPage* basicPage() { return basicSettingsPage_; }
    inline AdvancedSettingsPage* advancedPage() { return advancedSettingsPage_; }
    inline RunPage* runPage() { return runPage_; }
    inline PlanarFitSettingsDialog* pfDialog() { return advancedSettingsPage_
                                                            ->advancedSettingPages()
                                                            ->processingOptions()
                                                            ->getPlanarFitSettingsDialog(); }
    inline TimeLagSettingsDialog* tlDialog() { return advancedSettingsPage_
                                                            ->advancedSettingPages()
                                                            ->processingOptions()
                                                            ->getTimeLagSettingsDialog(); }
    inline AdvSpectralOptions* spectralOptions() { return advancedSettingsPage_
                                                            ->advancedSettingPages()
                                                            ->spectralOptions(); }

    Defs::CurrPage currentPage();

    bool smartFluxCloseRequest();

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

signals:
    void openProjectRequest(QString);
    void newProjectRequest();
    void updateMetadataReadResult(bool);
    void recentUpdated();
    void checkUpdatesRequest();
    void showSmartfluxBarRequest(bool on);
    void saveSilentlyRequest();
    void saveRequest();
    void mdCleanupRequest();
    void showSetPrototypeRequest();
};

#endif // MAINDIALOG_H
