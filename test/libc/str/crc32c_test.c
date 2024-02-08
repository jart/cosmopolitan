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
#include "libc/dce.h"
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

TEST(crc32c, test) {
  EXPECT_EQ(0, crc32c(0, "", 0));
  EXPECT_EQ(crc32c(0, "hello", 5), crc32c(0, "hello", 5));
  EXPECT_EQ(0xe3069283, crc32c(0, "123456789", 9));
  EXPECT_EQ(0x6d6eefba, crc32c(0, hyperion, strlen(hyperion)));
  EXPECT_EQ(0x6d6eefba, crc32c(crc32c(0, FANATICS, strlen(FANATICS)),
                               hyperion + strlen(FANATICS),
                               strlen(hyperion) - strlen(FANATICS)));
  EXPECT_EQ(0xf372f045, crc32c(0, hyperion + 1, strlen(hyperion) - 1));
  EXPECT_EQ(0x5aaad5f8, crc32c(0, hyperion + 7, strlen(hyperion) - 7));
  EXPECT_EQ(0xf8e51ea6, crc32c(0, hyperion + 7, strlen(hyperion) - 8));
  EXPECT_EQ(0xecc9871d, crc32c(0, kHyperion, kHyperionSize));
}

dontinline uint64_t fnv_hash(char *s, int len) {
  uint64_t hash = 0xcbf29ce484222325;
  for (int i = 0; i < len; i++) {
    hash *= 0x100000001b3;
    hash ^= (unsigned char)s[i];
  }
  return hash;
}

static unsigned KMH(const void *p, unsigned long n) {
  unsigned h, i;
  for (h = i = 0; i < n; i++) {
    h += ((unsigned char *)p)[i];
    h *= 0x9e3779b1;
  }
  return MAX(1, h);
}

BENCH(crc32c, bench) {
  for (int i = 1; i < 256; i *= 2) {
    EZBENCH_N("crc32c", i, __expropriate(crc32c(0, kHyperion, i)));
    EZBENCH_N("crc32_z", i, __expropriate(crc32_z(0, kHyperion, i)));
    EZBENCH_N("fnv_hash", i,
              __expropriate(fnv_hash(__veil("r", kHyperion), __veil("r", i))));
    EZBENCH_N("KMH", i,
              __expropriate(KMH(__veil("r", kHyperion), __veil("r", i))));
    fprintf(stderr, "\n");
  }
  EZBENCH_N("crc32c", kHyperionSize,
            __expropriate(crc32c(0, kHyperion, kHyperionSize)));
  EZBENCH_N("crc32_z", kHyperionSize,
            __expropriate(crc32_z(0, kHyperion, kHyperionSize)));
  EZBENCH_N("fnv_hash", kHyperionSize,
            __expropriate(
                fnv_hash(__veil("r", kHyperion), __veil("r", kHyperionSize))));
  EZBENCH_N(
      "KMH", kHyperionSize,
      __expropriate(KMH(__veil("r", kHyperion), __veil("r", kHyperionSize))));
}
