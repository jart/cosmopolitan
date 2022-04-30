#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/rlimit.h"
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigval.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/sysinfo.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/tms.h"
#include "libc/calls/struct/utsname.h"
#include "libc/dce.h"
#include "libc/fmt/pflink.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/consts/sig.h"

#define _POSIX_VERSION  200809L
#define _POSIX2_VERSION _POSIX_VERSION
#define _XOPEN_VERSION  700

#define EOF      -1  /* end of file */
#define WEOF     -1u /* end of file (multibyte) */
#define _IOFBF   0   /* fully buffered */
#define _IOLBF   1   /* line buffered */
#define _IONBF   2   /* no buffering */
#define SEEK_SET 0   /* relative to beginning */
#define SEEK_CUR 1   /* relative to current position */
#define SEEK_END 2   /* relative to end */

#define SIG_ERR ((void (*)(int))(-1))
#define SIG_DFL ((void *)0)
#define SIG_IGN ((void *)1)

#define MAP_FAILED ((void *)-1)

#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004

#define MAP_HUGE_2MB (21 << MAP_HUGE_SHIFT)
#define MAP_HUGE_1GB (30 << MAP_HUGE_SHIFT)

#define S_ISDIR(mode)  (((mode)&S_IFMT) == S_IFDIR)
#define S_ISCHR(mode)  (((mode)&S_IFMT) == S_IFCHR)
#define S_ISBLK(mode)  (((mode)&S_IFMT) == S_IFBLK)
#define S_ISREG(mode)  (((mode)&S_IFMT) == S_IFREG)
#define S_ISFIFO(mode) (((mode)&S_IFMT) == S_IFIFO)
#define S_ISLNK(mode)  (((mode)&S_IFMT) == S_IFLNK)
#define S_ISSOCK(mode) (((mode)&S_IFMT) == S_IFSOCK)

#define WCOREDUMP(s)    (0x80 & (s))
#define WEXITSTATUS(s)  ((0xff00 & (s)) >> 8)
#define WIFCONTINUED(s) ((s) == 0xffff)
#define WIFEXITED(s)    (!WTERMSIG(s))
#define WIFSIGNALED(s)  ((0xffff & (s)) - 1u < 0xffu)
#define WIFSTOPPED(s)   ((short)(((0xffff & (s)) * 0x10001) >> 8) > 0x7f00)
#define WSTOPSIG(s)     WEXITSTATUS(s)
#define WTERMSIG(s)     (127 & (s))
#define W_STOPCODE(s)   ((s) << 8 | 0177)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef int sig_atomic_t;

