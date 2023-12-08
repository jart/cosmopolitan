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
#include "dsp/core/q.h"
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/str/str.h"

/**
 * Same as GetIntegerCoefficients() but with eight terms.
 * @cost ~3ms
 */
long GetIntegerCoefficients8(long N[static 8], const double C[static 8], long M,
                             long L, long H) {
  int i;
  int j[8], J[8];
  int O[8] = {0};
  int S[3] = {0, -1, +1};
  double R[8], K[8], D[8], Z, least, error;
  least = 1;
  Z = 1L << M;
  for (i = 0; i < ARRAYLEN(J); ++i) {
    least *= H - L;
    if (fabs(C[i]) > DBL_MIN) {
      J[i] = ARRAYLEN(S);
      R[i] = C[i] * Z;
      K[i] = rint(R[i]);
      N[i] = K[i];
    } else {
      J[i] = 1;
      R[i] = 0;
      K[i] = 0;
      N[i] = 0;
    }
  }
  if (least > 1) {
    for (j[0] = 0; j[0] < J[0]; ++j[0]) {
      for (j[1] = 0; j[1] < J[1]; ++j[1]) {
        for (j[2] = 0; j[2] < J[2]; ++j[2]) {
          for (j[3] = 0; j[3] < J[3]; ++j[3]) {
            for (j[4] = 0; j[4] < J[4]; ++j[4]) {
              for (j[5] = 0; j[5] < J[5]; ++j[5]) {
                for (j[6] = 0; j[6] < J[6]; ++j[6]) {
                  for (j[7] = 0; j[7] < J[7]; ++j[7]) {
                    for (i = 0; i < ARRAYLEN(J); ++i) {
                      D[i] = S[j[i]] + K[i] - R[i];
                    }
                    if ((error = DifferSumSq8(D, L, H) / Z) < least) {
                      least = error;
                      memcpy(O, j, sizeof(j));
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    for (i = 0; i < 8; ++i) {
      N[i] += S[O[i]];
    }
  }
  return lround(least);
}
