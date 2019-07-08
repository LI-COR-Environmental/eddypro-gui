/***************************************************************************
  welcomepage.h
  -------------
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

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QAbstractButton>
#include <QWidget>

#include "configstate.h"

////////////////////////////////////////////////////////////////////////////////
/// \file src/welcomepage.h
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

class QPushButton;
class QButtonGroup;
class QListWidget;
class QListWidgetItem;
class QLabel;
class QToolButton;
class QGridLayout;
class QCheckBox;

struct ConfigState;
class ClickLabel;
class SmartFluxBar;
class EcProject;

/// \class WelcomePage
/// \brief Class representing the main page of the application (start page)
class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    WelcomePage(QWidget *parent, EcProject *ecProject, ConfigState* configState);

    void updateWelcomePage(bool small);
    void updateSmartfluxBar();
    void updateSmartfluxCheckBox();

    QGridLayout* mainLayout() { return mainLayout_; }

private:
    EcProject* ecProject_;
    ConfigState* configState_;

    ClickLabel *appLogoLabel;
    ClickLabel *toviLabel;
    QToolButton *newButton;
    QToolButton *openButton;
    QListWidget *recentListWidget;
    QListWidget *newsListWidget;
    QListWidget *helpListWidget;
    QListWidget *supportListWidget;
    QGridLayout *mainLayout_;
    SmartFluxBar* smartfluxBar_;
    QWidget* smartfluxBarPlaceholder_;
    QCheckBox* smartfluxModeCheckbox_;
    QPushButton* questionMark_1;

    void createQuestionMark();
private slots:
    void openProjectRequested();
    void recentOpenRequested(QListWidgetItem* item);
    void doNothing(QListWidgetItem* item);
    void newsItemRequested(QListWidgetItem* item);
    void helpItemRequested(QListWidgetItem* item);
    void supportItemRequested(QListWidgetItem* item);
    void updateRecentList();
    void openToviHomepage();
    void onlineHelpTrigger_1();

signals:
    void openProjectRequest(QString);
    void newProjectRequest();
    void checkUpdatesRequest();
};

#endif // MAINPAGE_H
