/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/mem/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define sqrtl(x) sqrtl(VEIL("t", (long double)(x)))
#define sqrt(x)  sqrt(VEIL("x", (double)(x)))
#define sqrtf(x) sqrtf(VEIL("x", (float)(x)))

TEST(sqrtl, test) {
  EXPECT_STREQ("7", gc(xdtoal(sqrtl(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoal(sqrtl(NAN))));
  EXPECT_STREQ("0", gc(xdtoal(sqrtl(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(sqrtl(INFINITY))));
  EXPECT_STREQ("-NAN", gc(xdtoal(sqrtl(-1))));
}

TEST(sqrt, test) {
  EXPECT_STREQ("7", gc(xdtoa(sqrt(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoa(sqrt(NAN))));
  EXPECT_STREQ("0", gc(xdtoa(sqrt(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(sqrt(INFINITY))));
  EXPECT_STREQ("-NAN", gc(xdtoa(sqrt(-1))));
}

TEST(sqrtf, test) {
  EXPECT_STREQ("7", gc(xdtoaf(sqrtf(7 * 7))));
  EXPECT_STREQ("NAN", gc(xdtoaf(sqrtf(NAN))));
  EXPECT_STREQ("0", gc(xdtoaf(sqrtf(0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(sqrtf(INFINITY))));
  EXPECT_STREQ("-NAN", gc(xdtoaf(sqrtf(-1))));
}
