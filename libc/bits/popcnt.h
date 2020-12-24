#ifndef COSMOPOLITAN_LIBC_BITS_POPCNT_H_
#define COSMOPOLITAN_LIBC_BITS_POPCNT_H_
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned long popcnt(unsigned long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define popcnt(X)                                         \
  (__builtin_constant_p(X) ? __builtin_popcountll(X) : ({ \
    unsigned long Res, Pop = (X);                         \
    if (X86_HAVE(POPCNT)) {                               \
      asm("popcnt\t%1,%0" : "=r"(Res) : "r"(Pop) : "cc"); \
    } else {                                              \
      Res = (popcnt)(Pop);                                \
    }                                                     \
    Res;                                                  \
  }))
#endif /* GNUC && !ANSI */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_POPCNT_H_ */