DIR *fdopendir(int) dontdiscard;
DIR *opendir(const char *) dontdiscard;
bool fileexists(const char *);
bool isdirectory(const char *);
bool isexecutable(const char *);
bool isregularfile(const char *);
bool issymlink(const char *);
bool32 isatty(int) nosideeffect;
bool32 ischardev(int) nosideeffect;
char *commandv(const char *, char *, size_t);
char *get_current_dir_name(void) dontdiscard;
char *getcwd(char *, size_t);
char *realpath(const char *, char *);
char *replaceuser(const char *) dontdiscard;
char *ttyname(int);
int access(const char *, int) dontthrow;
int arch_prctl();
int chdir(const char *);
int chmod(const char *, uint32_t);
int chown(const char *, uint32_t, uint32_t);
int chroot(const char *);
int close(int);
int closedir(DIR *);
int creat(const char *, uint32_t);
int dirfd(DIR *);
int dup(int);
int dup2(int, int);
int dup3(int, int, int);
int execl(const char *, const char *, ...) nullterminated();
int execle(const char *, const char *, ...) nullterminated((1));
int execlp(const char *, const char *, ...) nullterminated();
int execv(const char *, char *const[]);
int execve(const char *, char *const[], char *const[]);
int execvp(const char *, char *const[]);
int execvpe(const char *, char *const[], char *const[]);
int faccessat(int, const char *, int, uint32_t);
int fadvise(int, uint64_t, uint64_t, int);
int fchdir(int);
int fchmod(int, uint32_t) dontthrow;
int fchmodat(int, const char *, uint32_t, int);
int fchown(int, uint32_t, uint32_t);
int fchownat(int, const char *, uint32_t, uint32_t, int);
int fcntl(int, int, ...);
int fdatasync(int);
int filecmp(const char *, const char *);
int flock(int, int);
int fork(void);
int fstat(int, struct stat *);
int fstatat(int, const char *, struct stat *, int);
int fsync(int);
int ftruncate(int, int64_t);
int getdents(unsigned, void *, unsigned, long *);
int getdomainname(char *, size_t);
int getegid(void) nosideeffect;
int geteuid(void) nosideeffect;
int getgid(void) nosideeffect;
int gethostname(char *, size_t);
int getloadavg(double *, int);
int getpgid(int);
int getpgrp(void) nosideeffect;
int getpid(void);
int getppid(void);
int getpriority(int, unsigned);
int getrlimit(int, struct rlimit *);
int getrusage(int, struct rusage *);
int getsid(int) nosideeffect;
int gettid(void);
int getuid(void) nosideeffect;
int kill(int, int);
int killpg(int, int);
int link(const char *, const char *) dontthrow;
int linkat(int, const char *, int, const char *, int);
int lstat(const char *, struct stat *);
int lutimes(const char *, const struct timeval[2]);
int madvise(void *, uint64_t, int);
int mkdir(const char *, uint32_t);
int mkdirat(int, const char *, uint32_t);
int mkfifo(const char *, uint32_t);
int mkfifoat(int, const char *, uint32_t);
int mknod(const char *, uint32_t, uint64_t);
int mknodat(int, const char *, int32_t, uint64_t);
int mlock(const void *, size_t);
int mlock2(const void *, size_t, int);
int mlockall(int);
int munlock(const void *, size_t);
int munlockall(void);
int nice(int);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int pause(void);
int personality(uint64_t);
int pipe(int[hasatleast 2]);
int pipe2(int[hasatleast 2], int);
int posix_fadvise(int, uint64_t, uint64_t, int);
int posix_madvise(void *, uint64_t, int);
int prctl(int, ...);
int raise(int);
int reboot(int);
int remove(const char *);
int rename(const char *, const char *);
int renameat(int, const char *, int, const char *);
int renameat2(long, const char *, long, const char *, int);
int rmdir(const char *);
int sched_getaffinity(int, uint64_t, void *);
int sched_setaffinity(int, uint64_t, const void *);
int sched_yield(void);
int seccomp(unsigned, unsigned, void *);
int setegid(uint32_t);
int seteuid(uint32_t);
int setgid(int);
int setpgid(int, int);
int setpgrp(void);
int setpriority(int, unsigned, int);
int setregid(uint32_t, uint32_t);
int setresgid(uint32_t, uint32_t, uint32_t);
int setresuid(uint32_t, uint32_t, uint32_t);
int getresgid(uint32_t *, uint32_t *, uint32_t *);
int getresuid(uint32_t *, uint32_t *, uint32_t *);
int setreuid(uint32_t, uint32_t);
int setrlimit(int, const struct rlimit *);
int setsid(void);
int setuid(int);
int sigignore(int);
int siginterrupt(int, int);
int sigprocmask(int, const struct sigset *, struct sigset *);
int sigqueue(int, int, const union sigval);
int sigsuspend(const struct sigset *);
int stat(const char *, struct stat *);
int symlink(const char *, const char *);
int symlinkat(const char *, int, const char *);
int sync_file_range(int, int64_t, int64_t, unsigned);
int sysctl(const int *, unsigned, void *, size_t *, void *, size_t);
int sysinfo(struct sysinfo *);
int touch(const char *, uint32_t);
int truncate(const char *, uint64_t);
int ttyname_r(int, char *, size_t);
int umask(int);
int uname(struct utsname *);
int unlink(const char *);
int unlink_s(const char **);
int unlinkat(int, const char *, int);
int vfork(void) returnstwice;
int wait(int *);
int wait3(int *, int, struct rusage *);
int wait4(int, int *, int, struct rusage *);
int waitpid(int, int *, int);
intptr_t syscall(int, ...);
long ptrace(int, ...);
long telldir(DIR *);
long times(struct tms *);
size_t GetFileSize(const char *);
ssize_t copy_file_range(int, long *, int, long *, size_t, uint32_t);
ssize_t copyfd(int, int64_t *, int, int64_t *, size_t, uint32_t);
ssize_t getfiledescriptorsize(int);
ssize_t lseek(int, int64_t, unsigned);
ssize_t pread(int, void *, size_t, int64_t);
ssize_t preadv(int, struct iovec *, int, int64_t);
ssize_t pwrite(int, const void *, size_t, int64_t);
ssize_t pwritev(int, const struct iovec *, int, int64_t);
ssize_t read(int, void *, size_t);
ssize_t readansi(int, char *, size_t);
ssize_t readlink(const char *, char *, size_t);
ssize_t readlinkat(int, const char *, char *, size_t);
ssize_t splice(int, int64_t *, int, int64_t *, size_t, uint32_t);
ssize_t vmsplice(int, const struct iovec *, int64_t, uint32_t);
ssize_t write(int, const void *, size_t);
struct dirent *readdir(DIR *);
void rewinddir(DIR *);
void sync(void);
int pledge(const char *, const char *);

int clone(int (*)(void *), void *, size_t, int, void *, int *, void *, size_t,
          int *);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » formatting                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

int dprintf(int, const char *, ...) printfesque(2) paramsnonnull((2));
int vdprintf(int, const char *, va_list) paramsnonnull();

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls » link-time optimizations                    ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define dprintf(FD, FMT, ...) (dprintf)(FD, PFLINK(FMT), ##__VA_ARGS__)
#define vdprintf(FD, FMT, VA) (vdprintf)(FD, PFLINK(FMT), VA)

#endif /* GNU && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_ */
