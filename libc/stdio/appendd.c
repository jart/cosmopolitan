/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.internal.h"
#include "libc/str/str.h"

#define W sizeof(size_t)

/**
 * Appends raw data to buffer.
 */
int appendd(char **b, const void *s, size_t l) {
  char *p;
  struct appendz z;
  z = appendz((p = *b));
  if (ROUNDUP(z.i + l + 1, 8) + W > z.n) {
    if (!z.n) z.n = W * 2;
    while (ROUNDUP(z.i + l + 1, 8) + W > z.n) z.n += z.n >> 1;
    z.n = ROUNDUP(z.n, W);
    if ((p = realloc(p, z.n))) {
      z.n = malloc_usable_size(p);
      assert(!(z.n & (W - 1)));
      *b = p;
    } else {
      return -1;
    }
  }
  memcpy(p + z.i, s, l + 1);
  z.i += l;
  if (!IsTiny() && W == 8) {
    z.i |= (size_t)APPEND_COOKIE << 48;
  }
  *(size_t *)(p + z.n - W) = z.i;
  return l;
}
