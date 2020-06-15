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
#include "dsp/core/core.h"
#include "dsp/mpeg/mpeg.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/rand/rand.h"
#include "libc/runtime/buffer.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

short pcm[8][8];
float binary32[8][8];
struct GuardedBuffer b1, b2;

TEST(float2short, test) {
  binary32[0][0] = -0.5;
  binary32[0][1] = 0.0;
  binary32[0][2] = 0.5;
  float2short(8, pcm, binary32);
  EXPECT_EQ(-16384, pcm[0][0]);
  EXPECT_EQ(0, pcm[0][1]);
  EXPECT_EQ(16384, pcm[0][2]);
}

TEST(float2short, testOverflow) {
  binary32[0][0] = -1.1;
  binary32[0][1] = -1.0;
  binary32[0][2] = 1.0;
  binary32[0][3] = 1.1;
  float2short(8, pcm, binary32);
  EXPECT_EQ(-32768, pcm[0][0]);
  EXPECT_EQ(-32768, pcm[0][1]);
  EXPECT_EQ(32767, pcm[0][2]);
  EXPECT_EQ(32767, pcm[0][3]);
}

void unclamped(size_t n, short pcm16[n][8], const float binary32[n][8]) {
  size_t i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < 8; ++j) {
      pcm16[i][j] = binary32[i][j] * 32768;
    }
  }
}

plm_samples_t samps;

void randomizeaudio(void) {
  size_t i;
  for (i = 0; i < ARRAYLEN(samps.interleaved); ++i) {
    samps.interleaved[i] = randf();
  }
}

void float2short_pure(void) {
  float2short(ARRAYLEN(samps.interleaved) / 8, pcm, (void *)samps.interleaved);
}

void float2short_unclamped(void) {
  unclamped(ARRAYLEN(samps.interleaved) / 8, pcm, (void *)samps.interleaved);
}

BENCH(float2short, audioframe) {
  EZBENCH(randomizeaudio(), float2short_pure());
  EZBENCH(randomizeaudio(), float2short_unclamped());
}
