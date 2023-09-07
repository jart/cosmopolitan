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
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

/**
 * Compares memory byte by byte.
 *
 *     memcmp n=0                           2 nanoseconds
 *     memcmp n=1                           2 ns/byte            357 mb/s
 *     memcmp n=2                           1 ns/byte            530 mb/s
 *     memcmp n=3                           1 ns/byte            631 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=4                           1 ns/byte            849 mb/s
 *     memcmp n=5                         816 ps/byte          1,195 mb/s
 *     memcmp n=6                         888 ps/byte          1,098 mb/s
 *     memcmp n=7                         829 ps/byte          1,176 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=8                         773 ps/byte          1,261 mb/s
 *     memcmp n=9                         629 ps/byte          1,551 mb/s
 *     memcmp n=15                        540 ps/byte          1,805 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=16                        211 ps/byte          4,623 mb/s
 *     memcmp n=17                        268 ps/byte          3,633 mb/s
 *     memcmp n=31                        277 ps/byte          3,524 mb/s
 *     memcmp n=32                        153 ps/byte          6,351 mb/s
 *     memcmp n=33                        179 ps/byte          5,431 mb/s
 *     memcmp n=79                        148 ps/byte          6,576 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=80                         81 ps/byte             11 GB/s
 *     memcmp n=128                        76 ps/byte             12 GB/s
 *     memcmp n=256                        60 ps/byte             15 GB/s
 *     memcmp n=16384                      51 ps/byte             18 GB/s
 *     memcmp n=32768                      51 ps/byte             18 GB/s
 *     memcmp n=131072                     52 ps/byte             18 GB/s
 *
 * @return an integer that's (1) equal to zero if `a` is equal to `b`,
 *     (2) less than zero if `a` is less than `b`, or (3) greater than
 *     zero if `a` is greater than `b`
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  int c;
  const unsigned char *p, *q;
  if ((p = a) == (q = b) || !n) return 0;
  if ((c = *p - *q)) return c;
#if defined(__x86_64__) && !defined(__chibicc__)
  unsigned u;
  while (n >= 16 && (((uintptr_t)p & 0xfff) <= 0x1000 - 16 &&
                     ((uintptr_t)q & 0xfff) <= 0x1000 - 16)) {
    if (!(u = __builtin_ia32_pmovmskb128(*(xmm_t *)p == *(xmm_t *)q) ^
              0xffff)) {
      n -= 16;
      p += 16;
      q += 16;
    } else {
      u = __builtin_ctzl(u);
      return p[u] - q[u];
    }
  }
#endif /* __x86_64__ */
  for (; n; ++p, ++q, --n) {
    if ((c = *p - *q)) {
      return c;
    }
  }
  return 0;
}
