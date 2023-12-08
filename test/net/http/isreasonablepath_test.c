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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/http.h"

TEST(IsReasonablePath, test) {
  EXPECT_TRUE(IsReasonablePath("/", 1));
  EXPECT_TRUE(IsReasonablePath("index.html", 10));
  EXPECT_TRUE(IsReasonablePath("/index.html", 11));
  EXPECT_TRUE(IsReasonablePath("/index.html", -1));
  EXPECT_TRUE(IsReasonablePath("/redbean.png", -1));
  EXPECT_TRUE(IsReasonablePath("/.ca.key", -1));
  EXPECT_TRUE(IsReasonablePath(".ca.key", -1));
}

TEST(IsReasonablePath, testEmptyString_allowedIfYouLikeImplicitLeadingSlash) {
  EXPECT_TRUE(IsReasonablePath(0, 0));
  EXPECT_TRUE(IsReasonablePath(0, -1));
  EXPECT_TRUE(IsReasonablePath("", 0));
}

TEST(IsReasonablePath, testHiddenFiles_areAllowed) {
  EXPECT_TRUE(IsReasonablePath("/.index.html", 12));
  EXPECT_TRUE(IsReasonablePath("/x/.index.html", 14));
}

TEST(IsReasonablePath, testDoubleSlash_isAllowed) {
  EXPECT_TRUE(IsReasonablePath("//", 2));
  EXPECT_TRUE(IsReasonablePath("foo//", 5));
  EXPECT_TRUE(IsReasonablePath("/foo//", 6));
  EXPECT_TRUE(IsReasonablePath("/foo//bar", 9));
}

TEST(IsReasonablePath, testNoncanonicalDirectories_areForbidden) {
  EXPECT_FALSE(IsReasonablePath(".", 1));
  EXPECT_FALSE(IsReasonablePath("..", 2));
  EXPECT_FALSE(IsReasonablePath("/.", 2));
  EXPECT_FALSE(IsReasonablePath("/..", 3));
  EXPECT_FALSE(IsReasonablePath("./", 2));
  EXPECT_FALSE(IsReasonablePath("../", 3));
  EXPECT_FALSE(IsReasonablePath("/./", 3));
  EXPECT_FALSE(IsReasonablePath("/../", 4));
  EXPECT_FALSE(IsReasonablePath("x/.", 3));
  EXPECT_FALSE(IsReasonablePath("x/..", 4));
  EXPECT_FALSE(IsReasonablePath("x/./", 4));
  EXPECT_FALSE(IsReasonablePath("x/../", 5));
  EXPECT_FALSE(IsReasonablePath("/x/./", 5));
  EXPECT_FALSE(IsReasonablePath("/x/../", 6));
}

TEST(IsReasonablePath, testNoncanonicalWindowsDirs_areForbidden) {
  EXPECT_FALSE(IsReasonablePath(".", 1));
  EXPECT_FALSE(IsReasonablePath("..", 2));
  EXPECT_FALSE(IsReasonablePath("\\.", 2));
  EXPECT_FALSE(IsReasonablePath("\\..", 3));
  EXPECT_FALSE(IsReasonablePath(".\\", 2));
  EXPECT_FALSE(IsReasonablePath("..\\", 3));
  EXPECT_FALSE(IsReasonablePath("\\.\\", 3));
  EXPECT_FALSE(IsReasonablePath("\\..\\", 4));
  EXPECT_FALSE(IsReasonablePath("x\\.", 3));
  EXPECT_FALSE(IsReasonablePath("x\\..", 4));
  EXPECT_FALSE(IsReasonablePath("x\\.\\", 4));
  EXPECT_FALSE(IsReasonablePath("x\\..\\", 5));
  EXPECT_FALSE(IsReasonablePath("\\x\\.\\", 5));
  EXPECT_FALSE(IsReasonablePath("\\x\\..\\", 6));
}

TEST(IsReasonablePath, testOverlongSlashDot_isDetected) {
  EXPECT_FALSE(IsReasonablePath("/\300\256", 3));        /* /. */
  EXPECT_TRUE(IsReasonablePath("/\300\257", 3));         /* // */
  EXPECT_FALSE(IsReasonablePath("\300\256\300\256", 4)); /* .. */
}

BENCH(IsReasonablePath, bench) {
  EZBENCH2("IsReasonablePath", donothing, IsReasonablePath("/index.html", 11));
}
