/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/tinymath/magicu.h"
#include "libc/assert.h"

/**
 * Precomputes magic numbers for unsigned division by constant.
 *
 * The returned divisor may be passed to __magic_div() to perform
 * unsigned integer division way faster than normal division e.g.
 *
 *     assert(77 / 7 == __magicu_div(77, __magicu_get(7)));
 *
 * @param d is intended divisor, which must not be zero
 * @return magic divisor (never zero)
 */
struct magicu __magicu_get(uint32_t d) {
  // From Hacker's Delight by Henry S. Warren Jr., 9780321842688
  // Figure 10–3. Simplified algorithm for magic number unsigned
  int a, p;
  struct magicu magu;
  uint32_t p32, q, r, delta;
  npassert(d);             // Can't divide by zero.
  p32 = 0;                 // Avoid compiler warning.
  a = 0;                   // Initialize "add" indicator.
  p = 31;                  // Initialize p.
  q = 0x7FFFFFFF / d;      // Initialize q = (2**p - 1)/d.
  r = 0x7FFFFFFF - q * d;  // Init. r = rem(2**p - 1, d).
  do {
    p = p + 1;
    if (p == 32) {
      p32 = 1;  // Set p32 = 2**(p-32).
    } else {
      p32 = 2 * p32;
    }
    if (r + 1 >= d - r) {
      if (q >= 0x7FFFFFFF) a = 1;
      q = 2 * q + 1;      // Update q.
      r = 2 * r + 1 - d;  // Update r.
    } else {
      if (q >= 0x80000000) a = 1;
      q = 2 * q;
      r = 2 * r + 1;
    }
    delta = d - 1 - r;
  } while (p < 64 && p32 < delta);
  magu.M = q + 1;              // Magic number and
  magu.s = p - 32;             // Shift amount to return
  if (a) magu.s |= 64;         // Sets "add" indicator
  npassert(magu.M || magu.s);  // Never returns zero.
  return magu;
}
