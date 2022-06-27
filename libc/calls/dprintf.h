#ifndef COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
#define COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
#include "libc/fmt/pflink.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int dprintf(int, const char *, ...) printfesque(2) paramsnonnull((2));
int vdprintf(int, const char *, va_list) paramsnonnull();

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define dprintf(FD, FMT, ...) (dprintf)(FD, PFLINK(FMT), ##__VA_ARGS__)
#define vdprintf(FD, FMT, VA) (vdprintf)(FD, PFLINK(FMT), VA)

#endif /* GNU && !ANSI */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_ */
