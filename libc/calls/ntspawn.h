#ifndef COSMOPOLITAN_LIBC_CALLS_NTSPAWN_H_
#define COSMOPOLITAN_LIBC_CALLS_NTSPAWN_H_
#include "libc/nt/struct/processinformation.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/nt/struct/startupinfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int mkntcmdline(char16_t[ARG_MAX / 2], char *const[]) _Hide;
int mkntenvblock(char16_t[ARG_MAX / 2], char *const[], const char *,
                 char[ARG_MAX]) _Hide;
int ntspawn(const char *, char *const[], char *const[], const char *,
            struct NtSecurityAttributes *, struct NtSecurityAttributes *,
            bool32, uint32_t, const char16_t *, const struct NtStartupInfo *,
            struct NtProcessInformation *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_NTSPAWN_H_ */
