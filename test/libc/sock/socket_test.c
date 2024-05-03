/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/nt/winsock.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr6.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"

TEST(ipv4, test) {
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

TEST(ipv6, test) {
  char buf[16] = {0};
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
  SPAWN(fork);
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 5, read(3, buf, 16));
  ASSERT_STREQ("hello", buf);
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
}

TEST(getsockname, defaultsToZero) {
  struct sockaddr_in addr;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  memset(&addr, -1, sizeof(addr));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(sizeof(struct sockaddr_in), addrsize);
  ASSERT_EQ(AF_INET, addr.sin_family);
  ASSERT_EQ(0, addr.sin_addr.s_addr);
  ASSERT_EQ(0, addr.sin_port);
  EXPECT_SYS(0, 0, close(3));
}

TEST(getsockname, copiesSafely_givesFullSize) {
  struct sockaddr_in addr;
  uint32_t addrsize = 2;
  memset(&addr, -1, sizeof(addr));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_EQ(sizeof(struct sockaddr_in), addrsize);
  ASSERT_EQ(AF_INET, addr.sin_family);
  ASSERT_EQ(0xffffffff, addr.sin_addr.s_addr);
  ASSERT_EQ(0xffff, addr.sin_port);
  EXPECT_SYS(0, 0, close(3));
}

TEST(socket, canBeInheritedByForkedWorker) {
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
  ASSERT_SYS(0, 0, close(3));
  SPAWN(fork);
  ASSERT_SYS(0, 5, write(4, "hello", 5));
  PARENT();
  ASSERT_SYS(0, 0, close(4));
  WAIT(exit, 0);
  PARENT();
  EXPECT_SYS(0, 0, close(3));
  EXPECT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_SYS(0, 5, read(3, buf, 16));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(3));
  WAIT(exit, 0);
}

#ifdef __x86_64__

__attribute__((__constructor__)) static void StdioPro(int argc, char *argv[]) {
  if (argc >= 2 && !strcmp(argv[1], "StdioProg")) {
    ASSERT_EQ(NULL, getenv("__STDIO_SOCKETS"));
    ASSERT_EQ(5, write(1, "hello", 5));
    exit(0);
  }
}

TEST(socket, canBeUsedAsExecutedStdio) {
  char buf[16] = {0};
  const char *prog;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  prog = GetProgramExecutableName();
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  SPAWN(fork);
  ASSERT_SYS(0, 4,
             accept4(3, (struct sockaddr *)&addr, &addrsize, SOCK_CLOEXEC));
  ASSERT_SYS(0, 1, dup2(4, 1));
  SPAWN(vfork);
  execve(prog, (char *[]){(void *)prog, "StdioProg", 0}, (char *[]){0});
  abort();
  PARENT();
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
  PARENT();
  EXPECT_SYS(0, 0, close(3));
  EXPECT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_SYS(0, 5, read(3, buf, 16));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(3));
  WAIT(exit, 0);
}

#endif /* __x86_64__ */
