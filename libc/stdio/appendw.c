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
#include "libc/serialize.h"
#include "libc/intrin/bsr.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/append.h"

#define W sizeof(size_t)

/**
 * Appends character or word-encoded string to buffer.
 *
 * Up to eight characters can be appended. For example:
 *
 *     appendw(&s, 'h'|'i'<<8);
 *     appendw(&s, READ64LE("hi\0\0\0\0\0"));
 *
 * Are equivalent to:
 *
 *     appends(&s, "hi");
 *
 * The special case:
 *
 *     appendw(&s, 0);
 *
 * Will append a single NUL character.
 *
 * This function is slightly faster than appendd() and appends(). Its
 * main advantage is it improves code size in functions that call it.
 *
 * The resulting buffer is guaranteed to be NUL-terminated, i.e.
 * `!b[appendz(b).i]` will be the case.
 *
 * @return bytes appended or -1 if `ENOMEM`
 * @see appendz(b).i to get buffer length
 */
ssize_t appendw(char **b, uint64_t w) {
  size_t n;
  unsigned l;
  char *p, *q;
  struct appendz z;
  z = appendz((p = *b));
  l = w ? (_bsrl(w) >> 3) + 1 : 1;
  n = ROUNDUP(z.i + 8 + 1, 8) + W;
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
  q = p + z.i;
  WRITE64LE(q, w);
  q[8] = 0;
  z.i += l;
  if (!IsTiny() && W == 8) z.i |= (size_t)APPEND_COOKIE << 48;
  *(size_t *)(p + z.n - W) = z.i;
  return l;
}
