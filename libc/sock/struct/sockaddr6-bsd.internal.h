#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR6_BSD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR6_BSD_INTERNAL_H_
#include "libc/sock/struct/sockaddr6.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct sockaddr_in6_bsd {
  uint8_t sin6_len;
  uint8_t sin6_family;
  uint16_t sin6_port;
  uint32_t sin6_flowinfo;
  struct in6_addr sin6_addr;
  uint32_t sin6_scope_id;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_SOCKADDR6_BSD_INTERNAL_H_ */
