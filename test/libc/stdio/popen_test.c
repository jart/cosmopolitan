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
#include "libc/calls/struct/sigaction.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/testlib.h"
#include "libc/thread/thread.h"
#ifdef __x86_64__

FILE *f;
char buf[32];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

void CheckForFdLeaks(void) {
  int rc, i, l = 0, e = errno;
  for (i = 3; i < 16; ++i) {
    rc = fcntl(i, F_GETFL);
    if (rc == -1) {
      ASSERT_EQ(EBADF, errno);
      errno = e;
    } else {
      kprintf("file descriptor %d leaked!\n", i);
      ++l;
    }
  }
  if (l) {
    __die();
  }
}

TEST(popen, command) {
  char foo[6];
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  ASSERT_NE(NULL, (f = popen("./echo.com hello", "r")));
  ASSERT_NE(NULL, fgets(foo, sizeof(foo), f));
  ASSERT_STREQ("hello", foo);
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

TEST(popen, semicolon) {
  ASSERT_NE(NULL, (f = popen("echo hello;echo there", "r")));
  ASSERT_STREQ("hello\n", fgets(buf, sizeof(buf), f));
  ASSERT_STREQ("there\n", fgets(buf, sizeof(buf), f));
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

TEST(popen, singleQuotes) {
  setenv("there", "a b c", true);
  ASSERT_NE(NULL, (f = popen("echo 'hello $there'; echo yo", "r")));
  ASSERT_STREQ("hello $there\n", fgets(buf, sizeof(buf), f));
  ASSERT_STREQ("yo\n", fgets(buf, sizeof(buf), f));
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

TEST(popen, doubleQuotes) {
  setenv("hello", "a b c", true);
  ASSERT_NE(NULL, (f = popen("echo \"$hello there\"", "r")));
  ASSERT_STREQ("a b c there\n", fgets(buf, sizeof(buf), f));
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

TEST(popen, quoteless) {
  setenv("there", "a b c", true);
  ASSERT_NE(NULL, (f = popen("echo hello; echo a$there; echo yo", "r")));
  ASSERT_STREQ("hello\n", fgets(buf, sizeof(buf), f));
  ASSERT_STREQ("aa b c\n", fgets(buf, sizeof(buf), f));  // mixed feelings
  ASSERT_STREQ("yo\n", fgets(buf, sizeof(buf), f));
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

TEST(popen, pipe) {
  setenv("there", "a b c", true);
  ASSERT_NE(NULL, (f = popen("echo hello | toupper", "r")));
  ASSERT_STREQ("HELLO\n", fgets(buf, sizeof(buf), f));
  ASSERT_EQ(0, pclose(f));
  CheckForFdLeaks();
}

sig_atomic_t gotsig;

void OnSig(int sig) {
  gotsig = 1;
}

TEST(popen, complicated) {
  if (IsWindows()) return;  // windows treats sigusr1 as terminate
  char cmd[64];
  signal(SIGUSR1, OnSig);
  sprintf(cmd, "read a ; test \"x$a\" = xhello && kill -USR1 %d", getpid());
  ASSERT_NE(NULL, (f = popen(cmd, "w")));
  ASSERT_GE(fputs("hello", f), 0);
  ASSERT_EQ(0, pclose(f));
  ASSERT_EQ(1, gotsig);
  signal(SIGUSR1, SIG_DFL);
  CheckForFdLeaks();
}

void *Worker(void *arg) {
  FILE *f;
  char buf[32];
  char *arg1, *arg2, *cmd;
  for (int i = 0; i < 8; ++i) {
    cmd = malloc(64);
    arg1 = malloc(13);
    arg2 = malloc(13);
    FormatInt32(arg1, _rand64());
    FormatInt32(arg2, _rand64());
    sprintf(cmd, "echo %s; ./echo.com %s", arg1, arg2);
    strcat(arg1, "\n");
    strcat(arg2, "\n");
    ASSERT_NE(NULL, (f = popen(cmd, "r")));
    EXPECT_STREQ(arg1, fgets(buf, sizeof(buf), f));
    EXPECT_STREQ(arg2, fgets(buf, sizeof(buf), f));
    ASSERT_EQ(0, pclose(f));
    free(arg2);
    free(arg1);
    free(cmd);
  }
  return 0;
}

TEST(popen, torture) {
  if (IsWindows()) {
    // TODO: Why does pclose() return kNtSignalAccessViolationa?!
    return;
  }
  int i, n = 4;
  pthread_t *t = gc(malloc(sizeof(pthread_t) * n));
  testlib_extract("/zip/echo.com", "echo.com", 0755);
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_create(t + i, 0, Worker, 0));
  for (i = 0; i < n; ++i) ASSERT_EQ(0, pthread_join(t[i], 0));
  CheckForFdLeaks();
}

#endif /* __x86_64__ */
