/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/pcmpeqb.h"
#include "libc/intrin/pmovmskb.h"
#include "libc/nexgen32e/bsf.h"
#include "libc/str/str.h"

/**
 * Compares memory.
 *
 * @return unsigned char subtraction at stop index
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  int c;
  size_t i;
  unsigned m;
  uint8_t *p1, *p2;
  uint8_t v1[16], v2[16];
  if (n) {
    for (p1 = a, p2 = b, i = 0; i + 16 <= n; i += 16) {
      memcpy(v1, p1 + i, 16);
      memcpy(v2, p2 + i, 16);
      pcmpeqb(v1, v1, v2);
      if ((m = pmovmskb(v1) - 0xffff)) {
        i += bsf(m);
        return p1[i] - p2[i];
      }
    }
    for (; i < n; ++i) {
      if ((c = p1[i] - p2[i])) {
        return c;
      }
    }
  }
  return 0;
}
