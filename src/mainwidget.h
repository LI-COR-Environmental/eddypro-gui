/***************************************************************************
  mainwidget.h
  ------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
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
