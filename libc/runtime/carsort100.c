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
#include "libc/runtime/carsort.h"
#include "libc/str/str.h"

/**
 * Sorts int32 key-value arrays of trivial size.
 *
 * @see test/libc/alg/carsort_test.c
 * @see carsort1000() if larger
 */
textstartup void carsort100(size_t n, int32_t a[n][2]) {
  int32_t t[2];
  unsigned i, j;
  for (i = 1; i < n; ++i) {
    j = i;
    memcpy(t, a[i], sizeof(t));
    while (j > 0 && t[0] < a[j - 1][0]) {
      memcpy(a[j], a[j - 1], sizeof(t));
      --j;
    }
    memcpy(a[j], t, sizeof(t));
  }
}
