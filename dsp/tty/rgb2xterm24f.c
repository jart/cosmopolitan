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
