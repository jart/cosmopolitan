/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/inttypes.h"
#include "libc/limits.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

#define sscanf1(STR, FMT)               \
  ({                                    \
    errno = 0;                          \
    int128_t x = 0;                     \
    ASSERT_EQ(1, sscanf(STR, FMT, &x)); \
    x;                                  \
  })

TEST(sscanf, testMultiple) {
  int a, b, c;
  ASSERT_EQ(3, sscanf("123 123 123", "%d %x %o", &a, &b, &c));
  ASSERT_EQ(123, a);
  ASSERT_EQ(0x123, b);
  ASSERT_EQ(0123, c);
}

TEST(sscanf, testDecimal) {
  ASSERT_EQ(123, sscanf1("123", "%d"));
  ASSERT_EQ(123, sscanf1("123", "%u"));
  ASSERT_EQ((uint32_t)-123, sscanf1("-123", "%d"));
}

TEST(sscanf, testHex) {
  ASSERT_EQ(0x123, sscanf1("123", "%x"));
  ASSERT_EQ(0x123, sscanf1("0x123", "%x"));
  ASSERT_EQ(0x123, sscanf1("0123", "%x"));
  ASSERT_EQ(INT128_MAX,
            sscanf1("170141183460469231731687303715884105727", "%jjd"));
  ASSERT_EQ(INT128_MIN,
            sscanf1("-170141183460469231731687303715884105728", "%jjd"));
  ASSERT_EQ(UINT128_MAX, sscanf1("0xffffffffffffffffffffffffffffffff", "%jjx"));
}

TEST(sscanf, testOctal) {
  ASSERT_EQ(0123, sscanf1("123", "%o"));
  ASSERT_EQ(0123, sscanf1("0123", "%o"));
}

TEST(sscanf, testInttypes_macros) {
  int8_t i8 = (int8_t)0xFFFFFFFFFFFFFFFF;
  uint8_t u8 = (uint8_t)0xFFFFFFFFFFFFFFFF;
  int16_t i16 = (int16_t)0xFFFFFFFFFFFFFFFF;
  uint16_t u16 = (uint16_t)0xFFFFFFFFFFFFFFFF;
  int32_t i32 = (int32_t)0xFFFFFFFFFFFFFFFF;
  uint32_t u32 = (uint32_t)0xFFFFFFFFFFFFFFFF;
  int64_t i64 = (int64_t)0xFFFFFFFFFFFFFFFF;
  uint64_t u64 = (uint64_t)0xFFFFFFFFFFFFFFFF;
  intmax_t imax = (intmax_t)0xFFFFFFFFFFFFFFFF;
  uintmax_t umax = (uintmax_t)0xFFFFFFFFFFFFFFFF;
  int_least8_t il8 = (int_least8_t)0xFFFFFFFFFFFFFFFF;
  uint_least8_t ul8 = (uint_least8_t)0xFFFFFFFFFFFFFFFF;
  int_least16_t il16 = (int_least16_t)0xFFFFFFFFFFFFFFFF;
  uint_least16_t ul16 = (uint_least16_t)0xFFFFFFFFFFFFFFFF;
  int_least32_t il32 = (int_least32_t)0xFFFFFFFFFFFFFFFF;
  uint_least32_t ul32 = (uint_least32_t)0xFFFFFFFFFFFFFFFF;
  int_least64_t il64 = (int_least64_t)0xFFFFFFFFFFFFFFFF;
  uint_least64_t ul64 = (uint_least64_t)0xFFFFFFFFFFFFFFFF;
  int_fast8_t if8 = (int_fast8_t)0xFFFFFFFFFFFFFFFF;
  uint_fast8_t uf8 = (uint_fast8_t)0xFFFFFFFFFFFFFFFF;
  int_fast16_t if16 = (int_fast16_t)0xFFFFFFFFFFFFFFFF;
  uint_fast16_t uf16 = (uint_fast16_t)0xFFFFFFFFFFFFFFFF;
  int_fast32_t if32 = (int_fast32_t)0xFFFFFFFFFFFFFFFF;
  uint_fast32_t uf32 = (uint_fast32_t)0xFFFFFFFFFFFFFFFF;
  int_fast64_t if64 = (int_fast64_t)0xFFFFFFFFFFFFFFFF;
  uint_fast64_t uf64 = (uint_fast64_t)0xFFFFFFFFFFFFFFFF;

  ASSERT_EQ(sscanf("1", "%" SCNd8, &i8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu8, &u8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd16, &i16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu16, &u16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd32, &i32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu32, &u32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNd64, &i64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNu64, &u64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdMAX, &imax), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuMAX, &umax), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST8, &il8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST8, &ul8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST16, &il16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST16, &ul16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST32, &il32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST32, &ul32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdLEAST64, &il64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuLEAST64, &ul64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST8, &if8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST8, &uf8), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST16, &if16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST16, &uf16), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST32, &if32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST32, &uf32), 1);
  ASSERT_EQ(sscanf("1", "%" SCNdFAST64, &if64), 1);
  ASSERT_EQ(sscanf("1", "%" SCNuFAST64, &uf64), 1);

  ASSERT_EQ(i8, 1);
  ASSERT_EQ(u8, 1);
  ASSERT_EQ(i16, 1);
  ASSERT_EQ(u16, 1);
  ASSERT_EQ(i32, 1);
  ASSERT_EQ(u32, 1);
  ASSERT_EQ(i64, 1);
  ASSERT_EQ(u64, 1);
  ASSERT_EQ(imax, 1);
  ASSERT_EQ(umax, 1);
  ASSERT_EQ(il8, 1);
  ASSERT_EQ(ul8, 1);
  ASSERT_EQ(il16, 1);
  ASSERT_EQ(ul16, 1);
  ASSERT_EQ(il32, 1);
  ASSERT_EQ(ul32, 1);
  ASSERT_EQ(il64, 1);
  ASSERT_EQ(ul64, 1);
  ASSERT_EQ(if16, 1);
  ASSERT_EQ(uf16, 1);
  ASSERT_EQ(if32, 1);
  ASSERT_EQ(uf32, 1);
  ASSERT_EQ(if64, 1);
  ASSERT_EQ(uf64, 1);
}
