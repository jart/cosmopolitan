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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/log/check.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "tool/decode/lib/bitabuilder.h"

/**
 * @fileoverview Sparse bit array builder.
 */

struct BitaBuilder {
  size_t i, n;
  unsigned *p;
};

struct BitaBuilder *bitabuilder_new(void) {
  return calloc(1, sizeof(struct BitaBuilder));
}

void bitabuilder_free(struct BitaBuilder **bbpp) {
  if (*bbpp) {
    free_s(&(*bbpp)->p);
    free_s(bbpp);
  }
}

/**
 * Sets bit.
 *
 * @return false if out of memory
 */
bool bitabuilder_setbit(struct BitaBuilder *bb, size_t bit) {
  void *p2;
  size_t i, n;
  i = MAX(bb->i, ROUNDUP(bit / CHAR_BIT + 1, __BIGGEST_ALIGNMENT__));
  if (i > bb->n) {
    n = i + (i >> 2);
    if ((p2 = realloc(bb->p, n))) {
      memset((char *)p2 + bb->n, 0, n - bb->n);
      bb->n = n;
      bb->p = p2;
    } else {
      return false;
    }
  }
  bb->i = i;
  bts(bb->p, bit);
  return true;
}

bool bitabuilder_fwrite(const struct BitaBuilder *bb, FILE *f) {
  return fwrite(bb->p, bb->i, 1, f);
}
