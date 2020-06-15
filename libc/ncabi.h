#ifndef COSMOPOLITAN_LIBC_NCABI_H_
#define COSMOPOLITAN_LIBC_NCABI_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview No-Clobber ABI Function Veneers.
 *
 * These macros map be specified alongside alongside the prototype of
 * any function that promises to not clobber anything except %rax and
 * flags. These macros let the compiler know about that promise. That
 * allows the compiler to generate faster, smaller code.
 */

#define NCABI_PRUNABLE asm
#define NCABI_NOPRUNE  asm volatile

#define NCABI_CALL_0(KIND, T, NAME) \
  ({                                \
    T ax;                           \
    KIND("call\t" NAME              \
         : "=a"(ax)                 \
         : /* no inputs */          \
         : "memory", "cc");         \
    ax;                             \
  })

#define NCABI_CALL_1(KIND, T, NAME, T1, P1)                    \
  ({                                                           \
    T ax;                                                      \
    KIND("call\t" NAME : "=a"(ax) : "D"(P1) : "memory", "cc"); \
    ax;                                                        \
  })

#define NCABI_CALL_2(KIND, T, NAME, T1, P1, T2, P2)                     \
  ({                                                                    \
    T ax;                                                               \
    KIND("call\t" NAME : "=a"(ax) : "D"(P1), "S"(P2) : "memory", "cc"); \
    ax;                                                                 \
  })

#define NCABI_CALL_3(KIND, T, NAME, T1, P1, T2, P2, T3, P3) \
  ({                                                        \
    T ax;                                                   \
    KIND("call\t" NAME                                      \
         : "=a"(ax)                                         \
         : "D"(P1), "S"(P2), "d"(P3)                        \
         : "memory", "cc");                                 \
    ax;                                                     \
  })

#define NCABI_CALL_4(KIND, T, NAME, T1, P1, T2, P2, T3, P3, T4, P4) \
  ({                                                                \
    T ax;                                                           \
    KIND("call\t" NAME                                              \
         : "=a"(ax)                                                 \
         : "D"(P1), "S"(P2), "d"(P3), "c"(P4)                       \
         : "memory", "cc");                                         \
    ax;                                                             \
  })

#define NCABI_DECLARE_0(KIND, T, ALIAS, NAME) \
  forceinline nodebuginfo T ALIAS(void) { return NCABI_CALL_0(KIND, T, NAME); }

#define NCABI_DECLARE_1(KIND, T, ALIAS, NAME, T1) \
  forceinline nodebuginfo T ALIAS(T1 p1) {        \
    return NCABI_CALL_1(KIND, T, NAME, T1, p1);   \
  }

#define NCABI_DECLARE_2(KIND, T, ALIAS, NAME, T1, T2)   \
  forceinline nodebuginfo T ALIAS(T1 p1, T2 p2) {       \
    return NCABI_CALL_2(KIND, T, NAME, T1, p1, T2, p2); \
  }

#define NCABI_DECLARE_3(KIND, T, ALIAS, NAME, T1, T2, T3)       \
  forceinline nodebuginfo T ALIAS(T1 p1, T2 p2, T3 p3) {        \
    return NCABI_CALL_3(KIND, T, NAME, T1, p1, T2, p2, T3, p3); \
  }

#define NCABI_DECLARE_4(KIND, T, ALIAS, NAME, T1, T2, T3, T4)           \
  forceinline nodebuginfo T ALIAS(T1 p1, T2 p2, T3 p3, T4 p4) {         \
    return NCABI_CALL_4(KIND, T, NAME, T1, p1, T2, p2, T3, p3, T4, p4); \
  }

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NCABI_H_ */
