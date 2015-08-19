/***************************************************************************
  irga_model.h
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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

#ifndef IRGA_MODEL_H
#define IRGA_MODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QVariant>

#include "irga_desc.h" // for IrgaDescList typedef

class IrgaModel : public QAbstractTableModel {
    Q_OBJECT
public:
    // model columns
    enum IrgaItem
    {
        MANUFACTURER,
        MODEL,
        SWVERSION,
        ID,
//        HEIGHT,
        TUBELENGTH,
        TUBEDIAMETER,
        TUBEFLOWRATE,
        TUBENSEPARATION,
        TUBEESEPARATION,
        TUBEVSEPARATION,
        VPATHLENGTH,
        HPATHLENGTH,
        TAU,
        KWATER,
        KOXYGEN,
        IRGANUMCOLS
    };

    IrgaModel(QObject *parent, IrgaDescList *list);
    ~IrgaModel();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool insertColumns(int column,
                       int count,
                       const QModelIndex& parent = QModelIndex());
    bool removeColumns(int column,
                       int count,
                       const QModelIndex& parent = QModelIndex());
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    void flush();

signals:
    void modified();

private:
    IrgaDescList *list_;
};

#endif // IRGA_MODEL_H
