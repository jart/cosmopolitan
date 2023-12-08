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
#include "dsp/tty/quant.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "third_party/intel/xmmintrin.internal.h"

struct TtyRgb rgb2tty24f_(ttyrgb_m128 rgb) {
#ifdef __x86_64__
  const ttyrgb_m128 kMax = {1, 1, 1, 1};
  const ttyrgb_m128 kMin = {0, 0, 0, 0};
  struct TtyRgb res;
  rgb = _mm_min_ps(_mm_max_ps(rgb, kMin), kMax) * 255;
  res = (struct TtyRgb){rgb[0], rgb[1], rgb[2], rgb[3]};
  return res;
#else
  return (struct TtyRgb){
      MAX(0, MIN(1, rgb[0])) * 255,
      MAX(0, MIN(1, rgb[1])) * 255,
      MAX(0, MIN(1, rgb[2])) * 255,
      MAX(0, MIN(1, rgb[3])) * 255,
  };
#endif
}
