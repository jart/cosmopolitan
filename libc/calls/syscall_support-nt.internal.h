#ifndef COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_
#include "libc/intrin/fds.h"
#include "libc/limits.h"
#include "libc/nt/struct/overlapped.h"
COSMOPOLITAN_C_START_

#define POLL_INTERVAL_MS 200

bool isdirectory_nt(const char *) libcesque;
bool isregularfile_nt(const char *) libcesque;
bool issymlink_nt(const char *) libcesque;
bool32 sys_fchmod_nt_handle(intptr_t, uint32_t) libcesque;
char16_t *__create_pipe_name(char16_t *) libcesque;
int __mkntpath(const char *, char16_t[static PATH_MAX]) libcesque;
int __mkntpathat(int, const char *, char16_t[static 1024]) libcesque;
int __mkunixpath(const char16_t *, char[static PATH_MAX]) libcesque;
int __readntsym(intptr_t, char[static PATH_MAX], uint32_t *) libcesque;
int sys_fcntl_nt_setfl(struct Fd *, unsigned) libcesque;
int sys_pause_nt(void) libcesque;
char __getcosmosdrive(void) libcesque pureconst;
bool __hasregularparent(char16_t *) libcesque;
int64_t __fix_enotdir(int64_t, char16_t *) libcesque;
int64_t __fix_enotdir2(int64_t, char16_t *, char16_t *) libcesque;
int64_t __winerr(void) libcesque;
int64_t ntreturn(uint32_t) libcesque;
intptr_t __interruptible_start(uint64_t);
void __interruptible_end(void);
void *GetProcAddressModule(const char *, const char *) libcesque;
bool32 RestrictFileWin32(intptr_t, uint32_t);

int __mkntpathath(int64_t, const char *, char16_t[static PATH_MAX],
                  bool) libcesque;

ssize_t sys_readwrite_nt(int, void *, size_t, ssize_t, int64_t, uint64_t,
                         bool32 (*)(int64_t, void *, uint32_t, uint32_t *,
                                    struct NtOverlapped *));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SYSCALL_SUPPORT_NT_INTERNAL_H_ */
