/***************************************************************************
  filebrowsewidget.cpp
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

#include "filebrowsewidget.h"

#include <QFileDialog>
#include <QPushButton>

#include "customdroplineedit.h"
#include "widget_utils.h"

FileBrowseWidget::FileBrowseWidget() :
    dialogFilter_(QString())
{
    button()->setText(tr("Load..."));
    lineEdit()->setCanBeFile(true);

#if defined(Q_OS_MAC)
    lineEdit()->setPlaceholderText(tr("drag and drop here"));
#endif

    lineEdit()->setAcceptDrops(true);

    connect(lineEdit(), &CustomDropLineEdit::dropped,
            this, &LineEditAndBrowseWidget::pathSelected);
}

FileBrowseWidget::~FileBrowseWidget()
{
}

void FileBrowseWidget::onButtonClick()
{
    QString filename = QFileDialog::getOpenFileName(this,
                           dialogTitle(),
                           WidgetUtils::getSearchPathHint(),
                           dialogFilter());

    if (filename.isEmpty()) { return; }

    emit pathSelected(filename);
}
