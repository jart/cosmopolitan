#ifndef COSMOPOLITAN_LIBC_SOCK_IFADDRS_H_
#define COSMOPOLITAN_LIBC_SOCK_IFADDRS_H_
#include "libc/sock/struct/sockaddr.h"
COSMOPOLITAN_C_START_

struct ifaddrs {
  struct ifaddrs *ifa_next;
  char *ifa_name;
  unsigned ifa_flags;
  struct sockaddr *ifa_addr;
  struct sockaddr *ifa_netmask;
  union {
    struct sockaddr *ifa_broadaddr;
    struct sockaddr *ifa_dstaddr;
  };
  void *ifa_data;
};

void freeifaddrs(struct ifaddrs *) libcesque;
int getifaddrs(struct ifaddrs **) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_SOCK_IFADDRS_H_ */
