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
#include "libc/calls/struct/timeval.h"
#include "libc/fmt/conv.h"
#include "libc/nt/struct/filetime.h"
#include "libc/testlib/testlib.h"
#include "libc/time/time.h"

TEST(TimeValToFileTime, roundTrip) {
  struct timeval tv1, tv2;
  tv1.tv_sec = 31337;
  tv1.tv_usec = 1337;
  FileTimeToTimeVal(&tv2, TimeValToFileTime(&tv1));
  EXPECT_EQ(31337, tv2.tv_sec);
  EXPECT_EQ(1337, tv2.tv_usec);
}
