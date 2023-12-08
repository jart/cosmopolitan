/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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

/**
 * Transcodes UTF-32 to UTF-8.
 *
 * @param p is input value
 * @param n if -1 implies wcslen
 * @param z if non-NULL receives output length
 */
char *utf32to8(const wchar_t *p, size_t n, size_t *z) {
  size_t i;
  wint_t x;
  uint64_t w;
  char *r, *q;
  if (z) *z = 0;
  if (n == -1) n = p ? wcslen(p) : 0;
  if ((q = r = malloc(n * 6 + 1))) {
    for (i = 0; i < n; ++i) {
      x = p[i];
      w = tpenc(x);
      do {
        *q++ = w;
      } while ((w >>= 8));
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
