#ifndef COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_
#include "libc/calls/pledge.h"
#include "libc/intrin/promises.internal.h"
COSMOPOLITAN_C_START_

struct Pledges {
  const char *name;
  const uint16_t *syscalls;
  const size_t len;
};

extern const struct Pledges kPledge[PROMISE_LEN_];

int sys_pledge_linux(unsigned long, int);
int ParsePromises(const char *, unsigned long *, unsigned long);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_ */
