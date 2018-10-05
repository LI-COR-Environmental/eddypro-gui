/***************************************************************************
  customcheckbox.h
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#ifndef CUSTOMCHECKBOX_H
#define CUSTOMCHECKBOX_H

#include <QWidget>

class HoverCheckBox;
class ClickableLabel;

class CustomCheckBox : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCheckBox(QWidget *parent = nullptr);

    void setLabel(const QString& text);

signals:
    void toggled(bool checked);

public slots:
    void setChecked(bool checked);

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    HoverCheckBox* checkbox;
    ClickableLabel* label;
};

#endif  // CUSTOMCHECKBOX_H
