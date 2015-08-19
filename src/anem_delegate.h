/***************************************************************************
  anem_delegate.h
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

#ifndef ANEM_DELEGATE_H
#define ANEM_DELEGATE_H

#include <QItemDelegate>

class AnemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit AnemDelegate(QObject* parent = nullptr);
    ~AnemDelegate();

    QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                          const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;

    void setModelData(QWidget* editor, QAbstractItemModel* model,
                      const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const;

private slots:
    void commitAndCloseEditor();
    void commitAndCloseEditor(QObject* editor);

protected:
    bool eventFilter(QObject* editor, QEvent* event);
};

#endif // ANEM_DELEGATE_H
