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
#include "libc/intrin/phaddd.h"
#include "libc/str/str.h"

/**
 * Adds adjacent 32-bit integers.
 *
 * @param 𝑎 [w/o] receives reduced 𝑏 and 𝑐 concatenated
 * @param 𝑏 [r/o] supplies two pairs of ints
 * @param 𝑐 [r/o] supplies two pairs of ints
 * @note goes fast w/ ssse3 (intel c. 2004, amd c. 2011)
 * @mayalias
 */
void(phaddd)(int32_t a[4], const int32_t b[4], const int32_t c[4]) {
  int32_t t[4];
  t[0] = b[0] + b[1];
  t[1] = b[2] + b[3];
  t[2] = c[0] + c[1];
  t[3] = c[2] + c[3];
  memcpy(a, t, sizeof(t));
}
