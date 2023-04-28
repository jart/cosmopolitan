#ifndef COSMOPOLITAN_LIBC_TINYMATH_HORNER_WRAP_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_HORNER_WRAP_INTERNAL_H_

/*
 * Helper macros for Horner polynomial evaluation.
 *
 * Copyright (c) 2022-2023, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

// clang-format off
#define  HORNER_1_(x, c, i) FMA(c(i + 1), x, c(i))
#define  HORNER_2_(x, c, i) FMA(HORNER_1_ (x, c, i + 1), x, c(i))
#define  HORNER_3_(x, c, i) FMA(HORNER_2_ (x, c, i + 1), x, c(i))
#define  HORNER_4_(x, c, i) FMA(HORNER_3_ (x, c, i + 1), x, c(i))
#define  HORNER_5_(x, c, i) FMA(HORNER_4_ (x, c, i + 1), x, c(i))
#define  HORNER_6_(x, c, i) FMA(HORNER_5_ (x, c, i + 1), x, c(i))
#define  HORNER_7_(x, c, i) FMA(HORNER_6_ (x, c, i + 1), x, c(i))
#define  HORNER_8_(x, c, i) FMA(HORNER_7_ (x, c, i + 1), x, c(i))
#define  HORNER_9_(x, c, i) FMA(HORNER_8_ (x, c, i + 1), x, c(i))
#define HORNER_10_(x, c, i) FMA(HORNER_9_ (x, c, i + 1), x, c(i))
#define HORNER_11_(x, c, i) FMA(HORNER_10_(x, c, i + 1), x, c(i))
#define HORNER_12_(x, c, i) FMA(HORNER_11_(x, c, i + 1), x, c(i))

#define  HORNER_1(x, c) HORNER_1_ (x, c, 0)
#define  HORNER_2(x, c) HORNER_2_ (x, c, 0)
#define  HORNER_3(x, c) HORNER_3_ (x, c, 0)
#define  HORNER_4(x, c) HORNER_4_ (x, c, 0)
#define  HORNER_5(x, c) HORNER_5_ (x, c, 0)
#define  HORNER_6(x, c) HORNER_6_ (x, c, 0)
#define  HORNER_7(x, c) HORNER_7_ (x, c, 0)
#define  HORNER_8(x, c) HORNER_8_ (x, c, 0)
#define  HORNER_9(x, c) HORNER_9_ (x, c, 0)
#define HORNER_10(x, c) HORNER_10_(x, c, 0)
#define HORNER_11(x, c) HORNER_11_(x, c, 0)
#define HORNER_12(x, c) HORNER_12_(x, c, 0)
// clang-format on

#endif /* COSMOPOLITAN_LIBC_TINYMATH_HORNER_WRAP_INTERNAL_H_ */
