#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » structless support               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

bool __is_linux_2_6_23(void) hidden;
int __fixupnewfd(int, int) hidden;
int __notziposat(int, const char *);
int gethostname_bsd(char *, size_t) hidden;
int gethostname_linux(char *, size_t) hidden;
int gethostname_nt(char *, size_t, int) hidden;
long sys_bogus(void);
void *__vdsosym(const char *, const char *) hidden;
void __onfork(void) hidden;
void __restore_rt() hidden;
void __restore_rt_netbsd(void) hidden;
void cosmo2flock(uintptr_t);
void flock2cosmo(uintptr_t);
bool32 sys_isatty_metal(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_ */
