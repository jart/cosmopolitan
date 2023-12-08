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
#include "libc/stdio/rand.h"

static uint64_t g_vigna;

/**
 * Returns deterministic pseudorandom data, e.g.
 *
 *     uint64_t x = vigna();
 *
 * You can generate different types of numbers as follows:
 *
 *     int64_t x = vigna() >> 1;    // make positive signed integer
 *     double x = _real1(vigna());  // make float on [0,1]-interval
 *
 * You can seed this random number generator using:
 *
 *     svigna(rdseed());
 *
 * You may use the reentrant version of this function:
 *
 *     static uint64_t s = 0;
 *     uint64_t x = svigna_r(&s);
 *
 * If you want to fill a buffer with data then rngset() implements
 * vigna's algorithm to do that extremely well:
 *
 *     char buf[4096];
 *     rngset(buf, sizeof(buf), vigna, 0);
 *
 * If you want a fast pseudorandom number generator that seeds itself
 * automatically on startup and fork(), then consider _rand64. If you
 * want true random data then consider rdseed, rdrand, and getrandom.
 *
 * @return 64 bits of pseudorandom data
 * @note this function is not intended for cryptography
 * @note this function passes bigcrush and practrand
 * @note this function takes at minimum 4 cycles
 */
uint64_t vigna(void) {
  return vigna_r(&g_vigna);
}

/**
 * Returns pseudorandom data.
 * @see vigna() for easy api
 */
uint64_t vigna_r(uint64_t state[hasatleast 1]) {
  uint64_t z = (state[0] += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

/**
 * Seeds vigna() pseudorandom number generator.
 * @see vigna(), vigna_r()
 */
void svigna(uint64_t seed) {
  g_vigna = seed;
}
