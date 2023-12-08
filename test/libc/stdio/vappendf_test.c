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
#include "libc/serialize.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(vappendf, test) {
  char *b = 0;
  ASSERT_NE(-1, appendf(&b, "hello "));
  EXPECT_EQ(6, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  EXPECT_EQ(6, strlen(b));  // guarantees nul terminator
  ASSERT_NE(-1, appendf(&b, " world\n"));
  EXPECT_EQ(13, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  EXPECT_EQ(13, strlen(b));
  ASSERT_NE(-1, appendd(&b, "\0", 1));  // supports binary
  EXPECT_EQ(14, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  EXPECT_EQ(13, strlen(b));
  EXPECT_EQ(0, b[13]);
  EXPECT_EQ(0, b[14]);
  EXPECT_STREQ("hello  world\n", b);
  free(b);
}

TEST(appends, test) {
  char *b = 0;
  ASSERT_NE(-1, appends(&b, ""));
  EXPECT_NE(0, b);
  EXPECT_EQ(0, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  ASSERT_NE(-1, appends(&b, "hello "));
  EXPECT_EQ(6, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  EXPECT_EQ(6, strlen(b));  // guarantees nul terminator
  ASSERT_NE(-1, appends(&b, " world\n"));
  EXPECT_EQ(13, appendz(b).i);
  EXPECT_EQ(13, strlen(b));
  EXPECT_EQ(0, b[13]);
  EXPECT_STREQ("hello  world\n", b);
  free(b);
}

TEST(appendd, test) {
  char *b = 0;
  ASSERT_NE(-1, appendd(&b, 0, 0));
  EXPECT_NE(0, b);
  EXPECT_EQ(0, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  ASSERT_NE(-1, appendd(&b, "hello ", 6));
  EXPECT_EQ(6, appendz(b).i);
  EXPECT_EQ(0, b[appendz(b).i]);
  EXPECT_EQ(6, strlen(b));  // guarantees nul terminator
  ASSERT_NE(-1, appendd(&b, " world\n", 7));
  EXPECT_EQ(13, appendz(b).i);
  EXPECT_EQ(13, strlen(b));
  EXPECT_EQ(0, b[13]);
  EXPECT_STREQ("hello  world\n", b);
  free(b);
}

TEST(appendw, test) {
  char *b = 0;
  ASSERT_NE(-1, appendw(&b, 0));
  EXPECT_EQ(1, appendz(b).i);
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ(0, b[1]);
  ASSERT_NE(-1, appendw(&b, 'h'));
  EXPECT_EQ(2, appendz(b).i);
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ('h', b[1]);
  EXPECT_EQ(0, b[2]);
  ASSERT_NE(-1, appendw(&b, READ64LE("ello!\0\0")));
  EXPECT_EQ(7, appendz(b).i);
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ('h', b[1]);
  EXPECT_EQ('e', b[2]);
  EXPECT_EQ('l', b[3]);
  EXPECT_EQ('l', b[4]);
  EXPECT_EQ('o', b[5]);
  EXPECT_EQ('!', b[6]);
  EXPECT_EQ(0, b[7]);
  free(b);
}

TEST(appendr, testShrink_nulTerminates) {
  char *b = 0;
  ASSERT_NE(-1, appends(&b, "hello"));
  EXPECT_EQ(5, appendz(b).i);
  ASSERT_NE(-1, appendr(&b, 1));
  EXPECT_EQ(0, strcmp(b, "h"));
  EXPECT_EQ(1, appendz(b).i);
  ASSERT_NE(-1, appendr(&b, 0));
  EXPECT_EQ(0, appendz(b).i);
  EXPECT_EQ(0, strcmp(b, ""));
  ASSERT_NE(-1, appendr(&b, 5));
  EXPECT_EQ(0, b[0]);
  EXPECT_EQ(0, b[1]);
  EXPECT_EQ(0, b[2]);
  EXPECT_EQ(0, b[3]);
  EXPECT_EQ(0, b[4]);
  EXPECT_EQ(0, b[5]);
  free(b);
}

TEST(appendr, testExtend_zeroFills) {
  char *b = 0;
  ASSERT_NE(-1, appends(&b, "hello"));
  EXPECT_EQ(5, appendz(b).i);
  ASSERT_NE(-1, appendr(&b, 20));
  EXPECT_EQ(20, appendz(b).i);
  ASSERT_BINEQ(u"hello                ", b);
  ASSERT_NE(-1, appendr(&b, 20));
  EXPECT_EQ(20, appendz(b).i);
  ASSERT_BINEQ(u"hello                ", b);
  free(b);
}

TEST(appendr, testAbsent_allocatesNul) {
  char *b = 0;
  ASSERT_NE(-1, appendr(&b, 0));
  ASSERT_BINEQ(u" ", b);
  EXPECT_EQ(0, appendz(b).i);
  ASSERT_BINEQ(u" ", b);
  free(b);
}

TEST(appendd, testMemFail_doesntInitializePointer) {
  char *b = 0;
  ASSERT_EQ(-1, appendd(&b, 0, -1ull >> 8));
  EXPECT_EQ(0, b);
}

TEST(appendd, testMemFail_doesntFreeExistingAllocation) {
  char *b = 0;
  ASSERT_NE(-1, appends(&b, "hello"));
  EXPECT_STREQ("hello", b);
  ASSERT_EQ(-1, appendd(&b, 0, -1ull >> 7));
  EXPECT_STREQ("hello", b);
  free(b);
}

TEST(appendd, nontrivialAmountOfMemory) {
  char *b = 0;
  int i, n = 40000;
  for (i = 0; i < n; ++i) {
    ASSERT_EQ(2, appendd(&b, "hi", 2));
  }
  EXPECT_EQ(40000 * 2, appendz(b).i);
  free(b);
}

BENCH(vappendf, bench) {
  char *b = 0;
  EZBENCH2("appendf", donothing, appendf(&b, "hello"));
  EZBENCH2("kappendf", donothing, kappendf(&b, "hello"));
  free(b), b = 0;
  EZBENCH2("appends", donothing, appends(&b, "hello"));
  free(b), b = 0;
  EZBENCH2("appendw", donothing,
           appendw(&b, 'h' | 'e' << 8 | 'l' << 16 | 'l' << 24 |
                           (uint64_t)'o' << 32));
  free(b), b = 0;
  EZBENCH2("appendd", donothing, appendd(&b, "hello", 5));
  EZBENCH2("appendr", donothing, appendr(&b, 0));
  free(b), b = 0;
}
