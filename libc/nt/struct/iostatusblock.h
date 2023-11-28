#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IOSTATUSBLOCK_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IOSTATUSBLOCK_H_
#include "libc/nt/enum/status.h"

struct NtIoStatusBlock {
  union {
    NtStatus Status;
    void *Pointer; /* reserved for internal use */
  };
  uint32_t *Information; /* request dependent */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IOSTATUSBLOCK_H_ */
