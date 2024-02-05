#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IPV6_MREQ_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IPV6_MREQ_H_
#include "libc/sock/struct/sockaddr6.h"
COSMOPOLITAN_C_START_

struct ipv6_mreq {
  struct in6_addr ipv6mr_multiaddr; /* IPv6 multicast address of group */
  unsigned ipv6mr_interface;        /* local interface */
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IPV6_MREQ_H_ */
