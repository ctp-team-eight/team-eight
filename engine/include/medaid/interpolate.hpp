#pragma once

#include <span>

namespace medaid {

// Estimates y at time `at` using a straight line between the two nearest readings.
//
// t must be strictly increasing, as clean_series returns it. Throws
// InvalidSeriesError if `at` is outside the first and last readings, because
// the engine never guesses beyond the data.
double interpolate_linear(std::span<const double> t, std::span<const double> y, double at);

}  // namespace medaid
