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
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"

/**
 * Fills memory with random bytes, e.g.
 *
 *     char buf[512];
 *     rngset(buf, sizeof(buf), 0, 0);
 *
 * If reseed is zero then the internal PRNG is disabled and bytes are
 * simply copied in little-endian order from the seed function. If seed
 * is NULL then the reseed parameter is used as the seed value for the
 * internal PRNG. If seed!=NULL and reseed>8 then reseed is the number
 * of bytes after which the seed() function should be called again, to
 * freshen up the PRNG.
 *
 * The main advantage of this generator is that it produces data at 13
 * gigabytes per second since Vigna's Algorithm vectorizes better than
 * alternatives, going even faster than xorshift.
 *
 * @return original buf
 */
dontasan void *rngset(void *b, size_t n, uint64_t seed(void), size_t reseed) {
  size_t m;
  uint64_t x, t = 0;
  unsigned char *p = b;
  if (!seed) {
    t = reseed;
    reseed = -1;
  } else if (reseed < 8) {
    reseed = 8;
  }
  while (n) {
    if (seed) t = seed();
    if (!seed || reseed > 8) {
      n -= (m = reseed < n ? reseed : n);
      while (m >= 8) {
        x = (t += 0x9e3779b97f4a7c15);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        x = (x ^ (x >> 31));
        __builtin_memcpy(p, &x, 8);
        p += 8;
        m -= 8;
      }
      while (m--) {
        *p++ = t;
        t >>= 8;
      }
    } else if (n >= 8) {
      p[0] = (0x00000000000000FF & t) >> 000;
      p[1] = (0x000000000000FF00 & t) >> 010;
      p[2] = (0x0000000000FF0000 & t) >> 020;
      p[3] = (0x00000000FF000000 & t) >> 030;
      p[4] = (0x000000FF00000000 & t) >> 040;
      p[5] = (0x0000FF0000000000 & t) >> 050;
      p[6] = (0x00FF000000000000 & t) >> 060;
      p[7] = (0xFF00000000000000 & t) >> 070;
      p += 8;
      n -= 8;
    } else {
      while (n) {
        *p++ = t;
        t >>= 8;
        --n;
      }
    }
  }
  return b;
}
