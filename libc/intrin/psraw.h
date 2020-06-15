#ifndef COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_
#include "libc/bits/bits.h"
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Divides shorts by two power.
 *
 * @note c needs to be a literal, asmconstexpr, or linkconstsym
 * @mayalias
 */
static void psraw(short a[8], const short b[8], char c) {
  int i;
  for (i = 0; i < 8; ++i) {
    a[i] = SAR(b[i], c);
  }
}

#define psraw(A, B, I) INTRIN_SSEVEX_X_X_I_(psraw, SSE2, "psraw", A, B, I)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSRAW_H_ */
