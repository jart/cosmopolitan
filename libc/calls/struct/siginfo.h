#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_H_
#include "libc/calls/struct/sigval.h"
COSMOPOLITAN_C_START_

struct siginfo {
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code; /* {SICODE,SEGV,ILL,FPE,POLL}_xxx */
  union {
    struct {
      union {
        struct {
          /* signals sent by kill() and sigqueue() set these */
          int32_t si_pid;
          uint32_t si_uid;
        };
        struct {
          /* SIGALRM sets these */
          int32_t si_timerid;
          int32_t si_overrun;
        };
      };
      union {
        union sigval si_value; /* provided by third arg of sigqueue(2) */
        struct {
          int32_t si_status;
          int64_t si_utime;
          int64_t si_stime;
        };
      };
    };
    struct {
      void *si_addr;
      int16_t si_addr_lsb;
      union {
        struct {
          void *si_lower;
          void *si_upper;
        };
        uint32_t si_pkey;
      };
    };
    struct {
      int64_t si_band; /* SIGPOLL */
      int32_t si_fd;
    };
    struct {
      void *si_call_addr;
      int32_t si_syscall;
      uint32_t si_arch;
    };
    char __ignoreme[128 - 2 * sizeof(int32_t) - sizeof(int64_t)];
  };
};

typedef struct siginfo siginfo_t;

#ifdef _COSMO_SOURCE
void __minicrash(int, siginfo_t *, void *) libcesque;
char __is_stack_overflow(siginfo_t *, void *) libcesque;
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_H_ */
