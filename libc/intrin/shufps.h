#ifndef COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_
#define COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void shufps(float[4], const float[4], const float[4], uint8_t);

#if !defined(__STRICT_ANSI__) && !defined(__chibicc__)
typedef float shufps_t _Vector_size(16) forcealign(16) mayalias;
shufps_t shufpsjt(shufps_t, shufps_t);
#define shufps(C, B, A, I)                                          \
  do {                                                              \
    if (__builtin_expect(X86_NEED(SSE) && X86_HAVE(SSSE3), 1)) {    \
      shufps_t *Xmm0 = (void *)(C);                                 \
      const shufps_t *Xmm1 = (const shufps_t *)(B);                 \
      const shufps_t *Xmm2 = (const shufps_t *)(A);                 \
      if (__builtin_constant_p(I)) {                                \
        if (!X86_NEED(AVX)) {                                       \
          asm("shufps\t%2,%1,%0"                                    \
              : "=x"(*Xmm0)                                         \
              : "x"(*Xmm2), "i"(I), "0"(*Xmm1));                    \
        } else {                                                    \
          asm("vshufps\t%3,%2,%1,%0"                                \
              : "=x"(*Xmm0)                                         \
              : "x"(*Xmm1), "x"(*Xmm2), "i"(I));                    \
        }                                                           \
      } else {                                                      \
        uint8_t Vimm = (I);                                         \
        typeof(shufpsjt) *Fn;                                       \
        Fn = (typeof(shufpsjt) *)((uintptr_t)&shufpsjt + Vimm * 8); \
        *Xmm0 = Fn(*Xmm1, *Xmm2);                                   \
      }                                                             \
    } else {                                                        \
      shufps(C, B, A, I);                                           \
    }                                                               \
  } while (0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_ */
