/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/progn.internal.h"
#include "libc/intrin/palignr.h"
#include "libc/rand/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(palignr, testLeftpad) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {0};
  palignr(C, B, A, 12);
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignr, testLeftpad_variableImmediate) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {0};
  palignr(C, B, A, VEIL("r", 12));
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignr, test0) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  palignr(C, B, A, 0);
  ASSERT_EQ(1, C[0]);
  ASSERT_EQ(2, C[1]);
  ASSERT_EQ(3, C[2]);
  ASSERT_EQ(4, C[3]);
}

TEST(palignr, test4) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  palignr(C, B, A, 4);
  ASSERT_EQ(2, C[0]);
  ASSERT_EQ(3, C[1]);
  ASSERT_EQ(4, C[2]);
  ASSERT_EQ(5, C[3]);
}

TEST(palignr, test12) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  palignr(C, B, A, 12);
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignr, test16) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  palignr(C, B, A, 16);
  ASSERT_EQ(5, C[0]);
  ASSERT_EQ(6, C[1]);
  ASSERT_EQ(7, C[2]);
  ASSERT_EQ(8, C[3]);
}

TEST(palignr, test20) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {-1, -1, -1, -1};
  palignr(C, B, A, 20);
  ASSERT_EQ(6, C[0]);
  ASSERT_EQ(7, C[1]);
  ASSERT_EQ(8, C[2]);
  ASSERT_EQ(0, C[3]);
}

TEST(palignrc, testLeftpad) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {0};
  (palignr)(C, B, A, 12);
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignrc, testLeftpad_variableImmediate) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {0};
  (palignr)(C, B, A, VEIL("r", 12));
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignrc, test0) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  (palignr)(C, B, A, 0);
  ASSERT_EQ(1, C[0]);
  ASSERT_EQ(2, C[1]);
  ASSERT_EQ(3, C[2]);
  ASSERT_EQ(4, C[3]);
}

TEST(palignrc, test4) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  (palignr)(C, B, A, 4);
  ASSERT_EQ(2, C[0]);
  ASSERT_EQ(3, C[1]);
  ASSERT_EQ(4, C[2]);
  ASSERT_EQ(5, C[3]);
}

TEST(palignrc, test12) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  (palignr)(C, B, A, 12);
  ASSERT_EQ(4, C[0]);
  ASSERT_EQ(5, C[1]);
  ASSERT_EQ(6, C[2]);
  ASSERT_EQ(7, C[3]);
}

TEST(palignrc, test16) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4];
  (palignr)(C, B, A, 16);
  ASSERT_EQ(5, C[0]);
  ASSERT_EQ(6, C[1]);
  ASSERT_EQ(7, C[2]);
  ASSERT_EQ(8, C[3]);
}

TEST(palignrc, test20) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {-1, -1, -1, -1};
  (palignr)(C, B, A, 20);
  ASSERT_EQ(6, C[0]);
  ASSERT_EQ(7, C[1]);
  ASSERT_EQ(8, C[2]);
  ASSERT_EQ(0, C[3]);
}

TEST(palignr, test32orHigher_clearsOutput) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {-1, -1, -1, -1};
  palignr(C, B, A, 32);
  ASSERT_EQ(0, C[0]);
  ASSERT_EQ(0, C[1]);
  ASSERT_EQ(0, C[2]);
  ASSERT_EQ(0, C[3]);
  C[0] = 43;
  palignr(C, B, A, 123);
  ASSERT_EQ(0, C[0]);
  ASSERT_EQ(0, C[1]);
  ASSERT_EQ(0, C[2]);
  ASSERT_EQ(0, C[3]);
}

TEST(palignrv, test32orHigher_clearsOutput) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {-1, -1, -1, -1};
  palignr(C, B, A, VEIL("r", 32));
  ASSERT_EQ(0, C[0]);
  ASSERT_EQ(0, C[1]);
  ASSERT_EQ(0, C[2]);
  ASSERT_EQ(0, C[3]);
  C[0] = 43;
  palignr(C, B, A, VEIL("r", 123));
  ASSERT_EQ(0, C[0]);
  ASSERT_EQ(0, C[1]);
  ASSERT_EQ(0, C[2]);
  ASSERT_EQ(0, C[3]);
}

TEST(palignrc, test32orHigher_clearsOutput) {
  const int A[4] = {1, 2, 3, 4};
  const int B[4] = {5, 6, 7, 8};
  int C[4] = {-1, -1, -1, -1};
  (palignr)(C, B, A, 32);
  ASSERT_EQ(0, C[0]);
  ASSERT_EQ(0, C[1]);
  ASSERT_EQ(0, C[2]);
  ASSERT_EQ(0, C[3]);
}

BENCH(palignr, bench) {
  volatile __intrin_xmm_t A;
  volatile __intrin_xmm_t B;
  EZBENCH2("palignr const 𝑖", donothing, PROGN(palignr(&A, &A, &B, 7)));
  EZBENCH2("palignr var 𝑖", donothing,
           PROGN(palignr(&A, &A, &B, VEIL("r", 7))));
  EZBENCH2("palignr ansi", donothing, PROGN((palignr)(&A, &A, &B, 7)));
}
