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
#include "libc/calls/calls.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

TEST(bitreverse, test) {
  EXPECT_EQ(0xde, BITREVERSE8(123));
  EXPECT_EQ(0xde, bitreverse8(123));
  EXPECT_EQ(0xde00, BITREVERSE16(123));
  EXPECT_EQ(0xde00, bitreverse16(123));
  EXPECT_EQ(0xde000000u, bitreverse32(123));
  EXPECT_EQ(0xde00000000000000ul, bitreverse64(123));
  EXPECT_EQ(0x482d96c305f7c697ul, bitreverse64(0xe963efa0c369b412));
}

BENCH(bitreverse, bench) {
  EZBENCH2("BITREVERSE8", donothing,
           EXPROPRIATE(BITREVERSE8(CONCEAL("r", 123))));
  EZBENCH2("bitreverse8", donothing,
           EXPROPRIATE(bitreverse8(CONCEAL("r", 123))));
  EZBENCH2("BITREVERSE16", donothing,
           EXPROPRIATE(BITREVERSE16(CONCEAL("r", 123))));
  EZBENCH2("bitreverse32", donothing,
           EXPROPRIATE(bitreverse32(CONCEAL("r", 123))));
  EZBENCH2("bitreverse64", donothing,
           EXPROPRIATE(bitreverse64(CONCEAL("r", 123))));
}
