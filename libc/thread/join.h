#ifndef COSMOPOLITAN_LIBC_THREAD_JOIN_H_
#define COSMOPOLITAN_LIBC_THREAD_JOIN_H_
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int cthread_join(cthread_t, void **);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_JOIN_H_ */
