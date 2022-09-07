#ifndef COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_
#define COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/timespec.h"
#include "libc/intrin/pthread.h"
#include "libc/runtime/stack.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *);
int pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *);
int pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
                           const struct timespec *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_ */
