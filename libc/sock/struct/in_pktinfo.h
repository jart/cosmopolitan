#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_IN_PKTINFO_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_IN_PKTINFO_H_
#include "libc/sock/struct/sockaddr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct in_pktinfo {
  int ipi_ifindex;
  struct in_addr ipi_spec_dst;
  struct in_addr ipi_addr;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_IN_PKTINFO_H_ */
