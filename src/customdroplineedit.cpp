/***************************************************************************
  customdroplineedit.cpp
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
