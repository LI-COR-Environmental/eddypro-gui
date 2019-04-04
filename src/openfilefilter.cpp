/***************************************************************************
  openfilefilter.cpp
  ------------------
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
        const auto file_event = dynamic_cast<QFileOpenEvent *>(event);
        file_requested_ = file_event->file();
        return true;
    }
    return QObject::eventFilter(object, event);
}

const QString& OpenFileFilter::fileRequested() const
{
    return file_requested_;
}
