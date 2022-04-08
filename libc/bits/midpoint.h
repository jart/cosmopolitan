#ifndef COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_
#define COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_
#include "libc/assert.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Computes `(a + b) / 2` assuming unsigned.
 */
#define _midpoint(a, b)         \
  ({                            \
    typeof((a) + (b)) a_ = (a); \
    typeof(a_) b_ = (b);        \
    assert(a_ >= 0);            \
    assert(b_ >= 0);            \
    asm("add\t%1,%0\n\t"        \
        "rcr\t%0"               \
        : "+r"(a_)              \
        : "r"(b_));             \
    a_;                         \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_MIDPOINT_H_ */
