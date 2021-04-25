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
#include "libc/errno.h"
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "net/http/escape.h"

/**
 * Returns true if C0 or C1 control codes are present
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param f can have kControlWs, kControlC0, kControlC1 to forbid
 * @return true if forbidden characters were found
 * @see VisualizeControlCodes()
 */
bool HasControlCodes(const char *p, size_t n, int f) {
  int c;
  wint_t x, a, b;
  size_t i, j, m;
  if (n == -1) n = p ? strlen(p) : 0;
  for (i = 0; i < n;) {
    x = p[i++] & 0xff;
    if (x >= 0300) {
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
    if (((f & kControlC1) && 0x80 <= x && x < 0xA0) ||
        ((f & kControlC0) && (x < 32 || x == 0x7F) &&
         !(x == '\t' || x == '\r' || x == '\n' || x == '\v')) ||
        ((f & kControlWs) &&
         (x == '\t' || x == '\r' || x == '\n' || x == '\v'))) {
      return true;
    }
  }
  return false;
}
