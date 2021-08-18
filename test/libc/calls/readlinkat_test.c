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
#include "libc/log/log.h"
#include "libc/runtime/gc.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char testlib_enable_tmp_setup_teardown;

TEST(readlinkat, test) {
  ssize_t rc;
  char *p, *q;
  char buf[128];
  memset(buf, -1, sizeof(buf));
  ASSERT_NE(-1, xbarf("hello→", "hi", -1));
  ASSERT_STREQ("hi", gc(xslurp("hello→", 0)));
  ASSERT_NE(-1, symlink("hello→", "there→"));
  ASSERT_TRUE(isregularfile("hello→"));
  ASSERT_TRUE(issymlink("there→"));
  ASSERT_FALSE(isregularfile("there→"));
  ASSERT_NE(-1, (rc = readlink("there→", buf, sizeof(buf))));
  ASSERT_LT(rc, sizeof(buf));
  EXPECT_EQ(-1, buf[rc]);
  buf[rc] = 0;
  EXPECT_EQ(8, rc);
  EXPECT_STREQ("hello→", buf);
  p = gc(xjoinpaths(g_testlib_tmpdir, "hello→"));
  q = gc(realpath("there→", 0));
  EXPECT_EQ(0, strcmp(p, q), "%`'s\n\t%`'s", p, q);
}
