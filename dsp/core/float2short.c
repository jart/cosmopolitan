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
#include "libc/limits.h"
#include "libc/macros.h"
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
  float f[8], w[8];
  for (i = 0; i < n; ++i) {
    for (j = 0; j < 8; ++j) {
      pcm16[i][j] =
          MIN(SHRT_MAX, MAX(SHRT_MIN, floorf(binary32[i][j] * (SHRT_MAX + 1))));
    }
  }
}
