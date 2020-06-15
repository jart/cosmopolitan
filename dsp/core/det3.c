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
#include "dsp/core/core.h"

#define LEIBNIZ_FORMULA(a, b, c, d, e, f, g, h, i) \
  (a * e * i + b * f * g + c * d * h - c * e * g - b * d * i - a * f * h)

/**
 * Computes determinant of 3×3 matrix.
 * i.e. how much space is inside the cube
 *
 * @param 𝐀 is input matrix
 * @param 𝑑 is det(𝐀)
 * @return |𝐀| or 0 if degenerate
 */
double det3(const double A[3][3]) {
  return LEIBNIZ_FORMULA(A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], A[1][2],
                         A[2][0], A[2][1], A[2][2]);
}
