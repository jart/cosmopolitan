#ifndef COSMOPOLITAN_LIBC_INTRIN_MPSADBW_H_
#define COSMOPOLITAN_LIBC_INTRIN_MPSADBW_H_
#include "libc/intrin/macros.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void mpsadbw(uint16_t[8], const uint8_t[16], const uint8_t[16], uint8_t);

#ifndef __STRICT_ANSI__
__intrin_xmm_t __mpsadbws(__intrin_xmm_t, __intrin_xmm_t);
#define mpsadbw(C, B, A, I)                                                   \
  do {                                                                        \
    if (__builtin_expect(!IsModeDbg() && X86_NEED(SSE) && X86_HAVE(SSE4_1),   \
                         1)) {                                                \
      __intrin_xmm_t *Xmm0 = (void *)(C);                                     \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B);               \
      const __intrin_xmm_t *Xmm2 = (const __intrin_xmm_t *)(A);               \
      if (__builtin_constant_p(I)) {                                          \
        if (!X86_NEED(AVX)) {                                                 \
          asm("mpsadbw\t%2,%1,%0"                                             \
              : "=x"(*Xmm0)                                                   \
              : "x"(*Xmm2), "i"(I), "0"(*Xmm1));                              \
        } else {                                                              \
          asm("vmpsadbw\t%3,%2,%1,%0"                                         \
              : "=x"(*Xmm0)                                                   \
              : "x"(*Xmm1), "x"(*Xmm2), "i"(I));                              \
        }                                                                     \
      } else {                                                                \
        unsigned long Vimm = (I);                                             \
        typeof(__mpsadbws) *Fn;                                               \
        Fn = (typeof(__mpsadbws) *)((uintptr_t)&__mpsadbws + (Vimm & 7) * 8); \
        *Xmm0 = Fn(*Xmm1, *Xmm2);                                             \
      }                                                                       \
    } else {                                                                  \
      mpsadbw(C, B, A, I);                                                    \
    }                                                                         \
  } while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_MPSADBW_H_ */
