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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "net/http/escape.h"

/**
 * Canonicalizes overlong Thompson-Pike encodings.
 *
 * Please note that the IETF banned these numbers, so if you want to
 * enforce their ban you can simply strcmp() the result to check for
 * the existence of banned numbers.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *Underlong(const char *p, size_t n, size_t *z) {
  uint64_t w;
  char *r, *q;
  size_t i, j, m;
  wint_t x, a, b;
  int8_t v1[16], v2[16], vz[16];
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * 2 + 1))) {
    for (i = 0; i < n;) {
      bzero(vz, 16); /* 50x speedup for ASCII */
      while (i + 16 < n) {
        memcpy(v1, p + i, 16);
        pcmpgtb(v2, v1, vz);
        if (pmovmskb((void *)v2) != 0xFFFF) break;
        memcpy(q, v1, 16);
        q += 16;
        i += 16;
      }
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
