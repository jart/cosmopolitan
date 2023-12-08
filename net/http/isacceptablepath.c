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
#include "libc/intrin/likely.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "net/http/http.h"

/**
 * Returns true if path seems legit.
 *
 * 1. The substring "//" is disallowed.
 * 2. We won't serve hidden files (segment starts with '.').
 *    The only exception is `/.well-known/`.
 * 3. We won't serve paths with segments equal to "." or "..".
 *
 * It is assumed that the URI parser already took care of percent
 * escape decoding as well as ISO-8859-1 decoding. The input needs
 * to be a UTF-8 string. This function takes overlong encodings into
 * consideration, so you don't need to call Underlong() beforehand.
 *
 * @param size if -1 implies strlen
 * @see IsReasonablePath()
 */
bool IsAcceptablePath(const char *data, size_t size) {
  const char *p, *e;
  int x, y, a, b, t, i, n;
  if (size == -1) size = data ? strlen(data) : 0;
  t = 0;
  y = '/';
  p = data;
  e = p + size;
  while (p < e) {
    x = *p++ & 0xff;
    if (UNLIKELY(x >= 0300)) {
      a = ThomPikeByte(x);
      n = ThomPikeLen(x) - 1;
      if (p + n <= e) {
        for (i = 0;;) {
          b = p[i] & 0xff;
          if (!ThomPikeCont(b)) break;
          a = ThomPikeMerge(a, b);
          if (++i == n) {
            x = a;
            p += i;
            break;
          }
        }
      }
    }
    if (x == '\\') {
      x = '/';
    }
    if (y == '/') {
      if (x == '.' &&  // allow /.well-known/ in the first position
         (p - data > 2 ||
          size < 13 ||
          memcmp(data, "/.well-known/", 13) != 0)) return false;
      if (x == '/' && t) return false;
    }
    y = x;
    t = 1;
  }
  return true;
}
