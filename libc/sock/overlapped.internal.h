#ifndef COSMOPOLITAN_LIBC_SOCK_OVERLAPPED_H_
#define COSMOPOLITAN_LIBC_SOCK_OVERLAPPED_H_
#include "libc/nt/struct/overlapped.h"
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define wsa_overlapped_cleanup_push(handle, overlap)                        \
  {                                                                         \
    struct WsaOverlappedCleanup wsa_overlapped_cleanup = {handle, overlap}; \
    pthread_cleanup_push(wsa_overlapped_cleanup_callback,                   \
                         &wsa_overlapped_cleanup);

#define wsa_overlapped_cleanup_pop() \
  pthread_cleanup_pop(false);        \
  }

struct WsaOverlappedCleanup {
  int64_t handle;
  struct NtOverlapped *overlap;
};

void wsa_overlapped_cleanup_callback(void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_OVERLAPPED_H_ */
