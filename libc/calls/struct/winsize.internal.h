#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/winsize.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int ioctl_tiocgwinsz_nt(struct Fd *, struct winsize *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_ */
