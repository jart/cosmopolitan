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
#include "libc/intrin/bsr.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"
#include "libc/str/str.h"

#define W sizeof(size_t)

/**
 * Resets length of append buffer, e.g.
 *
 *     char *b = 0;
 *     appends(&b, "hello");
 *     appendr(&b, 1);
 *     assert(!strcmp(b, "h"));
 *     appendr(&b, 0);
 *     assert(!strcmp(b, ""));
 *     free(b);
 *
 * If `i` is greater than the current length then the extra bytes are
 * filled with NUL characters. If `i` is less than the current length
 * then memory is released to the system.
 *
 * The resulting buffer is guaranteed to be NUL-terminated, i.e.
 * `!b[appendz(b).i]` will be the case even if both params are 0.
 *
 * @return `i` or -1 if `ENOMEM`
 * @see appendz(b).i to get buffer length
 */
ssize_t appendr(char **b, size_t i) {
  char *p;
  size_t n;
  struct appendz z;
  unassert(b);
  z = appendz((p = *b));
  if (i != z.i || !p) {
    n = ROUNDUP(i + 1, 8) + W;
    if (n > z.n || _bsrl(n) < _bsrl(z.n)) {
      if ((p = realloc(p, n))) {
        z.n = malloc_usable_size(p);
        unassert(!(z.n & (W - 1)));
        *b = p;
      } else {
        return -1;
      }
    }
    if (i > z.i) {
      bzero(p + z.i, i - z.i + 1);
    } else {
      p[i] = 0;
    }
    *(size_t *)(p + z.n - W) =
        i | (!IsTiny() && W == 8 ? (size_t)APPEND_COOKIE << 48 : 0);
  }
  return i;
}
