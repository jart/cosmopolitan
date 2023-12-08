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
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/str/str.h"

/**
 * Converts floating point audio samples to pulse code modulation.
 *
 * @param rdi is number of sample chunks
 * @param rsi points to aligned pcm [-32k,+32k] output memory
 * @param rdx points to aligned float32 [-1,+1] input memory
 */
void float2short(size_t n, short pcm16[n][8], const float binary32[n][8]) {
  size_t i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < 8; ++j) {
      pcm16[i][j] =
          MIN(SHRT_MAX, MAX(SHRT_MIN, floorf(binary32[i][j] * (SHRT_MAX + 1))));
    }
  }
}
