#ifndef COSMOPOLITAN_LIBC_TINYMATH_HORNERF_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_HORNERF_INTERNAL_H_
#include "third_party/libcxx/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*
 * Helper macros for double-precision Horner polynomial evaluation.
 *
 * Copyright (c) 2022-2023, Arm Limited.
 * SPDX-License-Identifier: MIT OR Apache-2.0 WITH LLVM-exception
 */

#if WANT_VMATH
#define FMA(x, y, z) vfmaq_f32(z, x, y)
#else
#define FMA fmaf
#endif

#include "libc/tinymath/horner_wrap.internal.h"

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_HORNERF_INTERNAL_H_ */
