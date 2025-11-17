#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_INT128_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_INT128_H_
#include "libc/serialize.h"
COSMOPOLITAN_C_START_

inline static uint128_t
load128_le(uint8_t* b)
{
    uint128_t l = (uint128_t)READ64LE(b);
    uint128_t h = (uint128_t)READ64LE(b + 8);
    return (h << 64 | l);
}

inline static void
store128_le(uint8_t* b, uint128_t n)
{
    WRITE64LE(b, (uint64_t)n);
    WRITE64LE(b + 8, (uint64_t)(n >> 64));
}

inline static uint128_t
load128_be(uint8_t* b)
{
    uint128_t h = (uint128_t)READ64BE(b);
    uint128_t l = (uint128_t)READ64BE(b + 8);
    return (h << 64 | l);
}

inline static void
store128_be(uint8_t* b, uint128_t n)
{
    WRITE64BE(b, (uint64_t)(n >> 64));
    WRITE64BE(b + 8, (uint64_t)n);
}

inline static uint128_t
FStar_UInt128_add(uint128_t x, uint128_t y)
{
    return x + y;
}

inline static uint128_t
FStar_UInt128_mul(uint128_t x, uint128_t y)
{
    return x * y;
}

inline static uint128_t
FStar_UInt128_add_mod(uint128_t x, uint128_t y)
{
    return x + y;
}

inline static uint128_t
FStar_UInt128_sub(uint128_t x, uint128_t y)
{
    return x - y;
}

inline static uint128_t
FStar_UInt128_sub_mod(uint128_t x, uint128_t y)
{
    return x - y;
}

inline static uint128_t
FStar_UInt128_logand(uint128_t x, uint128_t y)
{
    return x & y;
}

inline static uint128_t
FStar_UInt128_logor(uint128_t x, uint128_t y)
{
    return x | y;
}

inline static uint128_t
FStar_UInt128_logxor(uint128_t x, uint128_t y)
{
    return x ^ y;
}

inline static uint128_t
FStar_UInt128_lognot(uint128_t x)
{
    return ~x;
}

inline static uint128_t
FStar_UInt128_shift_left(uint128_t x, uint32_t y)
{
    return x << y;
}

inline static uint128_t
FStar_UInt128_shift_right(uint128_t x, uint32_t y)
{
    return x >> y;
}

inline static uint128_t
FStar_UInt128_uint64_to_uint128(uint64_t x)
{
    return (uint128_t)x;
}

inline static uint64_t
FStar_UInt128_uint128_to_uint64(uint128_t x)
{
    return (uint64_t)x;
}

inline static uint128_t
FStar_UInt128_mul_wide(uint64_t x, uint64_t y)
{
    return ((uint128_t)x)*y;
}

inline static uint64_t
FStar_UInt128___proj__Mkuint128__item__low(uint128_t x)
{
    return (uint64_t)x;
}

inline static uint64_t
FStar_UInt128___proj__Mkuint128__item__high(uint128_t x)
{
    return (uint64_t)(x >> 64);
}

inline static uint128_t
FStar_UInt128_add_underspec(uint128_t x, uint128_t y)
{
    return x + y;
}

inline static uint128_t
FStar_UInt128_sub_underspec(uint128_t x, uint128_t y)
{
    return x - y;
}

inline static bool
FStar_UInt128_eq(uint128_t x, uint128_t y)
{
    return x == y;
}

inline static bool
FStar_UInt128_gt(uint128_t x, uint128_t y)
{
    return x > y;
}

inline static bool
FStar_UInt128_lt(uint128_t x, uint128_t y)
{
    return x < y;
}

inline static bool
FStar_UInt128_gte(uint128_t x, uint128_t y)
{
    return x >= y;
}

inline static bool
FStar_UInt128_lte(uint128_t x, uint128_t y)
{
    return x <= y;
}

inline static uint128_t
FStar_UInt128_mul32(uint64_t x, uint32_t y)
{
    return (uint128_t)x * (uint128_t)y;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_INT128_H_ */
