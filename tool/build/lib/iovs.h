#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_
#include "libc/calls/struct/iovec.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Iovs {
  struct iovec *p;
  unsigned i, n;
  struct iovec init[2];
};

int AppendIovs(struct Iovs *, void *, size_t);

forceinline void InitIovs(struct Iovs *ib) {
  ib->p = ib->init;
  ib->i = 0;
  ib->n = ARRAYLEN(ib->init);
}

forceinline void FreeIovs(struct Iovs *ib) {
  if (ib->p != ib->init) {
    free(ib->p);
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_IOVS_H_ */
