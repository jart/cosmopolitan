/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/stdio/append.h"
#include "libc/str/str.h"

#define W sizeof(size_t)

/**
 * Appends data to buffer, e.g.
 *
 *     char *b = 0;
 *     appendd(&b, "hello", 5);
 *     free(b);
 *
 * The resulting buffer is guaranteed to be NUL-terminated, i.e.
 * `!b[appendz(b).i]` will be the case.
 *
 * @param s may contain nul characters and may be null if `l` is zero
 * @param l is byte length of `s`
 * @return bytes appended (always `l`) or -1 if `ENOMEM`
 * @see appendz(b).i to get buffer length
 * @note 20% faster than appends()
 */
ssize_t appendd(char **b, const void *s, size_t l) {
  char *p;
  size_t n;
  struct appendz z;
  z = appendz((p = *b));
  n = ROUNDUP(z.i + l + 1, 8) + W;
  if (n > z.n) {
    if (!z.n) z.n = W * 2;
    while (n > z.n) z.n += z.n >> 1;
    z.n = ROUNDUP(z.n, W);
    if ((p = realloc(p, z.n))) {
      z.n = malloc_usable_size(p);
      unassert(!(z.n & (W - 1)));
      *b = p;
    } else {
      return -1;
    }
  }
  if (l) {
    *(char *)mempcpy(p + z.i, s, l) = 0;
  } else {
    p[z.i] = 0;
  }
  z.i += l;
  if (!IsTiny() && W == 8) z.i |= (size_t)APPEND_COOKIE << 48;
  *(size_t *)(p + z.n - W) = z.i;
  return l;
}
