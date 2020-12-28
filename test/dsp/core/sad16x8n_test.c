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
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/runtime/buffer.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void addsw$pure(size_t n, short x[n][8], const short y[n][8]) {
  size_t i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < 8; ++j) {
      x[i][j] = MIN(MAX(x[i][j] + y[i][j], INT16_MIN), INT16_MAX);
    }
  }
}

short *pcm1;
short *pcm2;
struct GuardedBuffer b1, b2;

TEST(sad16x8n, test) {
  CHECK_NOTNULL((pcm1 = balloc(&b1, 32, 128 * 2)));
  CHECK_NOTNULL((pcm2 = balloc(&b2, 32, 128 * 2)));
  pcm1[0] = 0;
  pcm2[0] = 0;
  pcm1[1] = 23;
  pcm2[1] = 10;
  pcm1[2] = 23;
  pcm2[2] = -10;
  pcm1[3] = 23;
  pcm2[3] = -46;
  pcm1[120 + 0] = 0;
  pcm2[120 + 0] = 0;
  pcm1[120 + 1] = 23;
  pcm2[120 + 1] = 10;
  pcm1[120 + 2] = 23;
  pcm2[120 + 2] = -10;
  pcm1[120 + 3] = 23;
  pcm2[120 + 3] = -46;
  sad16x8n(128 / 8, (void *)pcm1, (void *)pcm2);
  EXPECT_EQ(0, pcm1[0]);
  EXPECT_EQ(33, pcm1[1]);
  EXPECT_EQ(13, pcm1[2]);
  EXPECT_EQ(-23, pcm1[3]);
  EXPECT_EQ(0, pcm1[120 + 0]);
  EXPECT_EQ(33, pcm1[120 + 1]);
  EXPECT_EQ(13, pcm1[120 + 2]);
  EXPECT_EQ(-23, pcm1[120 + 3]);
  bfree(&b1);
  bfree(&b2);
}

//////////////////////////////////////////////////////////////////////
// audio frame mixing latency: ~60ns (sse2 10x faster than pure)
//
//   1f ~= 14ms (cd quality)
//   (1.0 / 41000) * (1.0 / 2) * (1152 / 1.0)
//
//   1f ~= 845µs (dolby truehd)
//   (1.0 / 192000) * (1.0 / 7.1) * (1152 / 1.0)
//
// @note plm frame always has exactly 1152 floats

void randomizeaudio(void) {
  size_t i;
  for (i = 0; i < PLM_AUDIO_SAMPLES_PER_FRAME; ++i) {
    pcm1[i] = (rand() - INT_MAX / 2) % INT16_MAX;
    pcm2[i] = (rand() - INT_MAX / 2) % INT16_MAX;
  }
}

void sad16x8n_sse2(void) {
  sad16x8n(PLM_AUDIO_SAMPLES_PER_FRAME / 8, (void *)pcm1, (void *)pcm2);
}
void sad16x8n_pure(void) {
  addsw$pure(PLM_AUDIO_SAMPLES_PER_FRAME / 8, (void *)pcm1, (void *)pcm2);
}

BENCH(sad16x8n, audioframe) {
  CHECK_NOTNULL((pcm1 = balloc(&b1, 32, PLM_AUDIO_SAMPLES_PER_FRAME * 2)));
  CHECK_NOTNULL((pcm2 = balloc(&b2, 32, PLM_AUDIO_SAMPLES_PER_FRAME * 2)));
  EZBENCH(randomizeaudio(), sad16x8n_pure());
  EZBENCH(randomizeaudio(), sad16x8n_sse2());
  bfree(&b1);
  bfree(&b2);
}
