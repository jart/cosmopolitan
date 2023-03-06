#ifndef COSMOPOLITAN_LIBC_INTRIN_SHUFPD_H_
#define COSMOPOLITAN_LIBC_INTRIN_SHUFPD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void shufpd(double[2], const double[2], const double[2], uint8_t);

#if !defined(__STRICT_ANSI__) && !defined(__chibicc__)
typedef double shufpd_t _Vector_size(16) forcealign(16) mayalias;
shufpd_t shufpdjt(shufpd_t, shufpd_t);
#define shufpd(C, B, A, I)                                          \
  do {                                                              \
    if (__builtin_expect(X86_NEED(SSE) && X86_HAVE(SSSE3), 1)) {    \
      shufpd_t *Xmm0 = (void *)(C);                                 \
      const shufpd_t *Xmm1 = (const shufpd_t *)(B);                 \
      const shufpd_t *Xmm2 = (const shufpd_t *)(A);                 \
      if (__builtin_constant_p(I)) {                                \
        if (!X86_NEED(AVX)) {                                       \
          asm("shufpd\t%2,%1,%0"                                    \
              : "=x"(*Xmm0)                                         \
              : "x"(*Xmm2), "i"(I), "0"(*Xmm1));                    \
        } else {                                                    \
          asm("vshufpd\t%3,%2,%1,%0"                                \
              : "=x"(*Xmm0)                                         \
              : "x"(*Xmm1), "x"(*Xmm2), "i"(I));                    \
        }                                                           \
      } else {                                                      \
        uint8_t Vimm = (I);                                         \
        typeof(shufpdjt) *Fn;                                       \
        Fn = (typeof(shufpdjt) *)((uintptr_t)&shufpdjt + Vimm * 8); \
        *Xmm0 = Fn(*Xmm1, *Xmm2);                                   \
      }                                                             \
    } else {                                                        \
      shufpd(C, B, A, I);                                           \
    }                                                               \
  } while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_SHUFPD_H_ */
