#ifndef COSMOPOLITAN_LIBC_CALLS_FLOCKS_H_
#define COSMOPOLITAN_LIBC_CALLS_FLOCKS_H_
#include "libc/calls/struct/sigset.h"
#include "libc/intrin/fds.h"
COSMOPOLITAN_C_START_

int __flocks_fcntl(struct Fd *, int, int, uintptr_t, sigset_t);
void __flocks_close(struct Fd *);
void __flocks_wipe(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_FLOCKS_H_ */
