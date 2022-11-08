#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool32 sys_isatty_nt(int) _Hide;
char *sys_getcwd_nt(char *, size_t) _Hide;
int sys_chdir_nt(const char *) _Hide;
int sys_close_epoll_nt(int) _Hide;
int sys_dup_nt(int, int, int, int) _Hide;
int sys_execve_nt(const char *, char *const[], char *const[]) _Hide;
int sys_faccessat_nt(int, const char *, int, uint32_t) _Hide;
int sys_fadvise_nt(int, uint64_t, uint64_t, int) _Hide;
int sys_fchdir_nt(int) _Hide;
int sys_fchmodat_nt(int, const char *, uint32_t, int) _Hide;
int sys_fcntl_nt(int, int, uintptr_t) _Hide;
int sys_fdatasync_nt(int) _Hide;
int sys_flock_nt(int, int) _Hide;
int sys_fork_nt(uint32_t) _Hide;
int sys_ftruncate_nt(int64_t, uint64_t) _Hide;
int sys_getloadavg_nt(double *, int) _Hide;
int sys_getppid_nt(void) _Hide;
int sys_getpriority_nt(int, unsigned) _Hide;
int sys_kill_nt(int, int) _Hide;
int sys_linkat_nt(int, const char *, int, const char *) _Hide;
int sys_madvise_nt(void *, size_t, int) _Hide;
int sys_mkdirat_nt(int, const char *, uint32_t) _Hide;
int sys_msync_nt(char *, size_t, int) _Hide;
int sys_open_nt(int, const char *, uint32_t, int32_t) dontdiscard _Hide;
int sys_pipe_nt(int[hasatleast 2], unsigned) _Hide;
int sys_renameat_nt(int, const char *, int, const char *) _Hide;
int sys_sched_yield_nt(void) _Hide;
int sys_setpriority_nt(int, unsigned, int) _Hide;
int sys_symlinkat_nt(const char *, int, const char *) _Hide;
int sys_sync_nt(void) _Hide;
int sys_truncate_nt(const char *, uint64_t) _Hide;
int sys_unlinkat_nt(int, const char *, int) _Hide;
int64_t sys_lseek_nt(int, int64_t, int) _Hide;
ssize_t sys_readlinkat_nt(int, const char *, char *, size_t) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_NT_INTERNAL_H_ */
