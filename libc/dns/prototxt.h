#ifndef COSMOPOLITAN_LIBC_DNS_PROTOTXT_H_
#define COSMOPOLITAN_LIBC_DNS_PROTOTXT_H_
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int LookupProtoByNumber(const int, char *, size_t, const char *)
    paramsnonnull((2));
int LookupProtoByName(const char *, char *, size_t, const char *)
    paramsnonnull((1, 2));
char *GetNtProtocolsTxtPath(char *, uint32_t);

/* TODO: implement like struct HostsTxt? */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_PROTOTXT_H_ */
