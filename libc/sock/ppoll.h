#ifndef COSMOPOLITAN_LIBC_SOCK_PPOLL_H_
#define COSMOPOLITAN_LIBC_SOCK_PPOLL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/sock/struct/pollfd.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int ppoll(struct pollfd *, uint64_t, const struct timespec *,
          const struct sigset *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_PPOLL_H_ */
