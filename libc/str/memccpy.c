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
#include "libc/bits/bits.h"
#include "libc/str/str.h"

static inline noasan uint64_t UncheckedAlignedRead64(unsigned char *p) {
  return (uint64_t)(255 & p[7]) << 070 | (uint64_t)(255 & p[6]) << 060 |
         (uint64_t)(255 & p[5]) << 050 | (uint64_t)(255 & p[4]) << 040 |
         (uint64_t)(255 & p[3]) << 030 | (uint64_t)(255 & p[2]) << 020 |
         (uint64_t)(255 & p[1]) << 010 | (uint64_t)(255 & p[0]) << 000;
}

/**
 * Copies at most N bytes from SRC to DST until 𝑐 is encountered.
 *
 * This is little-known C Standard Library approach, dating back to the
 * Fourth Edition of System Five, for copying a C strings to fixed-width
 * buffers, with added generality.
 *
 * For example, strictly:
 *
 *     char buf[16];
 *     CHECK_NOTNULL(memccpy(buf, s, '\0', sizeof(buf)));
 *
 * Or unstrictly:
 *
 *     if (!memccpy(buf, s, '\0', sizeof(buf))) strcpy(buf, "?");
 *
 * Are usually more sensible than the following:
 *
 *     char cstrbuf[16];
 *     snprintf(cstrbuf, sizeof(cstrbuf), "%s", CSTR);
 *
 * @param c is search character and is masked with 255
 * @return DST + idx(c) + 1, or NULL if 𝑐 ∉ 𝑠₀․․ₙ₋₁
 * @note DST and SRC can't overlap
 * @asyncsignalsafe
 */
void *memccpy(void *dst, const void *src, int c, size_t n) {
  size_t i;
  uint64_t v, w;
  unsigned char *d, *q;
  const unsigned char *s;
  i = 0;
  d = dst;
  s = src;
  c &= 255;
  v = 0x0101010101010101ul * c;
  for (; (uintptr_t)(s + i) & 7; ++i) {
    if (i == n) return NULL;
    if ((d[i] = s[i]) == c) return d + i + 1;
  }
  for (; i + 8 <= n; i += 8) {
    w = UncheckedAlignedRead64(s + i);
    if (~(w ^ v) & ((w ^ v) - 0x0101010101010101) & 0x8080808080808080) {
      break;
    } else {
      q = d + i;
      q[0] = (w & 0x00000000000000ff) >> 000;
      q[1] = (w & 0x000000000000ff00) >> 010;
      q[2] = (w & 0x0000000000ff0000) >> 020;
      q[3] = (w & 0x00000000ff000000) >> 030;
      q[4] = (w & 0x000000ff00000000) >> 040;
      q[5] = (w & 0x0000ff0000000000) >> 050;
      q[6] = (w & 0x00ff000000000000) >> 060;
      q[7] = (w & 0xff00000000000000) >> 070;
    }
  }
  for (; i < n; ++i) {
    if ((d[i] = s[i]) == c) {
      return d + i + 1;
    }
  }
  return NULL;
}
