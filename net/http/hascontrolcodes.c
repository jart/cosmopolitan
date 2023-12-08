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
#include "net/http/escape.h"

/**
 * Returns true if C0 or C1 control codes are present
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param f can have kControlWs, kControlC0, kControlC1 to forbid
 * @return index of first forbidden character or -1
 * @see VisualizeControlCodes()
 */
ssize_t HasControlCodes(const char *p, size_t n, int f) {
  char t[256];
  wint_t x, a, b;
  size_t i, j, m, g;
  bzero(t, sizeof(t));
  if (f & kControlC0) memset(t + 0x00, 1, 0x20 - 0x00), t[0x7F] = 1;
  if (f & kControlC1) memset(t + 0x80, 1, 0xA0 - 0x80);
  t['\t'] = t['\r'] = t['\n'] = t['\v'] = !!(f & kControlWs);
  if (n == -1) n = p ? strlen(p) : 0;
  for (i = 0; i < n;) {
    g = i;
    x = p[i++] & 0xff;
    if (UNLIKELY(x >= 0300)) {
      a = ThomPikeByte(x);
      m = ThomPikeLen(x) - 1;
      if (i + m <= n) {
        for (j = 0;;) {
          b = p[i + j] & 0xff;
          if (!ThomPikeCont(b)) break;
          a = ThomPikeMerge(a, b);
          if (++j == m) {
            x = a;
            i += j;
            break;
          }
        }
      }
    }
    if (x < 256 && t[x]) {
      return g;
    }
  }
  return -1;
}
