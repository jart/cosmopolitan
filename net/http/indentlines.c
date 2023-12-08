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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

/**
 * Inserts spaces before lines.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @param j is number of spaces to use
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *IndentLines(const char *p, size_t n, size_t *z, size_t j) {
  char *r, *q;
  const char *l;
  size_t t, m, a;
  if (n == -1) n = p ? strlen(p) : 0;
  r = 0;
  t = 0;
  do {
    if ((l = memchr(p, '\n', n))) {
      m = l + 1 - p;
      a = *p != '\r' && *p != '\n' ? j : 0;
    } else {
      m = n;
      a = n ? j : 0;
    }
    if ((q = realloc(r, t + a + m + 1))) {
      r = q;
    } else {
      free(r);
      if (z) *z = 0;
      return 0;
    }
    memset(r + t, ' ', a);
    memcpy(r + t + a, p, m);
    t += a + m;
    p += m;
    n -= m;
  } while (l);
  if (z) *z = t;
  r[t] = '\0';
  return r;
}
