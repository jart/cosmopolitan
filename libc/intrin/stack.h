#ifndef COSMOPOLITAN_LIBC_STACK_H_
#define COSMOPOLITAN_LIBC_STACK_H_
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

void cosmo_stack_lock(void);
void cosmo_stack_unlock(void);
void cosmo_stack_wipe(void);

bool TellOpenbsdThisIsStackMemory(void *, size_t);
bool FixupCustomStackOnOpenbsd(pthread_attr_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STACK_H_ */
