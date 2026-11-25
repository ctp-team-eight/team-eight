import numpy as np
import pytest
from scipy.integrate import trapezoid as scipy_trapezoid

import medaid_engine as engine


def test_trapezoid_matches_scipy_on_uneven_grids():
    rng = np.random.default_rng(8)
    for n in (2, 3, 12, 500):
        t = np.cumsum(rng.uniform(0.01, 2.0, n))
        y = rng.normal(6.0, 1.5, n)
        assert engine.trapezoid(t, y) == pytest.approx(scipy_trapezoid(y, t), rel=1e-12)


def test_clean_series_returns_sorted_merged_numpy_arrays():
    t, y = engine.clean_series(np.array([2.0, 1.0, 2.0]), np.array([10.0, 5.0, 20.0]))
    assert isinstance(t, np.ndarray)
    assert t.tolist() == [1.0, 2.0]
    assert y.tolist() == [5.0, 15.0]


def test_clean_series_arrays_outlive_the_inputs():
    t, y = engine.clean_series(np.array([1.0, 2.0]), np.array([3.0, 4.0]))
    garbage = [np.arange(1000.0) for _ in range(100)]
    assert len(garbage) == 100
    assert t.tolist() == [1.0, 2.0]
    assert y.tolist() == [3.0, 4.0]


def test_summarize_exposure_fields():
    result = engine.summarize_exposure(np.array([0.0, 1.0, 2.0]), np.array([5.0, 5.0, 9.0]))
    assert result.n_points == 3
    assert result.duration == pytest.approx(2.0)
    assert result.cumulative == pytest.approx(12.0)
    assert result.time_weighted_average == pytest.approx(6.0)


def test_summarize_exposure_accepts_plain_lists_and_integers():
    result = engine.summarize_exposure([0, 1, 2], [5, 5, 9])
    assert result.cumulative == pytest.approx(12.0)


def test_missing_values_raise_invalid_series():
    with pytest.raises(engine.InvalidSeriesError):
        engine.summarize_exposure([0.0, 1.0, 2.0], [5.0, None, 9.0])


def test_readme_example():
    result = engine.summarize_exposure([0.0, 0.5, 1.5, 3.0], [6.1, 6.4, 6.8, 6.5])
    assert round(result.cumulative, 2) == 19.7
    assert round(result.time_weighted_average, 2) == 6.57


def test_too_few_readings_raises_insufficient_data():
    with pytest.raises(engine.InsufficientDataError):
        engine.summarize_exposure(np.array([0.0, 1.0]), np.array([5.0, 6.0]))


def test_nan_raises_invalid_series():
    with pytest.raises(engine.InvalidSeriesError):
        engine.summarize_exposure(np.array([0.0, np.nan, 2.0]), np.array([5.0, 6.0, 7.0]))


def test_engine_errors_are_value_errors():
    assert issubclass(engine.InvalidSeriesError, ValueError)
    assert issubclass(engine.InsufficientDataError, ValueError)


def test_interpolate_linear():
    assert engine.interpolate_linear([0, 1, 3], [4, 6, 10], 2) == pytest.approx(8.0)


def test_average_between_matches_a_fine_grid():
    t = [0.0, 1.0, 3.0]
    y = [4.0, 6.0, 10.0]
    grid = np.linspace(0.5, 2.0, 200001)
    expected = scipy_trapezoid(np.interp(grid, t, y), grid) / 1.5
    assert engine.average_between(t, y, 0.5, 2.0) == pytest.approx(expected)
