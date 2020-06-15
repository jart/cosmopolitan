#ifndef COSMOPOLITAN_LIBC_VARINT_H_
#define COSMOPOLITAN_LIBC_VARINT_H_
#include "libc/limits.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define unzigzag(x) ((x >> 1) ^ -(x & 1))
#define zigzag(x)   _Generic((x), int32_t : zigzag32, default : zigzag64)(x)

forceinline int32_t zigzag32(int32_t x) {
  return (int64_t)((uint64_t)x << 1) ^ (x >> 31);
}

forceinline int64_t zigzag64(int64_t x) {
  return (int64_t)((uint64_t)x << 1) ^ (x >> 63);
}

/**
 * Copies variable-length integer to buffer.
 *
 * @param p needs 10+ bytes for 64-bit and 5+ for 32-bit
 * @param x is unsigned number to encode
 * @return pointer past last written byte
 * @see writesint() which has more efficient encoding for signed numbers
 */
forceinline uint8_t *writevint(uint8_t *p, uint64_t x) {
  do {
    *p++ = (uint8_t)(x | 0x80);
    x >>= 7;
  } while (x > 0);
  p[-1] &= 0x7f;
  return p;
}

/**
 * Copies variable-length signed integer to buffer.
 *
 * @param p needs 10+ bytes for 64-bit and 5+ for 32-bit
 * @param x is unsigned number to encode
 * @return pointer past last written byte
 */
forceinline uint8_t *writesint(uint8_t *p, int64_t x) {
  return writevint(p, zigzag(x));
}

/**
 * Reads variable-length integer from buffer.
 *
 * @param x is unsigned number to encode
 * @return pointer past last read byte or -1ul on error
 */
forceinline uint8_t *readvint(uint8_t *p, uint8_t *pe, uint64_t *res) {
  uint8_t b;
  uint64_t x, o;
  x = 0;
  b = 0;
  while (p < pe) {
    o = *p++;
    x |= ((uint64_t)o & 0x7f) << b;
    if (!(o & 0x80)) {
      *res = x;
      return p;
    }
    if ((b += 7) > 64) {
      break;
    }
  }
  return (uint8_t *)-1ul;
}

/**
 * Reads variable-length zig-zagged integer from buffer.
 *
 * @param x is unsigned number to encode
 * @return pointer past last read byte or -1ul on error
 */
forceinline uint8_t *readsint(uint8_t *p, uint8_t *pe, int64_t *res) {
  uint64_t u;
  if ((p = readvint(p, pe, &u)) != (uint8_t *)-1ul) {
    *res = unzigzag((int64_t)u);
    return p;
  } else {
    return (uint8_t *)-1ul;
  }
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_VARINT_H_ */
