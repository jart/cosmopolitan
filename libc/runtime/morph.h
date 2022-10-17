#ifndef COSMOPOLITAN_LIBC_RUNTIME_MORPH_H_
#define COSMOPOLITAN_LIBC_RUNTIME_MORPH_H_
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __morph_begin(sigset_t *);
void __morph_end(sigset_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_MORPH_H_ */
