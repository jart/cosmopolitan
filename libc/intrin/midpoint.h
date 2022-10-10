#ifndef COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_
#define COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_
#include "libc/assert.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86__)
/**
 * Computes `(a + b) / 2` assuming unsigned.
 *
 * This implementation is the fastest on AMD Zen architecture.
 */
#define _midpoint(a, b)         \
  ({                            \
    typeof((a) + (b)) a_ = (a); \
    typeof(a_) b_ = (b);        \
    _unassert(a_ >= 0);         \
    _unassert(b_ >= 0);         \
    asm("add\t%1,%0\n\t"        \
        "rcr\t%0"               \
        : "+r"(a_)              \
        : "r"(b_));             \
    a_;                         \
  })
#else
/**
 * Computes `(a + b) / 2` assuming unsigned.
 */
#define _midpoint(a, b) (((a) & (b)) + ((a) ^ (b)) / 2)
#endif /* __GNUC__ && !__STRICT_ANSI__ && x86 */

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_ */
