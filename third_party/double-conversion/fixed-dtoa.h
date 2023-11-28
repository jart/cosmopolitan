// -*- c++ -*-
#ifndef DOUBLE_CONVERSION_FIXED_DTOA_H_
#define DOUBLE_CONVERSION_FIXED_DTOA_H_
#include "third_party/double-conversion/utils.h"

namespace double_conversion {

// Produces digits necessary to print a given number with
// 'fractional_count' digits after the decimal point.
// The buffer must be big enough to hold the result plus one terminating null
// character.
//
// The produced digits might be too short in which case the caller has to fill
// the gaps with '0's.
// Example: FastFixedDtoa(0.001, 5, ...) is allowed to return buffer = "1", and
// decimal_point = -2.
// Halfway cases are rounded towards +/-Infinity (away from 0). The call
// FastFixedDtoa(0.15, 2, ...) thus returns buffer = "2", decimal_point = 0.
// The returned buffer may contain digits that would be truncated from the
// shortest representation of the input.
//
// This method only works for some parameters. If it can't handle the input it
// returns false. The output is null-terminated when the function succeeds.
bool FastFixedDtoa(double v, int fractional_count,
                   Vector<char> buffer, int* length, int* decimal_point);

}  // namespace double_conversion

#endif  // DOUBLE_CONVERSION_FIXED_DTOA_H_
