/***************************************************************************
  binarysettingsdialog.h
  -------------------
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

#ifndef BINARYSETTINGSDIALOG_H
#define BINARYSETTINGSDIALOG_H

#include <QDialog>

class ClickLabel;
class QSpinBox;
class QComboBox;
class EcProject;

class BinarySettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BinarySettingsDialog(QWidget* parent, EcProject *ecProject);
    ~BinarySettingsDialog();

public slots:
    void close();
    void initialize();
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
