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
#include "libc/calls/calls.h"
#include "libc/fmt/itoa.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/ifaddrs.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/iff.h"

/* example output:

   eth0
   addr: 10.10.10.237
   netmask: 255.255.255.0
   broadcast: 10.10.10.255
   flags: IFF_UP IFF_BROADCAST IFF_MULTICAST IFF_RUNNING

   lo
   addr: 127.0.0.1
   netmask: 255.0.0.0
   flags: IFF_UP IFF_LOOPBACK IFF_RUNNING */

const char *sockaddr2str(const struct sockaddr *sa, char *buf, size_t size) {
  if (sa->sa_family == AF_INET) {
    return inet_ntop(AF_INET, &(((const struct sockaddr_in *)sa)->sin_addr),
                     buf, size);
  } else if (sa->sa_family == AF_INET6) {
    return inet_ntop(AF_INET6, &(((const struct sockaddr_in6 *)sa)->sin6_addr),
                     buf, size);
  } else {
    return 0;
  }
}

int main(int argc, char *argv[]) {

  // get network interface list
  struct ifaddrs *ifaddrs;
  if (getifaddrs(&ifaddrs)) {
    perror("getifaddrs");
    exit(1);
  }

  // print network interface list
  for (struct ifaddrs *ifa = ifaddrs; ifa; ifa = ifa->ifa_next) {
    tinyprint(1, ifa->ifa_name, "\n", NULL);

    char buf[128];
    if (sockaddr2str(ifa->ifa_addr, buf, sizeof(buf))) {
      tinyprint(1, "addr: ", buf, "\n", NULL);
    }
    if (sockaddr2str(ifa->ifa_netmask, buf, sizeof(buf))) {
      tinyprint(1, "netmask: ", buf, "\n", NULL);
    }
    if ((ifa->ifa_flags & IFF_BROADCAST) &&
        sockaddr2str(ifa->ifa_broadaddr, buf, sizeof(buf))) {
      tinyprint(1, "broadcast: ", buf, "\n", NULL);
    } else if ((ifa->ifa_flags & IFF_POINTOPOINT) &&
               sockaddr2str(ifa->ifa_dstaddr, buf, sizeof(buf))) {
      tinyprint(1, "dstaddr: ", buf, "\n", NULL);
    }

    if (ifa->ifa_addr->sa_family == AF_INET6) {
      int scope = ((int *)ifa->ifa_data)[0];
      int aflags = ((int *)ifa->ifa_data)[1];
      // #define IPV6_ADDR_LOOPBACK	0x0010U
      // #define IPV6_ADDR_LINKLOCAL	0x0020U
      // #define IPV6_ADDR_SITELOCAL	0x0040U

      // #define IFA_F_TEMPORARY		0x01
      // #define	IFA_F_NODAD		0x02
      // #define IFA_F_OPTIMISTIC	0x04
      // #define IFA_F_DADFAILED		0x08
      // #define	IFA_F_HOMEADDRESS	0x10
      // #define IFA_F_DEPRECATED	0x20
      // #define IFA_F_TENTATIVE		0x40
      // #define IFA_F_PERMANENT		0x80
      // #define IFA_F_MANAGETEMPADDR	0x100
      // #define IFA_F_NOPREFIXROUTE	0x200
      // #define IFA_F_MCAUTOJOIN	0x400
      // #define IFA_F_STABLE_PRIVACY	0x800
      tinyprint(1, "scope:", NULL);
      if (scope == 0x10) {
        tinyprint(1, " loopback", NULL);
      }
      if (scope == 0x20) {
        tinyprint(1, " linklocal", NULL);
      }
      if (scope == 0x40) {
        tinyprint(1, " sitelocal", NULL);
      }
      if (scope == 0x00) {
        tinyprint(1, " global", NULL);
      }
      tinyprint(1, "\n", NULL);

      tinyprint(1, "addr flags:", NULL);
      if (aflags & 0x01) {
        tinyprint(1, " temporary", NULL);
      }
      if (aflags & 0x02) {
        tinyprint(1, " nodad", NULL);
      }
      if (aflags & 0x04) {
        tinyprint(1, " optimistic", NULL);
      }
      if (aflags & 0x08) {
        tinyprint(1, " dadfailed", NULL);
      }
      if (aflags & 0x10) {
        tinyprint(1, " homeaddress", NULL);
      }
      if (aflags & 0x20) {
        tinyprint(1, " deprecated", NULL);
      }
      if (aflags & 0x40) {
        tinyprint(1, " tentative", NULL);
      }
      if (aflags & 0x80) {
        tinyprint(1, " permanent", NULL);
      }
      if (aflags & 0x100) {
        tinyprint(1, " managetempaddr", NULL);
      }
      if (aflags & 0x200) {
        tinyprint(1, " noprefixroute", NULL);
      }
      if (aflags & 0x400) {
        tinyprint(1, " mcautojoin", NULL);
      }
      if (aflags & 0x800) {
        tinyprint(1, " stable_privacy", NULL);
      }
      tinyprint(1, "\n", NULL);
    }

    tinyprint(1, "flags:", NULL);
    if (ifa->ifa_flags & IFF_UP) {
      tinyprint(1, " IFF_UP", NULL);
    }
    if (ifa->ifa_flags & IFF_DEBUG) {
      tinyprint(1, " IFF_DEBUG", NULL);
    }
    if (ifa->ifa_flags & IFF_LOOPBACK) {
      tinyprint(1, " IFF_LOOPBACK", NULL);
    }
    if (ifa->ifa_flags & IFF_MULTICAST) {
      tinyprint(1, " IFF_MULTICAST", NULL);
    }
    if (ifa->ifa_flags & IFF_ALLMULTI) {
      tinyprint(1, " IFF_ALLMULTI", NULL);
    }
    if (ifa->ifa_flags & IFF_NOARP) {
      tinyprint(1, " IFF_NOARP", NULL);
    }
    if (ifa->ifa_flags & IFF_PROMISC) {
      tinyprint(1, " IFF_PROMISC", NULL);
    }
    tinyprint(1, "\n", NULL);

    tinyprint(1, "\n", NULL);
  }

  // perform cleanup
  freeifaddrs(ifaddrs);

  // report success
  exit(0);
}
