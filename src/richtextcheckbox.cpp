/***************************************************************************
  richtextcheckbox.cpp
  -------------------
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
