#ifndef COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_
#define COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_
#include "libc/assert.h"
#include "libc/intrin/macros.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

void pvalignr(void *, const void *, const void *, size_t);

/**
 * Overlaps vectors.
 *
 *     𝑖= 0 means 𝑐←𝑎
 *   0<𝑖<16 means 𝑐←𝑎║𝑏
 *     𝑖=16 means 𝑐←𝑏
 *  16<𝑖<32 means 𝑐←𝑏║0
 *     𝑖≥32 means 𝑐←0
 *
 * @param 𝑖 needs to be a literal, constexpr, or embedding
 * @see pvalignr()
 * @mayalias
 */
static void palignr(void *c, const void *b, const void *a, size_t i) {
  char t[48];
  memcpy(t, a, 16);
  memcpy(t + 16, b, 16);
  memset(t + 32, 0, 16);
  memcpy(c, t + MIN(32, i), 16);
}

#ifndef __STRICT_ANSI__
#define palignr(C, B, A, I)                                     \
  do {                                                          \
    if (!IsModeDbg() && X86_NEED(SSE) && X86_HAVE(SSSE3)) {     \
      __intrin_xmm_t *Xmm0 = (void *)(C);                       \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B); \
      const __intrin_xmm_t *Xmm2 = (const __intrin_xmm_t *)(A); \
      if (!X86_NEED(AVX)) {                                     \
        asm("palignr\t%2,%1,%0"                                 \
            : "=x"(*Xmm0)                                       \
            : "x"(*Xmm2), "i"(I), "0"(*Xmm1));                  \
      } else {                                                  \
        asm("vpalignr\t%3,%2,%1,%0"                             \
            : "=x"(*Xmm0)                                       \
            : "x"(*Xmm1), "x"(*Xmm2), "i"(I));                  \
      }                                                         \
    } else {                                                    \
      palignr(C, B, A, I);                                      \
    }                                                           \
  } while (0)
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PALIGNR_H_ */
