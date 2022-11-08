#ifndef COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_
#include "libc/calls/pledge.h"
#include "libc/intrin/promises.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Pledges {
  const char *name;
  const uint16_t *syscalls;
  const size_t len;
};

_Hide extern const struct Pledges kPledge[PROMISE_LEN_];

int sys_pledge_linux(unsigned long, int) _Hide;
int ParsePromises(const char *, unsigned long *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_PLEDGE_INTERNAL_H_ */
