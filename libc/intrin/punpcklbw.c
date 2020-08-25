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
#include "libc/intrin/punpcklbw.h"

/**
 * Interleaves low bytes.
 *
 *          0 1 2 3 4 5 6 7 8 9 A B C D E F
 *       B  A B C D E F G H i j k l m n o p
 *       C  Q R S T U V W X y z α σ π μ τ ε
 *          │ │ │ │ │ │ │ │
 *          │ │ │ └─────┐
 *          │ │ └───┐   │  etc...
 *          │ └─┐   │   │
 *          ├─┐ ├─┐ ├─┐ ├─┐
 *     → A  A Q B R C S D T E U F V G W H X
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 interleaved
 * @param 𝑏 [r/o] supplies eight words
 * @param 𝑐 [r/o] supplies eight words
 * @mayalias
 */
void(punpcklbw)(uint8_t a[16], const uint8_t b[16], const uint8_t c[16]) {
  a[0xf] = c[7];
  a[0xe] = b[7];
  a[0xd] = c[6];
  a[0xc] = b[6];
  a[0xb] = c[5];
  a[0xa] = b[5];
  a[0x9] = c[4];
  a[0x8] = b[4];
  a[0x7] = c[3];
  a[0x6] = b[3];
  a[0x5] = c[2];
  a[0x4] = b[2];
  a[0x3] = c[1];
  a[0x2] = b[1];
  a[0x1] = c[0];
  a[0x0] = b[0];
}
