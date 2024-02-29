#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#include "libc/limits.h"
#include "libc/nt/struct/overlapped.h"
COSMOPOLITAN_C_START_

bool isdirectory_nt(const char *);
bool isregularfile_nt(const char *);
bool issymlink_nt(const char *);
bool32 ntsetprivilege(int64_t, const char16_t *, uint32_t);
char16_t *__create_pipe_name(char16_t *);
size_t __normntpath(char16_t *, size_t);
int __mkntpath(const char *, char16_t[hasatleast PATH_MAX]);
int __mkntpath2(const char *, char16_t[hasatleast PATH_MAX], int);
int __mkntpathath(int64_t, const char *, int, char16_t[hasatleast PATH_MAX]);
int ntaccesscheck(const char16_t *, uint32_t) paramsnonnull();
int sys_fcntl_nt_setfl(int, unsigned);
int sys_pause_nt(void);
int64_t __fix_enotdir(int64_t, char16_t *);
int64_t __fix_enotdir3(int64_t, char16_t *, char16_t *);
int64_t __winerr(void) dontcallback privileged;
int64_t ntreturn(uint32_t);
void *GetProcAddressModule(const char *, const char *);
void WinMainForked(void);

ssize_t sys_readwrite_nt(int, void *, size_t, ssize_t, int64_t, uint64_t,
                         bool32 (*)(int64_t, void *, uint32_t, uint32_t *,
                                    struct NtOverlapped *));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_ */
