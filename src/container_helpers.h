/***************************************************************************
  container_helpers.h
  -------------------
  Copyright (C) 2014-2015, LI-COR Biosciences
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

#ifndef CONTAINER_HELPERS_H
#define CONTAINER_HELPERS_H

#include <algorithm>

namespace ContainerHelper {

// wrapper around std::equal to compare two containers on the same range
// with a cleaner call
template<typename T>
bool rangeEqual(const T& s1, const T& s2, int first, int last)
{
    return std::equal(s1.begin() + first, s1.begin() + last, s2.begin() + first);
}

}  // namespace ContainerHelper

#endif  // CONTAINER_HELPERS_H
