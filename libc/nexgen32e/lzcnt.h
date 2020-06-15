#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_LZCNT_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_LZCNT_H_
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned lzcnt(unsigned) libcesque pureconst;
unsigned long lzcntl(unsigned long) libcesque pureconst;

#define lzcnt(X)                                           \
  ({                                                       \
    typeof(X) Res;                                         \
    if (X86_HAVE(ABM)) {                                   \
      /* amd piledriver+ (a.k.a. bdver1) c. 2011 */        \
      /* intel haswell+ c. 2013 */                         \
      asm("lzcnt\t%1,%0" : "=r,r"(Res) : "r,m"(X) : "cc"); \
    } else {                                               \
      Res = (lzcnt)(X);                                    \
    }                                                      \
    Res;                                                   \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_LZCNT_H_ */
