/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/str/tab.internal.h"

/**
 * Compares memory case-insensitively.
 *
 *     memcasecmp n=0                     992 picoseconds
 *     memcasecmp n=1                       1 ns/byte            590 mb/s
 *     memcasecmp n=2                       1 ns/byte            843 mb/s
 *     memcasecmp n=3                       1 ns/byte            885 mb/s
 *     memcasecmp n=4                       1 ns/byte            843 mb/s
 *     memcasecmp n=5                       1 ns/byte            820 mb/s
 *     memcasecmp n=6                       1 ns/byte            770 mb/s
 *     memcasecmp n=7                       1 ns/byte            765 mb/s
 *     memcasecmp n=8                     206 ps/byte          4,724 mb/s
 *     memcasecmp n=9                     220 ps/byte          4,428 mb/s
 *     memcasecmp n=15                    617 ps/byte          1,581 mb/s
 *     memcasecmp n=16                    124 ps/byte          7,873 mb/s
 *     memcasecmp n=17                    155 ps/byte          6,274 mb/s
 *     memcasecmp n=31                    341 ps/byte          2,860 mb/s
 *     memcasecmp n=32                     82 ps/byte         11,810 mb/s
 *     memcasecmp n=33                    100 ps/byte          9,743 mb/s
 *     memcasecmp n=80                     53 ps/byte         18,169 mb/s
 *     memcasecmp n=128                    49 ps/byte         19,890 mb/s
 *     memcasecmp n=256                    45 ps/byte         21,595 mb/s
 *     memcasecmp n=16384                  42 ps/byte         22,721 mb/s
 *     memcasecmp n=32768                  40 ps/byte         24,266 mb/s
 *     memcasecmp n=131072                 40 ps/byte         24,337 mb/s
 *
 * @return is <0, 0, or >0 based on uint8_t comparison
 */
int memcasecmp(const void *p, const void *q, size_t n) {
  int c;
  size_t i;
  unsigned u;
  uint64_t w;
  const unsigned char *a, *b;
  if ((a = p) != (b = q)) {
    for (i = 0; i < n; ++i) {
      while (i + 8 <= n) {
        if ((w = (READ64LE(a) ^ READ64LE(b)))) {
          u = __builtin_ctzll(w);
          i += u >> 3;
          break;
        } else {
          i += 8;
        }
      }
      if (i == n) {
        break;
      } else if ((c = kToLower[a[i]] - kToLower[b[i]])) {
        return c;
      }
    }
  }
  return 0;
}
