/***************************************************************************
  irga_delegate.h
  ---------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#ifndef IRGA_DELEGATE_H
#define IRGA_DELEGATE_H

#include <QStyledItemDelegate>

class IrgaDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit IrgaDelegate(QObject* parent = nullptr);

    QWidget *createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const override;

private slots:
    void commitAndCloseEditor();
    void commitAndCloseEditor(QObject* editor);

protected:
    bool eventFilter(QObject* editor, QEvent* event) override;
};

#endif // IRGA_DELEGATE_H
