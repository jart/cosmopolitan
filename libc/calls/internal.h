#ifndef COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_INTERNAL_H_
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction-xnu.internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigval.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nt/struct/context.h"
#include "libc/nt/struct/ntexceptionpointers.h"
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
  int64_t handle;
  int64_t extra;
};

struct Fds {
  size_t f; /* lowest free slot */
  size_t n; /* monotonic capacity */
  struct Fd *p;
  struct Fd __init_p[OPEN_MAX];
};

extern const struct Fd kEmptyFd;

hidden extern volatile bool __interrupted;
hidden extern int __vforked;
hidden extern unsigned __sighandrvas[NSIG];
hidden extern struct Fds g_fds;
hidden extern const struct NtSecurityAttributes kNtIsInheritable;

int __reservefd(void) hidden;
void __releasefd(int) hidden;
int __ensurefds(int) hidden;

forceinline bool __isfdopen(int fd) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind != kFdEmpty;
}

forceinline bool __isfdkind(int fd, int kind) {
  return 0 <= fd && fd < g_fds.n && g_fds.p[fd].kind == kind;
}

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

axdx_t __sys_pipe(i32[hasatleast 2], i32) hidden;
axdx_t sys_fork(void) hidden;
axdx_t sys_getpid(void) hidden;
axdx_t sys_gettimeofday(struct timeval *, struct timezone *, void *) hidden;
char *sys_getcwd(char *, u64) hidden;
char *sys_getcwd_xnu(char *, u64) hidden;
i32 __sys_dup3(i32, i32, i32) hidden;
i32 __sys_execve(const char *, char *const[], char *const[]) hidden;
i32 __sys_fcntl(i32, i32, u64) hidden;
i32 __sys_fstat(i32, struct stat *) hidden;
i32 __sys_fstatat(i32, const char *, struct stat *, i32) hidden;
i32 __sys_getrusage(i32, struct rusage *) hidden;
i32 __sys_openat(i32, const char *, i32, u32) hidden;
i32 __sys_pipe2(i32[hasatleast 2], u32) hidden;
i32 __sys_utimensat(i32, const char *, const struct timespec *, i32) hidden;
i32 __sys_wait4(i32, i32 *, i32, struct rusage *) hidden;
i32 getdents(i32, void *, u32, i64 *) hidden;
i32 sys_chdir(const char *) hidden;
i32 sys_clock_gettime(i32, struct timespec *) hidden;
i32 sys_close(i32) hidden;
i32 sys_dup(i32) hidden;
i32 sys_dup2(i32, i32) hidden;
i32 sys_dup3(i32, i32, i32) hidden;
i32 sys_execve(const char *, char *const[], char *const[]) hidden;
i32 sys_faccessat(i32, const char *, i32, u32) hidden;
i32 sys_fadvise(i32, i64, i64, i32) hidden;
i32 sys_fchdir(i32) hidden;
i32 sys_fchmod(i32, u32) hidden;
i32 sys_fchmodat(i32, const char *, u32, u32) hidden;
i32 sys_fchown(i64, u32, u32) hidden;
i32 sys_fchownat(i32, const char *, u32, u32, u32) hidden;
i32 sys_fcntl(i32, i32, u64) hidden;
i32 sys_fdatasync(i32) hidden;
i32 sys_flock(i32, i32) hidden;
i32 sys_fstat(i32, struct stat *) hidden;
i32 sys_fstatat(i32, const char *, struct stat *, i32) hidden;
i32 sys_fsync(i32) hidden;
i32 sys_ftruncate(i32, i64, i64) hidden;
i32 sys_futimes(i32, const struct timeval *) hidden;
i32 sys_futimesat(i32, const char *, const struct timeval *) hidden;
i32 sys_getitimer(i32, struct itimerval *) hidden;
i32 sys_getppid(void) hidden;
i32 sys_getpriority(i32, u32) hidden;
i32 sys_getrlimit(i32, struct rlimit *) hidden;
i32 sys_getrusage(i32, struct rusage *) hidden;
i32 sys_ioctl(i32, u64, void *) hidden;
i32 sys_kill(i32, i32, i32) hidden;
i32 sys_linkat(i32, const char *, i32, const char *, i32) hidden;
i32 sys_lseek(i32, i64, i64, i64) hidden;
i32 sys_lutimes(const char *, const struct timeval *) hidden;
i32 sys_madvise(void *, size_t, i32) hidden;
i32 sys_memfd_create(const char *, u32) hidden;
i32 sys_mkdirat(i32, const char *, u32) hidden;
i32 sys_mkfifo(const char *, u32) hidden;
i32 sys_mknod(const char *, u32, u64) hidden;
i32 sys_mprotect(void *, u64, i32) hidden;
i32 sys_msync(void *, u64, i32) hidden;
i32 sys_munmap(void *, u64) hidden;
i32 sys_nanosleep(const struct timespec *, struct timespec *) hidden;
i32 sys_openat(i32, const char *, i32, u32) hidden;
i32 sys_pause(void) hidden;
i32 sys_pipe(i32[hasatleast 2]) hidden;
i32 sys_pipe2(i32[hasatleast 2], u32) hidden;
i32 sys_posix_openpt(i32) hidden;
i32 sys_renameat(i32, const char *, i32, const char *) hidden;
i32 sys_sched_setaffinity(i32, u64, const void *) hidden;
i32 sys_sched_yield(void) hidden;
i32 sys_setitimer(i32, const struct itimerval *, struct itimerval *) hidden;
i32 sys_setpriority(i32, u32, i32) hidden;
i32 sys_setresgid(uint32_t, uint32_t, uint32_t) hidden;
i32 sys_setresuid(uint32_t, uint32_t, uint32_t) hidden;
i32 sys_setrlimit(i32, const struct rlimit *) hidden;
i32 sys_setsid(void) hidden;
i32 sys_sigaction(i32, const void *, void *, i64, i64) hidden;
i32 sys_sigprocmask(i32, const sigset *, sigset *, u64) hidden;
i32 sys_sigqueue(i32, i32, const union sigval) hidden;
i32 sys_sigqueueinfo(i32, const siginfo_t *) hidden;
i32 sys_sigsuspend(const sigset *, u64) hidden;
i32 sys_symlinkat(const char *, i32, const char *) hidden;
i32 sys_sync(void) hidden;
i32 sys_sync_file_range(i32, i64, i64, u32) hidden;
i32 sys_sysinfo(struct sysinfo *) hidden;
i32 sys_truncate(const char *, u64, u64) hidden;
i32 sys_uname(char *) hidden;
i32 sys_unlinkat(i32, const char *, i32) hidden;
i32 sys_utime(const char *, const struct utimbuf *) hidden;
i32 sys_utimensat(i32, const char *, const struct timespec *, i32) hidden;
i32 sys_utimes(const char *, const struct timeval *) hidden;
i32 sys_wait4(i32, i32 *, i32, struct rusage *) hidden;
i64 sys_copy_file_range(i32, long *, i32, long *, u64, u32) hidden;
i64 sys_getrandom(void *, u64, u32) hidden;
i64 sys_pread(i32, void *, u64, i64, i64) hidden;
i64 sys_preadv(i32, struct iovec *, i32, i64, i64) hidden;
i64 sys_ptrace(int, i32, void *, void *) hidden;
i64 sys_pwrite(i32, const void *, u64, i64, i64) hidden;
i64 sys_pwritev(i32, const struct iovec *, i32, i64, i64) hidden;
i64 sys_read(i32, void *, u64) hidden;
i64 sys_sendfile(i32, i32, i64 *, u64) hidden;
i64 sys_splice(i32, i64 *, i32, i64 *, u64, u32) hidden;
i64 sys_vmsplice(i32, const struct iovec *, i64, u32) hidden;
i64 sys_write(i32, const void *, u64) hidden;
u32 sys_getgid(void) hidden;
u32 sys_getsid(int) hidden;
u32 sys_gettid(void) hidden;
u32 sys_getuid(void) hidden;
void *__sys_mmap(void *, u64, u32, u32, i64, i64, i64) hidden;
void *sys_mremap(void *, u64, u64, i32, void *) hidden;
void sys_exit(int) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » support                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

