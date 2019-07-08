/***************************************************************************
  dirbrowsewidget.cpp
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

#include "dirbrowsewidget.h"

#include <QFileDialog>
#include <QPushButton>

#include "customdroplineedit.h"
#include "widget_utils.h"

DirBrowseWidget::DirBrowseWidget()
{
    button()->setText(tr("Browse..."));

//#if defined(Q_OS_MACOS)
    lineEdit()->setPlaceholderText(tr("drag and drop here"));
//#endif

    lineEdit()->setAcceptDrops(true);

    connect(lineEdit(), &CustomDropLineEdit::dropped,
            this, &LineEditAndBrowseWidget::pathSelected);
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
