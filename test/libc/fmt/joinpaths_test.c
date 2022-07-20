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
#include "libc/str/path.h"
#include "libc/testlib/testlib.h"

TEST(xjoinpaths, test) {
  char b[PATH_MAX];
  EXPECT_EQ(NULL, _joinpaths(b, sizeof(b), 0, 0));
  EXPECT_STREQ("x", _joinpaths(b, sizeof(b), "x", 0));
  EXPECT_STREQ("x", _joinpaths(b, sizeof(b), 0, "x"));
  EXPECT_STREQ("", _joinpaths(b, sizeof(b), "", ""));
  EXPECT_STREQ("b", _joinpaths(b, sizeof(b), "", "b"));
  EXPECT_STREQ("a/b", _joinpaths(b, sizeof(b), "a", "b"));
  EXPECT_STREQ("a/b", _joinpaths(b, sizeof(b), "a/", "b"));
  EXPECT_STREQ("a/b/", _joinpaths(b, sizeof(b), "a", "b/"));
  EXPECT_STREQ("/b", _joinpaths(b, sizeof(b), "a", "/b"));
  EXPECT_STREQ("b", _joinpaths(b, sizeof(b), ".", "b"));
  EXPECT_EQ(NULL, _joinpaths(b, 3, "a", "b/"));
  EXPECT_EQ(NULL, _joinpaths(b, 4, "a", "b/"));
  EXPECT_STREQ("a/b", _joinpaths(b, 4, "a/", "b"));
  EXPECT_STREQ("a/b/", _joinpaths(b, 5, "a", "b/"));
}
