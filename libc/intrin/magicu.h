#ifndef COSMOPOLITAN_LIBC_TINYMATH_MAGICU_H_
#define COSMOPOLITAN_LIBC_TINYMATH_MAGICU_H_
COSMOPOLITAN_C_START_

struct magicu {
  uint32_t M;
  uint32_t s;
};

struct magicu __magicu_get(uint32_t);

/**
 * Performs fast division using precomputed magic for constant divisor.
 *
 * @param x is unsigned integer that shall be divided
 * @param d should be `__magicu_get(y)` if computing `x / y`
 * @return result of unsigned integer division
 */
forceinline uint32_t __magicu_div(uint32_t x, struct magicu d) {
  return ((((uint64_t)x * d.M) >> 32) + ((d.s & 64) ? x : 0)) >> (d.s & 63);
}

/**
 * Checks if 𝑑 contains a valid initialized divisor.
 */
static inline bool32 __magicu_valid(struct magicu d) {
  if (!d.M && !d.s) return false;     /* uninitialized */
  if (d.s & ~(64 | 63)) return false; /* corrupted */
  return true;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_MAGICU_H_ */
