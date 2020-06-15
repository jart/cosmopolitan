#ifndef COSMOPOLITAN_LIBC_NT_CREATEFILE_H_
#define COSMOPOLITAN_LIBC_NT_CREATEFILE_H_
#include "libc/nt/enum/accessmask.h"
#include "libc/nt/enum/creationdisposition.h"
#include "libc/nt/enum/fileflagandattributes.h"
#include "libc/nt/enum/filesharemode.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtSecurityAttributes;

int64_t CreateFile(
    const char16_t *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode,
    struct NtSecurityAttributes *opt_lpSecurityAttributes,
    int dwCreationDisposition,
    uint32_t dwFlagsAndAttributes, /* libc/nt/enum/fileflagandattributes.h */
    int64_t opt_hTemplateFile) paramsnonnull((1));

int64_t CreateFileA(
    const char *lpFileName, uint32_t dwDesiredAccess, uint32_t dwShareMode,
    struct NtSecurityAttributes *opt_lpSecurityAttributes,
    int dwCreationDisposition,
    uint32_t dwFlagsAndAttributes, /* libc/nt/enum/fileflagandattributes.h */
    int64_t opt_hTemplateFile) paramsnonnull((1));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_CREATEFILE_H_ */
