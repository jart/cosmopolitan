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
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/str/tpenc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

STATIC_YOINK("strwidth");

volatile uint64_t v;

TEST(tpenc, test) {
  EXPECT_EQ(0, tpenc(0));
  EXPECT_EQ(1, tpenc(1));
  EXPECT_EQ(' ', tpenc(' '));
  EXPECT_EQ(0x7f, tpenc(0x7f));
  EXPECT_EQ(0x008496E2, tpenc(L'▄'));
  EXPECT_EQ(0x8080808080FEul, tpenc(INT_MIN));
}

uint64_t Tpenc(int x) {
  return (v = tpenc(x));
}

BENCH(tpenc, bench) {
  EZBENCH(donothing, Tpenc(0));
  EZBENCH(donothing, Tpenc(1));
  EZBENCH(donothing, Tpenc(' '));
  EZBENCH(donothing, Tpenc(0x7f));
  EZBENCH(donothing, Tpenc(L'▄'));
  EZBENCH(donothing, Tpenc(INT_MIN));
  fprintf(stderr, "\n");
}
