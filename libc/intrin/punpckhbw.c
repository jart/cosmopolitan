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
#include "libc/intrin/punpckhbw.h"

/**
 * Interleaves high bytes.
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 interleaved
 * @param 𝑏 [r/o] supplies eight words
 * @param 𝑐 [r/o] supplies eight words
 * @mayalias
 */
void(punpckhbw)(uint8_t a[16], const uint8_t b[16], const uint8_t c[16]) {
  a[0x0] = b[0x8];
  a[0x1] = c[0x8];
  a[0x2] = b[0x9];
  a[0x3] = c[0x9];
  a[0x4] = b[0xa];
  a[0x5] = c[0xa];
  a[0x6] = b[0xb];
  a[0x7] = c[0xb];
  a[0x8] = b[0xc];
  a[0x9] = c[0xc];
  a[0xa] = b[0xd];
  a[0xb] = c[0xd];
  a[0xc] = b[0xe];
  a[0xd] = c[0xe];
  a[0xe] = b[0xf];
  a[0xf] = c[0xf];
}
