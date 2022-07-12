/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/errno.h"
#include "libc/math.h"

/**
 * Returns 𝑥^𝑦.
 * @note should take ~56ns
 */
long double powl(long double x, long double y) {
  long double t, u;
  if (!isunordered(x, y)) {
    if (!isinf(y)) {
      if (!isinf(x)) {
        if (x) {
          if (y) {
            if (x < 0 && y != truncl(y)) {
              errno = EDOM;
              return NAN;
            }
            asm("fyl2x" : "=t"(u) : "0"(fabsl(x)), "u"(y) : "st(1)");
            asm("fprem" : "=t"(t) : "0"(u), "u"(1.L));
            asm("f2xm1" : "=t"(t) : "0"(t));
            asm("fscale" : "=t"(t) : "0"(t + 1), "u"(u));
            if (signbit(x)) {
              if (y != truncl(y)) return -NAN;
              if ((int64_t)y & 1) t = -t;
            }
            return t;
          } else {
            return 1;
          }
        } else if (y > 0) {
          if (signbit(x) && y == truncl(y) && ((int64_t)y & 1)) {
            return -0.;
          } else {
            return 0;
          }
        } else if (!y) {
          return 1;
        } else {
          errno = ERANGE;
          if (y == truncl(y) && ((int64_t)y & 1)) {
            return copysignl(INFINITY, x);
          } else {
            return INFINITY;
          }
        }
      } else if (signbit(x)) {
        if (!y) return 1;
        x = y < 0 ? 0 : INFINITY;
        if (y == truncl(y) && ((int64_t)y & 1)) x = -x;
        return x;
      } else if (y < 0) {
        return 0;
      } else if (y > 0) {
        return INFINITY;
      } else {
        return 1;
      }
    } else {
      x = fabsl(x);
      if (x < 1) return signbit(y) ? INFINITY : 0;
      if (x > 1) return signbit(y) ? 0 : INFINITY;
      return 1;
    }
  } else if (!y || x == 1) {
    return 1;
  } else {
    return NAN;
  }
}
