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

#define NSIG 64

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

COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § system calls                                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

typedef int sig_atomic_t;

bool32 isatty(int) libcesque;
char *getcwd(char *, size_t) dontthrow;
char *realpath(const char *, char *) libcesque __wur;
char *ttyname(int) libcesque;
int access(const char *, int) libcesque;
int chdir(const char *) libcesque;
int chmod(const char *, unsigned) libcesque;
int chown(const char *, unsigned, unsigned) libcesque;
int chroot(const char *) libcesque;
int close(int) libcesque;
int close_range(unsigned, unsigned, unsigned) libcesque;
int closefrom(int) libcesque;
int creat(const char *, unsigned) libcesque;
int dup(int) libcesque;
int dup2(int, int) libcesque;
int dup3(int, int, int) libcesque;
int execl(const char *, const char *, ...) nullterminated() libcesque;
int execle(const char *, const char *, ...) nullterminated((1)) libcesque;
int execlp(const char *, const char *, ...) nullterminated() libcesque;
int execv(const char *, char *const[]) libcesque;
int execve(const char *, char *const[], char *const[]) libcesque;
int execvp(const char *, char *const[]) libcesque;
int faccessat(int, const char *, int, int) libcesque;
int fchdir(int) libcesque;
int fchmod(int, unsigned) libcesque;
int fchmodat(int, const char *, unsigned, int) libcesque;
int fchown(int, unsigned, unsigned) libcesque;
int fchownat(int, const char *, unsigned, unsigned, int) libcesque;
int fcntl(int, int, ...) libcesque;
int fdatasync(int) libcesque;
int fexecve(int, char *const[], char *const[]) libcesque;
int flock(int, int) libcesque;
int fork(void) libcesque;
int fsync(int) libcesque;
int ftruncate(int, int64_t) libcesque;
int getdomainname(char *, size_t) libcesque;
int getgroups(int, unsigned[]) libcesque;
int gethostname(char *, size_t) libcesque;
int getloadavg(double *, int) libcesque;
int getpgid(int) libcesque;
int getpgrp(void) libcesque nosideeffect;
int getpid(void) libcesque nosideeffect;
int getppid(void) libcesque;
int getpriority(int, unsigned) libcesque;
int getsid(int) nosideeffect libcesque;
int ioctl(int, unsigned long, ...) libcesque;
int issetugid(void) libcesque;
int kill(int, int) libcesque;
int killpg(int, int) libcesque;
int lchmod(const char *, unsigned) libcesque;
int lchown(const char *, unsigned, unsigned) libcesque;
int link(const char *, const char *) libcesque;
int linkat(int, const char *, int, const char *, int) libcesque;
int mincore(void *, size_t, unsigned char *) libcesque;
int mkdir(const char *, unsigned) libcesque;
int mkdirat(int, const char *, unsigned) libcesque;
int mknod(const char *, unsigned, uint64_t) libcesque;
int nice(int) libcesque;
int open(const char *, int, ...) libcesque;
int openat(int, const char *, int, ...) libcesque;
int pause(void) libcesque;
int pipe(int[hasatleast 2]) libcesque;
int pipe2(int[hasatleast 2], int) libcesque;
int posix_fadvise(int, int64_t, int64_t, int) libcesque;
int posix_madvise(void *, uint64_t, int) libcesque;
int raise(int) libcesque;
int reboot(int) libcesque;
int remove(const char *) libcesque;
int rename(const char *, const char *) libcesque;
int renameat(int, const char *, int, const char *) libcesque;
int rmdir(const char *) libcesque;
int sched_yield(void) libcesque;
int setegid(unsigned) libcesque;
int seteuid(unsigned) libcesque;
int setfsgid(unsigned) libcesque;
int setfsuid(unsigned) libcesque;
int setgid(unsigned) libcesque;
int setgroups(size_t, const unsigned[]) libcesque;
int setpgid(int, int) libcesque;
int setpgrp(void) libcesque;
int setpriority(int, unsigned, int) libcesque;
int setregid(unsigned, unsigned) libcesque;
int setreuid(unsigned, unsigned) libcesque;
int setsid(void) libcesque;
int setuid(unsigned) libcesque;
int shm_open(const char *, int, unsigned) libcesque;
int shm_unlink(const char *) libcesque;
int sigignore(int) libcesque;
int siginterrupt(int, int) libcesque;
int symlink(const char *, const char *) libcesque;
int symlinkat(const char *, int, const char *) libcesque;
int tcgetpgrp(int) libcesque;
int tcsetpgrp(int, int) libcesque;
int truncate(const char *, int64_t) libcesque;
int ttyname_r(int, char *, size_t) libcesque;
int unlink(const char *) libcesque;
int unlinkat(int, const char *, int) libcesque;
int usleep(uint64_t) libcesque;
int vfork(void) libcesque returnstwice;
int wait(int *) libcesque;
int waitpid(int, int *, int) libcesque;
int64_t clock(void) libcesque;
int64_t time(int64_t *) libcesque;
ssize_t copy_file_range(int, long *, int, long *, size_t, unsigned) libcesque;
ssize_t lseek(int, int64_t, int) libcesque;
ssize_t pread(int, void *, size_t, int64_t) libcesque;
ssize_t pwrite(int, const void *, size_t, int64_t) libcesque;
ssize_t read(int, void *, size_t) libcesque;
ssize_t readlink(const char *, char *, size_t) libcesque;
ssize_t readlinkat(int, const char *, char *, size_t) libcesque;
ssize_t write(int, const void *, size_t) libcesque;
unsigned alarm(unsigned) libcesque;
unsigned getegid(void) libcesque nosideeffect;
unsigned geteuid(void) libcesque nosideeffect;
unsigned getgid(void) libcesque nosideeffect;
unsigned getuid(void) libcesque;
unsigned sleep(unsigned) libcesque;
unsigned ualarm(unsigned, unsigned) libcesque;
unsigned umask(unsigned) libcesque;
void sync(void) libcesque;

