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
#include "libc/stdio/rand.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/lcg.internal.h"

/**
 * Returns 31-bit linear congruential pseudorandom number, e.g.
 *
 *     int x = rand();
 *     assert(x >= 0);
 *
 * This function always returns a positive number. If srand() isn't
 * called, then it'll return the same sequence each time your program
 * runs. Faster and more modern alternatives exist to this function.
 *
 * This function is not thread safe in the sense that multiple threads
 * might simultaneously generate the same random values.
 *
 * @note this function does well on bigcrush and practrand
 * @note this function is not intended for cryptography
 * @see lemur64(), _rand64(), rdrand()
 * @threadunsafe
 */
int rand(void) {
  return KnuthLinearCongruentialGenerator(&g_rando) >> 33;
}
