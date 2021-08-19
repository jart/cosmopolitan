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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(utf8toutf32, test) {
  EXPECT_STREQ(L"", gc(utf8toutf32(0, 0, 0)));
  EXPECT_STREQ(L"", gc(utf8toutf32("", -1, 0)));
  EXPECT_STREQ(L"hello", gc(utf8toutf32("hello", -1, 0)));
}

TEST(utf8toutf32, testLargeAscii) {
  EXPECT_STREQ(L"hellohellohelloz", gc(utf8toutf32("hellohellohelloz", -1, 0)));
  EXPECT_STREQ(L"hellohellohellozhellohellohelloz",
               gc(utf8toutf32("hellohellohellozhellohellohelloz", -1, 0)));
}

TEST(utf8toutf32, testLargeThompsonPikeEncoded) {
  EXPECT_STREQ(L"hellohellohello𝑧hellohellohelloz",
               gc(utf8toutf32("hellohellohello𝑧hellohellohelloz", -1, 0)));
  EXPECT_STREQ(L"hellohellohelloh𝑧ellohellohelloz",
               gc(utf8toutf32("hellohellohelloh𝑧ellohellohelloz", -1, 0)));
  EXPECT_STREQ(
      L"𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
      gc(utf8toutf32(
          "𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
          -1, 0)));
}

BENCH(utf8toutf32, bench) {
  EZBENCH2("utf8toutf32", donothing,
           free(utf8toutf32(kHyperion, kHyperionSize, 0)));
}
