#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "polyfit.hpp"

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
    std::vector<double> poly_boost(std::vector<double> x_range, std::vector<double> coeffs, int deg)
    {
        // evaluate the polynomium on the range x_range
        auto y = polyval(coeffs, x_range);

        // least-square fitting to obtain the inverse polynomium
        return polyfit(y, x_range, deg);
    }

} // VectorUtils

#endif // VECTOR_UTILS_H

