#ifndef COSMOPOLITAN_LIBC_NT_CREATEFILE_H_
#define COSMOPOLITAN_LIBC_NT_CREATEFILE_H_
#include "libc/nt/struct/securityattributes.h"
COSMOPOLITAN_C_START_

int64_t CreateFile(
    const char16_t *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode,
    const struct NtSecurityAttributes *opt_lpSecurityAttributes,
    int dwCreationDisposition,
    uint32_t dwFlagsAndAttributes, /* libc/nt/enum/fileflagandattributes.h */
    int64_t opt_hTemplateFile) paramsnonnull((1));

int64_t CreateFileA(
    const char *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode,
    const struct NtSecurityAttributes *opt_lpSecurityAttributes,
    int dwCreationDisposition,
    uint32_t dwFlagsAndAttributes, /* libc/nt/enum/fileflagandattributes.h */
    int64_t opt_hTemplateFile) paramsnonnull((1));

int GetNtOpenFlags(int, int, uint32_t *, uint32_t *, uint32_t *, uint32_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_CREATEFILE_H_ */
