/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Alison Winters                                                │
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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio tty", 0));
}

TEST(socketpair, testAfUnixStream) {
  int fd[2];
  const char ping[] = "ping";
  const char pong[] = "pong";
  char buf[32];

  ASSERT_NE(-1, socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
  ASSERT_EQ(sizeof(ping), write(fd[0], ping, sizeof(ping)));
  ASSERT_EQ(sizeof(ping), read(fd[1], buf, sizeof(ping)));
  EXPECT_STREQ(ping, buf);
  ASSERT_EQ(sizeof(pong), write(fd[1], pong, sizeof(pong)));
  ASSERT_EQ(sizeof(pong), read(fd[0], buf, sizeof(pong)));
  EXPECT_STREQ(pong, buf);
  ASSERT_NE(-1, close(fd[0]));
  ASSERT_NE(-1, close(fd[1]));
}

TEST(socketpair, testAfUnixDgram) {
  int fd[2];
  const char ping[] = "ping";
  const char pong[] = "pong";
  char buf[32];

  ASSERT_NE(-1, socketpair(AF_UNIX, SOCK_DGRAM, 0, fd));
  ASSERT_EQ(sizeof(ping), write(fd[0], ping, sizeof(ping)));
  ASSERT_EQ(sizeof(ping), read(fd[1], buf, sizeof(buf)));
  EXPECT_STREQ(ping, buf);
  ASSERT_EQ(sizeof(pong), write(fd[1], pong, sizeof(pong)));
  ASSERT_EQ(sizeof(pong), read(fd[0], buf, sizeof(buf)));
  EXPECT_STREQ(pong, buf);
  ASSERT_NE(-1, close(fd[0]));
  ASSERT_NE(-1, close(fd[1]));
}

TEST(socketpair, testCloexec) {
  int fd[2];
  const char ping[] = "ping";
  const char pong[] = "pong";
  char buf[32];

  ASSERT_NE(-1, socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, fd));
  ASSERT_EQ(sizeof(ping), write(fd[0], ping, sizeof(ping)));
  ASSERT_EQ(sizeof(ping), read(fd[1], buf, sizeof(ping)));
  EXPECT_STREQ(ping, buf);
  ASSERT_EQ(sizeof(pong), write(fd[1], pong, sizeof(pong)));
  ASSERT_EQ(sizeof(pong), read(fd[0], buf, sizeof(pong)));
  EXPECT_STREQ(pong, buf);
  ASSERT_NE(-1, close(fd[0]));
  ASSERT_NE(-1, close(fd[1]));
}
