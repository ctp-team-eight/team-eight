"""MedAid numerical engine. The calculations are implemented in C++ (see ../../src)."""

import numpy as np

from . import _core
from ._core import ExposureSummary, InsufficientDataError, InvalidSeriesError

__all__ = [
    "ExposureSummary",
    "InsufficientDataError",
    "InvalidSeriesError",
    "average_between",
    "clean_series",
    "interpolate_linear",
    "summarize_exposure",
    "trapezoid",
]


def _as_array(values):
    # The C++ layer only accepts float64 arrays, so lists and integers convert here.
    return np.asarray(values, dtype=np.float64)


def clean_series(t, y):
    """Validate, sort and merge readings. Returns (t, y) as NumPy arrays."""
    return _core.clean_series(_as_array(t), _as_array(y))


def trapezoid(t, y):
    """Integrate y over a strictly increasing, possibly uneven t."""
    return _core.trapezoid(_as_array(t), _as_array(y))


def summarize_exposure(t, y, min_points=3):
    """Clean the readings, then return cumulative exposure and time-weighted average."""
    return _core.summarize_exposure(_as_array(t), _as_array(y), min_points)


def interpolate_linear(t, y, at):
    """Estimate y at time `at` with a straight line between readings."""
    return _core.interpolate_linear(_as_array(t), _as_array(y), at)


def average_between(t, y, start, end):
    """Time-weighted average of y between start and end."""
    return _core.average_between(_as_array(t), _as_array(y), start, end)
