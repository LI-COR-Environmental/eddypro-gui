/***************************************************************************
  advancedsettingspage.h
  -------------------
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

#ifndef ADVANCEDSETTINGSPAGE_H
#define ADVANCEDSETTINGSPAGE_H

#include <QWidget>

#include "configstate.h"

class QListWidget;

class AdvSettingsContainer;
class DlProject;
class EcProject;
class SmartFluxBar;

class AdvancedSettingsPage : public QWidget
{
    Q_OBJECT

public:
    AdvancedSettingsPage(QWidget* parent,
                         DlProject* dlProject,
                         EcProject* ecProject,
                         ConfigState* config);

    AdvSettingsContainer* advancedSettingPages()
                            { return advancedSettingContainer; }
    void updateSmartfluxBar();

private slots:
    void changePage(int index);
    void resetButtonCLicked();

private:
    static const QString PAGE_TITLE_0;
    static const QString PAGE_TITLE_1;
    static const QString PAGE_TITLE_2;
    static const QString PAGE_TITLE_3;
    static const QString PAGE_TITLE_4;

    void createMenu();
    void createIcons();
    bool requestSettingsReset();

    DlProject* dlProject_;
    EcProject* ecProject_;
    ConfigState* configState_;

    SmartFluxBar* smartfluxBar {};
    QListWidget* menuWidget {};
    AdvSettingsContainer*  advancedSettingContainer {};
};

#endif // ADVANCEDSETTINGSPAGE_H
