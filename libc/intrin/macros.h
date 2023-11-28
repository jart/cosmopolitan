#ifndef COSMOPOLITAN_LIBC_INTRIN_MACROS_H_
#define COSMOPOLITAN_LIBC_INTRIN_MACROS_H_
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"

#define INTRIN_COMMUTATIVE "%"
#define INTRIN_NONCOMMUTATIVE

#if defined(__x86_64__) && !defined(__STRICT_ANSI__)

typedef char __intrin_xmm_t
    __attribute__((__vector_size__(16), __aligned__(16), __may_alias__));

#define INTRIN_SSEVEX_X_X_X_(PURE, ISA, OP, FLAGS, A, B, C)                    \
  do {                                                                         \
    if (X86_HAVE(ISA)) {                                                       \
      __intrin_xmm_t *Xmm0 = (void *)(A);                                      \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B);                \
      const __intrin_xmm_t *Xmm2 = (const __intrin_xmm_t *)(C);                \
      if (!X86_NEED(AVX)) {                                                    \
        asm(OP "\t%1,%0" : "=x"(*Xmm0) : FLAGS "x"(*Xmm2), "0"(*Xmm1));        \
      } else {                                                                 \
        asm("v" OP "\t%2,%1,%0" : "=x"(*Xmm0) : FLAGS "x"(*Xmm1), "x"(*Xmm2)); \
      }                                                                        \
    } else {                                                                   \
      PURE(A, B, C);                                                           \
    }                                                                          \
  } while (0)

#define INTRIN_SSEVEX_X_X_I_(PURE, ISA, OP, A, B, I)                 \
  do {                                                               \
    if (X86_HAVE(ISA)) {                                             \
      __intrin_xmm_t *Xmm0 = (void *)(A);                            \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B);      \
      if (!X86_NEED(AVX)) {                                          \
        asm(OP "\t%2,%1,%0" : "=x"(*Xmm0) : "x"(*Xmm1), "i"(I));     \
      } else {                                                       \
        asm("v" OP "\t%2,%1,%0" : "=x"(*Xmm0) : "x"(*Xmm1), "i"(I)); \
      }                                                              \
    } else {                                                         \
      PURE(A, B, I);                                                 \
    }                                                                \
  } while (0)

#define INTRIN_SSEVEX_X_X_(PURE, ISA, OP, A, B)                 \
  do {                                                          \
    if (X86_HAVE(ISA)) {                                        \
      __intrin_xmm_t *Xmm0 = (void *)(A);                       \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B); \
      if (!X86_NEED(AVX)) {                                     \
        asm(OP "\t%1,%0" : "=x"(*Xmm0) : "0"(*Xmm1));           \
      } else {                                                  \
        asm("v" OP "\t%1,%0" : "=x"(*Xmm0) : "x"(*Xmm1));       \
      }                                                         \
    } else {                                                    \
      PURE(A, B);                                               \
    }                                                           \
  } while (0)

#define INTRIN_SSEVEX_X_I_(PURE, ISA, OP, A, B, I)                   \
  do {                                                               \
    if (!IsModeDbg() && X86_HAVE(ISA)) {                             \
      __intrin_xmm_t *Xmm0 = (void *)(A);                            \
      const __intrin_xmm_t *Xmm1 = (const __intrin_xmm_t *)(B);      \
      if (!X86_NEED(AVX)) {                                          \
        asm(OP "\t%1,%0" : "=x"(*Xmm0) : "i"(I), "0"(*Xmm1));        \
      } else {                                                       \
        asm("v" OP "\t%2,%1,%0" : "=x"(*Xmm0) : "x"(*Xmm1), "i"(I)); \
      }                                                              \
    } else {                                                         \
      PURE(A, B, I);                                                 \
    }                                                                \
  } while (0)

#else
#define INTRIN_SSEVEX_X_X_X_(PURE, ISA, OP, FLAGS, A, B, C) PURE(A, B, C)
#define INTRIN_SSEVEX_X_X_I_(PURE, ISA, OP, A, B, I)        PURE(A, B, I)
#define INTRIN_SSEVEX_X_I_(PURE, ISA, OP, A, B, I)          PURE(A, B, I)
#define INTRIN_SSEVEX_X_X_(PURE, ISA, OP, A, B)             PURE(A, B)
#endif /* X86 && !ANSI */

#endif /* COSMOPOLITAN_LIBC_INTRIN_MACROS_H_ */
