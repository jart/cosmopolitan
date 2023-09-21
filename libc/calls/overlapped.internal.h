#ifndef COSMOPOLITAN_LIBC_CALLS_OVERLAPPED_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_OVERLAPPED_INTERNAL_H_
#include "libc/nt/struct/overlapped.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define overlapped_cleanup_push(handle, overlap)                     \
  {                                                                  \
    struct OverlappedCleanup overlapped_cleanup = {handle, overlap}; \
    pthread_cleanup_push(overlapped_cleanup_callback, &overlapped_cleanup);

#define overlapped_cleanup_pop() \
  pthread_cleanup_pop(false);    \
  }

struct OverlappedCleanup {
  int64_t handle;
  struct NtOverlapped *overlap;
};

void overlapped_cleanup_callback(void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_OVERLAPPED_INTERNAL_H_ */
