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
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "libc/x/x.h"

TEST(readansi, test) {
  char b[16];
  const char *s;
  int ws, pid, fds[2];
  ASSERT_NE(-1, pipe(fds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    s = "a\eM\e[A→\e[123;456R\e[>c\eOz\xc2\x9bM";
    write(fds[1], s, strlen(s));
    _exit(0);
  }
  close(fds[1]);
  EXPECT_EQ(1, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("a", b);
  EXPECT_EQ(2, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\eM", b);
  EXPECT_EQ(3, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\e[A", b);
  EXPECT_EQ(3, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("→", b);
  EXPECT_EQ(10, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\e[123;456R", b);
  EXPECT_EQ(4, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\e[>c", b);
  EXPECT_EQ(3, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\eOz", b);
  EXPECT_EQ(3, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("\xc2\x9bM", b);
  EXPECT_EQ(0, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("", b);
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

TEST(readansi, testOperatingSystemCommand) {
  char b[32];
  const char *s;
  int ws, pid, fds[2];
  s = "\e]rm -rf /\e\\";
  ASSERT_NE(-1, pipe(fds));
  ASSERT_NE(-1, (pid = fork()));
  if (!pid) {
    write(fds[1], s, strlen(s));
    _exit(0);
  }
  close(fds[1]);
  EXPECT_EQ(strlen(s), readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ(s, b);
  EXPECT_EQ(0, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("", b);
  close(fds[0]);
  ASSERT_NE(-1, wait(&ws));
  ASSERT_TRUE(WIFEXITED(ws));
  ASSERT_EQ(0, WEXITSTATUS(ws));
}

TEST(readansi, testSqueeze) {
  int fds[2];
  char b[32];
  const char *s = "\e\\";
  ASSERT_NE(-1, pipe(fds));
  write(fds[1], s, strlen(s));
  close(fds[1]);
  EXPECT_EQ(strlen(s), readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ(s, b);
  EXPECT_EQ(0, readansi(fds[0], b, sizeof(b)));
  EXPECT_STREQ("", b);
  close(fds[0]);
}
