/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(erf, test) {
  EXPECT_STREQ("NAN", gc(xdtoa(erf(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(erf(0))));
  EXPECT_STREQ("1", gc(xdtoa(erf(INFINITY))));
  EXPECT_STREQ(".999977909503001", gc(xdtoa(erf(3))));
}

TEST(erfc, test) {
  EXPECT_STREQ("NAN", gc(xdtoa(erfc(NAN))));
  EXPECT_STREQ("1", gc(xdtoa(erfc(0))));
  EXPECT_STREQ("1", gc(xdtoa(erfc(-0.))));
  EXPECT_STREQ("0", gc(xdtoa(erfc(INFINITY))));
  EXPECT_STREQ("2", gc(xdtoa(erfc(-INFINITY))));
  EXPECT_STREQ("2.20904969985854e-05", gc(xdtoa(erfc(3))));
}
