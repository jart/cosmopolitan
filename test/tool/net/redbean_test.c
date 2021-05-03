/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/sigbits.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/gc.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/auxv.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("o/" MODE "/tool/net/redbean.com");
char testlib_enable_tmp_setup_teardown;

void SetUp(void) {
  ssize_t n;
  char buf[1024];
  int fdin, fdout;
  ASSERT_NE(-1, mkdir("bin", 0755));
  ASSERT_NE(-1, (fdin = open("zip:o/" MODE "/tool/net/redbean.com", O_RDONLY)));
  ASSERT_NE(-1, (fdout = creat("bin/redbean.com", 0755)));
  for (;;) {
    ASSERT_NE(-1, (n = read(fdin, buf, sizeof(buf))));
    if (!n) break;
    ASSERT_EQ(n, write(fdout, buf, n));
  }
  close(fdout);
  close(fdin);
}

TEST(redbean, test) {
  char portbuf[16];
  int pid, port, pipefds[2];
  sigset_t chldmask, savemask;
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  ASSERT_NE(-1, pipe(pipefds));
  ASSERT_NE(-1, (pid = vfork()));
  if (!pid) {
    close(pipefds[0]);
    dup2(pipefds[1], 1);
    sigprocmask(SIG_SETMASK, &savemask, NULL);
    execv("bin/redbean.com",
          (char *const[]){"bin/redbean.com", "-zp0", "-l127.0.0.1", 0});
    _exit(127);
  }
  EXPECT_NE(-1, close(pipefds[1]));
  EXPECT_NE(-1, read(pipefds[0], portbuf, sizeof(portbuf)));
  port = atoi(portbuf);
  printf("port %d\n", port);
  fflush(stdout);
  EXPECT_NE(-1, kill(pid, SIGTERM));
  EXPECT_NE(-1, wait(0));
}
