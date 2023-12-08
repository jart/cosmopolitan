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
#include "net/http/escape.h"

/**
 * Decodes ISO-8859-1 to UTF-8.
 *
 * @param p is input value
 * @param n if -1 implies strlen
 * @param z if non-NULL receives output length
 * @return allocated NUL-terminated buffer, or NULL w/ errno
 */
char *DecodeLatin1(const char *p, size_t n, size_t *z) {
  int c;
  size_t i;
  char *r, *q;
  int8_t v1[16], v2[16], vz[16];
  if (z) *z = 0;
  if (n == -1) n = p ? strlen(p) : 0;
  if ((q = r = malloc(n * 2 + 1))) {
    for (i = 0; i < n;) {
      bzero(vz, 16); /* 3x speedup for ASCII */
      while (i + 16 < n) {
        memcpy(v1, p + i, 16);
        pcmpgtb(v2, v1, vz);
        if (pmovmskb((void *)v2) != 0xFFFF) break;
        memcpy(q, v1, 16);
        q += 16;
        i += 16;
      }
      c = p[i++] & 0xff;
      if (c < 0200) {
        *q++ = c;
      } else {
        *q++ = 0300 | c >> 6;
        *q++ = 0200 | (c & 077);
      }
    }
    if (z) *z = q - r;
    *q++ = '\0';
    if ((q = realloc(r, q - r))) r = q;
  }
  return r;
}
