/***************************************************************************
  lineeditandbrowsewidget.cpp
  -------------------
  Widget with combination of line edit and browse button
  -------------------
  Copyright (C) 2016, LI-COR Biosciences
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
