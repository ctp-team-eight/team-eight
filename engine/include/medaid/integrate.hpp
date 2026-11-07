#pragma once

#include <cstddef>
#include <span>

namespace medaid {

// Trapezoid rule on an uneven grid: the sum of (t[i+1] - t[i]) * (y[i] + y[i+1]) / 2.
//
// t must be finite and strictly increasing, and y must be finite.
// Throws InvalidSeriesError otherwise, or if there are fewer than 2 points.
double trapezoid(std::span<const double> t, std::span<const double> y);

}  // namespace medaid
