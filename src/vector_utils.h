/***************************************************************************
  vector_utils.h
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

#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <algorithm>
#include <iostream>
#include <vector>

namespace VectorUtils
{
    // python-like arange function
    template<typename T>
    std::vector<T> arange(T start, T stop, T step = 1)
    {
        std::vector<T> values;
        for (T value = start; value < stop; value += step)
        {
            values.push_back(value);
        }
        return values;
    }

    template<typename T>
    void print_partial_vector(std::vector<T> v)
    {
        std::cout << v.at(0) << "\n";
        std::cout << v.at(1) << "\n";
        std::cout << v.at(2) << "\n";
        std::cout << v.at(v.size() - 3) << "\n";
        std::cout << v.at(v.size() - 2) << "\n";
        std::cout << v.at(v.size() - 1) << "\n";
    }

    template<typename T>
    void print_short_vector(std::vector<T> v)
    {
        std::cout << "[";
        for (const auto& value : v)
        {
            std::cout << value << ", ";
        }
        std::cout << "]\n";
    }

    template<typename T>
    void print_vector(std::vector<T> v)
    {
        for (const auto& value : v)
        {
            std::cout << value << "\n";
        }
    }

    // polynomial inversion of a polynomium of degree deg with coefficients coeffs
    // in incremental powers on the range x_range
    std::vector<double> poly_boost(std::vector<double> x_range, std::vector<double> coeffs, int deg);

}  // namespace VectorUtils

#endif  // VECTOR_UTILS_H
