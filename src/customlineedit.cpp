/***************************************************************************
  customlineedit.cpp
  ------------------
  Copyright © 2014-2019, LI-COR Biosciences, Inc. All Rights Reserved.
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

#include "customlineedit.h"

#include <QAction>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

CustomLineEdit::CustomLineEdit(QWidget *parent) :
    QLineEdit(parent),
    action_(nullptr),
    icon_(QIcon()),
    defaultIcon_(QIcon()),
    re_(QString())
{
    action_ = addAction(icon_, QLineEdit::TrailingPosition);

    connect(action_, &QAction::triggered,
            this, &CustomLineEdit::buttonClicked);
    connect(this, &QLineEdit::textChanged,
            this, &CustomLineEdit::updateAction);
}

QIcon CustomLineEdit::icon() const
{
    return icon_;
}

void CustomLineEdit::setIcon(const QIcon &icon)
{
    icon_ = icon;
    action_->setIcon(icon);
}

QIcon CustomLineEdit::defaultIcon() const
{
    return defaultIcon_;
}

void CustomLineEdit::setDefaultIcon(const QIcon &icon)
{
    defaultIcon_ = icon;
}

// NOTE: never used
QString CustomLineEdit::regExp() const
{
    return re_;
}

void CustomLineEdit::setRegExp(const QString &s)
{
    // remove the current validator
    if (s.isEmpty())
    {
        setValidator(nullptr);
        return;
    }

    QRegularExpression re(s);
    setValidator(new QRegularExpressionValidator(re, this));

    re_ = s;
}

QAction *CustomLineEdit::action() const
{
    return action_;
}
