/***************************************************************************
  dirbrowsewidget.cpp
  -------------------
  Variation of lineedit and browse widget
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
****************************************************************************/

#include "dirbrowsewidget.h"

#include <QFileDialog>
#include <QPushButton>

#include "customdroplineedit.h"
#include "widget_utils.h"

DirBrowseWidget::DirBrowseWidget()
{
    button()->setText(tr("Browse..."));

#if defined(Q_OS_MAC)
    lineEdit()->setPlaceholderText(tr("drag and drop here"));
#endif

    lineEdit()->setAcceptDrops(true);

    connect(lineEdit(), &CustomDropLineEdit::dropped,
            this, &LineEditAndBrowseWidget::pathSelected);
}

DirBrowseWidget::~DirBrowseWidget()
{
}

bool DirBrowseWidget::isReadOnly() const
{
    return lineEdit()->isReadOnly();
}

void DirBrowseWidget::setReadOnly(bool on)
{
    lineEdit()->setReadOnly(on);
}

void DirBrowseWidget::onButtonClick()
{
    QString dirname = QFileDialog::getExistingDirectory(this,
                          dialogTitle(),
                          WidgetUtils::getSearchPathHint());

    if (dirname.isEmpty()) { return; }

    emit pathSelected(dirname);
}
