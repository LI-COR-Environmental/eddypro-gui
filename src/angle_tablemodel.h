/***************************************************************************
  angle_tablemodel.h
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

#ifndef ANGLE_TABLEMODEL_H
#define ANGLE_TABLEMODEL_H

#include <QAbstractTableModel>

#include "angle_item.h"

class AngleTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column { DEGREES };

    explicit AngleTableModel(QObject *parent, QList<AngleItem> *angles);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role=Qt::EditRole) override;
    bool setHeaderData(int, Qt::Orientation,
                       const QVariant&,
                       int = Qt::EditRole) override;

    bool insertRows(int row,
                    int count,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row,
                    int count,
                    const QModelIndex &parent = QModelIndex()) override;

    void clear();
    void flush();
    double angleSum();
    double offset() { return offset_; }
    void setOffset(double alpha);

    void setSkipPruning(bool skip);

private:
    double angleToBeInserted();
    void pruneEmptyRows();

    QList<AngleItem> *angles_;
    double offset_;
    bool skipPruning_;

signals:
    void offsetChanged(double);
    void modified();
};

#endif // ANGLE_TABLEMODEL_H
