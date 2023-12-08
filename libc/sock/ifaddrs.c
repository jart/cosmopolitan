/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/sock/ifaddrs.h"
#include "libc/calls/calls.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ifconf.h"
#include "libc/sock/struct/ifreq.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/iff.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/sock.h"

struct IfAddr {
  struct ifaddrs ifaddrs;
  char name[IFNAMSIZ];
  struct sockaddr_in addr;
  struct sockaddr_in netmask;
  struct sockaddr_in bstaddr;
};

/**
 * Frees network interface address list.
 */
void freeifaddrs(struct ifaddrs *ifp) {
  struct ifaddrs *next;
  while (ifp) {
    next = ifp->ifa_next;
    free(ifp);
    ifp = next;
  }
}

/**
 * Gets network interface address list.
 *
 * @return 0 on success, or -1 w/ errno
 * @see tool/viz/getifaddrs.c for example code
 */
int getifaddrs(struct ifaddrs **out_ifpp) {
  int rc = -1;
  int fd;
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) != -1) {
    char *data;
    size_t size;
    if ((data = malloc((size = 16384)))) {
      struct ifconf conf;
      conf.ifc_buf = data;
      conf.ifc_len = size;
      if (!ioctl(fd, SIOCGIFCONF, &conf)) {
        struct ifaddrs *res = 0;
        for (struct ifreq *ifr = (struct ifreq *)data;
             (char *)ifr < data + conf.ifc_len; ++ifr) {
          if (ifr->ifr_addr.sa_family != AF_INET) {
            continue;  // TODO(jart): IPv6 support
          }
          struct IfAddr *addr;
          if ((addr = calloc(1, sizeof(struct IfAddr)))) {
            memcpy(addr->name, ifr->ifr_name, IFNAMSIZ);
            addr->ifaddrs.ifa_name = addr->name;
            memcpy(&addr->addr, &ifr->ifr_addr, sizeof(struct sockaddr_in));
            addr->ifaddrs.ifa_addr = (struct sockaddr *)&addr->addr;
            addr->ifaddrs.ifa_netmask = (struct sockaddr *)&addr->netmask;
            if (!ioctl(fd, SIOCGIFFLAGS, ifr)) {
              addr->ifaddrs.ifa_flags = ifr->ifr_flags;
            }
            if (!ioctl(fd, SIOCGIFNETMASK, ifr)) {
              memcpy(&addr->netmask, &ifr->ifr_addr,
                     sizeof(struct sockaddr_in));
            }
            unsigned long op;
            if (addr->ifaddrs.ifa_flags & IFF_BROADCAST) {
              op = SIOCGIFBRDADDR;
            } else if (addr->ifaddrs.ifa_flags & IFF_POINTOPOINT) {
              op = SIOCGIFDSTADDR;
            } else {
              op = 0;
            }
            if (op && !ioctl(fd, op, ifr)) {
              memcpy(&addr->bstaddr, &ifr->ifr_addr,
                     sizeof(struct sockaddr_in));
              addr->ifaddrs.ifa_broadaddr =  // is union'd w/ ifu_dstaddr
                  (struct sockaddr *)&addr->bstaddr;
            }
            addr->ifaddrs.ifa_next = res;
            res = (struct ifaddrs *)addr;
          }
        }
        *out_ifpp = res;
        rc = 0;
      }
      free(data);
    }
    close(fd);
  }
  return rc;
}
