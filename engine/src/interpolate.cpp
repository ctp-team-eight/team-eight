#include "medaid/interpolate.hpp"

#include <algorithm>
#include <cstddef>

#include "medaid/errors.hpp"

namespace medaid {

double interpolate_linear(std::span<const double> t, std::span<const double> y, double at) {
    if (t.empty() || t.size() != y.size()) {
        throw InvalidSeriesError("t and y must have the same non-zero length");
    }
    if (!(at >= t.front() && at <= t.back())) {
        throw InvalidSeriesError("at is outside the range of the readings");
    }

    const auto right = std::upper_bound(t.begin(), t.end(), at);
    if (right == t.end()) {
        return y.back();
    }

    const auto i = static_cast<std::size_t>(right - t.begin());
    const double fraction = (at - t[i - 1]) / (t[i] - t[i - 1]);
    return y[i - 1] + fraction * (y[i] - y[i - 1]);
}

}  // namespace medaid
