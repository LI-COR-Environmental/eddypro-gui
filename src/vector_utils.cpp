/***************************************************************************
  vector_utils.cpp
  -------------------
  Copyright (C) 2016, LI-COR Biosciences
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

#include "vector_utils.h"

#include <vector>

#include "polyfit.hpp"

// polynomial inversion of a polynomium of degree deg with coefficients coeffs
// in incremental powers on the range x_range
std::vector<double> VectorUtils::poly_boost(std::vector<double> x_range, std::vector<double> coeffs, int deg)
{
    // evaluate the polynomium on the range x_range
    auto y = polyval(coeffs, x_range);

    // least-square fitting to obtain the inverse polynomium
    return polyfit(y, x_range, deg);
}
