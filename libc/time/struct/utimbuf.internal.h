#ifndef COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_INTERNAL_H_
#include "libc/time/struct/utimbuf.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_utime(const char *, const struct utimbuf *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TIME_STRUCT_UTIMBUF_INTERNAL_H_ */
