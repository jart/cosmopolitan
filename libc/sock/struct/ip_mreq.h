#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IP_MREQ_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IP_MREQ_H_
#include "libc/sock/struct/sockaddr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct ip_mreq {
  struct in_addr imr_multiaddr; /* IP multicast address of group */
  struct in_addr imr_interface; /* local IP address of interface */
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IP_MREQ_H_ */
