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
#include "libc/calls/calls.h"
#include "libc/intrin/popcnt.h"
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  ASSERT_SYS(0, 0, pledge("stdio", 0));
}

TEST(_countbits, testLow) {
  int i;
  char p[2];
  for (i = 0; i < 65536; ++i) {
    p[0] = i >> 0;
    p[1] = i >> 8;
    ASSERT_EQ(__builtin_popcount(i), _countbits(p, 2), "i=%d", i);
  }
}

TEST(_countbits, test) {
  int i;
  char *p = gc(memset(malloc(256), 0x55, 256));
  for (i = 0; i < 256; ++i) {
    ASSERT_EQ(i * CHAR_BIT / 2, _countbits(p, i), "i=%d", i);
  }
}

BENCH(countbits, bench) {
  EZBENCH_N("_countbits", 7, _countbits(kHyperion, 7));
  EZBENCH_N("_countbits", 8, _countbits(kHyperion, 8));
  EZBENCH_N("_countbits", 16, _countbits(kHyperion, 16));
  EZBENCH_N("_countbits", kHyperionSize, _countbits(kHyperion, kHyperionSize));
}
