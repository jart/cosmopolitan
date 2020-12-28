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
#include "libc/math.h"

#if 1
#define FLOAT long double
#define SQRT  sqrtl
#else
#define FLOAT float
#define SQRT  sqrt
#endif

/**
 * Computes π w/ François Viète method.
 *
 *   make -j8 MODE=tiny
 *   o/tiny/tool/build/emulator.com.dbg -t o/tiny/tool/build/pi.bin
 *
 */
FLOAT Main(void) {
  long i, n;
  FLOAT pi, q, t;
  n = VEIL("r", 10);
  q = 0;
  t = 1;
  for (i = 0; i < n; ++i) {
    q += 2;
    q = SQRT(q);
    t *= q / 2;
  }
  return 2 / t;
}

volatile FLOAT st0;

int main(int argc, char *argv[]) {
  st0 = Main();
  return 0;
}
