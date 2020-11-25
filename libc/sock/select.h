#ifndef COSMOPOLITAN_LIBC_SOCK_SELECT_H_
#define COSMOPOLITAN_LIBC_SOCK_SELECT_H_
#include "libc/calls/struct/timeval.h"
#include "libc/str/str.h"

#define FD_SETSIZE 1024 /* it's 64 on windows */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef struct fd_set {
  uint64_t fds_bits[FD_SETSIZE / 64];
} fd_set;

#define FD_ISSET(FD, SET) (((SET)->fds_bits[(FD) >> 6] >> ((FD)&63)) & 1)
#define FD_SET(FD, SET)   ((SET)->fds_bits[(FD) >> 6] |= 1ull << ((FD)&63))
#define FD_CLR(FD, SET)   ((SET)->fds_bits[(FD) >> 6] &= ~(1ull << ((FD)&63)))
#define FD_ZERO(SET)      memset((SET)->fds_bits, 0, sizeof((SET)->fds_bits))

int select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SELECT_H_ */
