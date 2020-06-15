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
#include "dsp/core/q.h"
#include "libc/intrin/pmulhrsw.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

#define ACCURACY powf(10, -4)

#define FOR8(STMT)          \
  for (y = 0; y < 8; ++y) { \
    STMT;                   \
  }

#define FOR88(STMT)           \
  for (y = 0; y < 8; ++y) {   \
    for (x = 0; x < 8; ++x) { \
      STMT;                   \
    }                         \
  }

FIXTURE(pmulhrsw, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

TEST(pmulhrsw, testLimits) {
  int i;
  short A[8], B[8];
  const short kPmulhrswTorture[][3] = {
      {SHRT_MIN, SHRT_MIN, SHRT_MIN},
      {SHRT_MIN, -1, 1},
      {SHRT_MIN, 0, 0},
      {SHRT_MIN, 1, -1},
      {-1, SHRT_MIN, 1},
      {-1, -1, 0},
      {-1, 0, 0},
      {-1, 1, 0},
      {-1, SHRT_MAX, -1},
      {0, SHRT_MIN, 0},
      {0, -1, 0},
      {0, 0, 0},
      {0, 1, 0},
      {0, SHRT_MAX, 0},
      {1, SHRT_MIN, -1},
      {1, -1, 0},
      {1, 0, 0},
      {1, 1, 0},
      {1, SHRT_MAX, 1},
      {SHRT_MAX, -1, -1},
      {SHRT_MAX, 0, 0},
      {SHRT_MAX, 1, 1},
  };
  memset(A, 0, sizeof(A));
  memset(B, 0, sizeof(B));
  for (i = 0; i < ARRAYLEN(kPmulhrswTorture); ++i) {
    A[0] = kPmulhrswTorture[i][0];
    B[0] = kPmulhrswTorture[i][1];
    pmulhrsw(A, A, B);
    EXPECT_EQ(kPmulhrswTorture[i][2], A[0], "pmulhrsw(%hd,%hd)→%hd",
              kPmulhrswTorture[i][0], kPmulhrswTorture[i][1], A[0]);
  }
}

TEST(pmulhrsw, testFakeFloat) {
  int y, x;
  float R[8][8];
  float Q[8][8];
  short QQ[8][8];
  short QD[8][8];
  short QM[8][8];
  float D[8][8] = /* clang-format off */ {
    {.929142, .147545,  .17061, .765948, .874296, .925816, .073955,  .10664},
    {.986743, .311924, .550892, .789301, .873408, .743376, .434021, .143184},
    {.405694, .080979, .894841, .625169, .465688, .877854,  .97371, .264295},
    {.781549,  .20985, .599735, .943491, .059135, .045806, .770352, .081862},
    {.584684, .701568, .022328, .177048, .412809, .185355, .992654, .252167},
    {.327565, .693878, .722431,  .84546, .060729, .383725, .589365, .435534},
    {.942854,  .62579, .177928, .809653, .143087, .624792, .851914, .072192},
    {.750157, .968502, .270052, .087784, .406716, .510766, .959699, .416836},
  };
  float M[8][8] = {
    {.009407, .882863, .000511, .565419,  .69844, .035758, .817049, .249922},
    {.072144, .703228, .479622, .121608, .288279,  .55492, .387912, .140278},
    {.047205, .748263, .683692, .805669, .137764, .858753, .787804, .059591},
    {.682286, .787778, .503573, .473795, .437378, .573171, .135995, .341236},
    {.588849, .723929, .624155, .710336, .480396, .462433, .865392, .071378},
    {.598636, .575209, .758356, .518674, .043861, .542574, .355843,  .02014},
    {.359636,  .95607, .698256, .492859, .149454, .795121, .790219, .357014},
    {.401603, .928426, .416429,  .11747, .643411, .907285, .074102, .411959},
  } /* clang-format on */;
  FOR88(QD[y][x] = F2Q(15, D[y][x]));
  FOR88(QM[y][x] = F2Q(15, M[y][x]));
  FOR8(pmulhrsw(QQ[y], QD[y], QM[y]));
  FOR88(Q[y][x] = Q2F(15, QQ[y][x]));
  FOR88(R[y][x] = D[y][x] * M[y][x]);
  FOR88(EXPECT_TRUE(ACCURACY > Q[y][x] - R[y][x]));
}
