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
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/ipproto.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/shut.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/sock.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

void SetUpOnce(void) {
  if (IsNetbsd()) exit(0);
  if (IsOpenbsd()) exit(0);
  testlib_enable_tmp_setup_teardown();
  ASSERT_SYS(0, 0, pledge("stdio rpath wpath cpath proc inet", 0));
}

int64_t GetFileOffset(int fd) {
  int64_t pos;
  ASSERT_NE(-1, (pos = lseek(fd, 0, SEEK_CUR)));
  return pos;
}

TEST(sendfile, testSeeking) {
  char buf[1024];
  int64_t inoffset = 0;
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, creat("hyperion.txt", 0644));
  ASSERT_SYS(0, 512, write(3, kHyperion, 512));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, 1));
  if (!fork()) {
    ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
    ASSERT_SYS(0, 5, open("hyperion.txt", O_RDONLY));
    ASSERT_SYS(0, 12, sendfile(4, 5, &inoffset, 12));
    ASSERT_EQ(0, GetFileOffset(5));
    ASSERT_SYS(0, 8, read(5, buf, 8));
    ASSERT_EQ(0, memcmp(buf, "The fall", 8));
    ASSERT_EQ(8, GetFileOffset(5));
    ASSERT_SYS(EBADF, -1, sendfile(5, 5, &inoffset, -1));
    ASSERT_EQ(-1, sendfile(5, 5, &inoffset, -1));
    ASSERT_TRUE(errno == ESPIPE || errno == EBADF);
    errno = 0;
    ASSERT_SYS(0, 500, sendfile(4, 5, &inoffset, -1));
    ASSERT_EQ(8, GetFileOffset(5));
    ASSERT_EQ(512, inoffset);
    inoffset = -1;
    ASSERT_SYS(EINVAL, -1, sendfile(4, 5, &inoffset, -1));
    _Exit(0);
  }
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 12, read(3, buf, 12));  // needed due to windows
  ASSERT_SYS(0, 500, read(3, buf + 12, 700));
  ASSERT_EQ(0, memcmp(buf, kHyperion, 512));
  ASSERT_SYS(0, 0, close(3));
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

TEST(sendfile, testPositioning) {
  // TODO(jart): fix test regression on windows
  if (IsWindows()) return;
  char buf[1024];
  uint32_t addrsize = sizeof(struct sockaddr_in);
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(0x7f000001),
  };
  ASSERT_SYS(0, 3, creat("hyperion.txt", 0644));
  ASSERT_SYS(0, 512, write(3, kHyperion, 512));
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, bind(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 0, getsockname(3, (struct sockaddr *)&addr, &addrsize));
  ASSERT_SYS(0, 0, listen(3, 1));
  if (!fork()) {
    signal(SIGPIPE, SIG_IGN);
    ASSERT_SYS(0, 4, accept(3, (struct sockaddr *)&addr, &addrsize));
    ASSERT_SYS(0, 5, open("hyperion.txt", O_RDONLY));
    ASSERT_SYS(0, 6, sendfile(4, 5, 0, 6));
    ASSERT_EQ(6, GetFileOffset(5));
    ASSERT_SYS(0, 6, sendfile(4, 5, 0, 6));
    ASSERT_SYS(0, 0, shutdown(4, SHUT_WR));
    ASSERT_EQ(-1, sendfile(4, 5, 0, 6));
    ASSERT_TRUE(errno == EINVAL || errno == EPIPE);
    errno = 0;
    // XXX: WSL1 clobbers file offset on failure!
    if (!__iswsl1()) {
      ASSERT_EQ(12, GetFileOffset(5));
    }
    _Exit(0);
  }
  ASSERT_SYS(0, 0, close(3));
  ASSERT_SYS(0, 3, socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
  ASSERT_SYS(0, 0, connect(3, (struct sockaddr *)&addr, sizeof(addr)));
  ASSERT_SYS(0, 6, read(3, buf, 6));
  ASSERT_SYS(0, 6, read(3, buf + 6, 6));
  ASSERT_SYS(0, 0, read(3, buf, 12));
  ASSERT_EQ(0, memcmp(buf, kHyperion, 12));
  ASSERT_SYS(0, 0, close(3));
  int ws;
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}
