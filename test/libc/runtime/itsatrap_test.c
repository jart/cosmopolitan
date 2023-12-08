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
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

/**
 * @fileoverview Tests for arithmetic overflow traps.
 *
 * This module assumes -ftrapv, but not -fsanitize=undefined; since
 * Ubsan provides a superset trapping functionality, and therefore
 * overrides the prior. The nice thing about -ftrapv is that it doesn't
 * leak huge amounts of information into the binary. So it's appropriate
 * to enable in a release build.
 *
 * @note LLVM's implementation of the runtime for this crashes due to
 *     relying on undefined behavior lool, the very thing the flag was
 *     meant to help prevent, so we don't get punked by the compiler
 * @see __addvsi3, __mulvsi3, etc.
 */

#ifndef __llvm__ /* TODO(jart): wut */

volatile bool overflowed_;

void __on_arithmetic_overflow(void) {
  overflowed_ = true;
}

void SetUp(void) {
  overflowed_ = false;
}

/* 32-BIT SIGNED NEGATION */

TEST(__negvsi2, testMax) {
  EXPECT_EQ(-INT_MAX, -__veil("r", INT_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__negvsi2, testMin0) {
  __expropriate(-__veil("r", INT_MIN));
  EXPECT_TRUE(overflowed_);
}

/* 64-BIT SIGNED NEGATION */

TEST(__negvdi2, testMax) {
  EXPECT_EQ(-LONG_MAX, -__veil("r", LONG_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__negvdi2, testMin0) {
  __expropriate(-__veil("r", LONG_MIN));
  EXPECT_TRUE(overflowed_);
}

/* 32-BIT SIGNED MULTIPLICATION */

TEST(__mulvsi3, testMin0) {
  EXPECT_EQ(0, 0 * __veil("r", INT_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvsi3, testMin1) {
  EXPECT_EQ(INT_MIN, 1 * __veil("r", INT_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvsi3, testMin2) {
  __expropriate(2 * __veil("r", INT_MIN));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvsi3, testMax0) {
  EXPECT_EQ(0, 0 * __veil("r", INT_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvsi3, testMax1) {
  EXPECT_EQ(INT_MAX, 1 * __veil("r", INT_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvsi3, testMax2) {
  __expropriate(2 * __veil("r", INT_MAX));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvsi3, test7) {
  EXPECT_EQ(0x70000000, 7 * __veil("r", 0x10000000));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvsi3, test8) {
  __expropriate(8 * __veil("r", 0x10000000));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvsi3, test31337) {
  __expropriate(0x31337 * __veil("r", 0x31337));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvsi3, standAndDeliver_aNegativeTimesANegativeEqualsAPositive) {
  EXPECT_EQ(25, -5 * __veil("r", -5));
  EXPECT_FALSE(overflowed_);
}

/* 64-BIT SIGNED MULTIPLICATION */

TEST(__mulvdi3, testMin0) {
  EXPECT_EQ(0, 0 * __veil("r", LONG_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, testMin1) {
  EXPECT_EQ(LONG_MIN, 1 * __veil("r", LONG_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, testMin2) {
  __expropriate(2 * __veil("r", LONG_MIN));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvdi3, testMax0) {
  EXPECT_EQ(0, 0 * __veil("r", LONG_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, testMax1) {
  EXPECT_EQ(LONG_MAX, 1 * __veil("r", LONG_MAX));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, testMax2) {
  __expropriate(2 * __veil("r", LONG_MAX));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvdi3, test7) {
  EXPECT_EQ(0x7000000000000000l, 7 * __veil("r", 0x1000000000000000l));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, test8) {
  __expropriate(8 * __veil("r", 0x1000000000000000l));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvdi3, test31337) {
  __expropriate(0x3133700000000l * __veil("r", 0x3133700000000l));
  EXPECT_TRUE(overflowed_);
}

TEST(__mulvdi3, standAndDeliver_aNegativeTimesANegativeEqualsAPositive) {
  EXPECT_EQ(25l, -5l * __veil("r", -5l));
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvdi3, testOverflow) {
  volatile int64_t x;
  x = 3037000500;
  x *= 3037000499;
  EXPECT_FALSE(overflowed_);
  x = 3037000500;
  x *= 3037000500;
  EXPECT_TRUE(overflowed_);
}

/* 32-BIT SIGNED ADDITION */

TEST(__addvsi3, testMin1) {
  EXPECT_EQ(INT_MIN + 1, 1 + __veil("r", INT_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__addvsi3, testMax1) {
  __expropriate(1 + __veil("r", INT_MAX));
  EXPECT_TRUE(overflowed_);
}

TEST(__addvsi3, testNegPos) {
  EXPECT_EQ(2, -2 + __veil("r", 4));
  EXPECT_FALSE(overflowed_);
}

TEST(__addvsi3, testPosNeg) {
  EXPECT_EQ(-2, 2 + __veil("r", -4));
  EXPECT_FALSE(overflowed_);
}

/* 64-BIT SIGNED ADDITION */

TEST(__addvdi3, testMin1) {
  EXPECT_EQ(LONG_MIN + 1, 1 + __veil("r", LONG_MIN));
  EXPECT_FALSE(overflowed_);
}

TEST(__addvdi3, testMax1) {
  __expropriate(1 + __veil("r", LONG_MAX));
  EXPECT_TRUE(overflowed_);
}

TEST(__addvdi3, testNegPos) {
  EXPECT_EQ(2l, -2l + __veil("r", 4l));
  EXPECT_FALSE(overflowed_);
}

TEST(__addvdi3, testPosNeg) {
  EXPECT_EQ(-2l, 2l + __veil("r", -4l));
  EXPECT_FALSE(overflowed_);
}

/* 32-BIT SIGNED SUBTRACTION */

TEST(__subvsi3, testMin1) {
  __expropriate(__veil("r", INT_MIN) - 1);
  EXPECT_TRUE(overflowed_);
}

TEST(__subvsi3, testMax1) {
  EXPECT_EQ(INT_MAX - 1, __veil("r", INT_MAX) - 1);
  EXPECT_FALSE(overflowed_);
}

TEST(__subvsi3, testPosNeg) {
  EXPECT_EQ(-2, 2 - __veil("r", 4));
  EXPECT_FALSE(overflowed_);
}

/* 64-BIT SIGNED SUBTRACTION */

TEST(__subvdi3, testMin1) {
  __expropriate(__veil("r", LONG_MIN) - 1);
  EXPECT_TRUE(overflowed_);
}

TEST(__subvdi3, testMax1) {
  EXPECT_EQ(LONG_MAX - 1, __veil("r", LONG_MAX) - 1);
  EXPECT_FALSE(overflowed_);
}

/* 128-BIT SIGNED ADDITION */

TEST(__addvti3, testMath) {
  volatile int128_t x;
  x = 2;
  x += 2;
  EXPECT_EQ(4, x);
  x = -2;
  x += -2;
  EXPECT_EQ(-4, x);
  x = UINT64_MAX;
  x += 1;
  EXPECT_EQ((int128_t)UINT64_MAX + 1, x);
  EXPECT_FALSE(overflowed_);
}

TEST(__addvti3, testOverflow) {
  volatile int128_t x;
  x = INT128_MAX;
  x += 1;
  EXPECT_TRUE(overflowed_);
}

/* 128-BIT SIGNED SUBTRACTION */

TEST(__subvti3, testMath) {
  volatile int128_t x;
  x = -2;
  x -= 2;
  EXPECT_EQ(-4, x);
  x = UINT64_MIN;
  x -= 1;
  EXPECT_EQ((int128_t)UINT64_MIN - 1, x);
  EXPECT_FALSE(overflowed_);
}

TEST(__subvti3, testOverflow) {
  volatile int128_t x;
  x = INT128_MIN;
  x -= 1;
  EXPECT_TRUE(overflowed_);
}

/* 128-BIT SIGNED NEGATION */

TEST(__negvti3, testMath) {
  volatile int128_t x;
  x = -2;
  x = -x;
  EXPECT_EQ(2, x);
  EXPECT_FALSE(overflowed_);
  x = INT128_MAX;
  x = -x;
  EXPECT_EQ(INT128_MIN + 1, x);
  EXPECT_FALSE(overflowed_);
  x = (uint128_t)0x8000000000000000 << 64 | 0x8000000000000000;
  x = -x;
  EXPECT_EQ((uint128_t)0x7fffffffffffffff << 64 | 0x8000000000000000, x);
  EXPECT_FALSE(overflowed_);
}

TEST(__negvti3, testOverflow) {
  volatile int128_t x;
  x = INT128_MIN;
  x = -x;
  EXPECT_TRUE(overflowed_);
}

/* 128-BIT SIGNED MULTIPLICATION */

TEST(__mulvti3, testMath) {
  volatile int128_t x;
  x = 7;
  x *= 11;
  EXPECT_EQ(77, x);
  EXPECT_FALSE(overflowed_);
  x = 0x1fffffffffffffff;
  x *= 0x1fffffffffffffff;
  EXPECT_EQ((uint128_t)0x3ffffffffffffff << 64 | 0xc000000000000001, x);
  EXPECT_FALSE(overflowed_);
  x = -0x1fffffffffffffff;
  x *= 0x1fffffffffffffff;
  EXPECT_EQ((uint128_t)0xfc00000000000000 << 64 | 0x3fffffffffffffff, x);
  EXPECT_FALSE(overflowed_);
}

TEST(__mulvti3, testOverflow) {
  volatile int128_t x;
  x = 0xb504f333f9de5be0;
  x *= 0xb504f333f9de6d28;
  EXPECT_FALSE(overflowed_);
  x = 0xb504f333f9de5be0;
  x *= 0xb504f333f9de6d29;
  EXPECT_TRUE(overflowed_);
}

#endif /* __llvm__ */
