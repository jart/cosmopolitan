#ifndef COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#include "libc/atomic.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigval.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/stdbool.h"

#define kSigactionMinRva 8 /* >SIG_{ERR,DFL,IGN,...} */

COSMOPOLITAN_C_START_

#define kIoMotion ((const int8_t[3]){1, 0, 0})

extern struct Fds g_fds;
extern atomic_int __umask;
extern const struct Fd kEmptyFd;

int __reservefd(int);
int __reservefd_unlocked(int);
void __releasefd(int);
int __ensurefds(int);
uint32_t sys_getuid_nt(void);
int __ensurefds_unlocked(int);
void __printfds(struct Fd *, size_t);
int CountConsoleInputBytes(void);
int FlushConsoleInputBytes(void);
int64_t GetConsoleInputHandle(void);
int64_t GetConsoleOutputHandle(void);
int IsWindowsExecutable(int64_t, const char16_t *);
void InterceptTerminalCommands(const char *, size_t);

forceinline bool __isfdopen(int fd) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind != kFdEmpty;
}

forceinline bool __isfdkind(int fd, int kind) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind == kind;
}

int _check_signal(bool);
int _check_cancel(void);
int sys_close_nt(int, int);
int _park_norestart(uint32_t, uint64_t);
int _park_restartable(uint32_t, uint64_t);
int sys_openat_metal(int, const char *, int, unsigned);

#ifdef __x86_64__
bool __iswsl1(void);
#else
#define __iswsl1() false
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_ */
