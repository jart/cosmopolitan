#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#define i32 int32_t
#define i64 int64_t
#define u32 uint32_t
#define u64 uint64_t
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » structless synthetic jump slots  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

axdx_t __sys_pipe(i32[hasatleast 2], i32) hidden;
axdx_t sys_fork(void) hidden;
axdx_t sys_getpid(void) hidden;
char *sys_getcwd(char *, u64) hidden;
char *sys_getcwd_xnu(char *, u64) hidden;
i32 __sys_dup3(i32, i32, i32) hidden;
i32 __sys_execve(const char *, char *const[], char *const[]) hidden;
i32 __sys_fcntl(i32, i32, ...) hidden;
i32 __sys_fstat(i32, void *) hidden;
i32 __sys_fstatat(i32, const char *, void *, i32) hidden;
i32 __sys_gettid(i64 *) hidden;
i32 __sys_munmap(void *, u64) hidden;
i32 __sys_openat(i32, const char *, i32, u32) hidden;
i32 __sys_pipe2(i32[hasatleast 2], u32) hidden;
i32 sys_arch_prctl(i32, i64) hidden;
i32 sys_chdir(const char *) hidden;
i32 sys_chroot(const char *) hidden;
i32 sys_close(i32) hidden;
i32 sys_close_range(u32, u32, u32) hidden;
i32 sys_closefrom(i32) hidden;
i32 sys_creat(const char *, u32) hidden;
i32 sys_dup(i32) hidden;
i32 sys_dup2(i32, i32) hidden;
i32 sys_dup3(i32, i32, i32) hidden;
i32 sys_execve(const char *, char *const[], char *const[]) hidden;
i32 sys_faccessat(i32, const char *, i32, u32) hidden;
i32 sys_faccessat2(i32, const char *, i32, u32) hidden;
i32 sys_fadvise(i32, i64, i64, i32) hidden;
i32 sys_fchdir(i32) hidden;
i32 sys_fchmod(i32, u32) hidden;
i32 sys_fchmodat(i32, const char *, u32, u32) hidden;
i32 sys_fchown(i64, u32, u32) hidden;
i32 sys_fchownat(i32, const char *, u32, u32, u32) hidden;
i32 sys_fcntl(i32, i32, u64) hidden;
i32 sys_fdatasync(i32) hidden;
i32 sys_flock(i32, i32) hidden;
i32 sys_fsync(i32) hidden;
i32 sys_ftruncate(i32, i64, i64) hidden;
i32 sys_getcontext(void *) hidden;
i32 sys_getpgid(i32) hidden;
i32 sys_getpgrp(void) hidden;
i32 sys_getppid(void) hidden;
i32 sys_getpriority(i32, u32) hidden;
i32 sys_getresgid(u32 *, u32 *, u32 *);
i32 sys_getresuid(u32 *, u32 *, u32 *);
i32 sys_getsid(int) hidden;
i32 sys_gettid(void) hidden;
i32 sys_ioctl(i32, u64, ...) hidden;
i32 sys_issetugid(void) hidden;
i32 sys_kill(i32, i32, i32) hidden;
i32 sys_linkat(i32, const char *, i32, const char *, i32) hidden;
i32 sys_madvise(void *, size_t, i32) hidden;
i32 sys_memfd_create(const char *, u32) hidden;
i32 sys_mincore(void *, u64, unsigned char *) hidden;
i32 sys_mkdirat(i32, const char *, u32) hidden;
i32 sys_mkfifo(const char *, u32) hidden;
i32 sys_mknod(const char *, u32, u64) hidden;
i32 sys_mprotect(void *, u64, i32) hidden;
i32 sys_msync(void *, u64, i32) hidden;
i32 sys_munmap(void *, u64) hidden;
i32 sys_open(const char *, i32, u32) hidden;
i32 sys_openat(i32, const char *, i32, u32) hidden;
i32 sys_pause(void) hidden;
i32 sys_pipe(i32[hasatleast 2]) hidden;
i32 sys_pipe2(i32[hasatleast 2], u32) hidden;
i32 sys_pivot_root(const char *, const char *) hidden;
i32 sys_pledge(const char *, const char *) hidden;
i32 sys_posix_openpt(i32) hidden;
i32 sys_renameat(i32, const char *, i32, const char *) hidden;
i32 sys_setfsgid(i32) hidden;
i32 sys_setfsuid(i32) hidden;
i32 sys_setgid(i32) hidden;
i32 sys_setpgid(i32, i32) hidden;
i32 sys_setpriority(i32, u32, i32) hidden;
i32 sys_setregid(u32, u32) hidden;
i32 sys_setresgid(u32, u32, u32) hidden;
i32 sys_setresuid(u32, u32, u32) hidden;
i32 sys_setreuid(u32, u32) hidden;
i32 sys_setsid(void) hidden;
i32 sys_setuid(i32) hidden;
i32 sys_sigaction(i32, const void *, void *, i64, i64) hidden;
i32 sys_sigaltstack(const void *, void *) hidden;
i32 sys_symlinkat(const char *, i32, const char *) hidden;
i32 sys_sync(void) hidden;
i32 sys_sync_file_range(i32, i64, i64, u32) hidden;
i32 sys_syslog(i32, char *, i32) hidden;
i32 sys_tgkill(i32, i32, i32) hidden;
i32 sys_tkill(i32, i32, void *) hidden;
i32 sys_truncate(const char *, u64, u64) hidden;
i32 sys_uname(void *) hidden;
i32 sys_unlink(const char *) hidden;
i32 sys_unlinkat(i32, const char *, i32) hidden;
i32 sys_unmount(const char *, i32) hidden;
i32 sys_unveil(const char *, const char *) hidden;
i64 __sys_ptrace(i32, i32, i64, long *) hidden;
i64 sys_copy_file_range(i32, long *, i32, long *, u64, u32) hidden;
i64 sys_getrandom(void *, u64, u32) hidden;
i64 sys_lseek(i32, i64, i64, i64) hidden;
i64 sys_pread(i32, void *, u64, i64, i64) hidden;
i64 sys_pwrite(i32, const void *, u64, i64, i64) hidden;
i64 sys_read(i32, void *, u64) hidden;
i64 sys_readlink(const char *, char *, u64) hidden;
i64 sys_readlinkat(int, const char *, char *, u64) hidden;
i64 sys_sendfile(i32, i32, i64 *, u64) hidden;
i64 sys_splice(i32, i64 *, i32, i64 *, u64, u32) hidden;
i64 sys_write(i32, const void *, u64) hidden;
int _isptmaster(int);
u32 sys_getegid(void) hidden;
u32 sys_geteuid(void) hidden;
u32 sys_getgid(void) hidden;
u32 sys_getuid(void) hidden;
u32 sys_umask(u32) hidden;
void *__sys_mmap(void *, u64, u32, u32, i64, i64, i64) hidden;
void *sys_mremap(void *, u64, u64, i32, void *) hidden;
void sys_exit(int) hidden;

#undef i32
#undef i64
#undef u32
#undef u64
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SYSV_INTERNAL_H_ */
