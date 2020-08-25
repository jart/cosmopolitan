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
#include "libc/intrin/psraw.h"

/**
 * Divides shorts by two power.
 *
 * @note c needs to be a literal, asmconstexpr, or linkconstsym
 * @note arithmetic shift right will sign extend negatives
 * @mayalias
 */
void(psraw)(int16_t a[8], const int16_t b[8], unsigned char k) {
  unsigned i, x, m;
  if (k > 15) k = 15;
  for (i = 0; i < 8; ++i) {
    m = 0;
    x = b[i];
    if (x & 0x8000) m = ~(0xffff >> k);
    x >>= k;
    x |= m;
    a[i] = x;
  }
}
