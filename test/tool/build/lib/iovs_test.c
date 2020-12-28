/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/testlib/testlib.h"
#include "tool/build/lib/iovs.h"

TEST(iovs, testEmpty) {
  struct Iovs iv;
  InitIovs(&iv);
  EXPECT_EQ(0, iv.i);
  EXPECT_GE(iv.n, iv.i);
  FreeIovs(&iv);
}

TEST(iovs, testAppendEmptyString_wontCreateEmptyEntries) {
  struct Iovs iv;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, "", 0));
  EXPECT_NE(-1, AppendIovs(&iv, NULL, 0));
  EXPECT_EQ(0, iv.i);
  EXPECT_GE(iv.n, iv.i);
  FreeIovs(&iv);
}

TEST(iovs, testAppendContiguousVectors_coalescesAdjacentEntries) {
  struct Iovs iv;
  char buf[8], *b = buf;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, b + 0, 4));
  EXPECT_NE(-1, AppendIovs(&iv, b + 4, 4));
  EXPECT_EQ(1, iv.i);
  EXPECT_GE(iv.n, iv.i);
  EXPECT_EQ(8, iv.p[0].iov_len);
  EXPECT_EQ(b, iv.p[0].iov_base);
  FreeIovs(&iv);
}

TEST(iovs, testAppendNoncontiguousVectors_growsMemoryAndPreservesOrdering) {
  struct Iovs iv;
  char buf[8], *b = buf;
  InitIovs(&iv);
  EXPECT_NE(-1, AppendIovs(&iv, b + 0, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 2, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 4, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 6, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 1, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 3, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 5, 1));
  EXPECT_NE(-1, AppendIovs(&iv, b + 7, 1));
  EXPECT_EQ(8, iv.i);
  EXPECT_GE(iv.n, iv.i);
  EXPECT_EQ(b + 0, iv.p[0].iov_base);
  EXPECT_EQ(1, iv.p[0].iov_len);
  EXPECT_EQ(b + 2, iv.p[1].iov_base);
  EXPECT_EQ(1, iv.p[1].iov_len);
  EXPECT_EQ(b + 4, iv.p[2].iov_base);
  EXPECT_EQ(1, iv.p[2].iov_len);
  EXPECT_EQ(b + 6, iv.p[3].iov_base);
  EXPECT_EQ(1, iv.p[3].iov_len);
  EXPECT_EQ(b + 1, iv.p[4].iov_base);
  EXPECT_EQ(1, iv.p[4].iov_len);
  EXPECT_EQ(b + 3, iv.p[5].iov_base);
  EXPECT_EQ(1, iv.p[5].iov_len);
  EXPECT_EQ(b + 5, iv.p[6].iov_base);
  EXPECT_EQ(1, iv.p[6].iov_len);
  EXPECT_EQ(b + 7, iv.p[7].iov_base);
  EXPECT_EQ(1, iv.p[7].iov_len);
  FreeIovs(&iv);
}
