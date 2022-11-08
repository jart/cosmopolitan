#ifndef COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigval.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"

#define kSigactionMinRva 8 /* >SIG_{ERR,DFL,IGN,...} */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kIoMotion ((const int8_t[3]){1, 0, 0})

_Hide extern struct Fds g_fds;
_Hide extern const struct Fd kEmptyFd;

int __reservefd(int) _Hide;
int __reservefd_unlocked(int) _Hide;
void __releasefd(int) _Hide;
int __ensurefds(int) _Hide;
int __ensurefds_unlocked(int) _Hide;
void __printfds(void) _Hide;

forceinline int64_t __getfdhandleactual(int fd) {
  return g_fds.p[fd].handle;
}

forceinline bool __isfdopen(int fd) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind != kFdEmpty;
}

forceinline bool __isfdkind(int fd, int kind) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind == kind;
}

forceinline size_t _clampio(size_t size) {
  if (!IsTrustworthy()) {
    return MIN(size, 0x7ffff000);
  } else {
    return size;
  }
}

int sys_close_nt(struct Fd *, int) _Hide;
int _check_interrupts(bool, struct Fd *) _Hide;
int sys_openat_metal(int, const char *, int, unsigned);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_ */
