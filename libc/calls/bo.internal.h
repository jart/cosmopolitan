#ifndef COSMOPOLITAN_LIBC_CALLS_BO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_BO_INTERNAL_H_
#include "libc/dce.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int begin_blocking_operation(void);
void end_blocking_operation(int);

#if SupportsWindows()
#define BEGIN_BLOCKING_OPERATION \
  do {                           \
    int _Flags;                  \
  _Flags = begin_blocking_operation()
#define END_BLOCKING_OPERATION    \
  end_blocking_operation(_Flags); \
  }                               \
  while (0)
#else
#define BEGIN_BLOCKING_OPERATION (void)0
#define END_BLOCKING_OPERATION   (void)0
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_BO_INTERNAL_H_ */
