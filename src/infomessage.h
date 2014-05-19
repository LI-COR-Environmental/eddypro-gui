/***************************************************************************
  infomessages.h
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

#ifndef INFOMESSAGE_H
#define INFOMESSAGE_H

#include <QDialog>
#include <QDialogButtonBox>

class QLabel;
class QCheckBox;

class InfoMessage : public QDialog
{
    Q_OBJECT

public:
    enum type
    {
        RUN_EXPRESS,
        RUN_ADVANCED,
        RUN_RETRIEVER,
        SELECTION_CLEANING,
        SMARTFLUX_CONFIG,
        NOAA_WEBSITE
    };

    explicit InfoMessage(QDialogButtonBox::StandardButtons buttons, QWidget *parent = 0);

    void setTitle(const QString& title);
    void setMessage(const QString& text);
    void setType(int type);
    void setDoNotShowAgainVisible(bool visible);
    void refresh();

signals:

private slots:
    void initialize();
    void onDoNotShowAgainCheckboxToggled_(bool toggled);
    void onOkButtonClicked();
    void onYesButtonClicked();
    void onNoButtonClicked();
    void onCancelButtonClicked();

private:
    QString title_;
    QLabel* message_;
    QCheckBox* doNotShowAgainCheckbox_;
    QDialogButtonBox* buttonBox_;
    int type_;
    bool doNotShoAgainVisible_;
};

#endif // INFOMESSAGE_H
