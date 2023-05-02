#ifndef COSMOPOLITAN_LIBC_TINYMATH_ESTRIN_WRAP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_ESTRIN_WRAP_INTERNAL_H_

/*
 * Helper macros for double-precision Estrin polynomial evaluation.
 *
 * Copyright (c) 2022-2023, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

// clang-format off
#define  ESTRIN_1_(x,                  c, i) FMA(x,   c(1 + i),                        c(i))
#define  ESTRIN_2_(x, x2,              c, i) FMA(x2,  c(2 + i),                        ESTRIN_1_(x,              c, i))
#define  ESTRIN_3_(x, x2,              c, i) FMA(x2,  ESTRIN_1_(x,         c,  2 + i), ESTRIN_1_(x,              c, i))
#define  ESTRIN_4_(x, x2, x4,          c, i) FMA(x4,  c(4 + i),                        ESTRIN_3_(x, x2,          c, i))
#define  ESTRIN_5_(x, x2, x4,          c, i) FMA(x4,  ESTRIN_1_(x,         c,  4 + i), ESTRIN_3_(x, x2,          c, i))
#define  ESTRIN_6_(x, x2, x4,          c, i) FMA(x4,  ESTRIN_2_(x, x2,     c,  4 + i), ESTRIN_3_(x, x2,          c, i))
#define  ESTRIN_7_(x, x2, x4,          c, i) FMA(x4,  ESTRIN_3_(x, x2,     c,  4 + i), ESTRIN_3_(x, x2,          c, i))
#define  ESTRIN_8_(x, x2, x4, x8,      c, i) FMA(x8,  c(8 + i),                        ESTRIN_7_(x, x2, x4,      c, i))
#define  ESTRIN_9_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_1_(x,         c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_10_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_2_(x, x2,     c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_11_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_3_(x, x2,     c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_12_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_4_(x, x2, x4, c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_13_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_5_(x, x2, x4, c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_14_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_6_(x, x2, x4, c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_15_(x, x2, x4, x8,      c, i) FMA(x8,  ESTRIN_7_(x, x2, x4, c,  8 + i), ESTRIN_7_(x, x2, x4,      c, i))
#define ESTRIN_16_(x, x2, x4, x8, x16, c, i) FMA(x16, c(16 + i),                       ESTRIN_15_(x, x2, x4, x8, c, i))
#define ESTRIN_17_(x, x2, x4, x8, x16, c, i) FMA(x16, ESTRIN_1_(x,         c, 16 + i), ESTRIN_15_(x, x2, x4, x8, c, i))
#define ESTRIN_18_(x, x2, x4, x8, x16, c, i) FMA(x16, ESTRIN_2_(x, x2,     c, 16 + i), ESTRIN_15_(x, x2, x4, x8, c, i))
#define ESTRIN_19_(x, x2, x4, x8, x16, c, i) FMA(x16, ESTRIN_3_(x, x2,     c, 16 + i), ESTRIN_15_(x, x2, x4, x8, c, i))

#define  ESTRIN_1(x,                  c)  ESTRIN_1_(x,                  c, 0)
#define  ESTRIN_2(x, x2,              c)  ESTRIN_2_(x, x2,              c, 0)
#define  ESTRIN_3(x, x2,              c)  ESTRIN_3_(x, x2,              c, 0)
#define  ESTRIN_4(x, x2, x4,          c)  ESTRIN_4_(x, x2, x4,          c, 0)
#define  ESTRIN_5(x, x2, x4,          c)  ESTRIN_5_(x, x2, x4,          c, 0)
#define  ESTRIN_6(x, x2, x4,          c)  ESTRIN_6_(x, x2, x4,          c, 0)
#define  ESTRIN_7(x, x2, x4,          c)  ESTRIN_7_(x, x2, x4,          c, 0)
#define  ESTRIN_8(x, x2, x4, x8,      c)  ESTRIN_8_(x, x2, x4, x8,      c, 0)
#define  ESTRIN_9(x, x2, x4, x8,      c)  ESTRIN_9_(x, x2, x4, x8,      c, 0)
#define ESTRIN_10(x, x2, x4, x8,      c) ESTRIN_10_(x, x2, x4, x8,      c, 0)
#define ESTRIN_11(x, x2, x4, x8,      c) ESTRIN_11_(x, x2, x4, x8,      c, 0)
#define ESTRIN_12(x, x2, x4, x8,      c) ESTRIN_12_(x, x2, x4, x8,      c, 0)
#define ESTRIN_13(x, x2, x4, x8,      c) ESTRIN_13_(x, x2, x4, x8,      c, 0)
#define ESTRIN_14(x, x2, x4, x8,      c) ESTRIN_14_(x, x2, x4, x8,      c, 0)
#define ESTRIN_15(x, x2, x4, x8,      c) ESTRIN_15_(x, x2, x4, x8,      c, 0)
#define ESTRIN_16(x, x2, x4, x8, x16, c) ESTRIN_16_(x, x2, x4, x8, x16, c, 0)
#define ESTRIN_17(x, x2, x4, x8, x16, c) ESTRIN_17_(x, x2, x4, x8, x16, c, 0)
#define ESTRIN_18(x, x2, x4, x8, x16, c) ESTRIN_18_(x, x2, x4, x8, x16, c, 0)
#define ESTRIN_19(x, x2, x4, x8, x16, c) ESTRIN_19_(x, x2, x4, x8, x16, c, 0)
// clang-format on

#endif /* COSMOPOLITAN_LIBC_TINYMATH_ESTRIN_WRAP_INTERNAL_H_ */
