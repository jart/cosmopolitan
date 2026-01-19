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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/newbie.h"
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ifconf.h"
#include "libc/sock/struct/ifreq.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/iff.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/sock.h"

#define SIOCGIFAFLAG_IN6   3240126793  // bsd
#define SIOCGIFNETMASK_IN6 3240126757  // bsd

struct IfAddr {
  struct ifaddrs ifaddrs;
  char name[IFNAMSIZ];
  struct sockaddr_in addr;
  struct sockaddr_in netmask;
  struct sockaddr_in bstaddr;
};

struct IfAddr6Info {
  int addr_scope;
  int addr_flags;
};

struct IfAddr6 {
  struct ifaddrs ifaddrs;
  char name[IFNAMSIZ];
  struct sockaddr_in6 addr;
  struct sockaddr_in6 netmask;
  struct sockaddr_in6 bstaddr;  // unused
  struct IfAddr6Info info;
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

// hex repr to network order int
static uint128_t hex2no(const char *str) {
  uint128_t res = 0;
  const int max_quads = sizeof(uint128_t) * 2;
  int i = 0;
  while ((i < max_quads) && str[i]) {
    uint8_t acc = (((str[i] & 0xF) + (str[i] >> 6)) | ((str[i] >> 3) & 0x8));
    acc = acc << 4;
    i += 1;
    if (str[i]) {
      acc = acc | (((str[i] & 0xF) + (str[i] >> 6)) | ((str[i] >> 3) & 0x8));
      i += 1;
    }
    res = (res >> 8) | (((uint128_t)acc) << ((sizeof(uint128_t) - 1) * 8));
  }
  res = res >> ((max_quads - i) * 4);
  return res;
}

/**
 * Gets network interface IPv6 address list on linux.
 *
 * @return 0 on success, or -1 w/ errno
 */
static int getifaddrs_linux_ip6(struct ifconf *conf) {
  int fd;
  int n = 0;
  struct ifreq *ifreq = conf->ifc_req;
  const int bufsz = 44 + IFNAMSIZ + 1;
  char buf[bufsz + 1];  // one line max size
  if ((fd = sys_openat(0, "/proc/net/if_inet6", O_RDONLY, 0)) == -1) {
    return -1;
  }

  while ((n = sys_read(fd, &buf[n], bufsz - n)) &&
         ((char *)ifreq < (conf->ifc_buf + conf->ifc_len))) {
    // flags linux include/uapi/linux/if_addr.h:44
    // scope linux include/net/ipv6.h:L99

    //           *addr,   *index,   *plen,    *scope,   *flags,   *ifname
    char *s[] = {&buf[0], &buf[33], &buf[36], &buf[39], &buf[42], &buf[45]};
    int ifnamelen = 0;
    while (*s[5] == ' ') {
      ++s[5];
    }
    while (s[5][ifnamelen] > '\n') {
      ++ifnamelen;
    }
    buf[32] = buf[35] = buf[38] = buf[41] = buf[44] = s[5][ifnamelen] = '\0';
    bzero(ifreq, sizeof(*ifreq));
    ifreq->ifr_addr.sa_family = AF_INET6;
    memcpy(&ifreq->ifr_name, s[5], ifnamelen);
    *((uint128_t *)&ifreq->ifr6_addr) = hex2no(s[0]);
    ifreq->ifr6_ifindex = hex2no(s[1]);
    ifreq->ifr6_prefixlen = hex2no(s[2]);
    ifreq->ifr6_scope = hex2no(s[3]);
    ifreq->ifr6_flags = hex2no(s[4]);
    ++ifreq;
    int tlen = &s[5][ifnamelen] - &buf[0] + 1;
    n = bufsz - tlen;
    memcpy(&buf, &buf[tlen], n);
  }

  conf->ifc_len = (char *)ifreq - conf->ifc_buf;
  return sys_close(fd);
}

/**
 * Gets network interface address list.
 *
 * @return 0 on success, or -1 w/ errno
 * @see tool/viz/getifaddrs.c for example code
 */
int getifaddrs(struct ifaddrs **out_ifpp) {
  int rc = 0;
  int fd, fd6 = -1;
  if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) != -1) {
    char *data;
    size_t size;
    if ((data = malloc((size = 16384)))) {
      struct ifconf conf, confl6;
      conf.ifc_buf = data;
      conf.ifc_len = size;
      if (!ioctl(fd, SIOCGIFCONF, &conf)) {
        confl6.ifc_buf = data + conf.ifc_len;
        confl6.ifc_len = size - conf.ifc_len;
        if (IsLinux()) {
          rc = getifaddrs_linux_ip6(&confl6);
        }
        if (rc)
          return rc;
        conf.ifc_len += confl6.ifc_len;

        struct ifaddrs *res = 0;
        rc = -1;
        for (struct ifreq *ifr = (struct ifreq *)data;
             (char *)ifr < data + conf.ifc_len; ++ifr) {
          uint16_t family = ifr->ifr_addr.sa_family;
          if (family == AF_INET) {
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
          } else if (family == AF_INET6) {
            struct IfAddr6 *addr6;
            if ((addr6 = calloc(1, sizeof(struct IfAddr6)))) {
              addr6->ifaddrs.ifa_name = addr6->name;
              addr6->ifaddrs.ifa_addr = (struct sockaddr *)&addr6->addr;
              addr6->ifaddrs.ifa_netmask = (struct sockaddr *)&addr6->netmask;
              addr6->ifaddrs.ifa_broadaddr = (struct sockaddr *)&addr6->bstaddr;
              addr6->ifaddrs.ifa_data = (void *)&addr6->info;

              addr6->info.addr_scope = ifr->ifr6_scope;
              addr6->info.addr_flags = ifr->ifr6_flags;

              memcpy(&addr6->name, &ifr->ifr_name, IFNAMSIZ);

              addr6->addr.sin6_family = AF_INET6;
              addr6->addr.sin6_port = 0;
              addr6->addr.sin6_flowinfo = 0;
              addr6->addr.sin6_scope_id = ifr->ifr6_ifindex;
              memcpy(&addr6->addr.sin6_addr, &ifr->ifr6_addr,
                     sizeof(struct in6_addr));

              addr6->netmask.sin6_family = AF_INET6;
              addr6->netmask.sin6_port = 0;
              addr6->netmask.sin6_flowinfo = 0;
              addr6->addr.sin6_scope_id = ifr->ifr6_ifindex;

              if (IsBsd()) {  // on bsd we miss prefixlen and addr flags
                if (fd6 == -1) {
                  fd6 = socket(AF_INET6, SOCK_DGRAM | SOCK_CLOEXEC, 0);
                }
                uint8_t in6req[288];  // BSD struct in6_ifreq
                bzero(&in6req, sizeof(in6req));
                memcpy(&in6req, &ifr->ifr_name, IFNAMSIZ);
                in6req[16] = 28;  // sin6_len sizeof(struct sockaddr_in6_bsd)
                in6req[17] = AF_INET6;  // sin6_family
                memcpy(&in6req[24], &addr6->addr.sin6_addr,
                       sizeof(struct in6_addr));  // sin6_addr
                if (!ioctl(fd6, SIOCGIFAFLAG_IN6, &in6req)) {
                  addr6->info.addr_flags =
                      *(int *)(&in6req[16]);  // ifru_flags6
                }
                in6req[16] = 28;        // sin6_len
                in6req[17] = AF_INET6;  // sin6_family
                if (!ioctl(fd6, SIOCGIFNETMASK_IN6, &in6req)) {
                  memcpy(&(addr6->netmask.sin6_addr), &in6req[24],
                         sizeof(struct in6_addr));
                }
              } else {
                int prefixlen = ifr->ifr6_prefixlen;
                *((uint128_t *)&(addr6->netmask.sin6_addr)) = htobe128(
                    prefixlen == 0 ? 0 : (UINT128_MAX << (128 - prefixlen)));
              }

              if (!ioctl(fd, SIOCGIFFLAGS, ifr)) {
                addr6->ifaddrs.ifa_flags = ifr->ifr_flags;
              }

              bzero(&addr6->bstaddr, sizeof(struct sockaddr_in6));
              addr6->ifaddrs.ifa_next = res;
              res = (struct ifaddrs *)addr6;
            }
          }
        }
        *out_ifpp = res;
        rc = 0;
      }
      free(data);
    }
    close(fd);
    if (fd6 != -1) {
      close(fd6);
    }
  }
  return rc;
}
