/***************************************************************************
  hovercheckbox.h
  -------------------
  Checkbox with hover effect
  -------------------
  Copyright (C) 2015, LI-COR Biosciences
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

#ifndef HOVERCHECKBOX_H
#define HOVERCHECKBOX_H

#include <QCheckBox>

class HoverCheckBox : public QCheckBox
{
    Q_OBJECT

public:
    explicit HoverCheckBox(QWidget *parent = 0);

private:
    bool isHover;
    bool isPressed;

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

public slots:
    void setHover();
    void setPressed();
    void clearPressed();
    void clearStates();
};

#endif  // HOVERCHECKBOX_H
