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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

size_t n;

TEST(Underlong, test) {
  EXPECT_BINEQ(u"e e", gc(Underlong("e\300\200e", -1, &n)));
  EXPECT_EQ(3, n);
  EXPECT_BINEQ(u"e ", gc(Underlong("e\300\200", -1, &n)));
  EXPECT_EQ(2, n);
}

TEST(Underlong, testWeirdInvalidLatin1) {
  EXPECT_BINEQ(u"e├Çe ", gc(Underlong("e\300e", -1, &n)));
  EXPECT_EQ(4, n);
}

TEST(Underlong, testNormalText) {
  EXPECT_STREQ(kHyperion, gc(Underlong(kHyperion, kHyperionSize, &n)));
  EXPECT_EQ(kHyperionSize, n);
}

BENCH(Underlong, bench) {
  EZBENCH2("Underlong", donothing,
           free(Underlong(kHyperion, kHyperionSize, 0)));
}
