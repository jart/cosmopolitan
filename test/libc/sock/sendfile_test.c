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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

char testlib_enable_tmp_setup_teardown;

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath proc inet", 0));
}

TEST(sendfile, test) {
  int ws;
  char *buf;
  int64_t inoffset;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, creat("hyperion.txt", 0644));
  ASSERT_SYS(0, 512, write(3, kHyperion, 512));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, &addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, &addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, 1));
  if (!fork()) {
    inoffset = 0;
    ASSERT_SYS(0, 4, accept(3, &addr, &addrsize));
    ASSERT_SYS(0, 5, open("hyperion.txt", O_RDONLY));
    ASSERT_SYS(0, 512, sendfile(4, 5, &inoffset, 512));
    EXPECT_EQ(512, inoffset);
    ASSERT_SYS(0, 0, close(5));
    ASSERT_SYS(0, 0, close(4));
    ASSERT_SYS(0, 0, close(3));
    _Exit(0);
  }
  buf = gc(malloc(512));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_SYS(0, 0, connect(3, &addr, sizeof(addr)));
  EXPECT_SYS(0, 512, read(3, buf, 512));
  EXPECT_EQ(0, memcmp(buf, kHyperion, 512));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}
