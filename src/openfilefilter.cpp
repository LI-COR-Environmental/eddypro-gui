/***************************************************************************
  openfilefilter.cpp
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
 ***************************************************************************/

#include "openfilefilter.h"

#include <QFileOpenEvent>

OpenFileFilter::OpenFileFilter(QObject *parent) :
    QObject(parent),
    file_requested_(QString())
{
}

bool OpenFileFilter::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::FileOpen)
    {
        const auto file_event = static_cast<QFileOpenEvent *>(event);
        file_requested_ = file_event->file();
        return true;
    }
    return QObject::eventFilter(object, event);
}

const QString& OpenFileFilter::fileRequested() const
{
    return file_requested_;
}
