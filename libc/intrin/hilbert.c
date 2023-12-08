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
#include "libc/intrin/hilbert.h"

static axdx_t RotateQuadrant(long n, long y, long x, long ry, long rx) {
  long t;
  if (ry == 0) {
    if (rx == 1) {
      y = n - 1 - y;
      x = n - 1 - x;
    }
    t = x;
    x = y;
    y = t;
  }
  return (axdx_t){y, x};
}

/**
 * Generates Hilbert space-filling curve.
 *
 * @see https://en.wikipedia.org/wiki/Hilbert_curve
 * @see unhilbert()
 */
long hilbert(long n, long y, long x) {
  axdx_t m;
  long d, s, ry, rx;
  d = 0;
  for (s = n / 2; s > 0; s /= 2) {
    ry = (y & s) > 0;
    rx = (x & s) > 0;
    d += s * s * ((3 * rx) ^ ry);
    m = RotateQuadrant(n, y, x, ry, rx);
    y = m.ax;
    x = m.dx;
  }
  return d;
}

/**
 * Decodes Hilbert space-filling curve.
 *
 * @see https://en.wikipedia.org/wiki/Hilbert_curve
 * @see hilbert()
 */
axdx_t unhilbert(long n, long i) {
  axdx_t m;
  long s, t, y, x, ry, rx;
  t = i;
  x = y = 0;
  for (s = 1; s < n; s *= 2) {
    rx = (t / 2) & 1;
    ry = (t ^ rx) & 1;
    m = RotateQuadrant(s, y, x, ry, rx);
    x = m.dx + s * rx;
    y = m.ax + s * ry;
    t /= 4;
  }
  return (axdx_t){y, x};
}
