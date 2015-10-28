#include "vector_utils.h"

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
