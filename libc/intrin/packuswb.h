#ifndef COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_
#include "libc/intrin/macros.h"
#include "libc/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Casts shorts to unsigned chars w/ saturation.
 *
 *   𝑎 ← {CLAMP[𝑏ᵢ]|𝑖∈[0,4)} ║ {CLAMP[𝑐ᵢ]|𝑖∈[4,8)}
 *
 * @see packsswb()
 * @mayalias
 */
static void packuswb(unsigned char a[16], const short b[8], const short c[8]) {
  int i;
  for (i = 0; i < 8; ++i) {
    a[i] = MIN(255, MAX(0, b[i]));
  }
  for (i = 0; i < 8; ++i) {
    a[i + 8] = MIN(255, MAX(0, c[i]));
  }
}

#define packuswb(A, B, C)                                                    \
  INTRIN_SSEVEX_X_X_X_(packuswb, SSE2, "packuswb", INTRIN_NONCOMMUTATIVE, A, \
                       B, C)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PACKUSWB_H_ */
