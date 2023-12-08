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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/pollfd.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/limits.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"

TEST(connect, nonblocking) {
  if (IsFreebsd()) return;  // TODO(jart): why did this start flaking?
  if (IsOpenbsd()) return;  // TODO(jart): why did this start freezing?
  char buf[16] = {0};
  atomic_uint *sem = _mapshared(sizeof(unsigned));
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
  while (!*sem) pthread_yield();
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 2, read(4, buf, 16));  // hi
  ASSERT_SYS(0, 5, write(4, "hello", 5));
  ASSERT_SYS(0, 3, read(4, buf, 16));  // bye
  PARENT();
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP));
  ASSERT_SYS(EINPROGRESS, -1,
             connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  if (!(IsLinux() || IsNetbsd())) {
    // this doens't work on rhel7 and netbsd
    ASSERT_SYS(EALREADY, -1,
               connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  }
  ASSERT_SYS(EAGAIN, -1, read(3, buf, 16));
  *sem = 1;
  {  // wait until connected
    struct pollfd pfd = {3, POLLOUT};
    ASSERT_SYS(0, 1, poll(&pfd, 1, -1));
    ASSERT_TRUE(!!(POLLOUT & pfd.revents));
  }
  ASSERT_SYS(0, 2, write(3, "hi", 2));
  {  // wait for other process to send us stuff
    struct pollfd pfd = {3, POLLIN};
    ASSERT_SYS(0, 1, poll(&pfd, 1, -1));
    ASSERT_TRUE(!!(POLLIN & pfd.revents));
  }
  ASSERT_SYS(0, 5, read(3, buf, 16));
  ASSERT_STREQ("hello", buf);
  ASSERT_SYS(0, 3, write(3, "bye", 3));
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
  munmap(sem, sizeof(unsigned));
}
