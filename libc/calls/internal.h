#ifndef COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction-xnu.internal.h"
#include "libc/calls/struct/siginfo-xnu.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigval.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/ucontext.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/ntexceptionpointers.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/nt/struct/win32fileattributedata.h"
#include "libc/runtime/runtime.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/struct/utimbuf.h"

#define kSigactionMinRva 8 /* >SIG_{ERR,DFL,IGN,...} */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kIoMotion ((const int8_t[3]){1, 0, 0})

struct IoctlPtmGet {
  int theduxfd;
  int workerfd;
  char theduxname[16];
  char workername[16];
};

enum FdKind {
  kFdEmpty,
  kFdFile,
  kFdSocket,
  kFdProcess,
  kFdConsole,
  kFdSerial,
  kFdZip,
  kFdEpoll,
  kFdReserved
};

struct Fd {
  enum FdKind kind;
  unsigned flags;
  unsigned mode;
  int64_t handle;
  int64_t extra;
  bool zombie;
};

struct Fds {
  int f;    /* lowest free slot */
  size_t n; /* monotonic capacity */
  struct Fd *p;
  struct Fd __init_p[OPEN_MAX];
};

hidden extern struct Fds g_fds;
hidden extern const struct Fd kEmptyFd;

int __reservefd(int) hidden;
int __reservefd_unlocked(int) hidden;
void __releasefd(int) hidden;
void __releasefd_unlocked(int) hidden;
int __ensurefds(int) hidden;
int __ensurefds_unlocked(int) hidden;
void __printfds(void) hidden;

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

#define i32    int32_t
#define i64    int64_t
#define u32    uint32_t
#define u64    uint64_t
#define sigset struct sigset

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » synthetic jump slots             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

axdx_t sys_gettimeofday(struct timeval *, struct timezone *, void *) hidden;
i32 __sys_getrusage(i32, struct rusage *) hidden;
i32 __sys_sigprocmask(i32, const sigset *, sigset *, u64) hidden;
i32 __sys_utimensat(i32, const char *, const struct timespec *, i32) hidden;
i32 __sys_wait4(i32, i32 *, i32, struct rusage *) hidden;
i32 sys_clock_gettime(i32, struct timespec *) hidden;
i32 sys_clock_gettime_xnu(i32, struct timespec *) hidden;
i32 sys_fstat(i32, struct stat *) hidden;
i32 sys_fstatat(i32, const char *, struct stat *, i32) hidden;
i32 sys_futimes(i32, const struct timeval *) hidden;
i32 sys_futimesat(i32, const char *, const struct timeval *) hidden;
i32 sys_getitimer(i32, struct itimerval *) hidden;
i32 sys_getrlimit(i32, struct rlimit *) hidden;
i32 sys_getrusage(i32, struct rusage *) hidden;
i32 sys_lutimes(const char *, const struct timeval *) hidden;
i32 sys_nanosleep(const struct timespec *, struct timespec *) hidden;
i32 sys_setitimer(i32, const struct itimerval *, struct itimerval *) hidden;
i32 sys_setrlimit(i32, const struct rlimit *) hidden;
i32 sys_sigprocmask(i32, const sigset *, sigset *) hidden;
i32 sys_sigqueue(i32, i32, const union sigval) hidden;
i32 sys_sigqueueinfo(i32, const siginfo_t *) hidden;
i32 sys_sigsuspend(const sigset *, u64) hidden;
i32 sys_sysinfo(struct sysinfo *) hidden;
i32 sys_utime(const char *, const struct utimbuf *) hidden;
i32 sys_utimensat(i32, const char *, const struct timespec *, i32) hidden;
i32 sys_utimes(const char *, const struct timeval *) hidden;
i32 sys_wait4(i32, i32 *, i32, struct rusage *) hidden;
i64 sys_preadv(i32, struct iovec *, i32, i64, i64) hidden;
i64 sys_pwritev(i32, const struct iovec *, i32, i64, i64) hidden;
i64 sys_vmsplice(i32, const struct iovec *, i64, u32) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » support                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int sys_nanosleep_xnu(const struct timespec *, struct timespec *) hidden;
int sys_utimensat_xnu(int, const char *, const struct timespec *, int) hidden;
size_t __iovec_size(const struct iovec *, size_t) hidden;
ssize_t WritevUninterruptible(int, struct iovec *, int);
void __rusage2linux(struct rusage *) hidden;
void __sigenter_xnu(void *, i32, i32, struct siginfo_xnu *,
                    struct __darwin_ucontext *) hidden;
void __stat2cosmo(struct stat *restrict, const union metastat *) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » veneers                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int ioctl_tiocgwinsz_nt(struct Fd *, struct winsize *) hidden;
int sys_close_nt(struct Fd *) hidden;
int sys_fstat_nt(i64, struct stat *) hidden;
int sys_fstatat_nt(int, const char *, struct stat *, int) hidden;
int sys_getrusage_nt(int, struct rusage *) hidden;
int sys_gettimeofday_nt(struct timeval *, struct timezone *) hidden;
int sys_lstat_nt(const char *, struct stat *) hidden;
int sys_nanosleep_nt(const struct timespec *, struct timespec *) hidden;
int sys_setitimer_nt(int, const struct itimerval *, struct itimerval *) hidden;
int sys_setrlimit_nt(int, const struct rlimit *) hidden;
int sys_sysinfo_nt(struct sysinfo *) hidden;
int sys_utimensat_nt(int, const char *, const struct timespec *, int) hidden;
int sys_utimes_nt(const char *, const struct timeval[2]) hidden;
ssize_t sys_read_nt(struct Fd *, const struct iovec *, size_t, ssize_t) hidden;
ssize_t sys_write_nt(int, const struct iovec *, size_t, ssize_t) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » support                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

bool _check_interrupts(bool, struct Fd *) hidden;
int sys_clock_gettime_nt(int, struct timespec *) hidden;
ssize_t sys_readv_nt(struct Fd *, const struct iovec *, int) hidden;
ssize_t sys_writev_nt(int, const struct iovec *, int) hidden;
unsigned __wincrash_nt(struct NtExceptionPointers *);
void _ntcontext2linux(struct ucontext *, const struct NtContext *) hidden;
void _ntlinux2context(struct NtContext *, const ucontext_t *) hidden;
struct NtOverlapped *_offset2overlap(int64_t, int64_t,
                                     struct NtOverlapped *) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » metal                                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int sys_fstat_metal(int, struct stat *);
int sys_openat_metal(int, const char *, int, unsigned);
ssize_t sys_readv_metal(struct Fd *, const struct iovec *, int) hidden;
ssize_t sys_writev_metal(struct Fd *, const struct iovec *, int) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » drivers                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

ssize_t sys_readv_serial(struct Fd *, const struct iovec *, int) hidden;
ssize_t sys_writev_serial(struct Fd *, const struct iovec *, int) hidden;

#undef sigset
#undef i32
#undef i64
#undef u32
#undef u64
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_ */
