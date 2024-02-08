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
#include "libc/limits.h"
#include "libc/log/log.h"
#include "libc/macros.internal.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void djbsort_avx2(int32_t *, long);

size_t n;
int32_t *a, *b, *c;

void insertionsort(int32_t *a, size_t n) {
  int t;
  unsigned i, j;
  for (i = 1; i < n; ++i) {
    j = i;
    t = a[i];
    while (j > 0 && t < a[j - 1]) {
      a[j] = a[j - 1];
      --j;
    }
    a[j] = t;
  }
}

#ifdef __x86_64__
TEST(djbsort, test4) {
  static const int kA[] = {4, 3, 2, 1};
  n = ARRAYLEN(kA);
  a = memcpy(gc(malloc(n * 4)), kA, n * 4);
  b = memcpy(gc(malloc(n * 4)), kA, n * 4);
  c = memcpy(gc(malloc(n * 4)), kA, n * 4);
  insertionsort(a, n);
  djbsort_avx2(b, n);
  djbsort(c, n);
  ASSERT_EQ(0, memcmp(a, b, n * 4));
  ASSERT_EQ(0, memcmp(a, c, n * 4));
}
#endif /* __x86_64__ */

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
  a = memcpy(gc(malloc(n * 4)), kA, n * 4);
  b = memcpy(gc(malloc(n * 4)), kA, n * 4);
  c = memcpy(gc(malloc(n * 4)), kA, n * 4);
  insertionsort(a, n);
  djbsort(c, n);
  ASSERT_EQ(0, memcmp(a, c, n * 4));
#ifdef __x86_64__
  if (X86_HAVE(AVX2)) {
    djbsort_avx2(b, n);
    ASSERT_EQ(0, memcmp(a, b, n * 4));
  }
#endif /* __x86_64__ */
}

static int CompareInt(const void *a, const void *b) {
  if (*(const int *)a < *(const int *)b) return -1;
  if (*(const int *)a > *(const int *)b) return +1;
  return 0;
}

BENCH(djbsort, bench) {
  n = 256;
  a = gc(memalign(32, n * 4));
  EZBENCH2("insertionsort[255]", rngset(a, n * 4, _rand64, -1),
           insertionsort(a, n));
  EZBENCH2("djbsort[255]", rngset(a, n * 4, _rand64, -1), djbsort(a, n));
  EZBENCH2("_intsort[255]", rngset(a, n * 4, _rand64, -1), _intsort(a, n));
  EZBENCH2("qsort[255]", rngset(a, n * 4, _rand64, -1),
           qsort(a, n, sizeof(int), CompareInt));
}
