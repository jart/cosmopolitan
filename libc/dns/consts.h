#ifndef COSMOPOLITAN_LIBC_DNS_CONSTS_H_
#define COSMOPOLITAN_LIBC_DNS_CONSTS_H_
#include "libc/sock/sock.h"

#define DNS_TYPE_A     1
#define DNS_TYPE_NS    2
#define DNS_TYPE_CNAME 5
#define DNS_TYPE_SOA   6
#define DNS_TYPE_PTR   12
#define DNS_TYPE_MX    15
#define DNS_TYPE_TXT   16

#define DNS_CLASS_IN 1

#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define kMinSockaddr4Size \
  (offsetof(struct sockaddr_in, sin_addr) + sizeof(struct in_addr))

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_DNS_CONSTS_H_ */
