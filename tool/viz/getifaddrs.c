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
#include "libc/intrin/bsr.h"
#include "libc/intrin/newbie.h"
#include "libc/limits.h"
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
vnet0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>
        inet6 fe80::fc54:ff:fefe:70d prefixlen 64 flags<128> class 0x20<link> ifidx 38
vnet1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>
        inet6 fe80::fc54:ff:fe6a:6545 prefixlen 64 flags<128> class 0x20<link> ifidx 44
enp51s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>
        inet6 fe80::ce3f:bd13:34ea:c170 prefixlen 64 flags<128> class 0x20<link> ifidx 43
        inet 192.168.1.2 netmask 255.255.255.0 broadcast 192.168.1.255
wlp0s20f3: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>
        inet6 fe80::9e1f:6462:15e2:2bf7 prefixlen 64 flags<128> class 0x20<link> ifidx 3
        inet 192.168.1.95 netmask 255.255.255.0 broadcast 192.168.1.255
lo: flags=73<UP,RUNNING,LOOPBACK>
        inet6 ::1 prefixlen 128 flags<128> class 0x10<host>
        inet 127.0.0.1 netmask 255.0.0.0
virbr1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>
        inet 192.168.121.1 netmask 255.255.255.0 broadcast 192.168.121.255
virbr0: flags=4099<UP,BROADCAST,MULTICAST>
        inet 192.168.122.1 netmask 255.255.255.0 broadcast 192.168.122.255 */

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

void print_ifaddr(struct ifaddrs *ifa) {
  char buf[128];
  if (ifa->ifa_addr->sa_family == AF_INET) {
    if (sockaddr2str(ifa->ifa_addr, buf, sizeof(buf))) {
      tinyprint(1, "        inet ", buf, NULL);
    }
    if (sockaddr2str(ifa->ifa_netmask, buf, sizeof(buf))) {
      tinyprint(1, " netmask ", buf, NULL);
    }
    if ((ifa->ifa_flags & IFF_BROADCAST) &&
        sockaddr2str(ifa->ifa_broadaddr, buf, sizeof(buf))) {
      tinyprint(1, " broadcast ", buf, NULL);
    } else if ((ifa->ifa_flags & IFF_POINTOPOINT) &&
               sockaddr2str(ifa->ifa_dstaddr, buf, sizeof(buf))) {
      tinyprint(1, " dstaddr ", buf, NULL);
    }
  } else if (ifa->ifa_addr->sa_family == AF_INET6) {
    if (sockaddr2str(ifa->ifa_addr, buf, sizeof(buf))) {
      tinyprint(1, "        inet6 ", buf, NULL);
    }

    uint128_t netmask = ~be128toh(*((uint128_t *)(&(
        ((const struct sockaddr_in6 *)ifa->ifa_netmask)->sin6_addr))));
    int prefixlen = 128;
    if (netmask > 0) {
      uint64_t hi = (uint64_t)(netmask >> 64);
      prefixlen = (hi > 0) ? (63 - bsrl(hi)) : (127 - bsrl((uint64_t)netmask));
    }

    FormatUint64(buf, prefixlen);
    tinyprint(1, " prefixlen ", buf, NULL);

    int aflags = ((int *)ifa->ifa_data)[1];
    FormatUint64(buf, aflags);
    tinyprint(1, " flags<", buf, ">", NULL);

    int scope = ((((int *)ifa->ifa_data)[0]) / 16) % 5;
    // #define IPV6_ADDR_GLOBAL 	0x0000U
    // #define IPV6_ADDR_LOOPBACK	0x0010U
    // #define IPV6_ADDR_LINKLOCAL	0x0020U
    // #define IPV6_ADDR_SITELOCAL	0x0040U
    char *addr_types[] = {"global", "host", "link", "??", "site"};
    buf[0] = '0' + scope;
    buf[1] = '\0';
    tinyprint(1, " class 0x", buf, "0<", addr_types[scope], ">", NULL);

    if (scope == 2 || scope == 4) {  // linklocal or sitelocal
      FormatUint64(buf,
                   ((const struct sockaddr_in6 *)ifa->ifa_addr)->sin6_scope_id);
      tinyprint(1, " ifidx ", buf, NULL);
    }
  }
  tinyprint(1, "\n", NULL);
}
void print_iface(struct ifaddrs *ifa) {
  char buf[32];
  FormatUint64(buf, ifa->ifa_flags);
  tinyprint(1, ifa->ifa_name, ": flags=", buf, "<", NULL);
  char *sflags[] = {"UP",    "BROADCAST", "POINTOPOINT", "RUNNING",
                    "DEBUG", "LOOPBACK",  "MULTICAST",   "ALLMULTI",
                    "NOARP", "IPROMISC"};
  int iflags[] = {IFF_UP,    IFF_BROADCAST, IFF_POINTOPOINT, IFF_RUNNING,
                  IFF_DEBUG, IFF_LOOPBACK,  IFF_MULTICAST,   IFF_ALLMULTI,
                  IFF_NOARP, IFF_PROMISC};
  int first = 1;
  for (int i = 0; i < (sizeof(sflags) / sizeof(*sflags)); i++) {
    if (ifa->ifa_flags & iflags[i]) {
      if (!first) {
        tinyprint(1, ",", NULL);
      }
      first = 0;
      tinyprint(1, sflags[i], NULL);
    }
  }
  tinyprint(1, ">\n", NULL);
}

int main(int argc, char *argv[]) {
  // get network interface list
  struct ifaddrs *ifaddrs;
  if (getifaddrs(&ifaddrs)) {
    perror("getifaddrs");
    exit(1);
  }

  struct ifaddrs *ifaddrs_next_if = ifaddrs;
  struct ifaddrs *ifaddrs_curr_if = NULL;
  struct ifaddrs *ifa = NULL;
  int first = 1;
  // print network interface list
  while (ifa || ifaddrs_next_if) {
    if (!ifa) {
      if (!first) {
        tinyprint(1, "\n", NULL);
        first = 0;
      }
      ifaddrs_curr_if = ifaddrs_next_if;
      ifa = ifaddrs_next_if;
      ifaddrs_next_if = NULL;
      print_iface(ifa);
    }
    if (strcmp(ifa->ifa_name, ifaddrs_curr_if->ifa_name)) {
      if (!ifaddrs_next_if) {
        struct ifaddrs *ifa2 = ifaddrs;
        while (ifa2 != ifa && strcmp(ifa->ifa_name, ifa2->ifa_name)) {
          ifa2 = ifa2->ifa_next;
        }
        if (ifa2 == ifa) {
          ifaddrs_next_if = ifa;
        }
      }
    } else {
      print_ifaddr(ifa);
    }
    ifa = ifa->ifa_next;
  }

  // perform cleanup
  freeifaddrs(ifaddrs);

  // report success
  exit(0);
}
