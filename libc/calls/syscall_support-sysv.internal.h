#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § syscalls » system five » structless support               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

bool __is_linux_2_6_23(void) _Hide;
bool32 sys_isatty_metal(int);
int __fixupnewfd(int, int) _Hide;
int __notziposat(int, const char *);
int __tkill(int, int, void *) _Hide;
int _fork(uint32_t) _Hide;
int _isptmaster(int) _Hide;
int _ptsname(int, char *, size_t) _Hide;
int getdomainname_linux(char *, size_t) _Hide;
int gethostname_bsd(char *, size_t, int) _Hide;
int gethostname_linux(char *, size_t) _Hide;
int gethostname_nt(char *, size_t, int) _Hide;
int sys_msyscall(void *, size_t);
long sys_bogus(void);
ssize_t __getrandom(void *, size_t, unsigned) _Hide;
void *__vdsosym(const char *, const char *) _Hide;
void __onfork(void) _Hide;
void __restore_rt() _Hide;
void __restore_rt_netbsd(void) _Hide;
void cosmo2flock(uintptr_t) _Hide;
void flock2cosmo(uintptr_t) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_SYSV_INTERNAL_H_ */
