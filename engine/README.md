# MedAid Engine

The C++20 library behind MedAid's health trend calculations. Python calls it through [nanobind](https://nanobind.readthedocs.io/) bindings.

Lab results arrive at uneven intervals, so a plain average can mislead. The engine calculates values over time instead, using the actual gaps between readings.

## What it does

| Function | What it does |
|---|---|
| `clean_series(t, y)` | Rejects NaN or infinite values, sorts by time and averages readings with the same time |
| `trapezoid(t, y)` | Integrates `y` over `t` on an uneven grid |
| `summarize_exposure(t, y, min_points=3)` | Runs both of the above and returns `n_points`, `duration`, `cumulative` and `time_weighted_average` |
| `interpolate_linear(t, y, at)` | Estimates `y` at a time between readings, using a straight line |
| `average_between(t, y, start, end)` | Returns the time-weighted average between two times, such as one calendar year |

## Setup

You need Python 3.10+, CMake 3.20+ and a C++20 compiler. Run these from the `engine/` folder:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install ".[test]"
pytest
```

Run `pip install ".[test]"` again after changing any C++ file. Python keeps using the old build until you do.

## Using it from Python

```python
import medaid_engine as engine

t_years = [0.0, 0.5, 1.5, 3.0]
a1c = [6.1, 6.4, 6.8, 6.5]

result = engine.summarize_exposure(t_years, a1c)
print(round(result.cumulative, 2))             # 19.7, in %·years
print(round(result.time_weighted_average, 2))  # 6.57
```

The engine doesn't track units. The backend passes time in fractional years (days / 365.25), and cumulative values come back in the lab's unit multiplied by years.

Convert dates to whole days before calling the engine, because only readings with exactly the same `t` are merged.

## Yearly averages

The engine doesn't know about calendars. For each year, the backend works out the start and end, keeps them within the first and last readings, and calls `average_between`. It should skip years with no real readings, because the result would only be an estimate from the line across the gap.

## Errors

| Python exception | Meaning | What the backend should do |
|---|---|---|
| `InsufficientDataError` | Too few readings at different times | Return `200` with `status: "insufficient_data"` |
| `InvalidSeriesError` | NaN, infinite values, mismatched lengths, unsorted times or a period outside the readings | Treat it as a bug: log it and return `500` |

Both are subclasses of `ValueError`. See [the API conventions](../docs/api-conventions.md).

## C++ tests

These don't need Python. CMake downloads Catch2 the first time you configure.

```bash
cmake -S . -B build -DMEDAID_BUILD_TESTS=ON
cmake --build build -j4
ctest --test-dir build --output-on-failure
```

## Layout

```text
include/medaid/   Public C++ headers
src/              C++ implementation
bindings/         The nanobind layer between C++ and Python
python/           The Python package
tests/cpp/        C++ unit tests
tests/python/     Binding tests, checked against SciPy
```

To add a method: write it in `src/` and declare it in `include/medaid/`, add a C++ test, expose it in `bindings/module.cpp` and `__init__.py`, then add a Python test.