void __onfork(void) hidden;
i32 __fixupnewfd(i32, i32) hidden;
u32 __prot2nt(i32, i32) privileged;
void __restore_rt() hidden;
int sys_utimensat_xnu(int, const char *, const struct timespec *, int) hidden;
int sys_nanosleep_xnu(const struct timespec *, struct timespec *) hidden;
void __stat2linux(void *) hidden;
void __restore_rt_netbsd(void) hidden;
void __sigenter_xnu(void *, i32, i32, struct __darwin_siginfo *,
                    struct __darwin_ucontext *) hidden;
int gethostname_linux(char *, size_t) hidden;
int gethostname_bsd(char *, size_t) hidden;
int gethostname_nt(char *, size_t) hidden;
size_t __iovec_size(const struct iovec *, size_t) hidden;
void __rusage2linux(struct rusage *) hidden;
ssize_t WritevUninterruptible(int, struct iovec *, int);
void flock2cosmo(uintptr_t);
void cosmo2flock(uintptr_t);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » veneers                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

bool32 sys_isatty_nt(int) hidden;
char *sys_getcwd_nt(char *, size_t) hidden;
i64 sys_lseek_nt(int, i64, int) hidden;
int sys_chdir_nt(const char *) hidden;
int sys_close_epoll_nt(int) hidden;
int sys_close_nt(struct Fd *) hidden;
int sys_dup_nt(int, int, int) hidden;
int sys_execve_nt(const char *, char *const[], char *const[]) hidden;
int sys_faccessat_nt(int, const char *, int, uint32_t) hidden;
int sys_fadvise_nt(int, u64, u64, int) hidden;
int sys_fchdir_nt(int) hidden;
int sys_fcntl_nt(int, int, uintptr_t) hidden;
int sys_fdatasync_nt(int) hidden;
int sys_flock_nt(int, int) hidden;
int sys_fork_nt(void) hidden;
int sys_fstat_nt(i64, struct stat *) hidden;
int sys_fstatat_nt(int, const char *, struct stat *, uint32_t) hidden;
int sys_ftruncate_nt(i64, u64) hidden;
int sys_getppid_nt(void) hidden;
int sys_getpriority_nt(int) hidden;
int sys_getrusage_nt(int, struct rusage *) hidden;
int sys_gettimeofday_nt(struct timeval *, struct timezone *) hidden;
int sys_kill_nt(int, int) hidden;
int sys_link_nt(const char *, const char *) hidden;
int sys_lstat_nt(const char *, struct stat *) hidden;
int sys_madvise_nt(void *, size_t, int) hidden;
int sys_mkdirat_nt(int, const char *, uint32_t) hidden;
int sys_msync_nt(void *, size_t, int) hidden;
int sys_nanosleep_nt(const struct timespec *, struct timespec *) hidden;
int sys_pipe_nt(int[hasatleast 2], unsigned) hidden;
int sys_renameat_nt(int, const char *, int, const char *) hidden;
int sys_sched_yield_nt(void) hidden;
int sys_setitimer_nt(int, const struct itimerval *, struct itimerval *) hidden;
int sys_setpriority_nt(int) hidden;
int sys_symlinkat_nt(const char *, int, const char *) hidden;
int sys_sync_nt(void) hidden;
int sys_sysinfo_nt(struct sysinfo *) hidden;
int sys_truncate_nt(const char *, u64) hidden;
int sys_unlinkat_nt(int, const char *, int) hidden;
int sys_utimes_nt(const char *, const struct timeval[2]) hidden;
int sys_utimensat_nt(int, const char *, const struct timespec *, int) hidden;
ssize_t sys_open_nt(int, const char *, u32, i32) nodiscard hidden;
ssize_t sys_read_nt(struct Fd *, const struct iovec *, size_t, ssize_t) hidden;
ssize_t sys_write_nt(struct Fd *, const struct iovec *, size_t, ssize_t) hidden;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » windows nt » support                           ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int64_t ntreturn(uint32_t);
void WinMainForked(void) hidden;
void *GetProcAddressModule(const char *, const char *) hidden;
int sys_getsetpriority_nt(int, int, int, int (*)(int));
void ntcontext2linux(struct ucontext *, const struct NtContext *) hidden;
struct NtOverlapped *offset2overlap(int64_t, struct NtOverlapped *) hidden;
bool32 ntsetprivilege(i64, const char16_t *, u32) hidden;
void __winalarm(void *, uint32_t, uint32_t) hidden;
int ntaccesscheck(const char16_t *, u32) paramsnonnull() hidden;
int64_t __winerr(void) nocallback privileged;
int __mkntpath(const char *, char16_t[hasatleast PATH_MAX - 16]) hidden;
int __mkntpath2(const char *, char16_t[hasatleast PATH_MAX - 16], int) hidden;
int __mkntpathat(int, const char *, int, char16_t[PATH_MAX]) hidden;
unsigned __wincrash_nt(struct NtExceptionPointers *);
ssize_t sys_readv_nt(struct Fd *, const struct iovec *, int) hidden;
ssize_t sys_writev_nt(struct Fd *, const struct iovec *, int) hidden;
char16_t *CreatePipeName(char16_t *) hidden;

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
