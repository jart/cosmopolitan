/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bsr.h"
#include "libc/macros.internal.h"

/**
 * Fingers IP+TCP SYN packet.
 *
 * This returns a hash-like magic number that reflects the SYN packet
 * structure, e.g. ordering of options, maximum segment size, etc.
 */
uint32_t FingerSyn(const char *p, size_t n) {
  uint32_t h = 0;
  int i, j, k, iplen, tcplen, ttl;
  if (n >= 20 + 20 && n >= (iplen = (p[0] & 0x0F) * 4) + 20 &&
      n >= iplen + (tcplen = ((p[iplen + 12] & 0xF0) >> 4) * 4)) {
    n = iplen + tcplen;
    // Time to Live
    // ttl<=256 Crisco, Solaris 6
    // ttl<=128 Windows, OpenVMS 8+
    // ttl<=64  Mac, Linux, BSD, Solaris 8+, Tru64, HP-UX
    ttl = p[8] & 255;
    h += _bsr(MAX(1, ttl - 1));
    h *= 0x9e3779b1;
    // TCP Options
    // We care about the order and presence of leading common options.
    for (j = 0, i = iplen + 20; i < n; ++j) {
      k = p[i] & 255;
      if (k == 0 || k == 1 || k == 2 || k == 3 || k == 4 || k == 8) {
        if (k <= 1) {
          ++i;
        } else if (i + 1 < n) {
          i += p[i + 1] & 255;
        } else {
          break;
        }
      } else {
        break;
      }
      h += j << 8 | k;
      h *= 0x9e3779b1;
    }
    if (!h) {
      ++h;
    }
  }
  return h;
}
