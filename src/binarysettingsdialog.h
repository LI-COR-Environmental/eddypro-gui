/***************************************************************************
  binarysettingsdialog.h
  ----------------------
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

#ifndef BINARYSETTINGSDIALOG_H
#define BINARYSETTINGSDIALOG_H

#include <QDialog>

////////////////////////////////////////////////////////////////////////////////
/// \file src/binarysettingsdialog.h
/// \brief
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QComboBox;
class QSpinBox;

class ClickLabel;
class EcProject;

/// \class BinarySettingsDialog
/// \brief Class representing the Binary settings dialog
class BinarySettingsDialog : public QDialog
{
    Q_OBJECT

public:
    BinarySettingsDialog(QWidget* parent, EcProject *ecProject);

public slots:
    void close();
    void refresh();

private slots:
    void onBinaryHLinesLabelClicked();
    void onBinaryNBytesLabelClicked();
    void onBinaryClickEolLabel();
    void onBinaryClickEndianessLabel();
    void updateBinaryHLine(int n);
    void updateBinaryEol(int n);
    void updateBinaryNBytes(int n);
    void updateBinaryEndianess(int n);

private:
    ClickLabel *binaryHLinesLabel;
    QSpinBox *binaryHLinesSpin;

    ClickLabel *binaryEolLabel;
    QComboBox *binaryEolCombo;

    ClickLabel *binaryNBytesLabel;
    QSpinBox *binaryNBytesSpin;

    ClickLabel *binaryEndianessLabel;
    QComboBox *binaryEndianessCombo;

    EcProject *ecProject_;

signals:
    void saveRequest();
};

#endif // BINARYSETTINGSDIALOG_H
