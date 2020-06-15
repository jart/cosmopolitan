#ifndef COSMOPOLITAN_LIBC_MACROS_H_
#define COSMOPOLITAN_LIBC_MACROS_H_
#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § macros                                                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

/**
 * @fileoverview Common C preprocessor, assembler, and linker macros.
 *
 * @see libc/bits/safemacros.h which it's really complicated
 */

#define TRUE  1
#define FALSE 0

#define ROUNDUP(X, K)       (((X) + ((K)-1)) & ~((K)-1))
#define ROUNDDOWN(X, K)     ((X) & -(K))
#define ABS(X)              ((X) >= 0 ? (X) : -(X))
#define MIN(X, Y)           ((Y) > (X) ? (X) : (Y))
#define MAX(X, Y)           ((Y) < (X) ? (X) : (Y))
#define MOD(X, Y)           ((X) - (ABS(Y)) * ((X) / ABS(Y)))
#define PASTE(A, B)         __PASTE(A, B)
#define STRINGIFY(A)        __STRINGIFY(A)
#define EQUIVALENT(X, Y)    (isconstant((X) == (Y)) && ((X) == (Y)))
#define TYPE_BIT(type)      (sizeof(type) * CHAR_BIT)
#define TYPE_SIGNED(type)   (((type)-1) < 0)
#define TYPE_INTEGRAL(type) (((type)0.5) != 0.5)
#define INT_STRLEN_MAXIMUM(type) \
  ((TYPE_BIT(type) - TYPE_SIGNED(type)) * 302 / 1000 + 1 + TYPE_SIGNED(type))

#define ARRAYLEN(A) \
  ((sizeof(A) / sizeof(*(A))) / ((unsigned)!(sizeof(A) % sizeof(*(A)))))

#define __STRINGIFY(A) #A
#define __PASTE(A, B)  A##B
#ifdef __ASSEMBLER__
#include "libc/macros-cpp.inc"
#include "libc/macros.inc"
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_MACROS_H_ */
