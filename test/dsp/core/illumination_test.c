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
