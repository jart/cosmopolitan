#ifndef COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_
#define COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_
#include "libc/calls/struct/cpuset.h"
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#include "libc/runtime/stack.h"
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_
/* clang-format off */

int pthread_attr_getschedparam(const pthread_attr_t *, struct sched_param *) libcesque paramsnonnull();
int pthread_attr_getsigmask_np(const pthread_attr_t *, sigset_t *) libcesque paramsnonnull((1));
int pthread_attr_setschedparam(pthread_attr_t *, const struct sched_param *) libcesque paramsnonnull();
int pthread_attr_setsigmask_np(pthread_attr_t *, const sigset_t *) libcesque paramsnonnull((1));
int pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *, const struct timespec *) libcesque paramsnonnull((1, 2));
int pthread_getaffinity_np(pthread_t, size_t, cpu_set_t *) libcesque paramsnonnull();
int pthread_getschedparam(pthread_t, int *, struct sched_param *) libcesque paramsnonnull();
int pthread_setaffinity_np(pthread_t, size_t, const cpu_set_t *) libcesque paramsnonnull();
int pthread_setschedparam(pthread_t, int, const struct sched_param *) libcesque paramsnonnull();
int pthread_timedjoin_np(pthread_t, void **, struct timespec *) libcesque;

/* clang-format off */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_PTHREAD2_H_ */
