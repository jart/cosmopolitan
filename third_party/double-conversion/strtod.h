// -*- c++ -*-
#ifndef DOUBLE_CONVERSION_STRTOD_H_
#define DOUBLE_CONVERSION_STRTOD_H_
#include "third_party/double-conversion/utils.h"

namespace double_conversion {

// The buffer must only contain digits in the range [0-9]. It must not
// contain a dot or a sign. It must not start with '0', and must not be empty.
double Strtod(Vector<const char> buffer, int exponent);

// The buffer must only contain digits in the range [0-9]. It must not
// contain a dot or a sign. It must not start with '0', and must not be empty.
float Strtof(Vector<const char> buffer, int exponent);

// Same as Strtod, but assumes that 'trimmed' is already trimmed, as if run
// through TrimAndCut. That is, 'trimmed' must have no leading or trailing
// zeros, must not be a lone zero, and must not have 'too many' digits.
double StrtodTrimmed(Vector<const char> trimmed, int exponent);

// Same as Strtof, but assumes that 'trimmed' is already trimmed, as if run
// through TrimAndCut. That is, 'trimmed' must have no leading or trailing
// zeros, must not be a lone zero, and must not have 'too many' digits.
float StrtofTrimmed(Vector<const char> trimmed, int exponent);

inline Vector<const char> TrimTrailingZeros(Vector<const char> buffer) {
  for (int i = buffer.length() - 1; i >= 0; --i) {
    if (buffer[i] != '0') {
      return buffer.SubVector(0, i + 1);
    }
  }
  return Vector<const char>(buffer.start(), 0);
}

}  // namespace double_conversion

#endif  // DOUBLE_CONVERSION_STRTOD_H_
