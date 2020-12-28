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

#define N 32
typedef uint8_t uint8_v _Vector_size(N);

/**
 * Searches for last instance of character in memory region.
 *
 * @param s is binary data to search
 * @param c is treated as unsigned char
 * @param n is byte length of s
 * @return address of last c in s, or NULL if not found
 */
void *memrchr(const void *s, int c, size_t n) {
  unsigned char ch = (unsigned char)c;
  const unsigned char *p = (const unsigned char *)s;
  if (n >= 32 && CheckAvx2()) {
    uint8_v cv;
    __builtin_memset(&cv, ch, sizeof(cv));
    do {
      uint32_t skip;
      uint8_v sv, tv;
      memcpy(&sv, s + n - N, N);
      asm("vpcmpeqb\t%2,%3,%1\n\t"
          "vpmovmskb\t%1,%0\n\t"
          "lzcnt\t%0,%0"
          : "=r"(skip), "=x"(tv)
          : "x"(sv), "x"(cv));
      n -= skip;
      if (skip != 32) break;
    } while (n >= 32);
  }
  while (n--) {
    if (p[n] == ch) return (/* unconst */ void *)&p[n];
  }
  return NULL;
}
