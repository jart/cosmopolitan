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
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

char buf[32];

TEST(sinl, testLongDouble) {
  EXPECT_STREQ(".479425538604203", gc(xdtoa(sinl(.5))));
  EXPECT_STREQ("-.479425538604203", gc(xdtoa(sinl(-.5))));
}

TEST(sinl, testDouble) {
  EXPECT_STREQ(".479425538604203", gc(xdtoa(sin(.5))));
  EXPECT_STREQ("-.479425538604203", gc(xdtoa(sin(-.5))));
}

TEST(sinl, testFloat) {
  EXPECT_STARTSWITH(".4794255", gc(xdtoa(sinf(.5f))));
  EXPECT_STARTSWITH("-.4794255", gc(xdtoa(sinf(-.5f))));
}

BENCH(sinl, bench) {
  EZBENCH(donothing, sinl(0.7));  /* ~30ns */
  EZBENCH(donothing, sin(0.7));   /* ~35ns */
  EZBENCH(donothing, sinf(0.7f)); /* ~35ns */
}
