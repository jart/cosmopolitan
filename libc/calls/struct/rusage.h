#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_
#include "libc/calls/struct/timeval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct rusage {
  union {
    struct {
      struct timeval ru_utime; /* user CPU time used */
      struct timeval ru_stime; /* system CPU time used */
      int64_t ru_maxrss;       /* maximum resident set size */
      int64_t ru_ixrss;        /* integral shared memory size */
      int64_t ru_idrss;        /* integral unshared data size */
      int64_t ru_isrss;        /* integral unshared stack size */
      int64_t ru_minflt;       /* page reclaims (soft page faults) */
      int64_t ru_majflt;       /* page faults (hard page faults) */
      int64_t ru_nswap;        /* swaps */
      int64_t ru_inblock;      /* block input operations */
      int64_t ru_oublock;      /* block output operations */
      int64_t ru_msgsnd;       /* IPC messages sent */
      int64_t ru_msgrcv;       /* IPC messages received */
      int64_t ru_nsignals;     /* signals received */
      int64_t ru_nvcsw;        /* voluntary context switches */
      int64_t ru_nivcsw;       /* involuntary context switches */
    };
    uint8_t __conservatism[1024];
  };
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RUSAGE_H_ */
