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
#include "libc/alg/alg.h"
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void djbsort$avx2(int32_t *, long);

size_t n;
int32_t *a, *b, *c;

TEST(djbsort, test4) {
  static const int kA[] = {4, 3, 2, 1};
  n = ARRAYLEN(kA);
  a = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  b = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  c = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  insertionsort(n, a);
  djbsort$avx2(b, n);
  djbsort(n, c);
  ASSERT_EQ(0, memcmp(a, b, n * 4));
  ASSERT_EQ(0, memcmp(a, c, n * 4));
}

TEST(djbsort, test64) {
  static const int kA[64] = {
      -578159677,  -506496753,  627992389,   -1352456426, -632122174,
      -1439460768, -1910607416, INT_MAX,     828129452,   388318786,
      1361377655,  178046,      -250539006,  -933303681,  553663398,
      801377571,   1798551167,  1926219590,  1322300030,  2005832294,
      190425814,   1896617905,  -549032465,  -930529122,  953163359,
      -1290004523, 447201234,   1351982281,  458539920,   791518325,
      -1086386708, 291355635,   INT_MIN,     -1891018285, 1780601656,
      1324222158,  1182663010,  -1223576562, -676035738,  1367175631,
      -1016599422, 1619595549,  -783089669,  -663931695,  -1082674213,
      1369114774,  -1944970907, 196888289,   1400094001,  -1170906601,
      835635598,   1506409902,  -1528765785, 1132926680,  -351522751,
      -1737707221, -209740191,  -1857759507, -353087096,  763588876,
      -1323943608, -1219421355, -582289873,  1062699814,
  };
  n = ARRAYLEN(kA);
  a = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  b = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  c = memcpy(tgc(tmalloc(n * 4)), kA, n * 4);
  insertionsort(n, a);
  djbsort(n, c);
  ASSERT_EQ(0, memcmp(a, c, n * 4));
  if (X86_HAVE(AVX2)) {
    djbsort$avx2(b, n);
    ASSERT_EQ(0, memcmp(a, b, n * 4));
  }
}

BENCH(djbsort, bench) {
  n = 256;
  a = gc(memalign(32, n * 4));
  EZBENCH2("insertionsort[255]", rngset(a, n * 4, rand64, -1),
           insertionsort(n, a));
  EZBENCH2("djbsort[255]", rngset(a, n * 4, rand64, -1), djbsort(n, a));
}
