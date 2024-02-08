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
#include "libc/mem/shuffle.internal.h"
#include "libc/mem/gc.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/testlib/viewables.h"
#include "libc/x/x.h"

TEST(utf8to32, test) {
  EXPECT_STREQ(L"", gc(utf8to32(0, 0, 0)));
  EXPECT_STREQ(L"", gc(utf8to32("", -1, 0)));
  EXPECT_STREQ(L"hello", gc(utf8to32("hello", -1, 0)));
}

TEST(utf8to32, poke) {
  wchar_t *p = gc(utf8to32("hi", 2, 0));
  ASSERT_EQ('h', p[0]);
  ASSERT_EQ('i', p[1]);
  ASSERT_EQ(0, p[2]);
}

TEST(utf32to8, poke) {
  char *p = gc(utf32to8(L"hi", 2, 0));
  ASSERT_EQ('h', p[0]);
  ASSERT_EQ('i', p[1]);
  ASSERT_EQ(0, p[2]);
}

TEST(utf8to32, testLargeAscii) {
  EXPECT_STREQ(L"hellohellohelloz", gc(utf8to32("hellohellohelloz", -1, 0)));
  EXPECT_STREQ(L"hellohellohellozhellohellohelloz",
               gc(utf8to32("hellohellohellozhellohellohelloz", -1, 0)));
}

TEST(utf32to8, testLargeAscii) {
  EXPECT_STREQ("hellohellohelloz", gc(utf32to8(L"hellohellohelloz", -1, 0)));
  EXPECT_STREQ("hellohellohellozhellohellohelloz",
               gc(utf32to8(L"hellohellohellozhellohellohelloz", -1, 0)));
}

TEST(utf8to32, testLargeThompsonPikeEncoded) {
  EXPECT_STREQ(L"hellohellohello𝑧hellohellohelloz",
               gc(utf8to32("hellohellohello𝑧hellohellohelloz", -1, 0)));
  EXPECT_STREQ(L"hellohellohelloh𝑧ellohellohelloz",
               gc(utf8to32("hellohellohelloh𝑧ellohellohelloz", -1, 0)));
  EXPECT_STREQ(
      L"𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
      gc(utf8to32(
          "𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
          -1, 0)));
}

TEST(utf32to8, testLargeThompsonPikeEncoded) {
  EXPECT_STREQ("hellohellohello𝑧hellohellohelloz",
               gc(utf32to8(L"hellohellohello𝑧hellohellohelloz", -1, 0)));
  EXPECT_STREQ("hellohellohelloh𝑧ellohellohelloz",
               gc(utf32to8(L"hellohellohelloh𝑧ellohellohelloz", -1, 0)));
  EXPECT_STREQ(
      "𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
      gc(utf32to8(
          L"𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑕𝑒𝑙𝑙𝑜𝑧",
          -1, 0)));
}

char *GenerateBranchyUtf8Text(size_t *out_n) {
  char *p;
  size_t n;
  wchar_t *q = gc(utf8to32(kViewables, kViewablesSize, &n));
  shuffle(lemur64, q, n);
  p = utf32to8(q, n, &n);
  if (out_n) *out_n = n;
  return p;
}

/*
 *     utf8to32            l:     5,806c     1,875ns   m:     5,863c     1,894ns
 *     utf32to8            l:   104,671c    33,808ns   m:   103,803c    33,528ns
 *     utf8to32 [branchy]  l:   746,846c   241,227ns   m:   747,312c   241,377ns
 */
BENCH(utf8to32, bench) {
  EZBENCH2("utf8to32", donothing, free(utf8to32(kHyperion, kHyperionSize, 0)));
  size_t n;
  wchar_t *h = gc(utf8to32(kHyperion, kHyperionSize, &n));
  EZBENCH2("utf32to8", donothing, free(utf32to8(h, n, 0)));
  char *p = gc(GenerateBranchyUtf8Text(&n));
  EZBENCH2("utf8to32 [branchy]", donothing, free(utf8to32(p, n, 0)));
}
