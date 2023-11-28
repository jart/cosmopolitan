#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_H_
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
COSMOPOLITAN_C_START_

struct pollfd {
  int32_t fd;
  int16_t events;
  int16_t revents;
};

int poll(struct pollfd *, uint64_t, int32_t);
int ppoll(struct pollfd *, uint64_t, const struct timespec *, const sigset_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_POLLFD_H_ */
