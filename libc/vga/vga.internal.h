#ifndef COSMOPOLITAN_LIBC_VGA_VGA_INTERNAL_H_
#define COSMOPOLITAN_LIBC_VGA_VGA_INTERNAL_H_

#if !(__ASSEMBLER__ + __LINKER__ + 0)
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"

COSMOPOLITAN_C_START_

ssize_t sys_writev_vga(struct Fd *, const struct iovec *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_VGA_VGA_H_ */
