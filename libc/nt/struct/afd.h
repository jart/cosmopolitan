#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_AFD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_AFD_H_
#include "libc/nt/enum/status.h"
COSMOPOLITAN_C_START_

struct NtAfdPollHandleInfo {
  int64_t Handle;
  uint32_t Events;
  NtStatus Status;
};

struct NtAfdPollInfo {
  int64_t Timeout;
  uint32_t NumberOfHandles;
  uint32_t Exclusive;
  struct NtAfdPollHandleInfo Handles[1];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_AFD_H_ */
