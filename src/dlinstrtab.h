/***************************************************************************
  dlinstrtab.h
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
 ***************************************************************************/

#ifndef DLINSTRTAB_H
#define DLINSTRTAB_H

#include <QWidget>

class QModelIndex;
class QToolButton;

class AnemDelegate;
class AnemModel;
class AnemView;
class DlProject;
class IrgaDelegate;
class IrgaModel;
class IrgaView;

class DlInstrTab : public QWidget {
    Q_OBJECT

public:
    DlInstrTab(QWidget *parent, DlProject *dlProject);
    virtual ~DlInstrTab();

    void reset();
    void refresh();

    AnemView *anemView_;
    IrgaView *irgaView_;

private slots:
    void updateScrollBars();

private:
    AnemModel *anemModel_;
    AnemDelegate *anemDelegate_;

    IrgaModel *irgaModel_;
    IrgaDelegate *irgaDelegate_;

    DlProject *dlProject_;
};

#endif // DLINSTRTAB_H
