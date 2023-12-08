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
#include "dsp/core/illumination.h"
#include "libc/log/log.h"
#include "libc/math.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

TEST(GetChromaticAdaptationMatrix, testSuperiorIlluminationBannedInCalifornia) {
  double M[3][3];
  GetChromaticAdaptationMatrix(M, kIlluminantD65, kIlluminantA);
  EXPECT_DBLMATRIXEQ(5, rint, 3, 3, M, "\n\
  1.2165   .11099  -.15493\n\
  .15333   .91523 -.055995\n\
-.023947  .035898   .31475");
}

TEST(GetChromaticAdaptationMatrix, testD65ToD50_soWeCanCieLab) {
  double M[3][3];
  GetChromaticAdaptationMatrix(M, kIlluminantD65, kIlluminantD50);
  EXPECT_DBLMATRIXEQ(6, rint, 3, 3, M, "\n\
   1.04781   .0228866   -.050127\n\
  .0295424    .990484  -.0170491\n\
-.00923449   .0150436    .752132");
}

BENCH(GetChromaticAdaptationMatrix, bench) {
  double M[3][3];
  EZBENCH2("GetChromaticAdaptationMatrix", donothing,
           GetChromaticAdaptationMatrix(M, kIlluminantD65, kIlluminantA));
}
