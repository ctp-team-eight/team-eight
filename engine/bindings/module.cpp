#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

#include "medaid/errors.hpp"
#include "medaid/integrate.hpp"
#include "medaid/interpolate.hpp"
#include "medaid/series.hpp"

namespace nb = nanobind;
using namespace nb::literals;

namespace {

using InputArray = nb::ndarray<const double, nb::ndim<1>, nb::c_contig, nb::device::cpu>;
using OutputArray = nb::ndarray<nb::numpy, double, nb::ndim<1>>;

std::span<const double> as_span(const InputArray& a) { return {a.data(), a.size()}; }

// Copies into a NumPy array that owns its memory, freed when Python is done with it.
OutputArray to_numpy(const std::vector<double>& values) {
    auto* data = new double[values.size()];
    std::copy(values.begin(), values.end(), data);
    nb::capsule owner(data, [](void* p) noexcept { delete[] static_cast<double*>(p); });
    return OutputArray(data, {values.size()}, owner);
}

}  // namespace

NB_MODULE(_core, m) {
    m.doc() = "MedAid numerical engine (C++ core)";

    nb::exception<medaid::InvalidSeriesError>(m, "InvalidSeriesError", PyExc_ValueError);
    nb::exception<medaid::InsufficientDataError>(m, "InsufficientDataError", PyExc_ValueError);

    nb::class_<medaid::ExposureSummary>(m, "ExposureSummary")
        .def_ro("n_points", &medaid::ExposureSummary::n_points)
        .def_ro("duration", &medaid::ExposureSummary::duration)
        .def_ro("cumulative", &medaid::ExposureSummary::cumulative)
        .def_ro("time_weighted_average", &medaid::ExposureSummary::time_weighted_average);

    m.def(
        "clean_series",
        [](InputArray t, InputArray y) {
            const medaid::Series series = medaid::clean_series(as_span(t), as_span(y));
            return nb::make_tuple(to_numpy(series.t), to_numpy(series.y));
        },
        "t"_a, "y"_a, "Validate, sort and merge readings. Returns (t, y).");

    m.def(
        "trapezoid",
        [](InputArray t, InputArray y) { return medaid::trapezoid(as_span(t), as_span(y)); },
        "t"_a, "y"_a, "Integrate y over a strictly increasing, possibly uneven t.");

    m.def(
        "summarize_exposure",
        [](InputArray t, InputArray y, std::size_t min_points) {
            return medaid::summarize_exposure(as_span(t), as_span(y), min_points);
        },
        "t"_a, "y"_a, "min_points"_a = medaid::kDefaultMinPoints,
        "Clean the readings, then return cumulative exposure and time-weighted average.");

    m.def(
        "interpolate_linear",
        [](InputArray t, InputArray y, double at) {
            return medaid::interpolate_linear(as_span(t), as_span(y), at);
        },
        "t"_a, "y"_a, "at"_a, "Estimate y at time `at` with a straight line between readings.");

    m.def(
        "average_between",
        [](InputArray t, InputArray y, double start, double end) {
            return medaid::average_between(as_span(t), as_span(y), start, end);
        },
        "t"_a, "y"_a, "start"_a, "end"_a, "Time-weighted average of y between start and end.");
}
