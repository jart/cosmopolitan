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
#include "libc/intrin/phaddsw.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "tool/viz/lib/formatstringtable-testlib.h"
/* clang-format off */

FIXTURE(phaddsw, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

TEST(phaddsw, testOverflow_saturates) {
  short M[2][8] = {
    {0x7fff,      0, 0x7fff,      1, 0x7fff, 0x7fff,  20777, -16389},
    {-28040,  13318,  -1336, -24798, -13876,   3599,  -7346, -23575},
  };
  phaddsw(M[0], M[0], M[1]);
  EXPECT_SHRTMATRIXEQ(2, 8, M, "\n\
 32767  32767  32767   4388 -14722 -26134 -10277 -30921\n\
-28040  13318  -1336 -24798 -13876   3599  -7346 -23575");
}

TEST(phaddsw, testAliasing_isOk) {
  short M[1][8] = {{0,1,2,3,4,5,6,7}};
  phaddsw(M[0],M[0],M[0]);
  EXPECT_SHRTMATRIXEQ(1, 8, M, "\n\
 1  5  9 13  1  5  9 13");
}
