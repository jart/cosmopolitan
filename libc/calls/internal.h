#ifndef COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#ifndef __STRICT_ANSI__
#include "libc/calls/calls.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/startupinfo.h"
#include "libc/nt/struct/systeminfo.h"
#include "libc/runtime/runtime.h"
#include "libc/time/struct/timezone.h"
#include "libc/time/struct/utimbuf.h"

#define kSigactionMinRva 8 /* >SIG_{ERR,DFL,IGN,...} */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kIoMotion ((const int8_t[3]){1, 0, 0})

struct NtContext;
struct NtWin32FileAttributeData;
struct ZiposHandle;
struct __darwin_siginfo;
struct __darwin_ucontext;

struct IoctlPtmGet {
  int theduxfd;
  int workerfd;
  char theduxname[16];
  char workername[16];
};

struct Fds {
  size_t f;  // arbitrary free slot start search index
  size_t n;  // monotonic capacity
  struct Fd {
    int64_t handle;
    int64_t extra;
    enum FdKind {
      kFdEmpty,
      kFdFile,
      kFdSocket,
      kFdProcess,
      kFdConsole,
      kFdSerial,
      kFdZip,
      kFdEpoll,
    } kind;
    unsigned flags;
  } * p;
  struct Fd __init_p[OPEN_MAX];
};

extern const struct Fd kEmptyFd;

hidden extern int g_sighandrvas[NSIG];
hidden extern struct Fds g_fds;
hidden extern struct NtSystemInfo g_ntsysteminfo;
hidden extern struct NtStartupInfo g_ntstartupinfo;
hidden extern const struct NtSecurityAttributes kNtIsInheritable;

ssize_t __getemptyfd(void) hidden;
int __ensurefds(int) hidden;
void __removefd(int) hidden;
bool __isfdopen(int) hidden nosideeffect;
bool __isfdkind(int, enum FdKind) hidden nosideeffect;

