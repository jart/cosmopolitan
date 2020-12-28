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
#include "libc/alg/alg.h"
#include "libc/macros.h"
#include "libc/rand/rand.h"
#include "libc/runtime/carsort.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

const int32_t kUnsorted[][2] = {
    {4, 'a'},  {65, 'b'}, {2, 'c'},  {-1, 'G'},  {-31, 'd'}, {0, 'e'},
    {99, 'f'}, {2, 'g'},  {83, 'h'}, {782, 'i'}, {1, 'j'},
};

const int32_t kGolden[][2] = {
    {-31, 'd'}, {-1, 'G'}, {0, 'e'},  {1, 'j'},  {2, 'c'},   {2, 'g'},
    {4, 'a'},   {65, 'b'}, {83, 'h'}, {99, 'f'}, {782, 'i'},
};

int32_t A[ARRAYLEN(kUnsorted)][2];
int32_t B[2100][2];

TEST(carsort100, test) {
  memcpy(A, kUnsorted, sizeof(A));
  carsort100(ARRAYLEN(A), A);
  ASSERT_EQ(0, memcmp(&A[0], &kGolden[0], sizeof(kUnsorted)));
}

TEST(carsort1000, test) {
  memcpy(A, kUnsorted, sizeof(A));
  carsort1000(ARRAYLEN(A), A);
  ASSERT_EQ(0, memcmp(&A[0], &kGolden[0], sizeof(kUnsorted)));
}

TEST(qsort, test) {
  memcpy(A, kUnsorted, sizeof(A));
  qsort(A, ARRAYLEN(A), 8, cmpsl);
  ASSERT_EQ(0, memcmp(&A[0], &kGolden[0], sizeof(kUnsorted)));
}

BENCH(carsort, benchMedium) {
  EZBENCH2("medium carsort100", rngset(B, sizeof(B), rand64, -1),
           carsort100(ARRAYLEN(B), B));
  EZBENCH2("medium carsort1000", rngset(B, sizeof(B), rand64, -1),
           carsort1000(ARRAYLEN(B), B));
  EZBENCH2("medium qsort", rngset(B, sizeof(B), rand64, -1),
           qsort(B, ARRAYLEN(B), 8, cmpsl));
}

BENCH(carsort, benchSmall) {
  EZBENCH2("small carsort100", memcpy(A, kUnsorted, sizeof(A)),
           carsort100(ARRAYLEN(A), A));
  EZBENCH2("small carsort1000", memcpy(A, kUnsorted, sizeof(A)),
           carsort1000(ARRAYLEN(A), A));
  EZBENCH2("small qsort", memcpy(A, kUnsorted, sizeof(A)),
           qsort(A, ARRAYLEN(A), 8, cmpsl));
}
