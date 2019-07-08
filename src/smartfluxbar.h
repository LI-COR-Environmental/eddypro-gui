/***************************************************************************
  smartfluxbar.h
  --------------
  Copyright © 2013-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
 ***************************************************************************/

#ifndef SMARTFLUXBAR_H
#define SMARTFLUXBAR_H

#include <QWidget>

class QLayoutItem;

struct ConfigState;
class CreatePackageDialog;
class EcProject;

class SmartFluxBar : public QWidget
{
    Q_OBJECT
public:
    SmartFluxBar(EcProject* ecProject,
                 ConfigState* config,
                 QWidget* parent = nullptr);
    ~SmartFluxBar() override;

signals:
    void showSmartfluxBarRequest(bool on);
    void saveRequest();
    void saveSilentlyRequest();

protected:
    void paintEvent(QPaintEvent* event) override;

    static void paintLayout(QPainter* painter, QLayoutItem* item);

private:
    EcProject* ecProject_;
    ConfigState* configState_;
    CreatePackageDialog* cpDialog_;
//    QScopedPointer<CreatePackageDialog> cpDialog_;

    void makeCreatePackageDialog();

private slots:
    void closeRequest();
    void createPackage();
    void showCreatePackageDialog();
};

#endif // SMARTFLUXBAR_H
