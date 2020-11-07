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
#include "libc/bits/hilbert.h"

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
 * @see morton()
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
 * @see unmorton()
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
