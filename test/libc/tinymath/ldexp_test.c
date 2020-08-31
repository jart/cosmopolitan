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
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(ldexp, test) {
  volatile int twopow = 5;
  volatile double pi = 3.14;
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", ldexp(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", ldexpf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", ldexpl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalb(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalbl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbn(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbnf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalbnl(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalbln(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2f", scalblnf(pi, twopow))));
  ASSERT_STREQ("100.48", gc(xasprintf("%.2Lf", scalblnl(pi, twopow))));
}
