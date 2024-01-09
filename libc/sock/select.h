#ifndef COSMOPOLITAN_LIBC_SOCK_SELECT_H_
#define COSMOPOLITAN_LIBC_SOCK_SELECT_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/str/str.h"

#define FD_SETSIZE 1024 /* it's 64 on windows */

COSMOPOLITAN_C_START_

typedef struct fd_set {
  unsigned long fds_bits[FD_SETSIZE / (sizeof(long) * 8)];
} fd_set;

#define FD_ISSET(FD, SET) (((SET)->fds_bits[(FD) >> 6] >> ((FD)&63)) & 1)
#define FD_SET(FD, SET)   ((SET)->fds_bits[(FD) >> 6] |= 1ull << ((FD)&63))
#define FD_CLR(FD, SET)   ((SET)->fds_bits[(FD) >> 6] &= ~(1ull << ((FD)&63)))
#define FD_ZERO(SET)      bzero((SET)->fds_bits, sizeof((SET)->fds_bits))
#define FD_SIZE(bits)     (((bits) + (sizeof(long) * 8) - 1) / sizeof(long))

int select(int, fd_set *, fd_set *, fd_set *, struct timeval *) libcesque;
int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *,
            const sigset_t *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_SELECT_H_ */
