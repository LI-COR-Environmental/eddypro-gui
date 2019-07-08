/***************************************************************************
  smartfluxbar.h
  -------------------
  Copyright (C) 2013-2018, LI-COR Biosciences
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
    explicit SmartFluxBar(EcProject* ecProject,
                          ConfigState* config,
                          QWidget* parent = nullptr);
    ~SmartFluxBar();

signals:
    void showSmartfluxBarRequest(bool on);
    void saveRequest();
    void saveSilentlyRequest();

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

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
