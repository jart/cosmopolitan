#ifndef COSMOPOLITAN_LIBC_INTRIN_PMOVMSKB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PMOVMSKB_H_
#include "libc/intrin/macros.h"
COSMOPOLITAN_C_START_

uint32_t pmovmskb(const uint8_t[16]);

#if defined(__x86_64__) && defined(__GNUC__)
#define pmovmskb(A)                                            \
  ({                                                           \
    uint32_t Mask;                                             \
    if (!IsModeDbg() && X86_HAVE(SSE2)) {                      \
      const __intrin_xmm_t *Xmm = (const __intrin_xmm_t *)(A); \
      if (!X86_NEED(AVX)) {                                    \
        asm("pmovmskb\t%1,%0" : "=r"(Mask) : "x"(*Xmm));       \
      } else {                                                 \
        asm("vpmovmskb\t%1,%0" : "=r"(Mask) : "x"(*Xmm));      \
      }                                                        \
    } else {                                                   \
      Mask = pmovmskb(A);                                      \
    }                                                          \
    Mask;                                                      \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PMOVMSKB_H_ */
