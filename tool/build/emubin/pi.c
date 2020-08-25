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
