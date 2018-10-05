/***************************************************************************
  richtextcheckbox.cpp
  -------------------
  Checkbox with rich text support
  -------------------
  Copyright (C) 2016-2018, LI-COR Biosciences
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

#include "richtextcheckbox.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextDocument>

#include "clickablelabel.h"
#include "hovercheckbox.h"
#include "widget_utils.h"

RichTextCheckBox::RichTextCheckBox(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);

    checkBox = new HoverCheckBox;

    label = new ClickableLabel;
    label->setTextFormat(Qt::RichText);

    questionMark = new QPushButton;
    questionMark->setObjectName(QLatin1String("questionMarkImg"));
    questionMark->setVisible(false);

    auto container = new QHBoxLayout(this);
    container->addWidget(checkBox);
    container->addWidget(label);
    container->addWidget(questionMark);
    container->setAlignment(Qt::AlignLeft);
    container->setContentsMargins(5, 0, 0, 0);
//    container->setSpacing(0);
//    container->setStretchFactor(label, 0);

    setLayout(container);

    connect(checkBox, &HoverCheckBox::toggled,
            this, &RichTextCheckBox::toggled);
    connect(checkBox, &HoverCheckBox::clicked,
            this, &RichTextCheckBox::clicked);

    connect(label, &ClickableLabel::pressed,
            checkBox, &HoverCheckBox::setPressed);

    connect(label, &ClickableLabel::released,
            checkBox, &HoverCheckBox::toggle);
    connect(label, &ClickableLabel::released,
            checkBox, &HoverCheckBox::clearPressed);
    connect(label, &ClickableLabel::released,
            this, &RichTextCheckBox::clicked);

    connect(label, &ClickableLabel::left,
            checkBox, &HoverCheckBox::clearStates);

    connect(questionMark, &QPushButton::clicked,
            this, &RichTextCheckBox::triggerOnlineHelp);
}

bool RichTextCheckBox::isChecked() const
{
    return checkBox->isChecked();
}

Qt::CheckState RichTextCheckBox::checkState() const
{
    return checkBox->checkState();
}

QString RichTextCheckBox::text() const
{
    return label->text();
}

void RichTextCheckBox::setText(const QString& text)
{
    label->setText(text);
}

// NOTE: never used
QString RichTextCheckBox::plainText() const
{
    QTextDocument doc;
    doc.setHtml(label->text());
    return doc.toPlainText();
}

void RichTextCheckBox::clearStates()
{
    checkBox->clearStates();
}

void RichTextCheckBox::setQuestionMark(const QString &url)
{
    questionMark->setVisible(true);
    questionMarkLink.setUrl(url);
}

void RichTextCheckBox::setCheckState(Qt::CheckState state)
{
    checkBox->setCheckState(state);
}

void RichTextCheckBox::setChecked(bool checked)
{
    checkBox->setChecked(checked);
}

void RichTextCheckBox::enterEvent(QEvent* event)
{
    Q_UNUSED(event)
    checkBox->setHover();
}

void RichTextCheckBox::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    checkBox->clearStates();
}

void RichTextCheckBox::triggerOnlineHelp()
{
    WidgetUtils::showHelp(questionMarkLink);
}
