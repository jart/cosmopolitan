#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_FREEBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_FREEBSD_H_
#include "libc/calls/struct/sigval.h"
COSMOPOLITAN_C_START_

struct siginfo_freebsd {
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code;
  int32_t si_pid;
  int32_t si_uid;
  int32_t si_status;
  void *si_addr;
  union sigval si_value;
  union {
    struct {
      int32_t si_trapno;
    };
    struct {
      int32_t si_timerid;
      int32_t si_overrun;
    };
    struct {
      int32_t si_mqd;
    };
    struct {
      long si_band;
    };
    struct {
      int64_t __pad1;
      int32_t __pad2[7];
    };
  };
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_FREEBSD_H_ */
