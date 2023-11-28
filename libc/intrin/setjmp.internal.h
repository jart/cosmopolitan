#ifndef COSMOPOLITAN_LIBC_INTRIN_SETJMP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_SETJMP_INTERNAL_H_
#include "libc/limits.h"
COSMOPOLITAN_C_START_

/**
 * Encodes nonzero number for longjmp().
 *
 * This is a workaround to the fact that the value has to be non-zero.
 * So we work around it by dedicating the highest bit to being a flag.
 */
static inline int EncodeLongjmp(int x) {
  return x | INT_MIN;
}

/**
 * Decodes nonzero number returned by setjmp().
 *
 * This is a workaround to the fact that the value has to be non-zero.
 * So we work around it by dedicating the highest bit to being a flag.
 */
static inline int DecodeSetjmp(int x) {
  return (int)((unsigned)x << 1) >> 1;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_SETJMP_INTERNAL_H_ */