#if defined(_COSMO_SOURCE) || defined(_GNU_SOURCE)
int syncfs(int) libcesque;
int prctl(int, ...) libcesque;
int gettid(void) libcesque;
int setresgid(unsigned, unsigned, unsigned) libcesque;
int setresuid(unsigned, unsigned, unsigned) libcesque;
int getresgid(unsigned *, unsigned *, unsigned *) libcesque;
int getresuid(unsigned *, unsigned *, unsigned *) libcesque;
char *get_current_dir_name(void) libcesque __wur;
ssize_t splice(int, int64_t *, int, int64_t *, size_t, unsigned) libcesque;
int memfd_create(const char *, unsigned int) libcesque;
int execvpe(const char *, char *const[], char *const[]) libcesque;
int euidaccess(const char *, int) libcesque;
int eaccess(const char *, int) libcesque;
int madvise(void *, uint64_t, int) libcesque;
#endif

#ifdef _COSMO_SOURCE
bool32 fdexists(int) libcesque;
bool32 fileexists(const char *) libcesque;
bool32 ischardev(int) libcesque;
bool32 isdirectory(const char *) libcesque;
bool32 isexecutable(const char *) libcesque;
bool32 isregularfile(const char *) libcesque;
bool32 issymlink(const char *) libcesque;
char *commandv(const char *, char *, size_t) libcesque;
int __getcwd(char *, size_t) libcesque;
int clone(void *, void *, size_t, int, void *, void *, void *, void *);
int fadvise(int, uint64_t, uint64_t, int) libcesque;
int makedirs(const char *, unsigned) libcesque;
int pivot_root(const char *, const char *) libcesque;
int pledge(const char *, const char *) libcesque;
int seccomp(unsigned, unsigned, void *) libcesque;
int sys_iopl(int) libcesque;
int sys_ioprio_get(int, int) libcesque;
int sys_ioprio_set(int, int, int) libcesque;
int sys_mlock(const void *, size_t) libcesque;
int sys_mlock2(const void *, size_t, int) libcesque;
int sys_mlockall(int) libcesque;
int sys_munlock(const void *, size_t) libcesque;
int sys_munlockall(void) libcesque;
int sys_personality(uint64_t) libcesque;
int sys_ptrace(int, ...) libcesque;
int sys_sysctl(const int *, unsigned, void *, size_t *, void *, size_t);
int tmpfd(void) libcesque;
int touch(const char *, unsigned) libcesque;
int unveil(const char *, const char *) libcesque;
long ptrace(int, ...) libcesque;
ssize_t copyfd(int, int, size_t) libcesque;
ssize_t readansi(int, char *, size_t) libcesque;
ssize_t tinyprint(int, const char *, ...) libcesque nullterminated();
void shm_path_np(const char *, char[hasatleast 78]) libcesque;
#endif /* _COSMO_SOURCE */

int system(const char *) libcesque;

int __wifstopped(int) libcesque pureconst;
int __wifcontinued(int) libcesque pureconst;
int __wifsignaled(int) libcesque pureconst;

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define lseek64     lseek
#define pread64     pread
#define pwrite64    pwrite
#define truncate64  truncate
#define ftruncate64 ftruncate
#define lockf64     lockf
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALLS_H_ */
