#ifndef COSMOPOLITAN_LIBC_SOCK_WSAID_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_WSAID_INTERNAL_H_
#include "libc/nt/struct/guid.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *__get_wsaid(const struct NtGuid *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_WSAID_INTERNAL_H_ */
