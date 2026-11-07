#pragma once

#include <stdexcept>

namespace medaid {

// The input can never be used: NaN values, mismatched lengths, unsorted times.
// This means a bug or bad data upstream.
class InvalidSeriesError : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

// The input is valid but has too few readings to calculate from.
// This is a normal outcome for a patient with little history.
class InsufficientDataError : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

}  // namespace medaid
