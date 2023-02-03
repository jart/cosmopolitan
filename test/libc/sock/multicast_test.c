/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/ip_mreq.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/sysv/consts/ip.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

struct Shared {
  atomic_int ready;
  atomic_int isworking;
} * g_shared;

void Receiver(void) {
  int y = 1;
  char buf[8];
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(31337),
      .sin_addr.s_addr = INADDR_ANY,
  };
  struct ip_mreq group = {
      .imr_multiaddr.s_addr = inet_addr("224.0.0.251"),
      .imr_interface.s_addr = INADDR_ANY,
  };
  EXPECT_SYS(0, 3, socket(AF_INET, SOCK_DGRAM, 0));
  EXPECT_SYS(0, 0, setsockopt(3, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)));
  EXPECT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_SYS(0, 0,
             setsockopt(3, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group,
                        sizeof(group)));
  g_shared->isworking = !g_testlib_failed;
  g_shared->ready = 1;
  if (g_shared->isworking) {
    ASSERT_SYS(0, 8, read(3, buf, 8));
  }
  ASSERT_SYS(0, 0, close(3));
}

void Sender(void) {
  int y = 1;
  char buf[8];
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = htons(31337),
      .sin_addr.s_addr = INADDR_ANY,
  };
  struct sockaddr_in dest = {
      .sin_family = AF_INET,
      .sin_port = htons(31337),
      .sin_addr.s_addr = inet_addr("224.0.0.251"),
  };
  struct ip_mreq group = {
      .imr_multiaddr.s_addr = inet_addr("224.0.0.251"),
      .imr_interface.s_addr = INADDR_ANY,
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, setsockopt(3, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(y)));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0,
             setsockopt(3, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group,
                        sizeof(group)));
  ASSERT_SYS(0, 8,
             sendto(3, buf, 8, 0, (struct sockaddr *)&dest, sizeof(dest)));
  ASSERT_SYS(0, 0, close(3));
}

TEST(multicast, test) {
  ASSERT_NE(NULL, (g_shared = _mapshared(FRAMESIZE)));
  SPAWN(fork);
  Receiver();
  PARENT();
  while (!g_shared->ready) {
  }
  if (g_shared->isworking) {
    Sender();
  }
  WAIT(exit, 0);
  munmap(g_shared, FRAMESIZE);
}
