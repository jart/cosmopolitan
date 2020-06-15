/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/mpeg/mpeg.h"
#include "libc/macros.h"
#include "libc/rand/rand.h"
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
