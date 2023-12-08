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
#include "dsp/mpeg/mpeg.h"
#include "libc/macros.internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"
#include "tool/viz/lib/graphic.h"
#include "tool/viz/lib/ycbcr.h"

#if 0
__v4sf fRGBA[4][3];
unsigned char iRGB[3][8][8];
unsigned char uRGB[4][4][3];

unsigned char kY[16][16] = {
    {16, 43, 16, 43},
    {70, 97, 70, 97},
    {16, 43, 16, 43},
    {70, 97, 70, 97},
};

unsigned char kCb[8][8] = {
    {240, 240},
    {240, 240},
};

unsigned char kCr[8][8] = {
    {35, 35},
    {35, 35},
};

plm_frame_t kFrame = {
    .width = 4,
    .height = 4,
    .y = {.width = 16, .height = 16, .data = (void *)kY},
    .cb = {.width = 8, .height = 8, .data = (void *)kCb},
    .cr = {.width = 8, .height = 8, .data = (void *)kCr},
};

TEST(ycbcr2rgb, testMyImpl) {
  memset(iRGB, 0, sizeof(iRGB));
  YCbCr2RGB(1, iRGB, kY, kCb, kCr);
  EXPECT_EQ(0, iRGB[0][0][0]);
  EXPECT_BINEQ(u"                                                              "
               u"                                    ",
               iRGB[1]);
}

TEST(ycbcr2rgb, testReferenceImpl) {
  memset(uRGB, 0, sizeof(uRGB));
  plm_frame_to_rgb(&kFrame, (void *)uRGB);
  EXPECT_BINEQ(u" :╓", uRGB[0][0]);
  EXPECT_BINEQ(u" U±", uRGB[0][1]);
  EXPECT_BINEQ(u" :╓", uRGB[0][2]);
  EXPECT_BINEQ(u" U±", uRGB[0][3]);
  EXPECT_BINEQ(u" pλ", uRGB[1][0]);
  EXPECT_BINEQ(u" ïλ", uRGB[1][1]);
  EXPECT_BINEQ(u" pλ", uRGB[1][2]);
  EXPECT_BINEQ(u" ïλ", uRGB[1][3]);
  EXPECT_BINEQ(u" :╓", uRGB[2][0]);
  EXPECT_BINEQ(u" U±", uRGB[2][1]);
  EXPECT_BINEQ(u" :╓", uRGB[2][2]);
  EXPECT_BINEQ(u" U±", uRGB[2][3]);
  EXPECT_BINEQ(u" pλ", uRGB[3][0]);
  EXPECT_BINEQ(u" ïλ", uRGB[3][1]);
  EXPECT_BINEQ(u" pλ", uRGB[3][2]);
  EXPECT_BINEQ(u" ïλ", uRGB[3][3]);
}
#endif
