#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IN6_PKTINFO_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IN6_PKTINFO_H_
#include "libc/sock/struct/sockaddr6.h"
COSMOPOLITAN_C_START_

struct in6_pktinfo {
  struct in6_addr ipi6_addr;
  unsigned ipi6_ifindex;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IN6_PKTINFO_H_ */
