/***************************************************************************
  angle_tablemodel.h
  ------------------
  Copyright © 2012-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/

#ifndef WINDFILTER_TABLEMODEL_H
#define WINDFILTER_TABLEMODEL_H

#include <QAbstractTableModel>

#include "sector_item.h"

class WindFilterTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column { START_ANGLE, END_ANGLE };

    WindFilterTableModel(QObject *parent, QList<SectorItem> *sectors);

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

private:
    double startAngleToBeInserted();
    double endAngleToBeInserted();

    QList<SectorItem> *sectors_;

signals:
    void modified();
};

#endif // WINDFILTER_TABLEMODEL_H
