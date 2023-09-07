#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_

#define _POSIX_VERSION  200809L
#define _POSIX2_VERSION _POSIX_VERSION
#define _XOPEN_VERSION  700

#define _POSIX_MAPPED_FILES               _POSIX_VERSION
#define _POSIX_FSYNC                      _POSIX_VERSION
#define _POSIX_IPV6                       _POSIX_VERSION
#define _POSIX_THREADS                    _POSIX_VERSION
#define _POSIX_THREAD_PROCESS_SHARED      _POSIX_VERSION
#define _POSIX_THREAD_SAFE_FUNCTIONS      _POSIX_VERSION
#define _POSIX_THREAD_ATTR_STACKADDR      _POSIX_VERSION
#define _POSIX_THREAD_ATTR_STACKSIZE      _POSIX_VERSION
#define _POSIX_THREAD_PRIORITY_SCHEDULING _POSIX_VERSION
#define _POSIX_THREAD_CPUTIME             _POSIX_VERSION
#define _POSIX_TIMEOUTS                   _POSIX_VERSION
#define _POSIX_MONOTONIC_CLOCK            _POSIX_VERSION
#define _POSIX_CPUTIME                    _POSIX_VERSION
#define _POSIX_BARRIERS                   _POSIX_VERSION
#define _POSIX_SPIN_LOCKS                 _POSIX_VERSION
#define _POSIX_READER_WRITER_LOCKS        _POSIX_VERSION
#define _POSIX_SEMAPHORES                 _POSIX_VERSION
#define _POSIX_SHARED_MEMORY_OBJECTS      _POSIX_VERSION
#define _POSIX_MEMLOCK_RANGE              _POSIX_VERSION
#define _POSIX_SPAWN                      _POSIX_VERSION

#define SEEK_SET 0          /* relative to beginning */
#define SEEK_CUR 1          /* relative to current position */
#define SEEK_END 2          /* relative to end */
#define __WALL   0x40000000 /* Wait on all children, regardless of type */
#define __WCLONE 0x80000000 /* Wait only on non-SIGCHLD children */

#define SIG_ERR ((void (*)(int))(-1))
#define SIG_DFL ((void (*)(int))0)
#define SIG_IGN ((void (*)(int))1)

#define CLOCKS_PER_SEC 1000000L

#define MAP_FAILED ((void *)-1)

#define WTERMSIG(x)     (127 & (x))
#define WCOREDUMP(x)    (128 & (x))
#define WIFEXITED(x)    (!WTERMSIG(x))
#define WEXITSTATUS(x)  ((x) >> 8)
#define WSTOPSIG(x)     ((0xff00 & (x)) >> 8)
#define WIFSTOPPED(x)   __wifstopped(x)
#define WIFSIGNALED(x)  __wifsignaled(x)
#define WIFCONTINUED(x) __wifcontinued(x)
#define W_STOPCODE(x)   ((x) << 8 | 0177)

#ifdef _COSMO_SOURCE
#define clone         __clone
#define commandv      __commandv
#define fileexists    __fileexists
#define ischardev     __ischardev
#define isdirectory   __isdirectory
#define isexecutable  __isexecutable
#define isregularfile __isregularfile
#define issymlink     __issymlink
#define makedirs      __makedirs
#define tmpfd         __tmpfd
#endif

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef int sig_atomic_t;

