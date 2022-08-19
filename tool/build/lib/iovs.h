#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_
#include "libc/calls/struct/iovec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Iovs {
  struct iovec *p;
  unsigned i, n;
  struct iovec init[2];
};

void InitIovs(struct Iovs *);
void FreeIovs(struct Iovs *);
int AppendIovs(struct Iovs *, void *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_ */
