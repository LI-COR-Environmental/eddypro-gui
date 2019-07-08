/***************************************************************************
  customdroplineedit.cpp
  ----------------------
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

#include "customdroplineedit.h"

#include <QDir>
#include <QDragEnterEvent>
#include <QFileInfo>
#include <QMimeData>

// NOTE: never used
bool CustomDropLineEdit::canBeFile() const
{
    return canBeFile_;
}

void CustomDropLineEdit::setCanBeFile(bool on)
{
    canBeFile_ = on;
}

void CustomDropLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText()
        || event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void CustomDropLineEdit::dropEvent(QDropEvent *event)
{
    QString dropPath(QUrl::fromUserInput(event->mimeData()->text())
                     .toDisplayString(QUrl::PreferLocalFile
                                      | QUrl::NormalizePathSegments));

    // remove leading slash on Windows
#ifdef Q_OS_WIN
    if (dropPath.at(0) == QChar(QLatin1Char('/')))
    {
        dropPath = dropPath.mid(1);
    }
#endif

    QFileInfo dropPathInfo(dropPath);
    if (!QFileInfo::exists(dropPath))
    {
        return;
    }

    if ((canBeFile_ && dropPathInfo.isFile()) || dropPathInfo.isDir())
    {
        const auto path = dropPathInfo.absoluteFilePath();
//        setText(QDir::toNativeSeparators(path));
//        emit QLineEdit::textChanged(path);
        emit dropped(path);
        event->acceptProposedAction();
    }
}
