/***************************************************************************
  variable_model.h
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

#ifndef VARIABLE_MODEL_H
#define VARIABLE_MODEL_H

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QStringList>
#include <QVariant>

#include "variable_desc.h" // NOTE: for VariableDescList, maybe to fix

class VariableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    // variable columns definition
    enum VarItem
    {
        IGNORE,
        NUMERIC,
        VARIABLE,
        INSTRUMENT,
        MEASURETYPE,
        INPUTUNIT,
        CONVERSIONTYPE,
        OUTPUTUNIT,
        AVALUE,
        BVALUE,
        NOMTIMELAG,
        MINTIMELAG,
        MAXTIMELAG,
        VARNUMCOLS
    };

    VariableModel(QObject *parent, VariableDescList *list);
    ~VariableModel();

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

    const QStringList instrModels() const;
    void setInstrModels(const QStringList& list);

signals:
    void modified();

private:
    VariableDescList *list_;
    QStringList instrModelList_;

private slots:
    void triggerSetData() const;
};

#endif // VARIABLE_MODEL_H
