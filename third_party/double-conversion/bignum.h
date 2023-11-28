// -*- c++ -*-
#ifndef DOUBLE_CONVERSION_BIGNUM_H_
#define DOUBLE_CONVERSION_BIGNUM_H_
#include "third_party/double-conversion/utils.h"

namespace double_conversion {

class Bignum {
 public:
  // 3584 = 128 * 28. We can represent 2^3584 > 10^1000 accurately.
  // This bignum can encode much bigger numbers, since it contains an
  // exponent.
  static const int kMaxSignificantBits = 3584;

  Bignum() : used_bigits_(0), exponent_(0) {}

  void AssignUInt16(const uint16_t value);
  void AssignUInt64(uint64_t value);
  void AssignBignum(const Bignum& other);

  void AssignDecimalString(const Vector<const char> value);
  void AssignHexString(const Vector<const char> value);

  void AssignPowerUInt16(uint16_t base, const int exponent);

  void AddUInt64(const uint64_t operand);
  void AddBignum(const Bignum& other);
  // Precondition: this >= other.
  void SubtractBignum(const Bignum& other);

  void Square();
  void ShiftLeft(const int shift_amount);
  void MultiplyByUInt32(const uint32_t factor);
  void MultiplyByUInt64(const uint64_t factor);
  void MultiplyByPowerOfTen(const int exponent);
  void Times10() { return MultiplyByUInt32(10); }
  // Pseudocode:
  //  int result = this / other;
  //  this = this % other;
  // In the worst case this function is in O(this/other).
  uint16_t DivideModuloIntBignum(const Bignum& other);

  bool ToHexString(char* buffer, const int buffer_size) const;

  // Returns
  //  -1 if a < b,
  //   0 if a == b, and
  //  +1 if a > b.
  static int Compare(const Bignum& a, const Bignum& b);
  static bool Equal(const Bignum& a, const Bignum& b) {
    return Compare(a, b) == 0;
  }
  static bool LessEqual(const Bignum& a, const Bignum& b) {
    return Compare(a, b) <= 0;
  }
  static bool Less(const Bignum& a, const Bignum& b) {
    return Compare(a, b) < 0;
  }
  // Returns Compare(a + b, c);
  static int PlusCompare(const Bignum& a, const Bignum& b, const Bignum& c);
  // Returns a + b == c
  static bool PlusEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) == 0;
  }
  // Returns a + b <= c
  static bool PlusLessEqual(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) <= 0;
  }
  // Returns a + b < c
  static bool PlusLess(const Bignum& a, const Bignum& b, const Bignum& c) {
    return PlusCompare(a, b, c) < 0;
  }
 private:
  typedef uint32_t Chunk;
  typedef uint64_t DoubleChunk;

  static const int kChunkSize = sizeof(Chunk) * 8;
  static const int kDoubleChunkSize = sizeof(DoubleChunk) * 8;
  // With bigit size of 28 we loose some bits, but a double still fits easily
  // into two chunks, and more importantly we can use the Comba multiplication.
  static const int kBigitSize = 28;
  static const Chunk kBigitMask = (1 << kBigitSize) - 1;
  // Every instance allocates kBigitLength chunks on the stack. Bignums cannot
  // grow. There are no checks if the stack-allocated space is sufficient.
  static const int kBigitCapacity = kMaxSignificantBits / kBigitSize;

  static void EnsureCapacity(const int size) {
    if (size > kBigitCapacity) {
      DOUBLE_CONVERSION_UNREACHABLE();
    }
  }
  void Align(const Bignum& other);
  void Clamp();
  bool IsClamped() const {
    return used_bigits_ == 0 || RawBigit(used_bigits_ - 1) != 0;
  }
  void Zero() {
    used_bigits_ = 0;
    exponent_ = 0;
  }
  // Requires this to have enough capacity (no tests done).
  // Updates used_bigits_ if necessary.
  // shift_amount must be < kBigitSize.
  void BigitsShiftLeft(const int shift_amount);
  // BigitLength includes the "hidden" bigits encoded in the exponent.
  int BigitLength() const { return used_bigits_ + exponent_; }
  Chunk& RawBigit(const int index);
  const Chunk& RawBigit(const int index) const;
  Chunk BigitOrZero(const int index) const;
  void SubtractTimes(const Bignum& other, const int factor);

  // The Bignum's value is value(bigits_buffer_) * 2^(exponent_ * kBigitSize),
  // where the value of the buffer consists of the lower kBigitSize bits of
  // the first used_bigits_ Chunks in bigits_buffer_, first chunk has lowest
  // significant bits.
  int16_t used_bigits_;
  int16_t exponent_;
  Chunk bigits_buffer_[kBigitCapacity];

  DOUBLE_CONVERSION_DISALLOW_COPY_AND_ASSIGN(Bignum);
};

}  // namespace double_conversion

#endif  // DOUBLE_CONVERSION_BIGNUM_H_
