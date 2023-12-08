/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(memrchr, testNotFoundSse) {
  char buf[16] = {0};
  ASSERT_EQ(NULL, memrchr(buf, 1, 16));
}

TEST(memrchr, testNotFoundPure) {
  char buf[15] = {0};
  ASSERT_EQ(NULL, memrchr(buf, 1, 15));
}

TEST(memrchr, testSse) {
  char buf[16];
  rngset(buf, sizeof(buf), lemur64, -1);
  ASSERT_EQ(buf + 0, memrchr(buf, buf[0], 16));
  ASSERT_EQ(buf + 15, memrchr(buf, buf[15], 16));
}

TEST(memrchr, testPure) {
  char buf[15];
  rngset(buf, sizeof(buf), lemur64, -1);
  ASSERT_EQ(buf + 0, memrchr(buf, buf[0], 15));
  ASSERT_EQ(buf + 14, memrchr(buf, buf[14], 15));
}

TEST(memrchr, testSse2) {
  char buf[32];
  rngset(buf, sizeof(buf), lemur64, -1);
  ASSERT_EQ(buf + 0, memrchr(buf, buf[0], 32));
  ASSERT_EQ(buf + 15, memrchr(buf, buf[15], 32));
  ASSERT_EQ(buf + 16, memrchr(buf, buf[16], 32));
  ASSERT_EQ(buf + 31, memrchr(buf, buf[31], 32));
}

TEST(memrchr, testSsePure) {
  char buf[20];
  rngset(buf, sizeof(buf), lemur64, -1);
  ASSERT_EQ(buf + 0, memrchr(buf, buf[0], 20));
  ASSERT_EQ(buf + 15, memrchr(buf, buf[15], 20));
  ASSERT_EQ(buf + 16, memrchr(buf, buf[16], 20));
  ASSERT_EQ(buf + 19, memrchr(buf, buf[19], 20));
}

BENCH(memrchr, bench) {
  void *memrchr_(const void *, int, size_t) asm("memrchr");
  EZBENCH2("memrchr", donothing, memrchr_(kHyperion, 255, kHyperionSize));
}
