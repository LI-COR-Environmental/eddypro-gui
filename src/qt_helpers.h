/***************************************************************************
  qt_helpers.h
  -------------------
  Copyright (C) 2014, LI-COR Biosciences
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

#ifndef QT_HELPERS_H
#define QT_HELPERS_H

////////////////////////////////////////////////////////////////////////////////
/// \file src/qt_helpers.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QString;

/// \namespace QtHelp
/// \brief Container class with Qt helper functions for common use
namespace QtHelper
{
    void prependApplicationPathToLibraryPaths(const QString& app_name);
} // QtHelper

#endif // QT_HELPERS_H