bool32 isatty(int);
char *getcwd(char *, size_t);
char *realpath(const char *, char *) __wur;
char *ttyname(int);
int access(const char *, int) dontthrow;
int chdir(const char *);
int chmod(const char *, unsigned);
int chown(const char *, unsigned, unsigned);
int chroot(const char *);
int close(int);
int close_range(unsigned, unsigned, unsigned);
int closefrom(int);
int creat(const char *, unsigned);
int dup(int);
int dup2(int, int);
int dup3(int, int, int);
int execl(const char *, const char *, ...) nullterminated();
int execle(const char *, const char *, ...) nullterminated((1));
int execlp(const char *, const char *, ...) nullterminated();
int execv(const char *, char *const[]);
int execve(const char *, char *const[], char *const[]);
int execvp(const char *, char *const[]);
int faccessat(int, const char *, int, int);
int fchdir(int);
int fchmod(int, unsigned) dontthrow;
int fchmodat(int, const char *, unsigned, int);
int fchown(int, unsigned, unsigned);
int fchownat(int, const char *, unsigned, unsigned, int);
int fcntl(int, int, ...);
int fdatasync(int);
int fexecve(int, char *const[], char *const[]);
int flock(int, int);
int fork(void);
int fsync(int);
int ftruncate(int, int64_t);
int getdomainname(char *, size_t);
int getgroups(int, unsigned[]);
int gethostname(char *, size_t);
int getloadavg(double *, int);
int getpgid(int) libcesque;
int getpgrp(void) nosideeffect;
int getpid(void) nosideeffect libcesque;
int getppid(void);
int getpriority(int, unsigned);
int getsid(int) nosideeffect libcesque;
int ioctl(int, unsigned long, ...);
int issetugid(void);
int kill(int, int);
int killpg(int, int);
int lchmod(const char *, unsigned);
int lchown(const char *, unsigned, unsigned);
int link(const char *, const char *) dontthrow;
int linkat(int, const char *, int, const char *, int);
int mincore(void *, size_t, unsigned char *);
int mkdir(const char *, unsigned);
int mkdirat(int, const char *, unsigned);
int mkfifo(const char *, unsigned);
int mkfifoat(int, const char *, unsigned);
int mknod(const char *, unsigned, uint64_t);
int nice(int);
int open(const char *, int, ...);
int openat(int, const char *, int, ...);
int pause(void);
int pipe(int[hasatleast 2]);
int pipe2(int[hasatleast 2], int);
int posix_fadvise(int, int64_t, int64_t, int);
int posix_madvise(void *, uint64_t, int);
int raise(int);
int reboot(int);
int remove(const char *);
int rename(const char *, const char *);
int renameat(int, const char *, int, const char *);
int rmdir(const char *);
int sched_yield(void);
int setegid(unsigned);
int seteuid(unsigned);
int setfsgid(unsigned);
int setfsuid(unsigned);
int setgid(unsigned);
int setgroups(size_t, const unsigned[]);
int setpgid(int, int);
int setpgrp(void);
int setpriority(int, unsigned, int);
int setregid(unsigned, unsigned);
int setreuid(unsigned, unsigned);
int setsid(void);
int setuid(unsigned);
int sigignore(int);
int siginterrupt(int, int);
int symlink(const char *, const char *);
int symlinkat(const char *, int, const char *);
int truncate(const char *, int64_t);
int ttyname_r(int, char *, size_t);
int unlink(const char *);
int unlinkat(int, const char *, int);
int usleep(unsigned);
int vfork(void) returnstwice;
int wait(int *);
int waitpid(int, int *, int);
int64_t clock(void);
int64_t time(int64_t *);
ssize_t copy_file_range(int, long *, int, long *, size_t, unsigned);
ssize_t lseek(int, int64_t, int);
ssize_t pread(int, void *, size_t, int64_t);
ssize_t pwrite(int, const void *, size_t, int64_t);
ssize_t read(int, void *, size_t);
ssize_t readlink(const char *, char *, size_t);
ssize_t readlinkat(int, const char *, char *, size_t);
ssize_t write(int, const void *, size_t);
unsigned alarm(unsigned);
unsigned getegid(void) nosideeffect;
unsigned geteuid(void) nosideeffect;
unsigned getgid(void) nosideeffect;
unsigned getuid(void) libcesque;
unsigned sleep(unsigned);
unsigned ualarm(unsigned, unsigned);
unsigned umask(unsigned);
void sync(void);

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE)
int syncfs(int);
int prctl(int, ...);
int gettid(void) libcesque;
int setresgid(unsigned, unsigned, unsigned);
int setresuid(unsigned, unsigned, unsigned);
int getresgid(unsigned *, unsigned *, unsigned *);
int getresuid(unsigned *, unsigned *, unsigned *);
char *get_current_dir_name(void) __wur;
int sync_file_range(int, int64_t, int64_t, unsigned);
ssize_t splice(int, int64_t *, int, int64_t *, size_t, unsigned);
int memfd_create(const char *, unsigned int);
int execvpe(const char *, char *const[], char *const[]);
int euidaccess(const char *, int);
int eaccess(const char *, int);
int madvise(void *, uint64_t, int);
#endif

#ifdef _COSMO_SOURCE
bool32 fileexists(const char *);
bool32 isdirectory(const char *);
bool32 isexecutable(const char *);
bool32 isregularfile(const char *);
bool32 issymlink(const char *);
bool32 ischardev(int);
char *commandv(const char *, char *, size_t);
int clone(void *, void *, size_t, int, void *, void *, void *, void *);
int makedirs(const char *, unsigned);
int pivot_root(const char *, const char *);
int pledge(const char *, const char *);
int seccomp(unsigned, unsigned, void *);
int sys_iopl(int);
int sys_mlock(const void *, size_t);
int sys_mlock2(const void *, size_t, int);
int sys_mlockall(int);
int sys_personality(uint64_t);
int sys_munlock(const void *, size_t);
int sys_munlockall(void);
int sys_ptrace(int, ...);
int sys_sysctl(const int *, unsigned, void *, size_t *, void *, size_t);
int sys_ioprio_get(int, int);
int sys_ioprio_set(int, int, int);
int tgkill(int, int, int);
int tkill(int, int);
int tmpfd(void);
int touch(const char *, unsigned);
int unveil(const char *, const char *);
long ptrace(int, ...);
int fadvise(int, uint64_t, uint64_t, int);
ssize_t copyfd(int, int, size_t);
ssize_t readansi(int, char *, size_t);
ssize_t tinyprint(int, const char *, ...) nullterminated();
#endif /* _COSMO_SOURCE */

int __wifstopped(int) pureconst;
int __wifcontinued(int) pureconst;
int __wifsignaled(int) pureconst;

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define lseek64     lseek
#define pread64     pread
#define pwrite64    pwrite
#define truncate64  truncate
#define ftruncate64 ftruncate
#define lockf64     lockf
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_ */
