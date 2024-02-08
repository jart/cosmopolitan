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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

size_t n;

TEST(DecodeLatin1, test) {
  EXPECT_STREQ("", gc(DecodeLatin1(NULL, 0, 0)));
  EXPECT_STREQ("¥atta", gc(DecodeLatin1("\245atta", -1, &n)));
  EXPECT_EQ(6, n);
  EXPECT_STREQ("\245atta", gc(EncodeLatin1("¥atta", -1, &n, 0)));
  EXPECT_EQ(5, n);
}

TEST(DecodeLatin1, testAbleToImposeCharacterRestrictions) {
  errno = 0;
  EXPECT_EQ(0, gc(EncodeLatin1("\200atta", -1, &n, kControlC1)));
  EXPECT_EQ(0, n);
  EXPECT_EQ(EILSEQ, errno);
  errno = 0;
  EXPECT_EQ(0, gc(EncodeLatin1("\002atta", -1, &n, kControlC0)));
  EXPECT_EQ(0, n);
  EXPECT_EQ(EILSEQ, errno);
}

TEST(EncodeLatin1, roundTrip) {
  int i;
  char b[256];
  for (i = 0; i < 256; ++i) b[i] = i;
  char *utf8 = gc(DecodeLatin1(b, 256, &n));
  EXPECT_EQ(384, n);
  char *lat1 = gc(EncodeLatin1(utf8, n, &n, 0));
  ASSERT_EQ(256, n);
  EXPECT_EQ(0, memcmp(b, lat1, 256));
}

TEST(DecodeLatin1, testOom_returnsNullAndSetsSizeToZero) {
  n = 31337;
  EXPECT_EQ(NULL, gc(DecodeLatin1("hello", 0x10000000000000, &n)));
  EXPECT_EQ(0, n);
}

BENCH(DecodeLatin1, bench) {
  EZBENCH2("DecodeLatin1", donothing,
           free(DecodeLatin1(kHyperion, kHyperionSize, 0)));
  EZBENCH2("EncodeLatin1", donothing,
           free(EncodeLatin1(kHyperion, kHyperionSize, 0, 0)));
}
