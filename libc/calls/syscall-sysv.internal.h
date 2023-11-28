#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_
COSMOPOLITAN_C_START_
#define i32 int32_t
#define i64 int64_t
#define u32 uint32_t
#define u64 uint64_t
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » structless synthetic jump slots  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

axdx_t __sys_fork(void);
axdx_t __sys_pipe(i32[hasatleast 2], i32);
axdx_t sys_getpid(void);
i32 __sys_dup3(i32, i32, i32);
i32 __sys_execve(const char *, char *const[], char *const[]);
i32 __sys_fcntl(i32, i32, ...);
i32 __sys_fcntl_cp(i32, i32, ...);
i32 __sys_fstat(i32, void *);
i32 __sys_fstatat(i32, const char *, void *, i32);
i32 __sys_gettid(i64 *);
i32 __sys_mprotect(void *, u64, i32);
i32 __sys_munmap(void *, u64);
i32 __sys_openat(i32, const char *, i32, u32);
i32 __sys_openat_nc(i32, const char *, i32, u32);
i32 __sys_pipe2(i32[hasatleast 2], u32);
i32 sys_arch_prctl(i32, i64);
i32 sys_chdir(const char *);
i32 sys_chroot(const char *);
i32 sys_close(i32);
i32 sys_close_range(u32, u32, u32);
i32 sys_closefrom(i32);
i32 sys_dup(i32);
i32 sys_dup2(i32, i32, i32);
i32 sys_dup3(i32, i32, i32);
i32 sys_execve(const char *, char *const[], char *const[]);
i32 sys_execveat(i32, const char *, char *const[], char *const[], i32);
i32 sys_faccessat(i32, const char *, i32, u32);
i32 sys_faccessat2(i32, const char *, i32, u32);
i32 sys_fadvise(i32, i64, i64, i32);
i32 sys_fchdir(i32);
i32 sys_fchmod(i32, u32);
i32 sys_fchmodat(i32, const char *, u32, u32);
i32 sys_fchown(i64, u32, u32);
i32 sys_fchownat(i32, const char *, u32, u32, u32);
i32 sys_fcntl(i32, i32, u64, i32 (*)(i32, i32, ...));
i32 sys_fdatasync(i32);
i32 sys_fexecve(i32, char *const[], char *const[]);
i32 sys_flock(i32, i32);
i32 sys_fork(void);
i32 sys_fsync(i32);
i32 sys_ftruncate(i32, i64, i64);
i32 sys_getcontext(void *);
i32 sys_getcwd(char *, u64);
i32 sys_getpgid(i32);
i32 sys_getppid(void);
i32 sys_getpriority(i32, u32);
i32 sys_getresgid(u32 *, u32 *, u32 *);
i32 sys_getresuid(u32 *, u32 *, u32 *);
i32 sys_getsid(i32);
i32 sys_gettid(void);
i32 sys_ioctl(i32, u64, ...);
i32 sys_ioctl_cp(i32, u64, ...);
i32 sys_issetugid(void);
i32 sys_kill(i32, i32, i32);
i32 sys_linkat(i32, const char *, i32, const char *, i32);
i32 sys_madvise(void *, size_t, i32);
i32 sys_memfd_create(const char *, u32);
i32 sys_mincore(void *, u64, unsigned char *);
i32 sys_mkdirat(i32, const char *, u32);
i32 sys_mkfifo(const char *, u32);
i32 sys_mknod(const char *, u32, u64);
i32 sys_mknodat(i32, const char *, u32, u64);
i32 sys_mprotect(void *, u64, i32);
i32 sys_msync(void *, u64, i32);
i32 sys_munmap(void *, u64);
i32 sys_openat(i32, const char *, i32, u32);
i32 sys_pause(void);
i32 sys_pipe(i32[hasatleast 2]);
i32 sys_pipe2(i32[hasatleast 2], u32);
i32 sys_pivot_root(const char *, const char *);
i32 sys_pledge(const char *, const char *);
i32 sys_posix_openpt(i32);
i32 sys_renameat(i32, const char *, i32, const char *);
i32 sys_sem_close(i64);
i32 sys_sem_destroy(i64);
i32 sys_sem_destroy(i64);
i32 sys_sem_getvalue(i64, u32 *);
i32 sys_sem_init(u32, i64 *);
i32 sys_sem_open(const char *, int, u32, i64 *);
i32 sys_sem_post(i64);
i32 sys_sem_trywait(i64);
i32 sys_sem_unlink(const char *);
i32 sys_sem_wait(i64);
i32 sys_setfsgid(i32);
i32 sys_setfsuid(i32);
i32 sys_setgid(i32);
i32 sys_setpgid(i32, i32);
i32 sys_setpriority(i32, u32, i32);
i32 sys_setregid(u32, u32);
i32 sys_setresgid(u32, u32, u32);
i32 sys_setresuid(u32, u32, u32);
i32 sys_setreuid(u32, u32);
i32 sys_setsid(void);
i32 sys_setuid(i32);
i32 sys_shm_open(const char *, i32, u32);
i32 sys_sigaction(i32, const void *, void *, i64, i64);
i32 sys_sigaltstack(const void *, void *);
i32 sys_symlinkat(const char *, i32, const char *);
i32 sys_sync(void);
i32 sys_syncfs(i32);
i32 sys_syslog(i32, char *, i32);
i32 sys_tgkill(i32, i32, i32);
i32 sys_tkill(i32, i32, void *);
i32 sys_truncate(const char *, u64, u64);
i32 sys_uname(void *);
i32 sys_unlink(const char *);
i32 sys_unlinkat(i32, const char *, i32);
i32 sys_unmount(const char *, i32);
i32 sys_unveil(const char *, const char *);
i64 __sys_ptrace(i32, i32, i64, long *);
i64 sys_copy_file_range(i32, long *, i32, long *, u64, u32);
i64 sys_getrandom(void *, u64, u32);
i64 sys_lseek(i32, i64, i64, i64);
i64 sys_pread(i32, void *, u64, i64, i64);
i64 sys_pwrite(i32, const void *, u64, i64, i64);
i64 sys_read(i32, void *, u64);
i64 sys_readlinkat(i32, const char *, char *, u64);
i64 sys_sendfile(i32, i32, i64 *, u64);
i64 sys_splice(i32, i64 *, i32, i64 *, u64, u32);
i64 sys_write(i32, const void *, u64);
u32 sys_getegid(void);
u32 sys_geteuid(void);
u32 sys_getgid(void);
u32 sys_getuid(void);
u32 sys_umask(u32);
unsigned long _sysret(unsigned long);
void *__sys_mmap(void *, u64, u32, u32, i64, i64, i64);
void *__sys_mremap(void *, u64, u64, i32, void *);
void *sys_mremap(void *, u64, u64, i32, void *);
void sys_exit(i32);

#undef i32
#undef i64
#undef u32
#undef u64
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_ */
