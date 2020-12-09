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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"

/**
 * Converts unsigned 64-bit integer to octal string.
 * @param a needs at least 24 bytes
 * @return bytes written w/o nul
 */
noinline size_t uint64toarray_radix8(uint64_t i, char a[hasatleast 24]) {
  size_t j;
  j = 0;
  do {
    a[j++] = i % 8 + '0';
    i /= 8;
  } while (i > 0);
  a[j] = '\0';
  reverse(a, j);
  return j;
}
