// clang-format off
// This file defines integral types for file input/output. We need to use
// these types instead of the plain integers (such as uint32_t or int32_t)
// when reading from/writing to an mmap'ed file area for the following
// reasons:
//
// 1. mold is always a cross linker and should not depend on what host it
//    is running on. Users should be able to run mold on a big-endian
//    SPARC machine to create a little-endian RV64 binary, for example.
//
// 2. Even though data members in all ELF data strucutres are naturally
//    aligned, they are not guaranteed to be aligned on memory. Because
//    archive file (.a file) aligns each member only to a 2 byte boundary,
//    anything larger than 2 bytes may be unaligned in an mmap'ed memory.
//    Unaligned access is an undefined behavior in C/C++, so we shouldn't
//    cast an arbitrary pointer to a uint32_t, for example, to read a
//    32-bits value.
//
// The data types defined in this file don't depend on host byte order and
// don't do unaligned access.

#pragma once

#include "third_party/libcxx/bit"
#include "third_party/libcxx/cstdint"
#include "third_party/libcxx/cstring"

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define __LITTLE_ENDIAN__ 1
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define __BIG_ENDIAN__ 1
# else
#  error "unknown host byte order"
# endif
#endif

namespace mold {

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

template <typename T>
static inline T bswap(T val) {
  switch (sizeof(T)) {
  case 2:  return __builtin_bswap16(val);
  case 4:  return __builtin_bswap32(val);
  case 8:  return __builtin_bswap64(val);
  default: __builtin_unreachable();
  }
}

template <typename T, int SIZE = sizeof(T)>
class LittleEndian {
public:
  LittleEndian() = default;
  LittleEndian(T x) { *this = x; }

  operator T() const {
    if constexpr (sizeof(T) == SIZE) {
      T x;
      memcpy(&x, val, sizeof(T));
      if constexpr (std::endian::native == std::endian::big)
        x = bswap(x);
      return x;
    } else {
      static_assert(SIZE == 3);
      return (val[2] << 16) | (val[1] << 8) | val[0];
    }
  }

  LittleEndian &operator=(T x) {
    if constexpr (sizeof(T) == SIZE) {
      if constexpr (std::endian::native == std::endian::big)
        x = bswap(x);
      memcpy(val, &x, sizeof(T));
    } else {
      static_assert(SIZE == 3);
      val[2] = x >> 16;
      val[1] = x >> 8;
      val[0] = x;
    }
    return *this;
  }

  LittleEndian &operator++() {
    return *this = *this + 1;
  }

  LittleEndian operator++(int) {
    T ret = *this;
    *this = *this + 1;
    return ret;
  }

  LittleEndian &operator--() {
    return *this = *this - 1;
  }

  LittleEndian operator--(int) {
    T ret = *this;
    *this = *this - 1;
    return ret;
  }

  LittleEndian &operator+=(T x) {
    return *this = *this + x;
  }

  LittleEndian &operator-=(T x) {
    return *this = *this - x;
  }

  LittleEndian &operator&=(T x) {
    return *this = *this & x;
  }

  LittleEndian &operator|=(T x) {
    return *this = *this | x;
  }

private:
  u8 val[SIZE];
};

using il16 = LittleEndian<i16>;
using il32 = LittleEndian<i32>;
using il64 = LittleEndian<i64>;
using ul16 = LittleEndian<u16>;
using ul24 = LittleEndian<u32, 3>;
using ul32 = LittleEndian<u32>;
using ul64 = LittleEndian<u64>;

template <typename T, int SIZE = sizeof(T)>
class BigEndian {
public:
  BigEndian() = default;
  BigEndian(T x) { *this = x; }

  operator T() const {
    if constexpr (sizeof(T) == SIZE) {
      T x;
      memcpy(&x, val, sizeof(T));
      if constexpr (std::endian::native == std::endian::little)
        x = bswap(x);
      return x;
    } else {
      static_assert(SIZE == 3);
      return (val[0] << 16) | (val[1] << 8) | val[2];
    }
  }

  BigEndian &operator=(T x) {
    if constexpr (sizeof(T) == SIZE) {
      if constexpr (std::endian::native == std::endian::little)
        x = bswap(x);
      memcpy(val, &x, sizeof(T));
    } else {
      static_assert(SIZE == 3);
      val[0] = x >> 16;
      val[1] = x >> 8;
      val[2] = x;
    }
    return *this;
  }

  BigEndian &operator++() {
    return *this = *this + 1;
  }

  BigEndian operator++(int) {
    T ret = *this;
    *this = *this + 1;
    return ret;
  }

  BigEndian &operator--() {
    return *this = *this - 1;
  }

  BigEndian operator--(int) {
    T ret = *this;
    *this = *this - 1;
    return ret;
  }

  BigEndian &operator+=(T x) {
    return *this = *this + x;
  }

  BigEndian &operator-=(T x) {
    return *this = *this - x;
  }

  BigEndian &operator&=(T x) {
    return *this = *this & x;
  }

  BigEndian &operator|=(T x) {
    return *this = *this | x;
  }

private:
  u8 val[SIZE];
};

using ib16 = BigEndian<i16>;
using ib32 = BigEndian<i32>;
using ib64 = BigEndian<i64>;
using ub16 = BigEndian<u16>;
using ub24 = BigEndian<u32, 3>;
using ub32 = BigEndian<u32>;
using ub64 = BigEndian<u64>;

} // namespace mold
