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
#include "dsp/core/core.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"

/**
 * Increases or decreases audio volume.
 *
 * @param 𝑝 is two-power w/ effective range [-15,15]
 */
void scalevolume(size_t n, int16_t pcm[n][8], int p) {
  /* TODO(jart): This isn't acceptable. */
  size_t i, j;
  if (p > 0) {
    if (p > 15) p = 15;
    for (i = 0; i < n; ++i) {
      for (j = 0; j < 8; ++j) {
        pcm[i][j] =
            MIN(SHRT_MAX, MAX(SHRT_MIN, (int)((unsigned)pcm[i][j] << p)));
      }
    }
  } else if (p < 0) {
    p = -p;
    if (p > 15) p = 15;
    for (i = 0; i < n; ++i) {
      for (j = 0; j < 8; ++j) {
        pcm[i][j] = pcm[i][j] >> p;
      }
    }
  }
}
