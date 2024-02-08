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
#include "libc/calls/internal.h"
#include "libc/log/check.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ifconf.h"
#include "libc/sock/struct/ifreq.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/sio.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

TEST(siocgifconf, test) {
  size_t n;
  char *data;
  int socketfd;
  struct ifreq *ifr;
  struct ifconf conf;
  uint32_t ip, netmask;
  bool foundloopback = false;
  data = gc(malloc((n = 4096)));
  ASSERT_NE(-1, (socketfd = socket(AF_INET, SOCK_DGRAM, 0)));
  conf.ifc_buf = data;
  conf.ifc_len = n;
  ASSERT_NE(-1, ioctl(socketfd, SIOCGIFCONF, &conf));
  for (ifr = (struct ifreq *)data; (char *)ifr < data + conf.ifc_len; ++ifr) {
    if (ifr->ifr_addr.sa_family != AF_INET) continue;
    ip = ntohl(((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr);
    EXPECT_NE(-1, ioctl(socketfd, SIOCGIFNETMASK, ifr));
    netmask = ntohl(((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr.s_addr);
#if 1
    fprintf(stderr,
            "%s %x %d\n"
            "  ip      %hhu.%hhu.%hhu.%hhu\n"
            "  netmask %hhu.%hhu.%hhu.%hhu\n"
            "\n",
            ifr->ifr_name, ip, ifr->ifr_addr.sa_family, ip >> 24, ip >> 16,
            ip >> 8, ip, netmask >> 24, netmask >> 16, netmask >> 8, netmask);
#endif
    if (ip == 0x7f000001) {
      foundloopback = true;
      EXPECT_EQ(0xFF000000, netmask);
    }
  }
  EXPECT_TRUE(foundloopback);
  ASSERT_NE(-1, close(socketfd));
}

TEST(siocgifconf, mkntenvblock_systemroot) {
  if (__argc != 1) return;
  SPAWN(fork);
  execve(GetProgramExecutableName(),
         (char *[]){GetProgramExecutableName(), "hi", NULL}, (char *[]){NULL});
  abort();
  EXITS(0);
}

TEST(fionread, pipe) {
  int pfds[2];
  int pending;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_SYS(0, 2, write(pfds[1], "hi", 2));
  // checking the reading end is agreed upon
  ASSERT_SYS(0, 0, ioctl(pfds[0], FIONREAD, &pending));
  ASSERT_EQ(2, pending);
  // checking the writing end is real hairy
  // ASSERT_SYS(0, 0, ioctl(pfds[1], FIONREAD, &pending));
  // ASSERT_EQ(2, pending);
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, close(pfds[0]));
}

TEST(fionread, eof_returnsZeroWithoutError) {
  char buf[8];
  int pfds[2];
  int pending;
  ASSERT_SYS(0, 0, pipe(pfds));
  ASSERT_SYS(0, 0, close(pfds[1]));
  ASSERT_SYS(0, 0, ioctl(pfds[0], FIONREAD, &pending));
  ASSERT_EQ(0, pending);
  ASSERT_SYS(0, 0, read(pfds[0], buf, 8));
  ASSERT_SYS(0, 0, close(pfds[0]));
}
