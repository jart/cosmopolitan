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
#include "libc/math.h"
#include "libc/str/str.h"

/**
 * Computes 𝐀⁻¹ inverted 3×3 matrix, if it exists.
 *
 * @param 𝐁 is destination memory
 * @param 𝐀 is input matrix, which can't overlap 𝐁
 * @param 𝑑 is |𝐀| the determinant scalar or 0 if degenerate
 * @return 𝐀⁻¹ stored inside 𝐁 or NaNs if 𝑑=0
 * @define 𝐀⁻¹=𝐁 such that 𝐀×𝐁=𝐁×𝐀=𝐈ₙ
 * @see det3()
 */
void *inv3(double B[restrict 3][3], const double A[restrict 3][3], double d) {
  d = d ? 1 / d : NAN;
  B[0][0] = (A[1][1] * A[2][2] - A[2][1] * A[1][2]) * d;
  B[0][1] = (A[2][1] * A[0][2] - A[0][1] * A[2][2]) * d;
  B[0][2] = (A[0][1] * A[1][2] - A[1][1] * A[0][2]) * d;
  B[1][0] = (A[2][0] * A[1][2] - A[1][0] * A[2][2]) * d;
  B[1][1] = (A[0][0] * A[2][2] - A[2][0] * A[0][2]) * d;
  B[1][2] = (A[1][0] * A[0][2] - A[0][0] * A[1][2]) * d;
  B[2][0] = (A[1][0] * A[2][1] - A[2][0] * A[1][1]) * d;
  B[2][1] = (A[2][0] * A[0][1] - A[0][0] * A[2][1]) * d;
  B[2][2] = (A[0][0] * A[1][1] - A[1][0] * A[0][1]) * d;
  return B;
}
