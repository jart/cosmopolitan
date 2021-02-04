#ifndef COSMOPOLITAN_LIBC_MEM_FMT_H_
#define COSMOPOLITAN_LIBC_MEM_FMT_H_
#include "libc/fmt/pflink.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int asprintf(char **, const char *, ...) printfesque(2)
    paramsnonnull((1, 2)) libcesque;
int vasprintf(char **, const char *, va_list) paramsnonnull() libcesque;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define asprintf(SP, FMT, ...) (asprintf)(SP, PFLINK(FMT), ##__VA_ARGS__)
#define vasprintf(SP, FMT, VA) (vasprintf)(SP, PFLINK(FMT), VA)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MEM_FMT_H_ */
