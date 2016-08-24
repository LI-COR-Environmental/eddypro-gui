/***************************************************************************
  variable_tableview.h
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

#ifndef VARIABLE_TABLEVIEW_H
#define VARIABLE_TABLEVIEW_H

#include <QTableView>

class CustomHeader;

class VariableTableView : public QTableView
{
    Q_OBJECT
public:
    explicit VariableTableView(QWidget* parent = nullptr);
    ~VariableTableView();

signals:

public slots:
    void hHeaderClicked(int section);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void closeEditor(QWidget * editor, QAbstractItemDelegate::EndEditHint hint) Q_DECL_OVERRIDE;

private:
    CustomHeader *m_header;

    void previousEditableIndex(const QModelIndex &originalIndex);
    void nextEditableIndex(const QModelIndex &originalIndex);
};

#endif // VARIABLE_TABLEVIEW_H
