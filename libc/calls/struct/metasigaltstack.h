#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_METASIGALTSTACK_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_METASIGALTSTACK_H_
#include "libc/calls/struct/sigaltstack.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sigaltstack_bsd {
  void *ss_sp;
  uint64_t ss_size;
  int32_t ss_flags;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_METASIGALTSTACK_H_ */
