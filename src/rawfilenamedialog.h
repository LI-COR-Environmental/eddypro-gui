/***************************************************************************
  rawfilenamedialog.h
  -------------------
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#ifndef RAWFILENAMEDIALOG_H
#define RAWFILENAMEDIALOG_H

#include <QDialog>

////////////////////////////////////////////////////////////////////////////////
/// \file src/rawfilenamedialog.h
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

class QButtonGroup;
class QLabel;
class QLineEdit;
class QFrame;
class QVBoxLayout;

class EcProject;

class RawFilenameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RawFilenameDialog(QWidget *parent, EcProject *ecProject);

signals:
    void updateFileFormatRequest(QString);

public slots:
    void close();
    void reset();
    void refresh();

private slots:
    void updateFormatEdit(int id);
    void updateFileList(const QString& file);

private:
    QLabel* title;
    QLabel* desc;
    QFrame* radioGroupBox;
    QVBoxLayout* radioGroupBoxLayout;
    QButtonGroup* extRadioGroup;
    QLineEdit* rawFilenameFormatEdit;
    QPushButton* okButton;

    void createRadioButtons(const QStringList& list);
    QStringList getRawFileTypeAvailable();

    EcProject *ecProject_;
    QStringList fileList_;
};

#endif // RAWFILENAMEDIALOG_H
