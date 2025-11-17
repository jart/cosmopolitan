#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CARRY_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CARRY_H_
#include "third_party/haclstar/haclstar.h"
#include "third_party/haclstar/int128.h"
#include "third_party/haclstar/mask.h"
#include "third_party/intel/immintrin.internal.h"
COSMOPOLITAN_C_START_

static inline uint32_t
Hacl_IntTypes_Intrinsics_add_carry_u32(uint32_t cin,
                                       uint32_t x,
                                       uint32_t y,
                                       uint32_t* r)
{
    uint64_t res = (uint64_t)x + (uint64_t)cin + (uint64_t)y;
    uint32_t c = (uint32_t)(res >> 32U);
    r[0U] = (uint32_t)res;
    return c;
}

static inline uint32_t
Hacl_IntTypes_Intrinsics_sub_borrow_u32(uint32_t cin,
                                        uint32_t x,
                                        uint32_t y,
                                        uint32_t* r)
{
    uint64_t res = (uint64_t)x - (uint64_t)y - (uint64_t)cin;
    uint32_t c = (uint32_t)(res >> 32U) & 1U;
    r[0U] = (uint32_t)res;
    return c;
}

static inline uint64_t
Hacl_IntTypes_Intrinsics_add_carry_u64(uint64_t cin,
                                       uint64_t x,
                                       uint64_t y,
                                       uint64_t* r)
{
    uint64_t res = x + cin + y;
    uint64_t c =
      (~FStar_UInt64_gte_mask(res, x) | (FStar_UInt64_eq_mask(res, x) & cin)) &
      1ULL;
    r[0U] = res;
    return c;
}

static inline uint64_t
Hacl_IntTypes_Intrinsics_sub_borrow_u64(uint64_t cin,
                                        uint64_t x,
                                        uint64_t y,
                                        uint64_t* r)
{
    uint64_t res = x - y - cin;
    uint64_t c =
      ((FStar_UInt64_gte_mask(res, x) & ~FStar_UInt64_eq_mask(res, x)) |
       (FStar_UInt64_eq_mask(res, x) & cin)) &
      1ULL;
    r[0U] = res;
    return c;
}

static inline uint64_t
Hacl_IntTypes_Intrinsics_128_add_carry_u64(uint64_t cin,
                                           uint64_t x,
                                           uint64_t y,
                                           uint64_t* r)
{
    FStar_UInt128_uint128 res = FStar_UInt128_add_mod(
      FStar_UInt128_add_mod(FStar_UInt128_uint64_to_uint128(x),
                            FStar_UInt128_uint64_to_uint128(cin)),
      FStar_UInt128_uint64_to_uint128(y));
    uint64_t c =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(res, 64U));
    r[0U] = FStar_UInt128_uint128_to_uint64(res);
    return c;
}

static inline uint64_t
Hacl_IntTypes_Intrinsics_128_sub_borrow_u64(uint64_t cin,
                                            uint64_t x,
                                            uint64_t y,
                                            uint64_t* r)
{
    FStar_UInt128_uint128 res = FStar_UInt128_sub_mod(
      FStar_UInt128_sub_mod(FStar_UInt128_uint64_to_uint128(x),
                            FStar_UInt128_uint64_to_uint128(y)),
      FStar_UInt128_uint64_to_uint128(cin));
    uint64_t c =
      FStar_UInt128_uint128_to_uint64(FStar_UInt128_shift_right(res, 64U)) &
      1ULL;
    r[0U] = FStar_UInt128_uint128_to_uint64(res);
    return c;
}

#ifdef __x86_64__
#define Lib_IntTypes_Intrinsics_add_carry_u32(x1, x2, x3, x4) \
    (_addcarry_u32(x1, x2, x3, (unsigned int*)x4))
#define Lib_IntTypes_Intrinsics_add_carry_u64(x1, x2, x3, x4) \
    (_addcarry_u64(x1, x2, x3, (long long unsigned int*)x4))
#define Lib_IntTypes_Intrinsics_sub_borrow_u32(x1, x2, x3, x4) \
    (_subborrow_u32(x1, x2, x3, (unsigned int*)x4))
#define Lib_IntTypes_Intrinsics_sub_borrow_u64(x1, x2, x3, x4) \
    (_subborrow_u64(x1, x2, x3, (long long unsigned int*)x4))
#else
#define Lib_IntTypes_Intrinsics_add_carry_u64(x1, x2, x3, x4) \
    (Hacl_IntTypes_Intrinsics_128_add_carry_u64(x1, x2, x3, x4))
#define Lib_IntTypes_Intrinsics_sub_borrow_u64(x1, x2, x3, x4) \
    (Hacl_IntTypes_Intrinsics_128_sub_borrow_u64(x1, x2, x3, x4))
#define Lib_IntTypes_Intrinsics_add_carry_u32(x1, x2, x3, x4) \
    (Hacl_IntTypes_Intrinsics_add_carry_u32(x1, x2, x3, x4))
#define Lib_IntTypes_Intrinsics_sub_borrow_u32(x1, x2, x3, x4) \
    (Hacl_IntTypes_Intrinsics_sub_borrow_u32(x1, x2, x3, x4))
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_CARRY_H_ */
