#ifndef COSMOPOLITAN_LIBC_RUNTIME_CLONE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CLONE_INTERNAL_H_
#include "libc/atomic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int clone(void *, void *, size_t, int, void *, void *, void *, void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CLONE_INTERNAL_H_ */
