#ifndef COSMOPOLITAN_LIBC_CALLS_SCHED_H_
#define COSMOPOLITAN_LIBC_CALLS_SCHED_H_
#include "libc/calls/struct/sched_param.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sched_setscheduler(int, int, const struct sched_param *);
int sched_getscheduler(int);
int sched_setparam(int, const struct sched_param *);
int sched_getparam(int, struct sched_param *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SCHED_H_ */
