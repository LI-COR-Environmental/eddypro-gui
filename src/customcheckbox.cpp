/***************************************************************************
  customcheckbox.cpp
  -------------------
  Copyright (C) 2015, LI-COR Biosciences
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
 ***************************************************************************/

#include "customcheckbox.h"

#include <QCheckBox>
#include <QHBoxLayout>

#include "clickablelabel.h"
#include "hovercheckbox.h"

CustomCheckBox::CustomCheckBox(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_Hover, true);

    checkbox = new HoverCheckBox;
    label = new ClickableLabel;

    auto container = new QHBoxLayout(this);
    container->addWidget(checkbox);
    container->addWidget(label);
    container->addStretch();
    container->setContentsMargins(0, 0, 11, 0);
    container->setSpacing(8);

    setLayout(container);

    connect(checkbox, &HoverCheckBox::toggled,
            this, &CustomCheckBox::toggled);

    connect(label, SIGNAL(released()), checkbox, SLOT(toggle()));
    connect(label, SIGNAL(released()), checkbox, SLOT(clearPressed()));
    connect(label, SIGNAL(pressed()), checkbox, SLOT(setPressed()));
    connect(label, SIGNAL(left()), checkbox, SLOT(clearStates()));
    connect(label, SIGNAL(released()), this, SIGNAL(clicked()));
}

CustomCheckBox::~CustomCheckBox()
{
}

void CustomCheckBox::setLabel(const QString &text)
{
    label->setText(text);
}

void CustomCheckBox::setChecked(bool checked)
{
    checkbox->setChecked(checked);
}

void CustomCheckBox::enterEvent(QEvent *event)
{
    checkbox->setHover();
    QWidget::enterEvent(event);
}

void CustomCheckBox::leaveEvent(QEvent *event)
{
    checkbox->clearStates();
    QWidget::leaveEvent(event);
}
