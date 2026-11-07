#include "medaid/integrate.hpp"

#include <cmath>

#include "medaid/errors.hpp"

namespace medaid {

double trapezoid(std::span<const double> t, std::span<const double> y) {
    if (t.size() != y.size()) {
        throw InvalidSeriesError("t and y must have the same length");
    }
    if (t.size() < 2) {
        throw InvalidSeriesError("trapezoid needs at least 2 points");
    }

    double total = 0.0;
    for (std::size_t i = 0; i < t.size(); ++i) {
        if (!std::isfinite(t[i]) || !std::isfinite(y[i])) {
            throw InvalidSeriesError("t and y must not contain NaN or infinite values");
        }
        if (i == 0) {
            continue;
        }
        const double gap = t[i] - t[i - 1];
        if (gap <= 0.0) {
            throw InvalidSeriesError("t must be strictly increasing");
        }
        total += gap * (y[i - 1] + y[i]) / 2.0;
    }
    return total;
}

}  // namespace medaid
