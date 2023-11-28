#ifndef COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPREQ_H_
#define COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPREQ_H_
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

struct arpreq {
  struct sockaddr arp_pa;
  struct sockaddr arp_ha;
  int arp_flags;
  struct sockaddr arp_netmask;
  char arp_dev[16];
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_STRUCT_ARPREQ_H_ */
