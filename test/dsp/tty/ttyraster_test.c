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
#include "dsp/tty/quant.h"
#include "libc/bits/progn.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/csscolor.h"

/*TODO(jart): Re-enable me

static const struct TtyRgb kBlack = {0, 0, 0, 16};
static const struct TtyRgb kWhite = {255, 255, 255, 231};
static const struct TtyRgb kRed = {0xff, 0, 0, 196};

char vtbuf[128];

void ttyraster_true_setup(void) {
  ttyquantsetup(kTtyQuantTrue, kTtyQuantRgb, kTtyBlocksUnicode);
}

void ttyraster2x2_true(void) {
  ttyraster(vtbuf,
            (const struct TtyRgb *)(unsigned[2][2]){
                {DARKRED, GRAY1},
                {DARKRED, DARKRED},
            },
            2, 2, kBlack, kBlack);
}

TEST(ttyraster, testCorner) {
  ttyraster_true_setup();
  EXPECT_STREQ("\e[48;2;139;0;0;38;2;3;3;3m▝",
               PROGN(ttyraster2x2_true(), vtbuf));
}

TEST(ttyraster, testFullBlock_favorsSpace) {
  ttyquantsetup(kTtyQuantTrue, kTtyQuantRgb, kTtyBlocksUnicode);
  ttyraster(vtbuf,
            (struct TtyRgb *)(unsigned[2][2]){
                {DARKRED, DARKRED},
                {DARKRED, DARKRED},
            },
            2, 2, kBlack, kBlack);
  EXPECT_STREQ("\e[48;2;139;0;0m ", vtbuf);
}

TEST(ttyraster, testFullBlock_favorsUnicodeWhenCurrenttFgMatchesButNotBg) {
  ttyquantsetup(kTtyQuantTrue, kTtyQuantRgb, kTtyBlocksUnicode);
  ttyraster(vtbuf,
            (struct TtyRgb *)(unsigned[2][4]){
                {DARKRED, GRAY1, GRAY1, GRAY1},
                {DARKRED, DARKRED, GRAY1, GRAY1},
            },
            2, 4, kBlack, kBlack);
  EXPECT_STREQ("\e[48;2;139;0;0;38;2;3;3;3m▝█", vtbuf);
}

TEST(ttyraster, testFullBlock_forcesSwitchBackToSpaceForRuns) {
  ttyquantsetup(kTtyQuantTrue, kTtyQuantRgb, kTtyBlocksUnicode);
  ttyraster(vtbuf,
            (struct TtyRgb *)(unsigned[2][8]){
                {DARKRED, GRAY1, GRAY1, GRAY1, GRAY1, GRAY1, GRAY1, GRAY1},
                {DARKRED, DARKRED, GRAY1, GRAY1, GRAY1, GRAY1, GRAY1, GRAY1},
            },
            2, 8, kBlack, kBlack);
  EXPECT_STREQ("\e[48;2;139;0;0;38;2;3;3;3m▝█\e[48;2;3;3;3m  ", vtbuf);
}

////////////////////////////////////////////////////////////////////////////////

TEST(ttyraster_cp437, testSide) {
  ttyquantsetup(kTtyQuantTrue, kTtyQuantRgb, kTtyBlocksCp437);
  ttyraster(vtbuf,
            (const struct TtyRgb *)(unsigned[2][2]){
                {DARKRED, GRAY1},
                {DARKRED, GRAY1},
            },
            2, 2, kBlack, kBlack);
  EXPECT_STREQ("\e[48;2;3;3;3;38;2;139;0;0m▌", vtbuf);
}

////////////////////////////////////////////////////////////////////////////////

void ttyraster_xterm256_setup(void) {
  ttyquantsetup(kTtyQuantXterm256, kTtyQuantRgb, kTtyBlocksUnicode);
}

void ttyraster2x2_xterm256(void) {
  ttyraster(vtbuf,
            (const struct TtyRgb *)(struct TtyRgb[2][2]){
                {kBlack, kWhite},
                {kBlack, kBlack},
            },
            2, 2, kRed, kRed);
}

TEST(ttyraster_xterm256, testCorner) {
  ttyraster_xterm256_setup();
  ttyraster2x2_xterm256();
  EXPECT_STREQ("\e[48;5;16;38;5;231m▝", vtbuf);
}

void ttyraster6x2_xterm256(void) {
  ttyraster(vtbuf,
            (const struct TtyRgb *)(struct TtyRgb[2][6]){
                {kBlack, kWhite, kBlack, kWhite, kBlack, kWhite},
                {kBlack, kBlack, kBlack, kBlack, kBlack, kBlack},
            },
            2, 6, kRed, kRed);
}

TEST(ttyraster_xterm256, testCornerRepeat3) {
  ttyraster_xterm256_setup();
  ttyraster6x2_xterm256();
  EXPECT_STREQ("\e[48;5;16;38;5;231m▝▝▝", vtbuf);
}

////////////////////////////////////////////////////////////////////////////////

BENCH(ttyraster_true, bench) {
  ttyraster_true_setup();
  EZBENCH(donothing, ttyraster2x2_true());
}

BENCH(ttyraster_xterm256, bench2) {
  ttyraster_xterm256_setup();
  EZBENCH(donothing, ttyraster2x2_xterm256());
}

BENCH(ttyraster_xterm256, bench6) {
  ttyraster_xterm256_setup();
  EZBENCH(donothing, ttyraster6x2_xterm256());
}

*/
