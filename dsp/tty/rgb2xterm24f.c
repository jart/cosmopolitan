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
#include "dsp/tty/quant.h"
#include "libc/math.h"

/*
struct TtyRgb rgb2tty24f_(__m128 rgb) {
  const __v4si kMax = {255, 255, 255, 255};
  const __v4si kMin = {0, 0, 0, 0};
  struct TtyRgb res;
  __v4si rgb255;
  rgb255 = _mm_min_ps(_mm_max_ps(_mm_cvtps_epi32(rgb * 255), kMin), kMax);
  res = (struct TtyRgb){rgb255[0], rgb255[1], rgb255[2], rgb255[3]};
  return res;
}
*/

struct TtyRgb rgb2tty24f_(__m128 rgb) {
  const __m128 kMax = {1, 1, 1, 1};
  const __m128 kMin = {0, 0, 0, 0};
  struct TtyRgb res;
  rgb = _mm_min_ps(_mm_max_ps(rgb, kMin), kMax) * 255;
  res = (struct TtyRgb){rgb[0], rgb[1], rgb[2], rgb[3]};
  return res;
}
