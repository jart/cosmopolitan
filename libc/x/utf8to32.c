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
#include "libc/intrin/pcmpgtb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/intrin/punpckhbw.h"
#include "libc/intrin/punpckhwd.h"
#include "libc/intrin/punpcklbw.h"
#include "libc/intrin/punpcklwd.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/thompike.h"
#include "libc/str/utf16.h"
#include "libc/x/x.h"

/**
 * Transcodes UTF-8 to UTF-32.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 */
wchar_t *utf8to32(const char *p, size_t n, size_t *z) {
  size_t i;
  unsigned m, j;
  wint_t x, a, b;
  wchar_t *r, *q;
  uint8_t v1[16], v2[16], v3[16], v4[16], vz[16];
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * sizeof(wchar_t) + sizeof(wchar_t)))) {
    for (i = 0; i < n;) {
      if (!((uintptr_t)(p + i) & 15) && i + 16 < n) {
        /* 10x speedup for ascii */
        bzero(vz, 16);
        do {
          memcpy(v1, p + i, 16);
          pcmpgtb((int8_t *)v2, (int8_t *)v1, (int8_t *)vz);
          if (pmovmskb(v2) != 0xFFFF) break;
          punpcklbw(v3, v1, vz);
          punpckhbw(v1, v1, vz);
          punpcklwd((void *)v4, (void *)v3, (void *)vz);
          punpckhwd((void *)v3, (void *)v3, (void *)vz);
          punpcklwd((void *)v2, (void *)v1, (void *)vz);
          punpckhwd((void *)v1, (void *)v1, (void *)vz);
          memcpy(q + 0, v4, 16);
          memcpy(q + 4, v3, 16);
          memcpy(q + 8, v2, 16);
          memcpy(q + 12, v1, 16);
          i += 16;
          q += 16;
        } while (i + 16 < n);
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
      *q++ = x;
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, (q - r) * sizeof(wchar_t)))) r = q;
  }
  return r;
}
