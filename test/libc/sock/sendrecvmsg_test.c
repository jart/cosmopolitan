/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/iovec.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/testlib.h"

TEST(sendrecvmsg, testPingPong) {
  int fd[2];
  const char hello[] = "HELLO";
  const char world[] = "WORLD";
  struct msghdr msg;
  struct iovec data[2];
  const uint32_t hwLen = strlen(hello) + strlen(world);

  memset(&msg, 0, sizeof(msg));
  memset(&data[0], 0, sizeof(data));

  data[0].iov_base = (void *)hello;
  data[0].iov_len = strlen(hello);
  data[1].iov_base = (void *)world;
  data[1].iov_len = strlen(world); /* Don't send the '\0' */

  msg.msg_iov = &data[0];
  msg.msg_iovlen = 2;

  ASSERT_NE(-1, socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
  ASSERT_EQ(hwLen, sendmsg(fd[0], &msg, 0));

  data[0].iov_base = gc(calloc(20, 1));
  data[0].iov_len = 20;
  msg.msg_iovlen = 1;
  ASSERT_EQ(hwLen, recvmsg(fd[1], &msg, 0));

  EXPECT_STREQ("HELLOWORLD", msg.msg_iov[0].iov_base);

  ASSERT_NE(-1, close(fd[0]));
  ASSERT_NE(-1, close(fd[1]));
}
