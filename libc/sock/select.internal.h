#ifndef COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_
#include "libc/sock/select.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int32_t select$sysv(int32_t, fd_set *, fd_set *, fd_set *, struct timeval *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SELECT_INTERNAL_H_ */
