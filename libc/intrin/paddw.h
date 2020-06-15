#ifndef COSMOPOLITAN_LIBC_INTRIN_PADDW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PADDW_H_
#include "libc/intrin/macros.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Adds signed 16-bit integers.
 *
 * @param 𝑎 [w/o] receives result
 * @param 𝑏 [r/o] supplies first input vector
 * @param 𝑐 [r/o] supplies second input vector
 * @note shorts can't overflow so ubsan won't report it when it happens
 * @see paddsw()
 * @mayalias
 */
static void paddw(short a[8], const short b[8], const short c[8]) {
  int i;
  for (i = 0; i < 8; ++i) {
    a[i] = b[i] + c[i];
  }
}

#define paddw(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(paddw, SSE2, "paddw", INTRIN_COMMUTATIVE, A, B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PADDW_H_ */
