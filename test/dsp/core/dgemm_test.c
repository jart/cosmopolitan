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
#include "libc/bits/xmmintrin.h"
#include "libc/log/log.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "third_party/blas/blas.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

TEST(dgemm, test) {
  double alpha, beta;
  long m, n, k, lda, ldb, ldc;
  double A[3][3] = {{1 / 8.}, {6 / 8.}, {1 / 8.}};
  double B[1][3] = {{1 / 8., 6 / 8., 1 / 8.}};
  double C[3][3] = {0};
  m = 3;
  n = 3;
  k = 1;
  lda = 3;
  ldb = 3;
  ldc = 3;
  beta = 1;
  alpha = 1;
  dgemm_("T", "T", &m, &n, &k, &alpha, &A[0][0], &lda, &B[0][0], &ldb, &beta,
         &C[0][0], &ldc);
  EXPECT_DBLMATRIXEQ(6, rint, 3, 3, C, "\n\
.015625  .09375 .015625\n\
 .09375   .5625  .09375\n\
.015625  .09375 .015625");
}

void dgemmer(long m, long n, long k, void *A, long lda, void *B, long ldb,
             void *C, long ldc) {
  double alpha, beta;
  beta = 1;
  alpha = 1;
  dgemm_("N", "N", &m, &n, &k, &alpha, A, &lda, B, &ldb, &beta, C, &ldc);
}

BENCH(dgemm, bench) {
  double(*A)[128][128] = tgc(tmalloc(128 * 128 * 8));
  double(*B)[128][128] = tgc(tmalloc(128 * 128 * 8));
  double(*C)[128][128] = tgc(tmalloc(128 * 128 * 8));
  EZBENCH2("dgemm_", donothing, dgemmer(128, 128, 128, A, 128, B, 128, C, 128));
}
