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
#include "libc/alg/reverse.h"
#include "libc/conv/conv.h"
#include "libc/conv/itoa.h"
#include "libc/limits.h"

uint128_t __udivmodti4(uint128_t, uint128_t, uint128_t *);

/**
 * Converts unsigned 128-bit integer to string.
 * @param a needs at least 40 bytes
 * @return bytes written w/o nul
 */
noinline size_t uint128toarray_radix10(uint128_t i, char *a) {
  size_t j;
  uint128_t rem;
  j = 0;
  do {
    i = __udivmodti4(i, 10, &rem);
    a[j++] = rem + '0';
  } while (i > 0);
  a[j] = '\0';
  reverse(a, j);
  return j;
}

/**
 * Converts signed 128-bit integer to string.
 * @param a needs at least 41 bytes
 * @return bytes written w/o nul
 */
size_t int128toarray_radix10(int128_t i, char *a) {
  if (i >= 0) return uint128toarray_radix10(i, a);
  *a++ = '-';
  return 1 + uint128toarray_radix10(-i, a);
}
