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
#include "libc/str/str.h"

/**
 * Returns pointer to first instance of character.
 *
 * @param m is memory to search
 * @param c is search byte which is masked with 255
 * @param n is byte length of p
 * @return is pointer to first instance of c or NULL if not found
 * @asyncsignalsafe
 */
void *memchr(const void *m, int c, size_t n) {
  uint64_t v, w;
  const char *p, *pe;
  c &= 255;
  v = 0x0101010101010101ul * c;
  for (p = m, pe = p + n; p + 8 <= pe; p += 8) {
    w = (uint64_t)(255 & p[7]) << 070 | (uint64_t)(255 & p[6]) << 060 |
        (uint64_t)(255 & p[5]) << 050 | (uint64_t)(255 & p[4]) << 040 |
        (uint64_t)(255 & p[3]) << 030 | (uint64_t)(255 & p[2]) << 020 |
        (uint64_t)(255 & p[1]) << 010 | (uint64_t)(255 & p[0]) << 000;
    if ((w = ~(w ^ v) & ((w ^ v) - 0x0101010101010101) & 0x8080808080808080)) {
      return p + ((unsigned)__builtin_ctzll(w) >> 3);
    }
  }
  for (; p < pe; ++p) {
    if ((*p & 255) == c) {
      return p;
    }
  }
  return NULL;
}
