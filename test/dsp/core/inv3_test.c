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
#include "libc/math.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "tool/viz/lib/formatstringtable-testlib.h"

TEST(inv3, test) {
  double M[3][3];
  inv3(M, kBradford, det3(kBradford));
  EXPECT_DBLMATRIXEQ(7, rint, 3, 3, M, "\n\
 .9869929 -.1470543  .1599627\n\
 .4323053  .5183603  .0492912\n\
-.0085287  .0400428  .9684867");
}

BENCH(inv3, bench) {
  double M[3][3], d;
  EZBENCH2("det3", donothing, EXPROPRIATE((d = det3(kBradford))));
  EZBENCH2("inv3", donothing, EXPROPRIATE(inv3(M, kBradford, d)));
}
