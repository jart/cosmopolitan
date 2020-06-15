#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_
#include "libc/intrin/macros.h"
#include "libc/limits.h"
#include "libc/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Adds signed 16-bit integers w/ saturation.
 *
 * @param 𝑎 [w/o] receives result
 * @param 𝑏 [r/o] supplies first input vector
 * @param 𝑐 [r/o] supplies second input vector
 * @see paddw()
 * @mayalias
 */
static void paddsw(short a[8], const short b[8], const short c[8]) {
  int i;
  for (i = 0; i < 8; ++i) {
    a[i] = MIN(SHRT_MAX, MAX(SHRT_MIN, b[i] + c[i]));
  }
}

#define paddsw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddsw, SSE2, "paddsw", INTRIN_COMMUTATIVE, A, B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDSW_H_ */
