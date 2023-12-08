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
#include "libc/fmt/itoa.h"
#include "libc/serialize.h"
#include "net/finger/finger.h"

/**
 * Describes IP+TCP SYN packet
 *
 * The layout looks as follows:
 *
 *     TTL:OPTIONS:WSIZE:MSS
 *
 * The `TTL`, `WSIZE`, and `MSS` fields are unsigned decimal fields.
 *
 * The `OPTIONS` field communicates the ordering of the commonly used
 * subset of tcp options. The following character mappings are defined.
 * TCP options not on this list will be ignored.
 *
 *   - E: End of Option list
 *   - N: No-Operation
 *   - M: Maxmimum Segment Size
 *   - K: Window Scale
 *   - O: SACK Permitted
 *   - A: SACK
 *   - e: Echo (obsolete)
 *   - r: Echo reply (obsolete)
 *   - T: Timestamps
 *
 * @param q receives nul-terminated string
 * @param m is byte capacity of `q`
 * @param p is syn packet
 * @param n is byte length of `p`
 * @return pointer past last written byte in `q` or null if not a syn packet
 * @see https://www.iana.org/assignments/tcp-parameters/tcp-parameters.xhtml
 */
char *DescribeSyn(char *q, size_t m, const char *p, size_t n) {
  char *e;
  int i, j, k, iplen, tcplen, ttl, wsize, mss;
  if (n >= 20 + 20 && n >= (iplen = (p[0] & 0x0F) * 4) + 20 &&
      n >= iplen + (tcplen = ((p[iplen + 12] & 0xF0) >> 4) * 4)) {
    e = q + m;
    n = iplen + tcplen;
    // Time to Live
    // ttl<=256 Crisco, Solaris 6
    // ttl<=128 Windows, OpenVMS 8+
    // ttl<= 64 Mac, Linux, BSD, Solaris 8+, Tru64, HP-UX
    ttl = p[8] & 255;
    wsize = READ16BE(p + 14);
    if (q + 13 <= e) {
      q = FormatUint32(q, ttl);
      *q++ = ':';
    }
    // TCP Options
    // We care about the order and presence of leading common options.
    for (mss = j = 0, i = iplen + 20; i < n; ++j) {
      k = p[i] & 255;
      if (k < 9 && q + 1 < e) {
        *q++ = "ENMKOAerT"[k];
      }
      if (k == 2 && i + 4 <= n) {
        mss = READ16BE(p + 2);
      }
      if (k == 1 ||  // no-operation
          k == 2 ||  // maximum segment size
          k == 3 ||  // window scale
          k == 4 ||  // sack permitted
          k == 8) {  // timestamps
        if (k == 1) {
          // no-operation option has no length byte
          //
          //     ┌───┐
          //     │ 1 │
          //     └───┘
          //
          ++i;
        } else if (i + 1 < n) {
          // a normal tcp option should have
          //
          //     ┌──┬───┬───┐
          //     │OP│LEN│...│
          //     └──┴───┴───┘
          //
          // e.g. sack permitted is encoded as
          //
          //     ┌───┬───┐
          //     │ 4 │ 2 │
          //     └───┴───┘
          //
          // e.g. window scale of `7` would be encoded as
          //
          //     ┌───┬───┬───┐
          //     │ 3 │ 3 │ 7 │
          //     └───┴───┴───┘
          //
          i += p[i + 1] & 255;
        } else {
          break;
        }
      } else {
        break;
      }
    }
    if (q + (1 + 12) * 2 <= e) {
      *q++ = ':';
      q = FormatUint32(q, wsize);
      *q++ = ':';
      q = FormatUint32(q, mss);
    }
    if (q < e) {
      *q = 0;
    }
    return q;
  } else {
    return 0;
  }
}
