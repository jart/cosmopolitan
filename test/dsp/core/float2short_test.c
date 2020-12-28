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
