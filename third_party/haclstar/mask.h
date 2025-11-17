#ifndef COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MASK_H_
#define COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MASK_H_
COSMOPOLITAN_C_START_

uint64_t
FStar_UInt64_eq_mask(uint64_t, uint64_t);

uint64_t
FStar_UInt64_gte_mask(uint64_t, uint64_t);

uint32_t
FStar_UInt32_eq_mask(uint32_t, uint32_t);

uint32_t
FStar_UInt32_gte_mask(uint32_t, uint32_t);

uint16_t
FStar_UInt16_eq_mask(uint16_t, uint16_t);

uint16_t
FStar_UInt16_gte_mask(uint16_t, uint16_t);

uint8_t
FStar_UInt8_eq_mask(uint8_t, uint8_t);

uint8_t
FStar_UInt8_gte_mask(uint8_t, uint8_t);

static inline uint128_t
FStar_UInt128_eq_mask(uint128_t x, uint128_t y)
{
    uint64_t mask =
      FStar_UInt64_eq_mask((uint64_t)(x >> 64), (uint64_t)(y >> 64)) &
      FStar_UInt64_eq_mask((uint64_t)x, (uint64_t)y);
    return ((uint128_t)mask) << 64 | mask;
}

static inline uint128_t
FStar_UInt128_gte_mask(uint128_t x, uint128_t y)
{
    uint64_t mask = (FStar_UInt64_gte_mask(x >> 64, y >> 64) &
                     ~(FStar_UInt64_eq_mask(x >> 64, y >> 64))) |
                    (FStar_UInt64_eq_mask(x >> 64, y >> 64) &
                     FStar_UInt64_gte_mask((uint64_t)x, (uint64_t)y));
    return ((uint128_t)mask) << 64 | mask;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_HACLSTAR_MASK_H_ */
