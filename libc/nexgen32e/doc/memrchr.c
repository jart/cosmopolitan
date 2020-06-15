/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/str/str.h"

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
