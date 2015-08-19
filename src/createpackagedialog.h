/***************************************************************************
  createpackagedialog.h
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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

class QLabel;
class QPushButton;
class QwwButtonLineEdit;
class QwwClearLineEdit;

class ClickLabel;
struct ConfigState;
class EcProject;

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

protected:
    bool eventFilter(QObject *o, QEvent *e);
private slots:
    void clearOutpathEdit();
    void updateOutpath(const QString &fp);
    void outpathBrowse_clicked();

    void updateFilename(const QString &fn);
    void onFilenameLabelClicked();

    void clearFilenameEdit();
private:
    EcProject *ecProject_;
    ConfigState* configState_;

    ClickLabel *filenameLabel;
    QwwClearLineEdit* filenameEdit;
    ClickLabel *outpathLabel;
    QwwButtonLineEdit* outpathEdit;
    QPushButton* outpathBrowse;
//    QLabel* resultLabel;
    QPushButton *createButton;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void refreshButtonStatus();
};

#endif // CREATEPACKAGEDIALOG_H
