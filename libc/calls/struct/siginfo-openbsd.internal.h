#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_
#include "libc/calls/struct/sigval.h"
COSMOPOLITAN_C_START_

struct siginfo_openbsd {
  int32_t si_signo;
  int32_t si_code;
  int32_t si_errno;
  union {
    int32_t _pad[(128 / 4) - 3];
    struct {
      int32_t si_pid;
      int32_t si_uid;
      union {
        struct {
          union sigval si_value;
        };
        struct {
          int64_t si_utime;
          int64_t si_stime;
          int si_status;
        };
      };
    };
    struct {
      void *si_addr;
      int si_trapno;
    };
  };
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_ */
