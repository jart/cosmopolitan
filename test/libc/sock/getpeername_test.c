/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Gavin Arthur Hayes                                            │
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
#include "libc/dce.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

TEST(getpeername, worksAfterAcceptingOnFork) {
  // https://github.com/jart/cosmopolitan/issues/1174
  if (IsWindows()) {
    return;
  }
  char buf[16] = {0};
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  SPAWN(fork);
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
  struct sockaddr_storage out = {0};
  uint32_t out_size = sizeof(out);
  ASSERT_SYS(0, 0, getpeername(4, (struct sockaddr *)&out, &out_size));
  EXPECT_GE(sizeof(struct sockaddr_in), out_size);
  EXPECT_EQ(AF_INET, ((struct sockaddr_in *)&out)->sin_family);
  EXPECT_EQ(htonl(0x7f000001), ((struct sockaddr_in *)&out)->sin_addr.s_addr);
  ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 5, read(3, buf, 16));
  ASSERT_STREQ("hello", buf);
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
}

TEST(getpeername, worksAfterAcceptingOnParent) {
  char buf[16] = {0};
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  SPAWN(fork);
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 5, read(3, buf, 16));
  ASSERT_STREQ("hello", buf);
  PARENT();
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
  struct sockaddr_storage out = {0};
  uint32_t out_size = sizeof(out);
  ASSERT_SYS(0, 0, getpeername(4, (struct sockaddr *)&out, &out_size));
  EXPECT_GE(sizeof(struct sockaddr_in), out_size);
  EXPECT_EQ(AF_INET, ((struct sockaddr_in *)&out)->sin_family);
  EXPECT_EQ(htonl(0x7f000001), ((struct sockaddr_in *)&out)->sin_addr.s_addr);
  ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
}
