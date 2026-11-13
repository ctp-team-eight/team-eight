#pragma once

#include <cstddef>
#include <span>

namespace medaid {

inline constexpr std::size_t kDefaultMinPoints = 3;

struct ExposureSummary {
    std::size_t n_points;          // readings left after cleaning
    double duration;               // last t minus first t
    double cumulative;             // integral of y over t, in y-units * t-units
    double time_weighted_average;  // cumulative / duration
};

// Trapezoid rule on an uneven grid: the sum of (t[i+1] - t[i]) * (y[i] + y[i+1]) / 2.
//
// t must be finite and strictly increasing, and y must be finite.
// Throws InvalidSeriesError otherwise, or if there are fewer than 2 points.
double trapezoid(std::span<const double> t, std::span<const double> y);

// Cleans the readings, then calculates exposure with the trapezoid rule.
//
// Throws InvalidSeriesError on bad input, and InsufficientDataError if fewer
// than min_points readings remain after cleaning.
ExposureSummary summarize_exposure(std::span<const double> t, std::span<const double> y,
                                   std::size_t min_points = kDefaultMinPoints);

}  // namespace medaid
