/***************************************************************************
  rawfilenamedialog.h
  -------------------
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
    RawFilenameDialog(QWidget *parent,
                      EcProject *ecProject,
                      QStringList *suffixList,
                      QStringList *rawFileList);

signals:
    void updateFileFormatRequest(QString);

public slots:
    void reset();
    void refresh();

private slots:
    void accept() override;
    void updateFormatEdit(int id);
    void updateFileList(const QString& file);

private:
    void createFileExtensionRadioButtons(const QStringList& list);
    QStringList getRawFileTypesAvailable();
    void updateOkButton();
    void populateDialog();
    void createGhgSuffixRadioButtons();
    void removeFileExtensionFromList(const QString &ext);

    QLabel* title;
    QLabel* desc;
    QFrame* radioGroupBox;
    QVBoxLayout* radioGroupBoxLayout;
    QButtonGroup* extRadioGroup;
    QLineEdit* rawFilenameFormatEdit;
    QPushButton* okButton;

    EcProject *ecProject_;
    QStringList fileList_;
    QStringList* ghgSuffixList_;
    QStringList* rawFileList_;
};

#endif // RAWFILENAMEDIALOG_H
