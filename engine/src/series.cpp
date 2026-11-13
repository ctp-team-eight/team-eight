#include "medaid/series.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>

#include "medaid/errors.hpp"

namespace medaid {

Series clean_series(std::span<const double> t, std::span<const double> y) {
    if (t.size() != y.size()) {
        throw InvalidSeriesError("t and y must have the same length");
    }
    for (std::size_t i = 0; i < t.size(); ++i) {
        if (!std::isfinite(t[i]) || !std::isfinite(y[i])) {
            throw InvalidSeriesError("t and y must not contain NaN or infinite values");
        }
    }

    std::vector<std::size_t> order(t.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::stable_sort(order.begin(), order.end(),
                     [&](std::size_t a, std::size_t b) { return t[a] < t[b]; });

    Series out;
    out.t.reserve(t.size());
    out.y.reserve(t.size());

    for (std::size_t i = 0; i < order.size();) {
        const double current_t = t[order[i]];
        double sum = 0.0;
        std::size_t count = 0;
        for (; i < order.size() && t[order[i]] == current_t; ++i) {
            sum += y[order[i]];
            ++count;
        }
        out.t.push_back(current_t);
        out.y.push_back(sum / static_cast<double>(count));
    }
    return out;
}

}  // namespace medaid
