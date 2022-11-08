#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_INTERNAL_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/mem/alloca.h"
#include "libc/sock/struct/pollfd.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int32_t sys_poll(struct pollfd *, uint64_t, signed) _Hide;
int sys_ppoll(struct pollfd *, size_t, const struct timespec *,
              const sigset_t *, size_t);
int sys_poll_metal(struct pollfd *, size_t, unsigned);
int sys_poll_nt(struct pollfd *, uint64_t, uint64_t *, const sigset_t *) _Hide;

const char *DescribePollFds(char[300], ssize_t, struct pollfd *, size_t);
#define DescribePollFds(x, y, z) DescribePollFds(alloca(300), x, y, z)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_INTERNAL_H_ */
