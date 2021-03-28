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
#include "libc/str/thompike.h"
#include "net/http/http.h"

/**
 * Returns true if request path seems legit.
 *
 * 1. Request path must start with '/'.
 * 2. The substring "//" is disallowed.
 * 3. We won't serve hidden files (segment starts with '.').
 * 4. We won't serve paths with segments equal to "." or "..".
 *
 * It is assumed that the URI parser already took care of percent
 * escape decoding as well as ISO-8859-1 decoding. The input needs
 * to be a UTF-8 string.
 */
bool IsAcceptableHttpRequestPath(const char *data, size_t size) {
  bool t;
  size_t i;
  unsigned n;
  wint_t x, y, a, b;
  const char *p, *e;
  if (!size || *data != '/') return false;
  t = 0;
  p = data;
  e = p + size;
  while (p < e) {
    x = *p++ & 0xff;
    if (x >= 0300) {
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
    if (!t) {
      t = true;
    } else {
      if ((x == '/' || x == '.') && y == '/') {
        return false;
      }
    }
    y = x;
  }
  return true;
}
