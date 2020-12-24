#ifndef COSMOPOLITAN_LIBC_INTRIN_PSLLDQ_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSLLDQ_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pslldq(uint8_t[16], const uint8_t[16], unsigned long);

#ifndef __STRICT_ANSI__
__intrin_xmm_t __pslldqs(__intrin_xmm_t);
#define pslldq(B, A, I)                                                   \
  do {                                                                    \
    if (__builtin_expect(!IsModeDbg() && X86_NEED(SSE) && X86_HAVE(SSE2), \
                         1)) {                                            \
      __intrin_xmm_t *Xmm0 = (void *)(B);                                 \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(A);           \
      if (__builtin_constant_p(I)) {                                      \
        if (!X86_NEED(AVX)) {                                             \
          asm("pslldq\t%1,%0" : "=x"(*Xmm0) : "i"(I), "0"(*Xmm1));        \
        } else {                                                          \
          asm("vpslldq\t%2,%1,%0" : "=x"(*Xmm0) : "x"(*Xmm1), "i"(I));    \
        }                                                                 \
      } else {                                                            \
        unsigned long Vimm = (I);                                         \
        typeof(__pslldqs) *Fn;                                            \
        if (Vimm > 16) Vimm = 16;                                         \
        Fn = (typeof(__pslldqs) *)((uintptr_t)&__pslldqs + Vimm * 8);     \
        *Xmm0 = Fn(*Xmm1);                                                \
      }                                                                   \
    } else {                                                              \
      pslldq(B, A, I);                                                    \
    }                                                                     \
  } while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSLLDQ_H_ */
