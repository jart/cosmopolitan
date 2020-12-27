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
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/sigbits.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

sigset_t ss;

TEST(sigemptyset, test) {
  EXPECT_EQ(0, sigemptyset(&ss));
  EXPECT_BINEQ(u"        ", &ss);
}

TEST(sigfillset, test) {
  EXPECT_EQ(0, sigfillset(&ss));
  EXPECT_BINEQ(u"λλλλλλλλ", &ss);
}

TEST(sigaddset, test) {
  sigemptyset(&ss);
  EXPECT_EQ(0, sigaddset(&ss, 1));
  EXPECT_BINEQ(u"☺       ", &ss);
  EXPECT_EQ(0, sigaddset(&ss, 64));
  EXPECT_BINEQ(u"☺      Ç", &ss);
}

TEST(sigdelset, test) {
  sigfillset(&ss);
  EXPECT_EQ(0, sigdelset(&ss, 1));
  EXPECT_BINEQ(u"■λλλλλλλ", &ss);
  EXPECT_EQ(0, sigdelset(&ss, 64));
  EXPECT_BINEQ(u"■λλλλλλ⌂", &ss);
}

TEST(sigismember, test) {
  sigfillset(&ss);
  EXPECT_TRUE(sigismember(&ss, 1));
  sigdelset(&ss, 1);
  EXPECT_FALSE(sigismember(&ss, 1));
  EXPECT_TRUE(sigismember(&ss, 64));
  sigdelset(&ss, 64);
  EXPECT_FALSE(sigismember(&ss, 64));
}
