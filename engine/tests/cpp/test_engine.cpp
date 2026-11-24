#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <limits>
#include <vector>

#include "medaid/errors.hpp"
#include "medaid/integrate.hpp"
#include "medaid/interpolate.hpp"
#include "medaid/series.hpp"

using Catch::Approx;
using Values = std::vector<double>;

TEST_CASE("clean_series sorts by time") {
    const Values t{3.0, 1.0, 2.0};
    const Values y{30.0, 10.0, 20.0};
    const medaid::Series s = medaid::clean_series(t, y);
    CHECK(s.t == Values{1.0, 2.0, 3.0});
    CHECK(s.y == Values{10.0, 20.0, 30.0});
}

TEST_CASE("clean_series averages readings at the same time") {
    const Values t{1.0, 2.0, 1.0, 1.0};
    const Values y{10.0, 50.0, 20.0, 30.0};
    const medaid::Series s = medaid::clean_series(t, y);
    CHECK(s.t == Values{1.0, 2.0});
    CHECK(s.y == Values{20.0, 50.0});
}

TEST_CASE("clean_series rejects bad input") {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    const Values good{1.0, 2.0};
    CHECK_THROWS_AS(medaid::clean_series(good, Values{1.0}), medaid::InvalidSeriesError);
    CHECK_THROWS_AS(medaid::clean_series(Values{1.0, nan}, good), medaid::InvalidSeriesError);
    CHECK_THROWS_AS(medaid::clean_series(good, Values{inf, 1.0}), medaid::InvalidSeriesError);
}


TEST_CASE("trapezoid is exact for a straight line on an uneven grid") {
    const Values t{0.0, 0.5, 2.0, 3.0, 7.0};
    Values y;
    for (double x : t) {
        y.push_back(2.0 * x + 1.0);
    }
    CHECK(medaid::trapezoid(t, y) == Approx(56.0));
}

TEST_CASE("trapezoid error shrinks about 4x when the spacing halves") {
    const auto error_with = [](int intervals) {
        Values t;
        Values y;
        for (int i = 0; i <= intervals; ++i) {
            const double x = 2.0 * i / intervals;
            t.push_back(x);
            y.push_back(std::exp(x));
        }
        return std::abs(medaid::trapezoid(t, y) - (std::exp(2.0) - 1.0));
    };
    CHECK(error_with(100) / error_with(200) == Approx(4.0).margin(0.01));
}

TEST_CASE("trapezoid rejects times that are not strictly increasing") {
    CHECK_THROWS_AS(medaid::trapezoid(Values{1.0, 1.0}, Values{1.0, 2.0}),
                    medaid::InvalidSeriesError);
    CHECK_THROWS_AS(medaid::trapezoid(Values{2.0, 1.0}, Values{1.0, 2.0}),
                    medaid::InvalidSeriesError);
    CHECK_THROWS_AS(medaid::trapezoid(Values{1.0}, Values{1.0}), medaid::InvalidSeriesError);
}


TEST_CASE("summarize_exposure calculates a time-weighted average") {
    // 5.0 for one year, then a rise to 9.0 over one more year.
    const Values t{0.0, 1.0, 2.0};
    const Values y{5.0, 5.0, 9.0};
    const medaid::ExposureSummary s = medaid::summarize_exposure(t, y);
    CHECK(s.n_points == 3);
    CHECK(s.duration == Approx(2.0));
    CHECK(s.cumulative == Approx(12.0));
    CHECK(s.time_weighted_average == Approx(6.0));
}

TEST_CASE("summarize_exposure counts readings after merging") {
    const Values t{1.0, 1.0, 2.0};
    const Values y{5.0, 7.0, 6.0};
    CHECK_THROWS_AS(medaid::summarize_exposure(t, y), medaid::InsufficientDataError);
    CHECK(medaid::summarize_exposure(t, y, 2).n_points == 2);
}

TEST_CASE("summarize_exposure never divides by a zero duration") {
    const Values t{1.0, 1.0};
    const Values y{5.0, 7.0};
    CHECK_THROWS_AS(medaid::summarize_exposure(t, y, 1), medaid::InsufficientDataError);
    CHECK_THROWS_AS(medaid::summarize_exposure(Values{}, Values{}, 0),
                    medaid::InsufficientDataError);
}


TEST_CASE("interpolate_linear estimates between readings") {
    const Values t{0.0, 1.0, 3.0};
    const Values y{4.0, 6.0, 10.0};
    CHECK(medaid::interpolate_linear(t, y, 2.0) == Approx(8.0));
    CHECK(medaid::interpolate_linear(t, y, 1.0) == Approx(6.0));
    CHECK(medaid::interpolate_linear(t, y, 3.0) == Approx(10.0));
    CHECK_THROWS_AS(medaid::interpolate_linear(t, y, 3.5), medaid::InvalidSeriesError);
}

TEST_CASE("average_between handles a range that starts and ends between readings") {
    const Values t{0.0, 1.0, 3.0};
    const Values y{4.0, 6.0, 10.0};
    CHECK(medaid::average_between(t, y, 0.0, 2.0) == Approx(6.0));
    CHECK(medaid::average_between(t, y, 0.5, 2.0) == Approx(6.5));
    CHECK(medaid::average_between(t, y, 1.5, 2.5) == Approx(8.0));
    CHECK_THROWS_AS(medaid::average_between(t, y, 2.0, 2.0), medaid::InvalidSeriesError);
    CHECK_THROWS_AS(medaid::average_between(t, y, 0.0, 4.0), medaid::InvalidSeriesError);
}
