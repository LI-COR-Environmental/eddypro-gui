/***************************************************************************
  variable_view.h
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

#ifndef VARIABLE_VIEW_H
#define VARIABLE_VIEW_H

#include "variable_tableview.h"

class QAction;
class QContextMenuEvent;

class VariableView : public VariableTableView {
    Q_OBJECT
public:
    explicit VariableView(QWidget *parent);
    int varCount();

public slots:
    void addVar();
    void removeVar();

private:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QAction *addAction_;
    QAction *removeAction_;
    QAction *clearAction_;
};

#endif // VARIABLE_VIEW_H
