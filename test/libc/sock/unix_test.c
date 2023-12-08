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
#include "libc/atomic.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/timeval.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/nt/version.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/testlib/subprocess.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath cpath proc unix", 0));
}

TEST(unix, datagram) {
  if (IsWindows()) return;  // no unix datagram on windows :'(
  atomic_bool *ready = _mapshared(1);
  SPAWN(fork);
  char buf[256] = {0};
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  *ready = true;
  bzero(&addr, sizeof(addr));
  ASSERT_SYS(0, 0, getsockname(3, (void *)&addr, &len));
  ASSERT_EQ(11, len);
  ASSERT_STREQ("foo.sock", addr.sun_path);
  ASSERT_SYS(0, 5, read(3, buf, 256));
  EXPECT_STREQ("hello", buf);
  ASSERT_SYS(0, 0, close(3));
  PARENT();
  while (!*ready) sched_yield();
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 5, sendto(3, "hello", 5, 0, (struct sockaddr *)&addr, len));
  ASSERT_SYS(0, 0, close(3));
  WAIT(exit, 0);
  munmap(ready, 1);
}

void StreamServer(atomic_bool *ready) {
  char buf[256] = {0};
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_STREAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  bzero(&addr, sizeof(addr));
  ASSERT_SYS(0, 0, getsockname(3, (void *)&addr, &len));
  ASSERT_EQ(2 + 8 + 1, len);
  ASSERT_EQ(AF_UNIX, addr.sun_family);
  ASSERT_STREQ("foo.sock", addr.sun_path);
  ASSERT_SYS(0, 0, listen(3, 10));
  bzero(&addr, sizeof(addr));
  len = sizeof(addr);
  *ready = true;
  ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &len));
  ASSERT_EQ(AF_UNIX, addr.sun_family);
  EXPECT_STREQ("", addr.sun_path);
  ASSERT_SYS(0, 5, read(4, buf, 256));
  EXPECT_STREQ("hello", buf);
  ASSERT_SYS(0, 0, close(3));
}

TEST(unix, stream) {
  int ws;
  if (IsWindows() && !IsAtLeastWindows10()) return;
  atomic_bool *ready = _mapshared(1);
  // TODO(jart): move this line down when kFdProcess is gone
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_STREAM, 0));
  if (!fork()) {
    close(3);
    StreamServer(ready);
    _Exit(0);
  }
  while (!*ready) sched_yield();
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 0, connect(3, (void *)&addr, len));
  ASSERT_SYS(0, 5, write(3, "hello", 5));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_NE(-1, wait(&ws));
  EXPECT_TRUE(WIFEXITED(ws));
  EXPECT_EQ(0, WEXITSTATUS(ws));
  munmap(ready, 1);
}

TEST(unix, serverGoesDown_deletedSockFile) {  // field of landmine
  if (IsWindows()) return;
  if (IsCygwin()) return;
  char buf[8] = {0};
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  ASSERT_SYS(0, 4, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, connect(4, (void *)&addr, len));
  ASSERT_SYS(0, 5, write(4, "hello", 5));
  ASSERT_SYS(0, 5, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_EQ(-1, write(4, "hello", 5));
  ASSERT_TRUE(errno == ECONNREFUSED ||  // Linux
              errno == ECONNRESET);     // BSDs
  errno = 0;
  ASSERT_SYS(0, 0, unlink(addr.sun_path));
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  int rc = write(4, "hello", 5);
  ASSERT_TRUE(rc == -1 && (errno == ECONNRESET ||    //
                           errno == ENOTCONN ||      //
                           errno == ECONNREFUSED ||  //
                           errno == EDESTADDRREQ));
  errno = 0;
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 4, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, connect(4, (void *)&addr, len));
  ASSERT_SYS(0, 5, write(4, "hello", 5));
  ASSERT_SYS(0, 5, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}

TEST(unix, serverGoesDown_usingSendTo_unlink) {  // much easier
  if (IsWindows()) return;
  if (IsCygwin()) return;
  char buf[8] = {0};
  uint32_t len = sizeof(struct sockaddr_un);
  struct sockaddr_un addr = {AF_UNIX, "foo.sock"};
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  ASSERT_SYS(0, 4, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 5, sendto(4, "hello", 5, 0, (void *)&addr, len));
  ASSERT_SYS(0, 5, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(__iswsl1() ? ENOTCONN : ECONNREFUSED, -1,
             sendto(4, "hello", 5, 0, (void *)&addr, len));
  ASSERT_SYS(0, 0, unlink(addr.sun_path));
  ASSERT_SYS(ENOENT, -1, sendto(4, "hello", 5, 0, (void *)&addr, len));
  ASSERT_SYS(0, 3, socket(AF_UNIX, SOCK_DGRAM, 0));
  ASSERT_SYS(0, 0, bind(3, (void *)&addr, len));
  ASSERT_SYS(0, 5, sendto(4, "hello", 5, 0, (void *)&addr, len));
  ASSERT_SYS(0, 5, read(3, buf, 8));
  ASSERT_SYS(0, 0, close(4));
  ASSERT_SYS(0, 0, close(3));
}
