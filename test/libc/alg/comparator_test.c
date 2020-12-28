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
#include "libc/alg/alg.h"
#include "libc/bits/bits.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(comparator, testByteCompare) {
  char *b1 = tmalloc(1);
  char *b2 = tmalloc(1);
  /* sign doesn't matter */
  EXPECT_EQ(cmpsb(memcpy(b1, "a", 1), memcpy(b2, "a", 1)), 0);
  EXPECT_LT(cmpsb(memcpy(b1, "a", 1), memcpy(b2, "z", 1)), 0);
  EXPECT_GT(cmpsb(memcpy(b1, "z", 1), memcpy(b2, "a", 1)), 0);
  EXPECT_EQ(cmpub(memcpy(b1, "a", 1), memcpy(b2, "a", 1)), 0);
  EXPECT_LT(cmpub(memcpy(b1, "a", 1), memcpy(b2, "z", 1)), 0);
  EXPECT_GT(cmpub(memcpy(b1, "z", 1), memcpy(b2, "a", 1)), 0);
  /* sign matters */
  EXPECT_EQ(cmpsb(memcpy(b1, "\xf0", 1), memcpy(b2, "\xf0", 1)), 0);
  EXPECT_LT(cmpsb(memcpy(b1, "\xf0", 1), memcpy(b2, "\x10", 1)), 0);
  EXPECT_GT(cmpsb(memcpy(b1, "\x10", 1), memcpy(b2, "\xf0", 1)), 0);
  EXPECT_EQ(cmpub(memcpy(b1, "\xf0", 1), memcpy(b2, "\xf0", 1)), 0);
  EXPECT_GT(cmpub(memcpy(b1, "\xf0", 1), memcpy(b2, "\x10", 1)), 0);
  EXPECT_LT(cmpub(memcpy(b1, "\x10", 1), memcpy(b2, "\xf0", 1)), 0);
  /* two's complement bane */
  EXPECT_GT(cmpsb(memcpy(b1, "\x7f", 1), memcpy(b2, "\x80", 1)), 0);
  EXPECT_LT(cmpub(memcpy(b1, "\x7f", 1), memcpy(b2, "\x80", 1)), 0);
  tfree(b2);
  tfree(b1);
}

TEST(comparator, testWordCompare) {
  char *b1 = tmalloc(2);
  char *b2 = tmalloc(2);
  EXPECT_EQ(cmpsw(memcpy(b1, "\x00\x80", 2), memcpy(b2, "\x00\x80", 2)), 0);
  EXPECT_GT(cmpsw(memcpy(b1, "\x00\x7f", 2), memcpy(b2, "\x00\x80", 2)), 0);
  EXPECT_LT(cmpsw(memcpy(b1, "\x00\x80", 2), memcpy(b2, "\x00\x7f", 2)), 0);
  EXPECT_EQ(cmpuw(memcpy(b1, "\x00\x80", 2), memcpy(b2, "\x00\x80", 2)), 0);
  EXPECT_LT(cmpuw(memcpy(b1, "\x00\x7f", 2), memcpy(b2, "\x00\x80", 2)), 0);
  EXPECT_GT(cmpuw(memcpy(b1, "\x00\x80", 2), memcpy(b2, "\x00\x7f", 2)), 0);
  tfree(b2);
  tfree(b1);
}

TEST(comparator, testDoublewordCompare) {
  char *b1 = tmalloc(4);
  char *b2 = tmalloc(4);
  EXPECT_EQ(cmpsl(memcpy(b1, "\x00\x00\x00\x80", 4),
                  memcpy(b2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_GT(cmpsl(memcpy(b1, "\x00\x00\x00\x7f", 4),
                  memcpy(b2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_LT(cmpsl(memcpy(b1, "\x00\x00\x00\x80", 4),
                  memcpy(b2, "\x00\x00\x00\x7f", 4)),
            0);
  EXPECT_EQ(cmpul(memcpy(b1, "\x00\x00\x00\x80", 4),
                  memcpy(b2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_LT(cmpul(memcpy(b1, "\x00\x00\x00\x7f", 4),
                  memcpy(b2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_GT(cmpul(memcpy(b1, "\x00\x00\x00\x80", 4),
                  memcpy(b2, "\x00\x00\x00\x7f", 4)),
            0);
  tfree(b2);
  tfree(b1);
}

TEST(comparator, testQuadwordCompare) {
  char *b1 = tmalloc(8);
  char *b2 = tmalloc(8);
  EXPECT_EQ(cmpsq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x80", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x80", 8)),
            0);
  EXPECT_GT(cmpsq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x7f", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x80", 8)),
            0);
  EXPECT_LT(cmpsq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x80", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x7f", 8)),
            0);
  EXPECT_EQ(cmpuq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x80", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x80", 8)),
            0);
  EXPECT_LT(cmpuq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x7f", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x80", 8)),
            0);
  EXPECT_GT(cmpuq(memcpy(b1, "\x00\x00\x00\x00\x00\x00\x00\x80", 8),
                  memcpy(b2, "\x00\x00\x00\x00\x00\x00\x00\x7f", 8)),
            0);
  tfree(b2);
  tfree(b1);
}
