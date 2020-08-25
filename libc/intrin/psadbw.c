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
#include "libc/intrin/psadbw.h"
#include "libc/macros.h"

/**
 * Computes sum of absolute differences.
 *
 * @param 𝑎 [w/o] receives sum at first index and rest is zero'd
 * @param 𝑏 [r/o] is your first unsigned byte array
 * @param 𝑐 [r/o] is your second unsigned byte array
 * @mayalias
 */
void(psadbw)(uint64_t a[2], const uint8_t b[16], const uint8_t c[16]) {
  unsigned i, x, y;
  for (x = i = 0; i < 8; ++i) x += ABS(b[i] - c[i]);
  for (y = 0; i < 16; ++i) y += ABS(b[i] - c[i]);
  a[0] = x;
  a[1] = y;
}
