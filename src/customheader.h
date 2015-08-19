/***************************************************************************
  customheader.h
  -------------------
  Custom header for table view classes
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#ifndef CUSTOMHEADER_H
#define CUSTOMHEADER_H

#include <QWidget>

#include "clicklabel.h"

class QGridLayout;

class CustomHeader : public QWidget
{
Q_OBJECT

public:
    enum class  QuestionMarkHint { NoQuestionMark, QuestionMark };

    explicit CustomHeader(QWidget* parent = nullptr);
    void addSection(const QString& txt,
                    const QString& tooltipTxt,
                    QuestionMarkHint questionMark = QuestionMarkHint::NoQuestionMark,
                    ClickLabel::HeaderData headerData = ClickLabel::NoHeader);
    int sectionCount();

signals:

private slots:
    void onlineHelpTrigger();

private:
    QGridLayout *layout;
};

#endif // CUSTOMHEADER_H
