#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_XNU_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_XNU_H_
#include "libc/calls/struct/sigval.h"
COSMOPOLITAN_C_START_

struct siginfo_xnu {
  int32_t si_signo;
  int32_t si_errno;
  int32_t si_code;
  int32_t si_pid;
  int32_t si_uid;
  int32_t si_status;
  void *si_addr;
  union sigval si_value;
  int64_t si_band;
  uint64_t __pad[7];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_XNU_H_ */
