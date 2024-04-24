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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

char *p;
size_t n;

TEST(EncodeHttpHeaderValue, emptyStrings_arePermitted) {
  EXPECT_STREQ("", gc(EncodeHttpHeaderValue(NULL, 0, 0)));
  EXPECT_STREQ("", gc(EncodeHttpHeaderValue("", 0, 0)));
  EXPECT_STREQ("", gc(EncodeHttpHeaderValue(" ", 1, 0)));
}

TEST(EncodeHttpHeaderValue, testPadded_trimsWhitespace) {
  EXPECT_STREQ("hello \tthere",
               gc(EncodeHttpHeaderValue(" \thello \tthere\t ", -1, 0)));
}

TEST(EncodeHttpHeaderValue, testUtf8_isConvertedToLatin1) {
  EXPECT_STREQ("\241\377\300", gc(EncodeHttpHeaderValue("¡ÿÀ", -1, 0)));
}

TEST(EncodeHttpHeaderValue, testUtf8_nonLatin1ResultsInError) {
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("☻", -1, 0)));
  EXPECT_EQ(EILSEQ, errno);
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("𐌰", -1, 0)));
  EXPECT_EQ(EILSEQ, errno);
}

TEST(EncodeHttpHeaderValue, testLatin1_willJustWorkIfYoureLucky) {
  EXPECT_STREQ("\241\377\300",
               gc(EncodeHttpHeaderValue("\241\377\300", -1, &n)));
  EXPECT_EQ(3, n);
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\377\241\300", -1, 0)));
}

TEST(EncodeHttpHeaderValue, testC0_isForbiddenExceptHorizontalTab) {
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\0", 1, 0)));
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\r", 1, 0)));
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\n", 1, 0)));
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\v", 1, 0)));
  EXPECT_STREQ("", gc(EncodeHttpHeaderValue("\t", 1, 0)));
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\177", 1, 0)));
}

TEST(EncodeHttpHeaderValue, testC1_isForbidden) {
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\205", 1, 0)));
  EXPECT_EQ(NULL, gc(EncodeHttpHeaderValue("\302\205", 2, 0)));
}

TEST(EncodeHttpHeaderValue, testOom_returnsNullAndSetsSizeToZero) {
  n = 31337;
  EXPECT_EQ(NULL, EncodeHttpHeaderValue("hello", 0x1000000000000, &n));
  EXPECT_EQ(0, n);
}

BENCH(EncodeHttpHeaderValue, bench) {
  n = 22851;
  p = gc(malloc(n));
  memset(p, 'a', n);
  EZBENCH2("EncodeHttpHeaderValue ascii", donothing,
           free(EncodeHttpHeaderValue(p, n, 0)));
  memset(p, '\300', n);
  EZBENCH2("EncodeHttpHeaderValue latin1", donothing,
           free(EncodeHttpHeaderValue(p, n, 0)));
}