forceinline size_t clampio(size_t size) {
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

char *getcwd$sysv(char *, u64) hidden;
i32 __dup3$sysv(i32, i32, i32) hidden;
i32 __fstat$sysv(i32, struct stat *) hidden;
i32 __fstatat$sysv(i32, const char *, struct stat *, i32) hidden;
i32 __pipe2$sysv(i32[hasatleast 2], u32) hidden;
i32 __utimensat$sysv(i32, const char *, const struct timespec *, i32) hidden;
i32 chdir$sysv(const char *) hidden;
i32 clock_gettime$sysv(i32, struct timespec *) hidden;
i32 close$sysv(i32) hidden;
i32 dup$sysv(i32) hidden;
i32 dup2$sysv(i32, i32) hidden;
i32 dup3$sysv(i32, i32, i32) hidden;
i32 execve$sysv(const char *, char *const[], char *const[]) hidden;
i32 faccessat$sysv(i32, const char *, i32, u32) hidden;
i32 fadvise$sysv(i32, i64, i64, i32) hidden;
i32 fallocate$sysv(i64, i32, i64, i64) hidden;
i32 fchmod$sysv(i32, u32) hidden;
i32 fchmodat$sysv(i32, const char *, u32, u32) hidden;
i32 fchown$sysv(i64, u32, u32) hidden;
i32 fchownat$sysv(i32, const char *, u32, u32, u32) hidden;
i32 fcntl$sysv(i32, i32, i32) hidden;
i32 fdatasync$sysv(i32) hidden;
i32 flock$sysv(i32, i32) hidden;
i32 fork$sysv(void) hidden;
i32 fstat$sysv(i32, struct stat *) hidden;
i32 fstatat$sysv(i32, const char *, struct stat *, i32) hidden;
i32 fsync$sysv(i32) hidden;
i32 ftruncate$sysv(i32, i64) hidden;
i32 futimes$sysv(i32, const struct timeval *) hidden;
i32 futimesat$sysv(i32, const char *, const struct timeval *) hidden;
i32 getdents(i32, char *, u32) hidden;
i32 getppid$sysv(void) hidden;
i32 getpriority$sysv(i32, u32) hidden;
i32 getrlimit$sysv(i32, struct rlimit *) hidden;
i32 getrusage$sysv(i32, struct rusage *) hidden;
i32 gettimeofday$sysv(struct timeval *, struct timezone *) hidden;
i32 ioctl$sysv(i32, u64, void *) hidden;
i32 kill$sysv(i32, i32, i32) hidden;
i32 linkat$sysv(i32, const char *, i32, const char *, i32) hidden;
i32 lseek$sysv(i32, i64, i32) hidden;
i32 lutimes$sysv(const char *, const struct timeval *) hidden;
i32 madvise$sysv(void *, size_t, i32) hidden;
i32 memfd_create$sysv(const char *, u32) hidden;
i32 mkdirat$sysv(i32, const char *, u32) hidden;
i32 mkfifo$sysv(const char *, u32) hidden;
i32 mknod$sysv(const char *, u32, u64) hidden;
i32 mprotect$sysv(void *, u64, i32) hidden;
i32 msync$sysv(void *, u64, i32) hidden;
i32 munmap$sysv(void *, u64) hidden;
i32 nanosleep$sysv(const struct timespec *, struct timespec *) hidden;
i32 openat$sysv(i32, const char *, i32, i32) hidden;
i32 pause$sysv(void) hidden;
i32 pipe$sysv(i32[hasatleast 2]) hidden;
i32 pipe2$sysv(i32[hasatleast 2], u32) hidden;
i32 posix_fallocate$sysv(i64, i64, i64) hidden;
i32 posix_openpt$sysv(i32) hidden;
i32 renameat$sysv(i32, const char *, i32, const char *) hidden;
i32 sched_setaffinity$sysv(i32, u64, const void *) hidden;
i32 sched_yield$sysv(void) hidden;
i32 setitimer$sysv(i32, const struct itimerval *, struct itimerval *) hidden;
i32 setpriority$sysv(i32, u32, i32) hidden;
i32 setrlimit$sysv(i32, const struct rlimit *) hidden;
i32 sigaction$sysv(i32, const void *, void *, i64) hidden;
i32 sigprocmask$sysv(i32, const sigset *, sigset *, u64) hidden;
i32 sigsuspend$sysv(const sigset *, u64) hidden;
i32 symlinkat$sysv(const char *, i32, const char *) hidden;
i32 sync$sysv(void) hidden;
i32 sync_file_range$sysv(i32, i64, i64, u32) hidden;
i32 sysinfo$sysv(struct sysinfo *) hidden;
i32 truncate$sysv(const char *, u64) hidden;
i32 uname$sysv(char *) hidden;
i32 unlinkat$sysv(i32, const char *, i32) hidden;
i32 utime$sysv(const char *, const struct utimbuf *) hidden;
i32 utimensat$sysv(i32, const char *, const struct timespec *, i32) hidden;
i32 utimes$sysv(const char *, const struct timeval *) hidden;
i32 wait4$sysv(i32, i32 *, i32, struct rusage *) hidden;
i64 copy_file_range$sysv(i32, long *, i32, long *, u64, u32) hidden;
i64 getrandom$sysv(void *, u64, u32) hidden;
i64 pread$sysv(i32, void *, u64, i64) hidden;
i64 preadv$sysv(i32, struct iovec *, i32, i64) hidden;
i64 pwrite$sysv(i32, const void *, u64, i64) hidden;
i64 pwritev$sysv(i32, const struct iovec *, i32, i64) hidden;
i64 read$sysv(i32, void *, u64) hidden;
i64 sendfile$sysv(i32, i32, i64 *, u64) hidden;
i64 splice$sysv(i32, i64 *, i32, i64 *, u64, u32) hidden;
i64 vmsplice$sysv(i32, const struct iovec *, i64, u32) hidden;
i64 write$sysv(i32, const void *, u64) hidden;
int getitimer$sysv(i32, struct itimerval *) hidden;
int setresgid$sysv(uint32_t, uint32_t, uint32_t) hidden;
int setresuid$sysv(uint32_t, uint32_t, uint32_t) hidden;
int setsid$sysv(void) hidden;
u32 getgid$sysv(void) hidden;
u32 getpid$sysv(void) hidden;
u32 getsid$sysv(int) hidden;
u32 gettid$sysv(void) hidden;
u32 getuid$sysv(void) hidden;
void *mmap$sysv(void *, u64, u32, u32, i64, i64) hidden;
void *mremap$sysv(void *, u64, u64, i32, void *) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » support                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void __onfork(void) hidden;
bool32 __sigenter(i32, struct siginfo *, struct ucontext *) hidden;
i32 fixupnewfd$sysv(i32, i32) hidden;
i32 tunefd$sysv(i32, i32, i32, i32) hidden;
u32 fprot2nt(i32, i32) hidden;
u32 prot2nt(i32, i32) privileged;
void __restore_rt() hidden;
void __sigenter$xnu(void *, i32, i32, void *, void *) hidden wontreturn;
int utimensat$xnu(int, const char *, const struct timespec *, int) hidden;
int nanosleep$xnu(const struct timespec *, struct timespec *) hidden;
void stat2linux(void *) hidden;
void xnutrampoline(void *, i32, i32, const struct __darwin_siginfo *,
                   const struct __darwin_ucontext *) hidden wontreturn;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » veneers                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int gettimeofday$nt(struct timeval *, struct timezone *) hidden;
bool32 isatty$nt(int) hidden;
char *getcwd$nt(char *, size_t) hidden;
int fork$nt(void) hidden;
int chdir$nt(const char *) hidden;
int close$nt(int) hidden;
int dup$nt(int, int, int) hidden;
int fadvise$nt(int, u64, u64, int) hidden;
int fcntl$nt(int, int, unsigned) hidden;
int getpriority$nt(int) hidden;
int setpriority$nt(int) hidden;
int fdatasync$nt(int) hidden;
int flock$nt(int, int) hidden;
int fstat$nt(i64, struct stat *) hidden;
int ftruncate$nt(int, u64) hidden;
int kill$nt(i64, int) hidden;
int link$nt(const char *, const char *) hidden;
int lstat$nt(const char *, struct stat *) hidden;
int madvise$nt(void *, size_t, int) hidden;
int msync$nt(void *, size_t, int) hidden;
ssize_t open$nt(const char *, u32, i32) nodiscard hidden;
int pipe$nt(int[hasatleast 2], unsigned) hidden;
int rename$nt(const char *, const char *) hidden;
int rmdir$nt(const char *) hidden;
int sched_yield$nt(void) hidden;
int stat$nt(const char *, struct stat *) hidden;
int sync$nt(void) hidden;
int symlink$nt(const char *, const char *) hidden;
int sysinfo$nt(struct sysinfo *) hidden;
int truncate$nt(const char *, u64) hidden;
int unlink$nt(const char *) hidden;
i64 lseek$nt(int, i64, int) hidden;
ssize_t read$nt(struct Fd *, const struct iovec *, size_t, ssize_t) hidden;
ssize_t write$nt(struct Fd *, const struct iovec *, size_t, ssize_t) hidden;
int utimensat$nt(int, const char *, const struct timespec *, int) hidden;
int getrusage$nt(int, struct rusage *) hidden;
int setitimer$nt(int, const struct itimerval *, struct itimerval *) hidden;
int nanosleep$nt(const struct timespec *, struct timespec *) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » support                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t ntreturn(uint32_t);
void WinMainForked(void) hidden;
void *GetProcAddressModule(const char *, const char *) hidden;
int getsetpriority$nt(int, unsigned, int, int (*)(int));
void ntcontext2linux(struct ucontext *, const struct NtContext *) hidden;
struct NtOverlapped *offset2overlap(int64_t, struct NtOverlapped *) hidden;
bool32 ntsetprivilege(i64, const char16_t *, u32) hidden;
bool32 onntconsoleevent$nt(u32) hidden;
void __winalarm(void *, uint32_t, uint32_t) hidden;
int ntaccesscheck(const char16_t *, u32) paramsnonnull() hidden;
int64_t __winerr(void) nocallback privileged;
int __mkntpath(const char *, char16_t[hasatleast PATH_MAX - 16]) hidden;
int __mkntpath2(const char *, char16_t[hasatleast PATH_MAX - 16], int) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » drivers                                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

ssize_t readv$serial(struct Fd *, const struct iovec *, int) hidden;
ssize_t writev$serial(struct Fd *, const struct iovec *, int) hidden;

#undef sigset
#undef i32
#undef i64
#undef u32
#undef u64
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_ */
