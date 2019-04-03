/***************************************************************************
  lineeditandbrowsewidget.cpp
  ---------------------------
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

#include "lineeditandbrowsewidget.h"

#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QPushButton>

#include "customdroplineedit.h"
#include "widget_utils.h"

LineEditAndBrowseWidget::LineEditAndBrowseWidget(QWidget *parent) :
    QWidget(parent),
    lineEdit_{},
    button_{},
    dialogTitle_(QString()),
    dialogDir_(QString())
{
    lineEdit_ = new CustomDropLineEdit;
    lineEdit_->setReadOnly(true);
    lineEdit_->setProperty("loadEdit", true);

    button_ = new QPushButton;
    button_->setProperty("loadButton", true);

    auto container = new QHBoxLayout(this);
    container->addWidget(lineEdit_);
    container->addWidget(button_);
    container->setStretch(2, 1);
    container->setContentsMargins(0, 0, 0, 0);
    container->setSpacing(0);

    setLayout(container);

    connect(lineEdit_, &CustomClearLineEdit::buttonClicked,
            this, &LineEditAndBrowseWidget::clearRequested);
    connect(lineEdit_, &QLineEdit::textChanged,
            this, &LineEditAndBrowseWidget::pathChanged);
    connect(lineEdit_, &QLineEdit::textChanged,
            this, &LineEditAndBrowseWidget::updatePathTooltip);
    connect(button_, &QPushButton::clicked,
            this, &LineEditAndBrowseWidget::onButtonClick);
}

void LineEditAndBrowseWidget::clear()
{
    lineEdit_->clear();
}

void LineEditAndBrowseWidget::setEnabled(bool enable)
{
    lineEdit_->setEnabled(enable);
    button_->setEnabled(enable);
}

void LineEditAndBrowseWidget::setToolTip(const QString &text)
{
    lineEdit_->setToolTip(text);
    button_->setToolTip(text);
}

QString LineEditAndBrowseWidget::toolTip() const
{
    return lineEdit_->toolTip();
}

void LineEditAndBrowseWidget::setButtonText(const QString& text)
{
    button_->setText(text);
}

// NOTE: never used
QString LineEditAndBrowseWidget::buttonText() const
{
    return button_->text();
}

void LineEditAndBrowseWidget::setText(const QString &text)
{
    lineEdit_->setText(text);
}

void LineEditAndBrowseWidget::setPath(const QString &path)
{
    QFileInfo filePath(path);
    QString canonicalFilePath = filePath.canonicalFilePath();
    lineEdit_->setText(QDir::toNativeSeparators(canonicalFilePath));
}

void LineEditAndBrowseWidget::disableClearAction() const
{
    lineEdit_->setDisconnectedAction();
}

void LineEditAndBrowseWidget::disableClickAction() const
{
    disconnect(button_, &QPushButton::clicked,
               this, &LineEditAndBrowseWidget::onButtonClick);
}

int LineEditAndBrowseWidget::returnLineEditWidth() const
{
    return lineEdit_->width();
}

void LineEditAndBrowseWidget::focusAndSelect() const
{
    lineEdit_->setFocus();
    lineEdit_->selectAll();
}

CustomDropLineEdit *LineEditAndBrowseWidget::lineEdit() const
{
    return lineEdit_;
}

QPushButton *LineEditAndBrowseWidget::button() const
{
    return button_;
}

QString LineEditAndBrowseWidget::text() const
{
    return lineEdit_->text();
}

QString LineEditAndBrowseWidget::path() const
{
    return text();
}

void LineEditAndBrowseWidget::updatePathTooltip()
{
    WidgetUtils::updateLineEditToolip(lineEdit_);
}
