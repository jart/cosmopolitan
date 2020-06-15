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
#include "libc/mem/mem.h"
#include "tool/viz/lib/convolution.h"

/**
 * Creates padded array of array indices.
 *
 * This can be used to virtualize the memory of a matrix in one
 * dimension, to make it easy to avoid out-of-bounds access.
 *
 * @return value needs free(retval - leftpad)
 */
unsigned *convoindex(unsigned leftpad, unsigned n, unsigned rightpad) {
  unsigned i, j, *p;
  if ((p = malloc(sizeof(unsigned) * (leftpad + n + rightpad)))) {
    i = 0;
    for (j = 0; j < leftpad; ++j) p[i++] = 0;
    for (j = 0; j < n; ++j) p[i++] = j;
    for (j = 0; j < rightpad; ++j) p[i++] = n - 1;
  }
  return p;
}
