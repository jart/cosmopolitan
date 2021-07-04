#ifndef COSMOPOLITAN_LIBC_DNS_SERVICESTXT_H_
#define COSMOPOLITAN_LIBC_DNS_SERVICESTXT_H_
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int LookupServicesByPort(const int, char **, char *, size_t)
    paramsnonnull((2, 3));
int LookupServicesByName(const char *, char **) paramsnonnull((1, 2));

/* TODO: implement like struct HostsTxt? */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_SERVICESTXT_H_ */
