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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

long I[8];

TEST(GetIntegerCoefficients8, testBt601Vectors) {
  const struct {
    int m, L, H;
    double r[8];
    long n[8];
  } V[] = {
      {8, 16, 235, {.299, .587, .114}, {77, 150, 29}},
      {9, 16, 235, {.299, .587, .114}, {153, 301, 58}},
      {10, 16, 235, {.299, .587, .114}, {306, 601, 117}},
      {11,
       16,
       235,
       {.299, .587, .114, 1, 1, 1},
       {612, 1202, 234, 2048, 2048, 2048}},
      {12, 16, 235, {.299, .587, .114}, {1225, 2404, 467}},
      {13, 16, 235, {.299, .587, .114}, {2449, 4809, 934}},
      {14, 16, 235, {.299, .587, .114}, {4899, 9617, 1868}},
      {15, 16, 235, {.299, .587, .114}, {9798, 19235, 3735}},
      {16, 16, 235, {.299, .587, .114}, {19595, 38470, 7471}},
  };
  long i, got[8];
  for (i = 0; i < ARRAYLEN(V); ++i) {
    GetIntegerCoefficients8(got, V[i].r, V[i].m, V[i].L, V[i].H);
    EXPECT_EQ(0, memcmp(V[i].n, got, sizeof(got)),
              "got={%ld,%ld,%ld,%ld,%ld,%ld}, want = { % ld, % ld, % ld, % ld, "
              "% ld, % ld } ",
              got[0], got[1], got[2], got[3], got[4], got[5], V[i].n[0],
              V[i].n[1], V[i].n[2], V[i].n[3], V[i].n[4], V[i].n[5]);
  }
}

BENCH(GetIntegerCoefficients8, bench) {
  double C[8] = {.299, .587, .114, .299, .587, .114, .114, .114};
  EZBENCH2("GetIntegerCoefficients8", donothing,
           GetIntegerCoefficients8(I, C, 11, 16, 232));
}
