#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_H_
#include "libc/calls/struct/timeval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct itimerval {
  struct timeval it_interval; /* {0,0} means singleshot */
  struct timeval it_value;    /* {0,0} means disarm */
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_H_ */
