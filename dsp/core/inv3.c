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
