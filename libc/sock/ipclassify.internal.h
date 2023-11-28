#ifndef COSMOPOLITAN_LIBC_SOCK_IPCLASSIFY_H_
#define COSMOPOLITAN_LIBC_SOCK_IPCLASSIFY_H_
#ifndef __STRICT_ANSI__
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/errfuns.h"
COSMOPOLITAN_C_START_

/**
 * Returns true if address is part of standardized test-only subnet.
 */
forceinline bool istestip(int sin_family, void *sin_addr) {
  if (sin_family == AF_INET) {
    uint32_t ip4net24 = ntohl(*(const uint32_t *)sin_addr) & 0xffffff00u;
    if (ip4net24 == INADDR_TESTNET1 || ip4net24 == INADDR_TESTNET2 ||
        ip4net24 == INADDR_TESTNET3) {
      return true;
    }
  } else {
    eafnosupport();
  }
  return false;
}

/**
 * Returns true if address is part of a local-only subnet.
 */
forceinline bool islocalip(int sin_family, void *sin_addr) {
  if (sin_family == AF_INET) {
    uint32_t ip4net8 = ntohl(*(const uint32_t *)sin_addr) & 0xff000000u;
    if (ip4net8 == 0x7f000000u) {
      return true;
    }
  } else {
    eafnosupport();
  }
  return false;
}

/**
 * Returns true if address is part of a well-known private subnet.
 */
forceinline bool isprivateip(int sin_family, void *sin_addr) {
  if (sin_family == AF_INET) {
    uint32_t ip4 = ntohl(*(const uint32_t *)sin_addr);
    if ((0x0a000000u <= ip4 && ip4 <= 0x0affffffu) /* 10.0.0.0/8     */ ||
        (0xac100000u <= ip4 && ip4 <= 0xac1fffffu) /* 172.16.0.0/12  */ ||
        (0xc0a80000u <= ip4 && ip4 <= 0xc0a8ffffu) /* 192.168.0.0/16 */) {
      return true;
    }
  } else {
    eafnosupport();
  }
  return false;
}

/**
 * Returns true if address is most likely part of the public Internet.
 */
forceinline bool ispublicip(int sin_family, void *sin_addr) {
  return !islocalip(sin_family, sin_addr) &&
         !isprivateip(sin_family, sin_addr) && !istestip(sin_family, sin_addr);
}

COSMOPOLITAN_C_END_
#endif /* !ANSI */
#endif /* COSMOPOLITAN_LIBC_SOCK_IPCLASSIFY_H_ */
