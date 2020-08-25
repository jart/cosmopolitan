/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dns/dns.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ai.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/socknames.h"

void lookup(const char *name) {
  struct addrinfo hints = (struct addrinfo){.ai_family = AF_INET,
                                            .ai_socktype = SOCK_STREAM,
                                            .ai_protocol = IPPROTO_TCP,
                                            .ai_flags = AI_NUMERICSERV};
  struct addrinfo *addrs = NULL;
  if (getaddrinfo(name, "80", &hints, &addrs) == -1) {
    perror("getaddrinfo");
    exit(1);
  }
  if (addrs) {
    for (struct addrinfo *addr = addrs; addr; addr = addr->ai_next) {
      const unsigned char *ip =
          addr->ai_family == AF_INET
              ? (const unsigned char *)&((struct sockaddr_in *)addr->ai_addr)
                    ->sin_addr
              : (const unsigned char *)"\0\0\0\0";
      printf("%-12s = %s\n", "ai_flags",
             RecreateFlags(kAddrInfoFlagNames, addr->ai_flags), addr->ai_flags);
      printf("%-12s = %s (%d)\n", "ai_family",
             findnamebyid(kAddressFamilyNames, addr->ai_family),
             addr->ai_family);
      printf("%-12s = %s (%d)\n", "ai_socktype",
             findnamebyid(kSockTypeNames, addr->ai_socktype),
             addr->ai_socktype);
      printf("%-12s = %s (%d)\n", "ai_protocol",
             findnamebyid(kProtocolNames, addr->ai_protocol),
             addr->ai_protocol);
      printf("%-12s = %d\n", "ai_addrlen", addr->ai_addrlen);
      printf("%-12s = %hhu.%hhu.%hhu.%hhu\n", "ai_addr", ip[0], ip[1], ip[2],
             ip[3]);
      printf("%-12s = %s\n", "ai_canonname", addr->ai_canonname);
    }
    freeaddrinfo(addrs);
  } else {
    fprintf(stderr, "%s: %s\n", name, "no results");
  }
}

int main(int argc, char *argv[]) {
  int i;
  showcrashreports();
  for (i = 1; i < argc; ++i) lookup(argv[i]);
  return 0;
}
