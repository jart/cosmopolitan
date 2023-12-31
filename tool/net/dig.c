/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sock.h"
#include "third_party/musl/netdb.h"
#include "tool/decode/lib/flagger.h"
#include "tool/decode/lib/idname.h"
#include "tool/decode/lib/socknames.h"

void lookup(const char *name) {
  int rc;
  struct addrinfo *ai = NULL;
  struct addrinfo hint = {AI_NUMERICSERV, AF_INET, SOCK_STREAM, IPPROTO_TCP};
  switch ((rc = getaddrinfo(name, "80", &hint, &ai))) {
    case 0:
      break;
    case EAI_SYSTEM:
      perror("getaddrinfo");
      exit(1);
    default:
      fprintf(stderr, "getaddrinfo failed: %d (EAI_%s)\n", rc,
              gai_strerror(rc));
      exit(1);
  }
  if (ai) {
    for (struct addrinfo *addr = ai; addr; addr = addr->ai_next) {
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
    freeaddrinfo(ai);
  } else {
    fprintf(stderr, "%s: %s\n", name, "no results");
  }
}

int main(int argc, char *argv[]) {
  int i;
  ShowCrashReports();
  for (i = 1; i < argc; ++i) lookup(argv[i]);
  return 0;
}
