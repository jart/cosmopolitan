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
#include "libc/intrin/packuswb.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/str/str.h"

/**
 * Casts shorts to unsigned chars w/ saturation.
 *
 *   𝑎 ← {CLAMP[𝑏ᵢ]|𝑖∈[0,4)} ║ {CLAMP[𝑐ᵢ]|𝑖∈[4,8)}
 *
 * @see packsswb()
 * @mayalias
 */
void(packuswb)(uint8_t a[16], const int16_t b[8], const int16_t c[8]) {
  unsigned i;
  uint8_t r[16];
  for (i = 0; i < 8; ++i) r[i + 0] = MIN(UINT8_MAX, MAX(UINT8_MIN, b[i]));
  for (i = 0; i < 8; ++i) r[i + 8] = MIN(UINT8_MAX, MAX(UINT8_MIN, c[i]));
  memcpy(a, r, 16);
}
