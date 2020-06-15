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
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(qsort, test) {
  const int32_t A[][2] = {{4, 'a'},   {65, 'b'}, {2, 'c'}, {-31, 'd'},
                          {0, 'e'},   {99, 'f'}, {2, 'g'}, {83, 'h'},
                          {782, 'i'}, {1, 'j'}};
  const int32_t B[][2] = {{-31, 'd'}, {0, 'e'},  {1, 'j'},  {2, 'c'},
                          {2, 'g'},   {4, 'a'},  {65, 'b'}, {83, 'h'},
                          {99, 'f'},  {782, 'i'}};
  int32_t(*M)[2] = tmalloc(sizeof(A));
  memcpy(M, B, sizeof(A));
  qsort(M, ARRAYLEN(A), sizeof(*M), cmpsl);
  EXPECT_EQ(0, memcmp(M, B, sizeof(B)));
  tfree(M);
}
