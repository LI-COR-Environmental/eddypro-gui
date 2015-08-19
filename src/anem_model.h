/***************************************************************************
  anem_model.h
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

#ifndef ANEM_MODEL_H
#define ANEM_MODEL_H

#include <QAbstractTableModel>
#include <QIcon>
#include <QModelIndex>
#include <QVariant>

#include "anem_desc.h" // for AnemDescList typedef

class AnemModel : public QAbstractTableModel {
    Q_OBJECT
public:
    // model columns
    enum AnemItem
    {
        MANUFACTURER,
        MODEL,
        ID,
        HEIGHT,
        WINDFORMAT,
        NORTHALIGNMENT,
        NORTHOFFSET,
        NSEPARATION,
        ESEPARATION,
        VSEPARATION,
        VPATHLENGTH,
        HPATHLENGTH,
        TAU,
        ANEMNUMCOLS
    };

    AnemModel(QObject *parent, AnemDescList *list);
    ~AnemModel();

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
    AnemDescList *list_;
};

#endif // ANEM_MODEL_H

