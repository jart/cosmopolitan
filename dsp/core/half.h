#ifndef COSMOPOLITAN_DSP_CORE_HALF_H_
#define COSMOPOLITAN_DSP_CORE_HALF_H_
#include "libc/macros.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Divides integer in half w/ rounding.
 */
#define HALF(X) (((X) + 1) / 2)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_HALF_H_ */
