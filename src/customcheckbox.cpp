/***************************************************************************
  customcheckbox.cpp
  ------------------
  Copyright © 2016-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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
 ***************************************************************************/

#include "customcheckbox.h"

#include <QCheckBox>
#include <QHBoxLayout>

#include "clickablelabel.h"
#include "hovercheckbox.h"

CustomCheckBox::CustomCheckBox(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_Hover);

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

// NOTE: never used
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
