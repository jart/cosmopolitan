#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_
#include "libc/calls/struct/timeval.h"
COSMOPOLITAN_C_START_

struct rusage {
  struct timeval ru_utime; /* user CPU time used */
  struct timeval ru_stime; /* system CPU time used */
  int64_t ru_maxrss;       /* maximum resident set size in (kb) */
  int64_t ru_ixrss;        /* shared memory size (integral kb CLK_TCK) */
  int64_t ru_idrss;        /* unshared data size (integral kb CLK_TCK) */
  int64_t ru_isrss;        /* unshared stack size (integral kb CLK_TCK) */
  int64_t ru_minflt;       /* page reclaims */
  int64_t ru_majflt;       /* page faults */
  int64_t ru_nswap;        /* swaps */
  int64_t ru_inblock;      /* block input operations */
  int64_t ru_oublock;      /* block output operations */
  int64_t ru_msgsnd;       /* IPC messages sent */
  int64_t ru_msgrcv;       /* IPC messages received */
  int64_t ru_nsignals;     /* signals received */
  int64_t ru_nvcsw;        /* voluntary context switches */
  int64_t ru_nivcsw;       /* involuntary context switches */
};

int getrusage(int, struct rusage *) libcesque;
int wait3(int *, int, struct rusage *) libcesque;
int wait4(int, int *, int, struct rusage *) libcesque;
void rusage_add(struct rusage *, const struct rusage *) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_ */
