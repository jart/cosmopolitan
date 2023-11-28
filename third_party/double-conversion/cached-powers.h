// -*- c++ -*-
#ifndef DOUBLE_CONVERSION_CACHED_POWERS_H_
#define DOUBLE_CONVERSION_CACHED_POWERS_H_
#include "third_party/double-conversion/diy-fp.h"

namespace double_conversion {

namespace PowersOfTenCache {

  // Not all powers of ten are cached. The decimal exponent of two neighboring
  // cached numbers will differ by kDecimalExponentDistance.
  static const int kDecimalExponentDistance = 8;

  static const int kMinDecimalExponent = -348;
  static const int kMaxDecimalExponent = 340;

  // Returns a cached power-of-ten with a binary exponent in the range
  // [min_exponent; max_exponent] (boundaries included).
  void GetCachedPowerForBinaryExponentRange(int min_exponent,
                                            int max_exponent,
                                            DiyFp* power,
                                            int* decimal_exponent);

  // Returns a cached power of ten x ~= 10^k such that
  //   k <= decimal_exponent < k + kCachedPowersDecimalDistance.
  // The given decimal_exponent must satisfy
  //   kMinDecimalExponent <= requested_exponent, and
  //   requested_exponent < kMaxDecimalExponent + kDecimalExponentDistance.
  void GetCachedPowerForDecimalExponent(int requested_exponent,
                                        DiyFp* power,
                                        int* found_exponent);

}  // namespace PowersOfTenCache

}  // namespace double_conversion

#endif  // DOUBLE_CONVERSION_CACHED_POWERS_H_
