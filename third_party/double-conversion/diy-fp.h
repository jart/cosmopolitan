// -*- c++ -*-
#ifndef DOUBLE_CONVERSION_DIY_FP_H_
#define DOUBLE_CONVERSION_DIY_FP_H_
#include "third_party/double-conversion/utils.h"

namespace double_conversion {

// This "Do It Yourself Floating Point" class implements a floating-point number
// with a uint64 significand and an int exponent. Normalized DiyFp numbers will
// have the most significant bit of the significand set.
// Multiplication and Subtraction do not normalize their results.
// DiyFp store only non-negative numbers and are not designed to contain special
// doubles (NaN and Infinity).
class DiyFp {
 public:
  static const int kSignificandSize = 64;

  DiyFp() : f_(0), e_(0) {}
  DiyFp(const uint64_t significand, const int32_t exponent) : f_(significand), e_(exponent) {}

  // this -= other.
  // The exponents of both numbers must be the same and the significand of this
  // must be greater or equal than the significand of other.
  // The result will not be normalized.
  void Subtract(const DiyFp& other) {
    DOUBLE_CONVERSION_ASSERT(e_ == other.e_);
    DOUBLE_CONVERSION_ASSERT(f_ >= other.f_);
    f_ -= other.f_;
  }

  // Returns a - b.
  // The exponents of both numbers must be the same and a must be greater
  // or equal than b. The result will not be normalized.
  static DiyFp Minus(const DiyFp& a, const DiyFp& b) {
    DiyFp result = a;
    result.Subtract(b);
    return result;
  }

  // this *= other.
  void Multiply(const DiyFp& other) {
    // Simply "emulates" a 128 bit multiplication.
    // However: the resulting number only contains 64 bits. The least
    // significant 64 bits are only used for rounding the most significant 64
    // bits.
    const uint64_t kM32 = 0xFFFFFFFFU;
    const uint64_t a = f_ >> 32;
    const uint64_t b = f_ & kM32;
    const uint64_t c = other.f_ >> 32;
    const uint64_t d = other.f_ & kM32;
    const uint64_t ac = a * c;
    const uint64_t bc = b * c;
    const uint64_t ad = a * d;
    const uint64_t bd = b * d;
    // By adding 1U << 31 to tmp we round the final result.
    // Halfway cases will be rounded up.
    const uint64_t tmp = (bd >> 32) + (ad & kM32) + (bc & kM32) + (1U << 31);
    e_ += other.e_ + 64;
    f_ = ac + (ad >> 32) + (bc >> 32) + (tmp >> 32);
  }

  // returns a * b;
  static DiyFp Times(const DiyFp& a, const DiyFp& b) {
    DiyFp result = a;
    result.Multiply(b);
    return result;
  }

  void Normalize() {
    DOUBLE_CONVERSION_ASSERT(f_ != 0);
    uint64_t significand = f_;
    int32_t exponent = e_;

    // This method is mainly called for normalizing boundaries. In general,
    // boundaries need to be shifted by 10 bits, and we optimize for this case.
    const uint64_t k10MSBits = DOUBLE_CONVERSION_UINT64_2PART_C(0xFFC00000, 00000000);
    while ((significand & k10MSBits) == 0) {
      significand <<= 10;
      exponent -= 10;
    }
    while ((significand & kUint64MSB) == 0) {
      significand <<= 1;
      exponent--;
    }
    f_ = significand;
    e_ = exponent;
  }

  static DiyFp Normalize(const DiyFp& a) {
    DiyFp result = a;
    result.Normalize();
    return result;
  }

  uint64_t f() const { return f_; }
  int32_t e() const { return e_; }

  void set_f(uint64_t new_value) { f_ = new_value; }
  void set_e(int32_t new_value) { e_ = new_value; }

 private:
  static const uint64_t kUint64MSB = DOUBLE_CONVERSION_UINT64_2PART_C(0x80000000, 00000000);

  uint64_t f_;
  int32_t e_;
};

}  // namespace double_conversion

#endif  // DOUBLE_CONVERSION_DIY_FP_H_
