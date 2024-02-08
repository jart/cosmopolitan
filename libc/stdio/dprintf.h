#ifndef COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
#define COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_
COSMOPOLITAN_C_START_

libcesque int dprintf(int, const char *, ...) paramsnonnull((2));
libcesque int vdprintf(int, const char *, va_list) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_DPRINTF_H_ */
