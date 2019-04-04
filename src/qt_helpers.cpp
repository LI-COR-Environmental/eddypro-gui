/***************************************************************************
  qt_helpers.cpp
  --------------
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
****************************************************************************/

#include "qt_helpers.h"

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStringList>

// NOTE: fix for Qt5
// from http://blog.inventic.eu/2014/08/qt5-application-crashed-with-error-0xc0000005/
// see https://bugreports.qt-project.org/browse/QTBUG-40738
void QtHelper::prependApplicationPathToLibraryPaths(const QString& app_name)
{
    // get application path
    QString executablePath;
#if defined(Q_OS_WIN)
    executablePath = app_name.mid(0, app_name.lastIndexOf(QStringLiteral("\\")));
#elif defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    executablePath = app_name.mid(0, app_name.lastIndexOf(QLatin1String("/")));
#endif

    // get library paths
    QString installPathPlugins = QLibraryInfo::location(QLibraryInfo::PluginsPath);

    // set paths prepending the application path
    QCoreApplication::setLibraryPaths(QStringList() << executablePath << installPathPlugins);
}
