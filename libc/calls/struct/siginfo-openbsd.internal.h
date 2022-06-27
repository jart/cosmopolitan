#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_
#include "libc/calls/struct/sigval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct siginfo_openbsd {
  int32_t si_signo;
  int32_t si_code;
  int32_t si_errno;
  union {
    int32_t _pad[(128 / 4) - 3];
    struct {
      int32_t si_pid;
      union {
        struct {
          int32_t si_uid;
          union sigval si_value;
        };
        struct {
          int64_t si_utime;
          int64_t si_stime;
          int32_t si_status;
        };
      };
    };
    struct {
      void *si_addr;
      int32_t si_trapno;
    };
  };
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_OPENBSD_H_ */
