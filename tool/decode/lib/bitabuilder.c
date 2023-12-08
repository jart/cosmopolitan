/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "tool/decode/lib/bitabuilder.h"
#include "libc/assert.h"
#include "libc/limits.h"
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"

/**
 * @fileoverview Sparse bit array builder.
 */

struct BitaBuilder {
  size_t i, n;
  uint32_t *p;
};

struct BitaBuilder *bitabuilder_new(void) {
  return calloc(1, sizeof(struct BitaBuilder));
}

void bitabuilder_free(struct BitaBuilder **bbpp) {
  if (*bbpp) {
    free((*bbpp)->p);
    free(*bbpp);
    *bbpp = 0;
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
      bzero((char *)p2 + bb->n, n - bb->n);
      bb->n = n;
      bb->p = p2;
    } else {
      return false;
    }
  }
  bb->i = i;
  bb->p[bit / 32] |= 1u << (bit % 32);
  return true;
}

bool bitabuilder_fwrite(const struct BitaBuilder *bb, FILE *f) {
  return fwrite(bb->p, bb->i, 1, f);
}
