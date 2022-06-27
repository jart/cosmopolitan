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
#include "libc/calls/struct/sigaction.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/mem/io.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("zip_uri_support");
STATIC_YOINK("apetest.com");
STATIC_YOINK("apetest2.com");

char testlib_enable_tmp_setup_teardown_once;

__attribute__((__constructor__)) static void init(void) {
  pledge("stdio rpath wpath cpath tty proc exec", 0);
  errno = 0;
}

void Extract(const char *from, const char *to, int mode) {
  ASSERT_SYS(0, 3, open(from, O_RDONLY));
  ASSERT_SYS(0, 4, creat(to, mode));
  ASSERT_NE(-1, _copyfd(3, 4, -1));
  EXPECT_SYS(0, 0, close(4));
  EXPECT_SYS(0, 0, close(3));
}

void SetUpOnce(void) {

  // nothing to do if we're using elf
  if (~SUPPORT_VECTOR & (WINDOWS | XNU)) {
    exit(0);
  }

  ASSERT_SYS(0, 0, mkdir("bin", 0755));
  Extract("/zip/apetest.com", "bin/apetest.com", 0755);
  Extract("/zip/apetest2.com", "bin/apetest2.com", 0755);

  // force the extraction of ape payload
  // if system does not have binfmt_misc
  ASSERT_SYS(0, 0, mkdir("tmp", 0755));
  setenv("TMPDIR", "tmp", true);
}

void RunApeTest(const char *path) {
  size_t n;
  ssize_t rc, got;
  char buf[512] = {0};
  sigset_t chldmask, savemask;
  int i, pid, fdin, wstatus, pfds[2];
  struct sigaction ignore, saveint, savequit, savepipe;
  ignore.sa_flags = 0;
  ignore.sa_handler = SIG_IGN;
  sigemptyset(&ignore.sa_mask);
  sigaction(SIGINT, &ignore, &saveint);
  sigaction(SIGQUIT, &ignore, &savequit);
  sigaction(SIGPIPE, &ignore, &savepipe);
  sigemptyset(&chldmask);
  sigaddset(&chldmask, SIGCHLD);
  sigprocmask(SIG_BLOCK, &chldmask, &savemask);
  ASSERT_SYS(0, 0, pipe2(pfds, O_CLOEXEC));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    __strace = 0;
    __ftrace = 0;
    close(1);
    dup(pfds[1]);
    sigaction(SIGINT, &saveint, 0);
    sigaction(SIGQUIT, &savequit, 0);
    sigaction(SIGPIPE, &savepipe, 0);
    sigprocmask(SIG_SETMASK, &savemask, 0);
    execv(path, (char *const[]){path, 0});
    _Exit(127);
  }
  close(pfds[1]);
  EXPECT_SYS(0, 8, read(pfds[0], buf, sizeof(buf)));
  EXPECT_STREQ("success\n", buf);
  close(pfds[0]);
  EXPECT_NE(-1, wait(&wstatus));
  EXPECT_TRUE(WIFEXITED(wstatus));
  EXPECT_EQ(0, WEXITSTATUS(wstatus));
  sigaction(SIGINT, &saveint, 0);
  sigaction(SIGQUIT, &savequit, 0);
  sigaction(SIGPIPE, &savepipe, 0);
  sigprocmask(SIG_SETMASK, &savemask, 0);
  EXPECT_SYS(0, 3, open(path, O_RDONLY));
  EXPECT_SYS(0, 6, read(3, buf, 6));
  EXPECT_SYS(0, 0, close(3));
  EXPECT_STREQN("MZqFpD", buf, 6);
}

TEST(ape, noAccidentalQuotesInMasterBootRecord) {
  int i, quotes = 0;
  char buf[512] = {0};
  EXPECT_SYS(0, 3, open("bin/apetest.com", O_RDONLY));
  EXPECT_SYS(0, 512, read(3, buf, 512));
  EXPECT_SYS(0, 0, close(3));
  for (i = 0; i < 512; ++i) {
    if (buf[i] == '\'') {
      ++quotes;
    }
  }
  EXPECT_EQ(1, quotes);
}

TEST(apeNoModifySelf, runsWithoutModifyingSelf) {
  RunApeTest("bin/apetest.com");
}

TEST(apeCopySelf, runsWithoutModifyingSelf) {
  RunApeTest("bin/apetest2.com");
}
