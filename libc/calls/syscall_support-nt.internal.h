#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool isdirectory_nt(const char *) _Hide;
bool isregularfile_nt(const char *) _Hide;
bool issymlink_nt(const char *) _Hide;
bool32 ntsetprivilege(int64_t, const char16_t *, uint32_t) _Hide;
char16_t *CreatePipeName(char16_t *) _Hide;
int __mkntpath(const char *, char16_t[hasatleast PATH_MAX]) _Hide;
int __mkntpath2(const char *, char16_t[hasatleast PATH_MAX], int) _Hide;
int __mkntpathat(int, const char *, int, char16_t[hasatleast PATH_MAX]) _Hide;
int __sample_pids(int[hasatleast 64], int64_t[hasatleast 64], bool) _Hide;
int ntaccesscheck(const char16_t *, uint32_t) paramsnonnull() _Hide;
int sys_pause_nt(void) _Hide;
int64_t __fix_enotdir(int64_t, char16_t *) _Hide;
int64_t __fix_enotdir3(int64_t, char16_t *, char16_t *) _Hide;
int64_t __winerr(void) nocallback privileged;
int64_t ntreturn(uint32_t);
void *GetProcAddressModule(const char *, const char *) _Hide;
void WinMainForked(void) _Hide;
void _check_sigalrm(void) _Hide;
void _check_sigchld(void) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_ */
