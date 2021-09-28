#ifndef COSMOPOLITAN_LIBC_BITS_POPCNT_H_
#define COSMOPOLITAN_LIBC_BITS_POPCNT_H_
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

size_t _countbits(const void *, size_t);
unsigned long popcnt(unsigned long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define popcnt(X)                                                \
  (__builtin_constant_p(X) ? __builtin_popcountll(X) : ({        \
    unsigned long PoP = (X);                                     \
    if (X86_HAVE(POPCNT)) {                                      \
      asm("popcnt\t%0,%0" : "+r"(PoP) : /* no inputs */ : "cc"); \
    } else {                                                     \
      PoP = (popcnt)(PoP);                                       \
    }                                                            \
    PoP;                                                         \
  }))
#endif /* GNUC && !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_POPCNT_H_ */
