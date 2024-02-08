/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "tool/viz/lib/bilinearscale.h"
#include "libc/log/log.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "tool/viz/lib/graphic.h"

TEST(BilinearScale, testWindmill_1x1_to_2x2) {
  EXPECT_BINEQ(
      u"λλ"
      u"λλ",
      BilinearScale(1, 2, 2, gc(malloc(1 * 2 * 2)), 1, 1, 1, gc(xunbing(u"λ")),
                    0, 1, 2, 2, 1, 1, .5, .5, 0, 0));
}

TEST(BilinearScale, testWindmill_4x4_to_2x2) {
  EXPECT_BINEQ(u"λ "
               u" λ",
               BilinearScale(1, 2, 2, gc(malloc(2 * 2)), 1, 4, 4,
                             gc(xunbing(u"λλ  "
                                        u"λλ  "
                                        u"  λλ"
                                        u"  λλ")),
                             0, 1, 2, 2, 4, 4, 2, 2, 0, 0));
}

TEST(BilinearScale, testWindmill_8x8_to_4x4) {
  EXPECT_BINEQ(u"λλ  "
               u"λλ  "
               u"  λλ"
               u"  λλ",
               BilinearScale(1, 4, 4, gc(malloc(4 * 4)), 1, 8, 8,
                             gc(xunbing(u"λλλλ    "
                                        u"λλλλ    "
                                        u"λλλλ    "
                                        u"λλλλ    "
                                        u"    λλλλ"
                                        u"    λλλλ"
                                        u"    λλλλ"
                                        u"    λλλλ")),
                             0, 1, 4, 4, 8, 8, 2, 2, 0, 0));
}

TEST(BilinearScale, testWindmill_4x4_to_8x8) {
  EXPECT_BINEQ(u"λλλλ┐   "
               u"λλλλ┐   "
               u"λλλλ┐   "
               u"λλλλ┐   "
               u"┐┐┐┐ƒ⁇⁇⁇"
               u"    ⁇λλλ"
               u"    ⁇λλλ"
               u"    ⁇λλλ",
               BilinearScale(1, 8, 8, gc(malloc(8 * 8)), 1, 4, 4,
                             gc(xunbing(u"λλ  "
                                        u"λλ  "
                                        u"  λλ"
                                        u"  λλ")),
                             0, 1, 8, 8, 4, 4, .5, .5, 0, 0));
}

TEST(BilinearScale, testWindmill_5x5_to_8x8_withRatioIntent) {
  EXPECT_BINEQ(u"λλλλλ   "
               u"λλλλλ   "
               u"λλλλλ   "
               u"¬¬¬╞λTTT"
               u"   Tλλλλ"
               u"     λλλ"
               u"     λλλ"
               u"     λλλ",
               BilinearScale(1, 8, 8, gc(malloc(8 * 8)), 1, 5, 5,
                             gc(xunbing(u"λλλ  "
                                        u"λλλ  "
                                        u"  λλλ"
                                        u"   λλ"
                                        u"   λλ")),
                             0, 1, 8, 8, 5, 5, 2 / 3., 2 / 3., 0, 0));
}

TEST(BilinearScale, testWindmill_5x5_to_8x8_withoutRatioIntent) {
  EXPECT_BINEQ(u"λλλλλÅ  "
               u"λλλλλÅ  "
               u"λλλλλÅ  "
               u"╧╧╧╪λñ//"
               u"   /λλλλ"
               u"   →Å└λλ"
               u"     oλλ"
               u"     oλλ",
               BilinearScale(1, 8, 8, gc(malloc(8 * 8)), 1, 5, 5,
                             gc(xunbing(u"λλλ  "
                                        u"λλλ  "
                                        u"  λλλ"
                                        u"   λλ"
                                        u"   λλ")),
                             0, 1, 8, 8, 5, 5, 5 / 8., 5 / 8., 0, 0));
}

TEST(BilinearScale, testNyquistTorture) {
  EXPECT_BINEQ(u"███ "
               u"████"
               u" ███"
               u"███ ",
               BilinearScale(1, 4, 4, gc(malloc(4 * 4)), 1, 8, 8,
                             gc(xunbing(u"█ █ █   "
                                        u" █ █ █  "
                                        u"█ █ █ █ "
                                        u" █ █ █ █"
                                        u"  █ █ █ "
                                        u" █ █ █  "
                                        u"█ █ █   "
                                        u" █ █    ")),
                             0, 1, 4, 4, 8, 8, 2, 2, 0, 0));
}

BENCH(BilinearScale, Bench) {
  void *src, *dst;
  double c, w1, h1, w2, h2;
  c = 3;
  w1 = 1920;
  h1 = 1080;
  w2 = 1136;
  h2 = 136;
  src = gc(memalign(32, w1 * h1 * c));
  dst = gc(memalign(32, w2 * h2 * c));
  EZBENCH2("BilinearScale", donothing,
           BilinearScale(c, h2, w2, dst, c, h1, w1, src, 0, c, h2, w2, h1, w1,
                         h2 / h1, w2 / w1, 0, 0));
}
