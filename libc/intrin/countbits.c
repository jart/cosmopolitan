/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"

/**
 * Returns population count of array.
 *
 * @param a is byte sequence
 * @return number of bits set to one
 * @note 30gbps on Nehalem (Intel 2008+) otherwise 3gbps
 */
size_t _countbits(const void *a, size_t n) {
  size_t t;
  unsigned b;
  uint64_t x;
  const char *p, *e;
  t = 0;
  p = a;
  e = p + n;
  if (!IsTiny()) {
#ifdef __x86_64__
    long Ai, Bi, Ci, Di;
    long Ao, Bo, Co, Do;
    if (X86_HAVE(POPCNT)) {
      while (p + sizeof(long) * 4 <= e) {
        __builtin_memcpy(&Ai, p + 000, sizeof(long));
        __builtin_memcpy(&Bi, p + 010, sizeof(long));
        __builtin_memcpy(&Ci, p + 020, sizeof(long));
        __builtin_memcpy(&Di, p + 030, sizeof(long));
        asm("popcnt\t%1,%0" : "=r"(Ao) : "r"(Ai) : "cc");
        asm("popcnt\t%1,%0" : "=r"(Bo) : "r"(Bi) : "cc");
        asm("popcnt\t%1,%0" : "=r"(Co) : "r"(Ci) : "cc");
        asm("popcnt\t%1,%0" : "=r"(Do) : "r"(Di) : "cc");
        t += Ao + Bo + Co + Do;
        p += sizeof(long) * 4;
      }
      while (p + sizeof(long) <= e) {
        __builtin_memcpy(&Ai, p, 8);
        asm("popcnt\t%1,%0" : "=r"(Ao) : "rm"(Ai) : "cc");
        p += sizeof(long);
        t += Ao;
      }
    } else {
#endif
      while (p + 8 <= e) {
        __builtin_memcpy(&x, p, 8);
        x = x - ((x >> 1) & 0x5555555555555555);
        x = ((x >> 2) & 0x3333333333333333) + (x & 0x3333333333333333);
        x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0f;
        x = (x + (x >> 32)) & 0xffffffff;
        x = x + (x >> 16);
        x = (x + (x >> 8)) & 0x7f;
        t += x;
        p += 8;
      }
#ifdef __x86_64__
    }
#endif
  }
  while (p < e) {
    b = *p++ & 255;
    b = b - ((b >> 1) & 0x55);
    b = ((b >> 2) & 0x33) + (b & 0x33);
    b = (b + (b >> 4)) & 15;
    t += b;
  }
  return t;
}
