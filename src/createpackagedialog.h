/***************************************************************************
  createpackagedialog.h
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

#ifndef CREATEPACKAGEDIALOG_H
#define CREATEPACKAGEDIALOG_H

#include <QDialog>

class QAction;
class QLabel;
class QLineEdit;
class QPushButton;

class ClickLabel;
struct ConfigState;
class CustomClearLineEdit;
class EcProject;
class DirBrowseWidget;

class CreatePackageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreatePackageDialog(EcProject* ecProject,
                                 ConfigState* config,
                                 QWidget* parent = nullptr);
    void refresh();
    void showResult(bool ok, const QString& pkgname);

signals:
    void createPackageRequest();

public slots:
    void close();

private slots:
    void clearOutpathBrowse();
    void updateOutpath(const QString &fp);
    void outpathBrowseSelected(const QString &dir_path);

    void updateFilename(const QString &fn);
    void onFilenameLabelClicked();

    void clearFilenameEdit();

private:
    EcProject *ecProject_;
    ConfigState* configState_;

    ClickLabel *filenameLabel;
    CustomClearLineEdit* filenameEdit;
    ClickLabel *outpathLabel;
    DirBrowseWidget* outpathBrowse;
    QPushButton *createButton;
    QPushButton *cancelButton;

    void refreshButtonStatus();
};

#endif // CREATEPACKAGEDIALOG_H
