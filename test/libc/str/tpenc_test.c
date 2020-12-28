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
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("strwidth");

volatile uint64_t v;

TEST(tpenc, test) {
  EXPECT_EQ(0, tpenc(0));
  EXPECT_EQ(1, tpenc(1));
  EXPECT_EQ(' ', tpenc(' '));
  EXPECT_EQ(0x7f, tpenc(0x7f));
  EXPECT_EQ(0x008496E2, tpenc(L'▄'));
  EXPECT_EQ(0x8080808080FEul, tpenc(INT_MIN));
}

TEST(tpenc, theimp) {
  ASSERT_EQ(0x88989FF0, tpenc(L'😈'));
}

TEST(tpenc, testBeyondTheStandard) {
  ASSERT_EQ(0xBFBFBFBFBFFF, tpenc(-1));
}

uint64_t Tpenc(int x) {
  return (v = tpenc(VEIL("r", x)));
}

BENCH(tpenc, bench) {
  EZBENCH(donothing, Tpenc(0));
  EZBENCH(donothing, Tpenc(1));
  EZBENCH(donothing, Tpenc(' '));
  EZBENCH(donothing, Tpenc(0x7f));
  EZBENCH(donothing, Tpenc(L'▄'));
  EZBENCH(donothing, Tpenc(-1));
  EZBENCH(donothing, Tpenc(INT_MIN));
  fprintf(stderr, "\n");
}
