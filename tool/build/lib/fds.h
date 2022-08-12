#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_
#include "libc/calls/struct/iovec.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MachineFdClosed {
  unsigned fd;
  struct MachineFdClosed *next;
};

struct MachineFdCb {
  int (*close)(int);
  ssize_t (*readv)(int, const struct iovec *, int);
  ssize_t (*writev)(int, const struct iovec *, int);
  int (*ioctl)(int, int, ...);
  int (*poll)(struct pollfd *, uint64_t, int32_t);
};

struct MachineFd {
  int fd;
  struct MachineFdCb *cb;
};

struct MachineFds {
  size_t i, n;
  struct MachineFd *p;
  struct MachineFdClosed *closed;
};

int MachineFdAdd(struct MachineFds *);
void MachineFdRemove(struct MachineFds *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_FDS_H_ */
