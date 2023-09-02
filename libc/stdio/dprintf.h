#ifndef COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
#define COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int dprintf(int, const char *, ...) paramsnonnull((2));
int vdprintf(int, const char *, va_list) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_ */
