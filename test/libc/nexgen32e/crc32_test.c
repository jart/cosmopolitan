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
#include "libc/macros.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/zlib/zlib.h"

TEST(crc32, testBigText) {
  size_t size;
  void *hyperion;
  size = kHyperionSize;
  hyperion = kHyperion;
  EXPECT_EQ(0xe9ded8e6, crc32(0, hyperion, size));
  EXPECT_EQ(0xe9ded8e6, crc32_z(0, hyperion, size));
  if (X86_HAVE(PCLMUL)) {
    size = ROUNDDOWN(size, 64);
    EXPECT_EQ(0xc7adc04f, crc32(0, hyperion, size));
    EXPECT_EQ(0xc7adc04f, crc32_z(0, hyperion, size));
    EXPECT_EQ(0xc7adc04f,
              0xffffffffu ^ crc32$pclmul(0 ^ 0xffffffffu, hyperion, size));
  }
}

#define TESTSTR "libc/calls/typedef/sighandler_t.h"

BENCH(crc32c, bench) {
  EZBENCH2("crc32c", donothing,
           EXPROPRIATE(crc32c(0, VEIL("r", TESTSTR), sizeof(TESTSTR) - 1)));
}
