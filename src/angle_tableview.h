/***************************************************************************
  angle_tableview.h
  -------------------
  Copyright (C) 2012-2018, LI-COR Biosciences
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

#ifndef ANGLE_TABLEVIEW_H
#define ANGLE_TABLEVIEW_H

#include <QTableView>

class AngleTableView : public QTableView
{
    Q_OBJECT

public:
    explicit AngleTableView(QWidget* parent = nullptr);
    QSize sizeHint() const override;
    int sizeHintForRow(int row) const override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // ANGLE_TABLEVIEW_H
