/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"

TEST(ipv4, test) {
  int ws, pid;
  char buf[16] = {0};
  int64_t inoffset;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
    ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
    ASSERT_SYS(0, 0, close(4));
    ASSERT_SYS(0, 0, close(3));
    _Exit(0);
  }
  EXPECT_SYS(0, 0, close(3));
  EXPECT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_SYS(0, 5, read(3, buf, 16));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(3));
  EXPECT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

TEST(ipv6, test) {
  int ws, pid;
  char buf[16] = {0};
  int64_t inoffset;
  uint32_t addrsize = sizeof(struct sockaddr_in6);
  struct sockaddr_in6 addr = {
      .sin6_family = AF_INET6,
      .sin6_addr.s6_addr[15] = 1,
  };
  ASSERT_SYS(0, 3, socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(AF_INET6, addr.sin6_family);
  ASSERT_NE(0, addr.sin6_port);
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
    ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
    ASSERT_SYS(0, 0, close(4));
    ASSERT_SYS(0, 0, close(3));
    _Exit(0);
  }
  EXPECT_SYS(0, 0, close(3));
  EXPECT_SYS(0, 3, socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_SYS(0, 5, read(3, buf, 16));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(3));
  EXPECT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}
