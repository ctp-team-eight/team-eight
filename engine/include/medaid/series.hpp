#pragma once

#include <span>
#include <vector>

namespace medaid {

// One lab or vital sign over time for one patient.
// The engine does not care about units: t can be in any unit, as long as the
// caller uses the same one throughout. The backend passes fractional years.
struct Series {
    std::vector<double> t;
    std::vector<double> y;
};

// Prepares raw readings for the numerical methods:
//   - rejects NaN or infinite values and mismatched lengths
//   - sorts by time
//   - averages readings with exactly the same t, because repeated times would
//     cause a division by zero in methods that divide by the gap between points
//
// Throws InvalidSeriesError on bad input.
Series clean_series(std::span<const double> t, std::span<const double> y);

}  // namespace medaid
