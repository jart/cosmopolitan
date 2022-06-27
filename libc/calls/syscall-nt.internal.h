#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool32 sys_isatty_nt(int) hidden;
char *sys_getcwd_nt(char *, size_t) hidden;
int sys_chdir_nt(const char *) hidden;
int sys_close_epoll_nt(int) hidden;
int sys_dup_nt(int, int, int, int) hidden;
int sys_execve_nt(const char *, char *const[], char *const[]) hidden;
int sys_faccessat_nt(int, const char *, int, uint32_t) hidden;
int sys_fadvise_nt(int, uint64_t, uint64_t, int) hidden;
int sys_fchdir_nt(int) hidden;
int sys_fchmodat_nt(int, const char *, uint32_t, int) hidden;
int sys_fcntl_nt(int, int, uintptr_t) hidden;
int sys_fdatasync_nt(int) hidden;
int sys_flock_nt(int, int) hidden;
int sys_fork_nt(void) hidden;
int sys_ftruncate_nt(int64_t, uint64_t) hidden;
int sys_getloadavg_nt(double *, int) hidden;
int sys_getppid_nt(void) hidden;
int sys_getpriority_nt(int) hidden;
int sys_getsetpriority_nt(int, int, int, int (*)(int));
int sys_kill_nt(int, int) hidden;
int sys_linkat_nt(int, const char *, int, const char *) hidden;
int sys_madvise_nt(void *, size_t, int) hidden;
int sys_mkdirat_nt(int, const char *, uint32_t) hidden;
int sys_msync_nt(char *, size_t, int) hidden;
int sys_open_nt(int, const char *, uint32_t, int32_t) dontdiscard hidden;
int sys_pipe_nt(int[hasatleast 2], unsigned) hidden;
int sys_renameat_nt(int, const char *, int, const char *) hidden;
int sys_sched_yield_nt(void) hidden;
int sys_setpriority_nt(int) hidden;
int sys_symlinkat_nt(const char *, int, const char *) hidden;
int sys_sync_nt(void) hidden;
int sys_truncate_nt(const char *, uint64_t) hidden;
int sys_unlinkat_nt(int, const char *, int) hidden;
int64_t sys_lseek_nt(int, int64_t, int) hidden;
ssize_t sys_readlinkat_nt(int, const char *, char *, size_t) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_ */
