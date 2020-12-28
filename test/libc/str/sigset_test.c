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
