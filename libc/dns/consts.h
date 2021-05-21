#ifndef COSMOPOLITAN_LIBC_DNS_CONSTS_H_
#define COSMOPOLITAN_LIBC_DNS_CONSTS_H_
#include "libc/sock/sock.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define DNS_TYPE_A   0x01
#define DNS_TYPE_PTR 0x0c

#define DNS_CLASS_IN 1

#define kMinSockaddr4Size \
  (offsetof(struct sockaddr_in, sin_addr) + sizeof(struct in_addr))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_CONSTS_H_ */
