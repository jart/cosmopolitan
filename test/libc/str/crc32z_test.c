/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/crc32.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "third_party/zlib/zlib.h"

#define FANATICS "Fanatics"

static const char hyperion[] =
    FANATICS " have their dreams, wherewith they weave / "
             "A paradise for a sect; the savage too / "
             "From forth the loftiest fashion of his sleep / "
             "...";

TEST(crc32_z, test) {
  char *p;
  p = gc(strdup(kHyperion));
  EXPECT_EQ(0, crc32_z(0, 0, 0));
  EXPECT_EQ(0, crc32_z(0, "", 0));
  EXPECT_EQ(0xcbf43926, crc32_z(0, "123456789", 9));
  EXPECT_EQ(0xc386e7e4, crc32_z(0, hyperion, strlen(hyperion)));
  EXPECT_EQ(0xc386e7e4, crc32_z(crc32_z(0, FANATICS, strlen(FANATICS)),
                                hyperion + strlen(FANATICS),
                                strlen(hyperion) - strlen(FANATICS)));
  EXPECT_EQ(0xcbfc3df2, crc32_z(0, hyperion + 1, strlen(hyperion) - 1));
  EXPECT_EQ(0x9feb0e30, crc32_z(0, hyperion + 7, strlen(hyperion) - 7));
  EXPECT_EQ(0x5b80e54e, crc32_z(0, hyperion + 7, strlen(hyperion) - 8));
  EXPECT_EQ(0xe9ded8e6, crc32_z(0, p, kHyperionSize));
}

BENCH(crc32_z, bench) {
  EZBENCH_N("crc32_z", kHyperionSize, crc32_z(0, kHyperion, kHyperionSize));
  EZBENCH_N("crc32_z", kHyperionSize, crc32_z(0, kHyperion, kHyperionSize));
}
