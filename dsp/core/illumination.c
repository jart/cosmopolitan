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
#include "dsp/core/illumination.h"
#include "dsp/core/core.h"
#include "libc/str/str.h"

const double kIlluminantA[3] = {1.0985, 1, 0.35585};
const double kIlluminantAD10[3] = {1.11144, 1, 0.35200};
const double kIlluminantC[3] = {0.98074, 1, 1.18232};
const double kIlluminantCD10[3] = {0.97285, 1, 1.16145};
const double kIlluminantD50[3] = {0.96422, 1, 0.82521};
const double kIlluminantD50D10[3] = {0.9672, 1, 0.81427};
const double kIlluminantD55[3] = {0.95682, 1, 0.92149};
const double kIlluminantD55D10[3] = {0.95799, 1, 0.90926};
const double kIlluminantD65[3] = {0.95047, 1, 1.08883};
const double kIlluminantD65D10[3] = {0.94811, 1, 1.07304};
const double kIlluminantD75[3] = {0.94972, 1, 1.22638};
const double kIlluminantD75D10[3] = {0.94416, 1, 1.20641};
const double kIlluminantF2[3] = {0.99187, 1, 0.67395};
const double kIlluminantF2D10[3] = {1.0328, 1, 0.69026};
const double kIlluminantF7[3] = {0.95044, 1, 1.08755};
const double kIlluminantF7D10[3] = {0.95792, 1, 1.07687};
const double kIlluminantF11[3] = {1.00966, 1, 0.6437};
const double kIlluminantF11D10[3] = {1.03866, 1, 0.65627};

/**
 * System of equations used for changing illumination.
 *
 * @see brucelindbloom.com/Eqn_ChromAdapt.html “The Bradford method is
 *     the newest of the three methods, and is considered by most
 *     experts to be the best of them. This is the method used in Adobe
 *     Photoshop. A related article comparing the chromatic adaptation
 *     algorithms may be found here.” ─Quoth Bruce Lindbloom
 */
const double kBradford[3][3] = {
    {0.8951, 0.2664, -.1614},
    {-.7502, 1.7135, 0.0367},
    {0.0389, -.0685, 1.0296},
};

/**
 * Computes lightbulb changing coefficients.
 *
 * @param R will store result
 * @param S has intended input illuminant primaries
 * @param D has desired output illuminant primaries
 * @return R
 * @see brucelindbloom.com/Eqn_ChromAdapt.html
 * @see fluxometer.com/rainbow/
 */
void *GetChromaticAdaptationMatrix(double R[3][3], const double S[3],
                                   const double D[3]) {
  double M[3][3], T[3][3], U[3][3], V[3], W[3];
  matvmul3(V, kBradford, S);
  matvmul3(W, kBradford, D);
  memset(M, 0, sizeof(M));
  M[0][0] = W[0] / V[0];
  M[1][1] = W[1] / V[1];
  M[2][2] = W[2] / V[2];
  return matmul3(R, matmul3(T, kBradford, M),
                 inv3(U, kBradford, det3(kBradford)));
}
