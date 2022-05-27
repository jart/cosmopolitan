#ifndef COSMOPOLITAN_LIBC_THREAD_ZOMBIE_H_
#define COSMOPOLITAN_LIBC_THREAD_ZOMBIE_H_
#include "libc/thread/descriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void cthread_zombies_add(cthread_t);
void cthread_zombies_reap(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_THREAD_ZOMBIE_H_ */
