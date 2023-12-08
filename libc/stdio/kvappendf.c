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
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"

#define W sizeof(size_t)

/**
 * Appends formatted string to buffer.
 *
 * This is an alternative to vappendf() that uses the kprintf()
 * formatting facility. This has some advantages in terms of
 * performance, code size, and memory safety. There's also
 * disadvantages, such as lack of floating-point directives.
 *
 * @see kprintf()
 */
ssize_t kvappendf(char **b, const char *f, va_list v) {
  char *p;
  int r, s;
  size_t n;
  va_list w;
  struct appendz z;
  z = appendz((p = *b));
  va_copy(w, v);
  if ((r = kvsnprintf(p + z.i, z.n ? z.n - W - z.i : 0, f, v)) >= 0) {
    n = ROUNDUP(z.i + r + 1, 8) + W;
    if (n > z.n) {
      if (!z.n) z.n = W * 2;
      while (n > z.n) z.n += z.n >> 1;
      z.n = ROUNDUP(z.n, W);
      if ((p = realloc(p, z.n))) {
        z.n = malloc_usable_size(p);
        unassert(!(z.n & (W - 1)));
        s = kvsnprintf(p + z.i, z.n - W - z.i, f, w);
        unassert(s == r);
        *b = p;
      } else {
        va_end(w);
        return -1;
      }
    }
    z.i += r;
    if (!IsTiny() && W == 8) z.i |= (size_t)APPEND_COOKIE << 48;
    *(size_t *)(p + z.n - W) = z.i;
  }
  va_end(w);
  return r;
}
