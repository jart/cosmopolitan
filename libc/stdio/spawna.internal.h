#ifndef COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct _posix_spawnattr {
  int16_t posix_attr_flags;
  int32_t posix_attr_pgroup;
  sigset_t posix_attr_sigmask;
  sigset_t posix_attr_sigdefault;
  int32_t posix_attr_schedpolicy;
  struct sched_param posix_attr_schedparam;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_SPAWNA_INTERNAL_H_ */
