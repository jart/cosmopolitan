#ifndef COSMOPOLITAN_LIBC_SOCK_SOCKDEBUG_H_
#define COSMOPOLITAN_LIBC_SOCK_SOCKDEBUG_H_
#include "libc/sock/sock.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *__describe_sockaddr(const struct sockaddr *, size_t);
const char *__describe_socket_family(int);
const char *__describe_socket_type(int);
const char *__describe_socket_protocol(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_SOCKDEBUG_H_ */
