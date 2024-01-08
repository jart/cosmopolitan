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
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

long I[6];

TEST(GetIntegerCoefficients, testBt601Vectors) {
  const struct {
    int m, L, H;
    double r[6];
    long n[6];
  } V[] = {
      {8, 16, 235, {.299, .587, .114}, {77, 150, 29}},
      {9, 16, 235, {.299, .587, .114}, {153, 301, 58}},
      {10, 16, 235, {.299, .587, .114}, {306, 601, 117}},
      {11,
       16,
       235,
       {.299, .587, .114, 1, 1, 1},
       {612, 1202, NoDebug() ? 233 : 234, 2048, 2048, 2048}},
      {12, 16, 235, {.299, .587, .114}, {1225, 2404, 467}},
      {13, 16, 235, {.299, .587, .114}, {2449, 4809, 934}},
      {14, 16, 235, {.299, .587, .114}, {4899, 9617, 1868}},
      {15, 16, 235, {.299, .587, .114}, {9798, 19235, NoDebug() ? 3736 : 3735}},
      {16, 16, 235, {.299, .587, .114}, {19595, 38470, 7471}},
  };
  long i, got[6];
  for (i = 0; i < ARRAYLEN(V); ++i) {
    GetIntegerCoefficients(got, V[i].r, V[i].m, V[i].L, V[i].H);
    EXPECT_EQ(0, memcmp(V[i].n, got, sizeof(got)),
              "got={%ld,%ld,%ld,%ld,%ld,%ld}, want={%ld,%ld,%ld,%ld,%ld,%ld}",
              got[0], got[1], got[2], got[3], got[4], got[5], V[i].n[0],
              V[i].n[1], V[i].n[2], V[i].n[3], V[i].n[4], V[i].n[5]);
  }
}

TEST(GetIntegerCoefficients, testForYCbCr2Rgb) {
  double C[6] = {.299, .587, .114};
  GetIntegerCoefficients(I, C, 11, 16, 232);
  EXPECT_EQ(612, I[0]);
  EXPECT_EQ(1202, I[1]);
  EXPECT_EQ(NoDebug() ? 233 : 234, I[2]);
}

TEST(GetIntegerCoefficients, testForGaussian) {
#define G(A, B, C, D, E) lrint((A + 4 * B + 6 * C + 4 * D + E) / 16.)
  double C[6] = {1 / 16., 4 / 16., 6 / 16., 4 / 16., 1 / 16.};
  long M = 22, N[6], B[6] = {12, 191, 174, 205, 35};
  GetIntegerCoefficients(N, C, M, 0, 255);
  EXPECT_EQ(262144, N[0]);
  EXPECT_EQ(1048576, N[1]);
  EXPECT_EQ(1572864, N[2]);
  EXPECT_EQ(1048576, N[3]);
  EXPECT_EQ(262144, N[4]);
  EXPECT_EQ(G(B[0], B[1], B[2], B[3], B[4]),
            (N[0] * B[0] + N[1] * B[1] + N[2] * B[2] + N[3] * B[3] +
             N[4] * B[4] + N[5] * B[5] + (1l << (M - 1))) >>
                M);
}

BENCH(GetIntegerCoefficients, bench) {
  double C[6] = {.299, .587, .114};
  EZBENCH2("getintegercoefficients", donothing,
           GetIntegerCoefficients(I, C, 11, 16, 232));
}
