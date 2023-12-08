/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/emmintrin.internal.h"

/**
 * Mixes audio.
 *
 * This function performs saturated addition on an array of shorts.
 *
 * @param x needs to be 16-byte aligned
 * @param y needs to be 16-byte aligned
 */
void sad16x8n(size_t n, short x[n][8], const short y[n][8]) {
  size_t i;
  for (i = 0; i < n; ++i) {
#ifdef __x86_64__
    *(__m128i *)x[i] = _mm_adds_epi16(*(__m128i *)x[i], *(__m128i *)y[i]);
#elif defined(__aarch64__)
    *(int16x4_t *)x[i] = vqadd_s16(*(int16x4_t *)x[i], *(int16x4_t *)y[i]);
#else
    size_t j;
    for (j = 0; j < 8; ++j) {
      x[i][j] = MIN(MAX(x[i][j] + y[i][j], INT16_MIN), INT16_MAX);
    }
#endif
  }
}
