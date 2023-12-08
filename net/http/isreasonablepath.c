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
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "net/http/http.h"

/**
 * Returns true if path doesn't contain "." or ".." segments.
 *
 * @param size if -1 implies strlen
 * @see IsAcceptablePath()
 */
bool IsReasonablePath(const char *data, size_t size) {
  const char *p, *e;
  int x, y, z, a, b, i, n;
  if (size == -1) size = data ? strlen(data) : 0;
  z = '/';
  y = '/';
  x = '/';
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
    if (z == '/' && y == '.' && x == '/') return false;
    if (z == '/' && y == '.' && x == '.') return false;
    z = y;
    y = x;
  }
  if (y == '/' && x == '.') return false;
  if (z == '/' && y == '.' && x == '.') return false;
  return true;
}
