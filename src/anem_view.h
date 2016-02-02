/***************************************************************************
  anem_view.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#ifndef ANEM_VIEW_H
#define ANEM_VIEW_H

#include "anem_tableview.h"

class QAction;
class QContextMenuEvent;

class AnemView : public AnemTableView {
    Q_OBJECT

public:
    explicit AnemView(QWidget *parent);
    ~AnemView();

    int anemCount();

public slots:
    void addAnem();
    void removeAnem();

private:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QAction *addAction_;
    QAction *removeAction_;
    QAction *clearAction_;
};

#endif // ANEM_VIEW_H
