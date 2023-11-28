#ifndef COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_
#define COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_
#include "libc/intrin/macros.h"
#include "libc/str/str.h"
COSMOPOLITAN_C_START_

void palignr(void *, const void *, const void *, unsigned long);

#if !defined(__STRICT_ANSI__) && !defined(__chibicc__) && defined(__x86_64__)
__intrin_xmm_t __palignrs(__intrin_xmm_t, __intrin_xmm_t);
#define palignr(C, B, A, I)                                                \
  do {                                                                     \
    if (__builtin_expect(!IsModeDbg() && X86_NEED(SSE) && X86_HAVE(SSSE3), \
                         1)) {                                             \
      __intrin_xmm_t *Xmm0 = (void *)(C);                                  \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B);            \
      const __intrin_xmm_t *Xmm2 = (const __intrin_xmm_t *)(A);            \
      if (__builtin_constant_p(I)) {                                       \
        if (!X86_NEED(AVX)) {                                              \
          asm("palignr\t%2,%1,%0"                                          \
              : "=x"(*Xmm0)                                                \
              : "x"(*Xmm2), "i"(I), "0"(*Xmm1));                           \
        } else {                                                           \
          asm("vpalignr\t%3,%2,%1,%0"                                      \
              : "=x"(*Xmm0)                                                \
              : "x"(*Xmm1), "x"(*Xmm2), "i"(I));                           \
        }                                                                  \
      } else {                                                             \
        unsigned long Vimm = (I);                                          \
        typeof(__palignrs) *Fn;                                            \
        if (__builtin_expect(Vimm < 32, 1)) {                              \
          Fn = (typeof(__palignrs) *)((uintptr_t)&__palignrs + Vimm * 8);  \
          *Xmm0 = Fn(*Xmm1, *Xmm2);                                        \
        } else {                                                           \
          memset(Xmm0, 0, 16);                                             \
        }                                                                  \
      }                                                                    \
    } else {                                                               \
      palignr(C, B, A, I);                                                 \
    }                                                                      \
  } while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_ */
