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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.internal.h"
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
