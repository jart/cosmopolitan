/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2014 Google Inc.                                                   │
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
#include "libc/limits.h"
#include "libc/str/str.h"

asm(".ident\t\"\\n\\n\
timingsafe_memcmp (ISC License)\\n\
Copyright 2014 Google Inc.\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * Lexicographically compares the first 𝑛 bytes in 𝑝 and 𝑞.
 *
 * The following expression:
 *
 *     timingsafe_memcmp(p, q, n)
 *
 * Is functionally equivalent to:
 *
 *     MAX(-1, MIN(1, memcmp(p, q, n)))
 *
 * Running time is independent of the byte sequences compared, making
 * this safe to use for comparing secret values such as cryptographic
 * MACs. In contrast, memcmp() may short-circuit after finding the first
 * differing byte.
 *
 *     timingsafe_memcmp n=0              661 picoseconds
 *     timingsafe_memcmp n=1                1 ns/byte            590 mb/s
 *     timingsafe_memcmp n=2                1 ns/byte            738 mb/s
 *     timingsafe_memcmp n=3                1 ns/byte            805 mb/s
 *     timingsafe_memcmp n=4                1 ns/byte            843 mb/s
 *     timingsafe_memcmp n=5                1 ns/byte            922 mb/s
 *     timingsafe_memcmp n=6                1 ns/byte            932 mb/s
 *     timingsafe_memcmp n=7                1 ns/byte            939 mb/s
 *     timingsafe_memcmp n=8              992 ps/byte            984 mb/s
 *     timingsafe_memcmp n=9              992 ps/byte            984 mb/s
 *     timingsafe_memcmp n=15             926 ps/byte          1,054 mb/s
 *     timingsafe_memcmp n=16             950 ps/byte          1,026 mb/s
 *     timingsafe_memcmp n=17             933 ps/byte          1,045 mb/s
 *     timingsafe_memcmp n=31             896 ps/byte          1,089 mb/s
 *     timingsafe_memcmp n=32             888 ps/byte          1,098 mb/s
 *     timingsafe_memcmp n=33             972 ps/byte          1,004 mb/s
 *     timingsafe_memcmp n=80             913 ps/byte          1,068 mb/s
 *     timingsafe_memcmp n=128            891 ps/byte          1,095 mb/s
 *     timingsafe_memcmp n=256            873 ps/byte          1,118 mb/s
 *     timingsafe_memcmp n=16384          858 ps/byte          1,138 mb/s
 *     timingsafe_memcmp n=32768          856 ps/byte          1,140 mb/s
 *     timingsafe_memcmp n=131072         857 ps/byte          1,138 mb/s
 *     bcmp ne n=256                        3 ps/byte            246 gb/s
 *     bcmp eq n=256                       32 ps/byte         30,233 mb/s
 *     memcmp ne n=256                      3 ps/byte            246 gb/s
 *     memcmp eq n=256                     31 ps/byte         31,493 mb/s
 *     timingsafe_bcmp ne n=256            27 ps/byte         35,992 mb/s
 *     timingsafe_bcmp eq n=256            27 ps/byte         35,992 mb/s
 *     timingsafe_memcmp ne n=256         877 ps/byte          1,113 mb/s
 *     timingsafe_memcmp eq n=256         883 ps/byte          1,105 mb/s
 *
 * @note each byte is interpreted as unsigned char
 * @return -1, 0, or 1 based on comparison
 * @see timingsafe_bcmp() it's 100x faster
 * @asyncsignalsafe
 */
int timingsafe_memcmp(const void *p, const void *q, size_t n) {
  const unsigned char *p1 = p, *p2 = q;
  size_t i;
  int res = 0, done = 0;
  for (i = 0; i < n; i++) {
    /* lt is -1 if p1[i] < p2[i]; else 0. */
    int lt = (p1[i] - p2[i]) >> CHAR_BIT;
    /* gt is -1 if p1[i] > p2[i]; else 0. */
    int gt = (p2[i] - p1[i]) >> CHAR_BIT;
    /* cmp is 1 if p1[i] > p2[i]; -1 if p1[i] < p2[i]; else 0. */
    int cmp = lt - gt;
    /* set res = cmp if !done. */
    res |= cmp & ~done;
    /* set done if p1[i] != p2[i]. */
    done |= lt | gt;
  }
  return res;
}
