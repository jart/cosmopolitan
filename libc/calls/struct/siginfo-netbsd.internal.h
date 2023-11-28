#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_NETBSD_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_NETBSD_H_
#include "libc/calls/struct/sigval.h"
COSMOPOLITAN_C_START_

struct siginfo_netbsd {
  int32_t si_signo;
  int32_t si_code;
  int32_t si_errno;
  int32_t __pad;
  union {
    struct { /* RT */
      int32_t si_pid;
      int32_t si_uid;
      union sigval si_value;
    };
    struct { /* chld */
      int32_t _pid;
      int32_t _uid;
      int32_t si_status;
      int64_t si_utime;
      int64_t si_stime;
    };
    struct { /* fault */
      void *si_addr;
      int32_t si_trap;
      int32_t si_trap2;
      int32_t si_trap3;
    };
    struct { /* poll */
      int64_t si_band;
      int32_t si_fd;
    };
    struct { /* syscall */
      int32_t si_sysnum;
      int32_t si_retval[2];
      int32_t si_error;
      uint64_t si_args[8];
    };
    struct { /* ptrace */
      int32_t si_pe_report_event;
      union {
        int32_t si_pe_other_pid;
        int32_t si_pe_lwp;
      };
    };
  };
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_NETBSD_H_ */
