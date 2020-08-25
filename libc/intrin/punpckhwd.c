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
#include "libc/intrin/punpckhwd.h"
#include "libc/str/str.h"

/**
 * Interleaves high words.
 *
 *          0  1  2  3  4  5  6  7
 *       B  aa bb cc dd EE FF GG HH
 *       C  ii jj kk ll MM NN OO PP
 *                      └┤ └┤ └┤ └┤
 *              ┌────────┘  │  │  │
 *              │     ┌─────┘  │  │
 *              │     │     ┌──┘  │
 *          ┌───┤ ┌───┤ ┌───┤ ┌───┤
 *     → A  EE MM FF NN GG OO HH PP
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 interleaved
 * @param 𝑏 [r/o] supplies eight words
 * @param 𝑐 [r/o] supplies eight words
 * @mayalias
 */
void(punpckhwd)(uint16_t a[8], const uint16_t b[8], const uint16_t c[8]) {
  a[0] = b[4];
  a[1] = c[4];
  a[2] = b[5];
  a[3] = c[5];
  a[4] = b[6];
  a[5] = c[6];
  a[6] = b[7];
  a[7] = c[7];
}
