/***************************************************************************
  welcomepage.h
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

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QAbstractButton>
#include "configstate.h"

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

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    WelcomePage(QWidget *parent, EcProject *ecProject, ConfigState* configState);
    ~WelcomePage();

    void updateWelcomePage(bool small);
    void updateSmartfluxBar();
    void updateSmartfluxCheckBox();

    QGridLayout* mainLayout() { return mainLayout_; }

private:
    EcProject* ecProject_;
    ConfigState* configState_;

    ClickLabel *appLogoLabel;
    ClickLabel *feedbackLabel;
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
    void openWebSite();
    void openForumFeedback();
    void updateMainLayout(bool on);
    void onlineHelpTrigger_1();

signals:
    void openProjectRequest(QString);
    void newProjectRequest();
    void checkUpdatesRequest();
};

#endif // MAINPAGE_H
