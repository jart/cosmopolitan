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
#include "libc/intrin/mpsadbw.h"
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * Computes multiple sum of absolute differences.
 *
 * This appears to be intended for video encoding motion estimation. It
 * can be combined with phminposuw. That allows us to search for an int
 * overlapping inside 𝑏 that's nearest to an aligned int in 𝑎.
 *
 * @note goes fast w/ sse4 cf. core c. 2006 cf. bulldozer c. 2011
 * @mayalias
 */
void(mpsadbw)(uint16_t c[8], const uint8_t b[16], const uint8_t a[16],
              uint8_t control) {
  unsigned i, j;
  uint16_t r[8];
  for (i = 0; i < 8; ++i) {
    r[i] = 0;
    for (j = 0; j < 4; ++j) {
      r[i] += ABS(b[(control & 4) + i + j] - a[(control & 3) * 4 + j]);
    }
  }
  memcpy(c, r, 16);
}
