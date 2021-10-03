#ifndef COSMOPOLITAN_LIBC_THREAD_DETACH_H_
#define COSMOPOLITAN_LIBC_THREAD_DETACH_H_
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * @fileoverview detach a thread
 */

int cthread_detach(cthread_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_DETACH_H_ */
 
