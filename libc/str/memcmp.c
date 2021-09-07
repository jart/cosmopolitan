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
#include "libc/dce.h"
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
  uint64_t w;
  unsigned m;
  uint8_t A[16], B[16];
  const uint8_t *p = a, *q = b;
  if (p == q) return 0;
  if (IsTiny()) {
    for (; n >= 8; p += 8, q += 8, n -= 8) {
      w = READ64LE(p) ^ READ64LE(q);
      if (w) {
        m = bsfl(w) >> 3;
        return p[m] - q[m];
      }
    }
    for (; n; ++p, ++q, --n) {
      if ((c = *p - *q)) {
        return c;
      }
    }
    return 0;
  }
StartOver:
  switch (n) {
    case 0:
      return 0;
    case 1:
      return *p - *q;
    case 2:
      w = (p[0] << 000 | p[1] << 010) ^ (q[0] << 000 | q[1] << 010);
      break;
    case 3:
      w = (p[0] << 000 | p[1] << 010 | p[2] << 020) ^
          (q[0] << 000 | q[1] << 010 | q[2] << 020);
      break;
    case 4:
      w = ((uint32_t)p[0] << 000 | (uint32_t)p[1] << 010 |
           (uint32_t)p[2] << 020 | (uint32_t)p[3] << 030) ^
          ((uint32_t)q[0] << 000 | (uint32_t)q[1] << 010 |
           (uint32_t)q[2] << 020 | (uint32_t)q[3] << 030);
      break;
    case 5:
      w = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
           (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
           (uint64_t)p[4] << 040) ^
          ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
           (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
           (uint64_t)q[4] << 040);
      break;
    case 6:
      w = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
           (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
           (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050) ^
          ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
           (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
           (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050);
      break;
    case 7:
      w = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
           (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
           (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
           (uint64_t)p[6] << 060) ^
          ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
           (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
           (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050 |
           (uint64_t)q[6] << 060);
      break;
    case 8:
      w = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
           (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
           (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
           (uint64_t)p[6] << 060 | (uint64_t)p[7] << 070) ^
          ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
           (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
           (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050 |
           (uint64_t)q[6] << 060 | (uint64_t)q[7] << 070);
      break;
    default:
      for (; n >= 16; p += 16, q += 16, n -= 16) {
        memcpy(A, p, 16);
        memcpy(B, q, 16);
        pcmpeqb(A, A, B);
        if ((m = pmovmskb(A) - 0xffff)) {
          m = bsf(m);
          return p[m] - q[m];
        }
      }
      if (n > 8) {
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
          w = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
               (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
               (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
               (uint64_t)p[6] << 060 | (uint64_t)p[7] << 070) ^
              ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
               (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
               (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050 |
               (uint64_t)q[6] << 060 | (uint64_t)q[7] << 070);
          if (w) goto ItsDifferent;
          p += 8;
          q += 8;
          n -= 8;
      }
      goto StartOver;
  }
  if (!w) return 0;
ItsDifferent:
  m = bsfl(w) >> 3;
  return p[m] - q[m];
}
