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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/syslib.internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

TEST(O_NONBLOCK, canBeSetBySocket_toMakeListenNonBlocking) {
  // TODO(jart): this doesn't make any sense on windows
  if (IsWindows()) return;
  char buf[16] = {0};
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  pthread_spinlock_t *phaser = _mapshared(4096);
  EXPECT_EQ(0, pthread_spin_lock(phaser + 0));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 1));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  SPAWN(fork);
  ASSERT_SYS(EAGAIN, -1, accept(3, (struct sockaddr *)&addr, &addrsize));
  EXPECT_EQ(0, pthread_spin_unlock(phaser + 0));
  for (;;) {
    int e = errno;
    int fd = accept(3, (struct sockaddr *)&addr, &addrsize);
    if (fd == -1 && errno == EAGAIN) {
      errno = e;
      usleep(1000);
      continue;
    }
    ASSERT_SYS(0, 4, fd);
    break;
  }
  ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
  EXPECT_EQ(0, pthread_spin_unlock(phaser + 1));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
  PARENT();
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 0));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 1));
  EXPECT_SYS(0, 5, read(3, buf, 16));
  EXPECT_STREQ("hello", buf);
  EXPECT_SYS(0, 0, close(3));
  WAIT(exit, 0);
  munmap(phaser, 4096);
}

TEST(O_NONBLOCK, canBeTunedWithFcntl_toMakeReadNonBlocking) {
  char buf[16] = {0};
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  pthread_spinlock_t *phaser = _mapshared(4096);
  EXPECT_EQ(0, pthread_spin_lock(phaser + 0));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 1));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, SOMAXCONN));
  SPAWN(fork);
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 0));
  ASSERT_SYS(0, 5, send(4, "hello", 5, 0));
  EXPECT_EQ(0, pthread_spin_lock(phaser + 1));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
  PARENT();
  EXPECT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, O_RDWR, fcntl(3, F_GETFL));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, fcntl(3, F_SETFL, O_RDWR | O_NONBLOCK));
  ASSERT_SYS(EAGAIN, -1, read(3, buf, 16));
  EXPECT_EQ(0, pthread_spin_unlock(phaser + 0));
TryAgain:
  int e = errno;
  ssize_t rc = read(3, buf, 16);
  if (rc == -1 && errno == EAGAIN) {
    errno = e;
    usleep(1000);
    goto TryAgain;
  }
  EXPECT_SYS(0, 5, rc);
  EXPECT_STREQ("hello", buf);
  EXPECT_EQ(0, pthread_spin_unlock(phaser + 1));
  EXPECT_SYS(0, 0, close(3));
  WAIT(exit, 0);
  munmap(phaser, 4096);
}
