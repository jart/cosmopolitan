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
#include "libc/str/str.h"
#include "libc/x/x.h"

/**
 * Concatenates strings / chars to newly allocated memory, e.g.
 *
 *     xstrcat("hi", ' ', "there")
 *
 * Or without the C99 helper macro:
 *
 *     (xstrcat)("hi", ' ', "there", NULL)
 *
 * This goes twice as fast as the more powerful xasprintf(). It's not
 * quadratic like strcat(). It's much slower than high-effort stpcpy(),
 * particularly with string literals.
 *
 * @see gc()
 */
char *(xstrcat)(const char *s, ...) {
  va_list va;
  intptr_t q;
  char *p, b[2];
  size_t n, m, c;
  n = 0;
  c = 32;
  p = xmalloc(c);
  va_start(va, s);
  do {
    q = (intptr_t)s;
    if (q > 0 && q <= 255) {
      b[0] = q;
      b[1] = '\0';
      s = b;
      m = 1;
    } else {
      m = strlen(s);
    }
    if (n + m + 1 > c) {
      do {
        c += c >> 1;
      } while (n + m + 1 > c);
      p = xrealloc(p, c);
    }
    memcpy(p + n, s, m + 1);
    n += m;
  } while ((s = va_arg(va, const char *)));
  va_end(va);
  return p;
}
