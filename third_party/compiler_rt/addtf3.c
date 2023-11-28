//===-- lib/addtf3.c - Quad-precision addition --------------------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements quad-precision soft-float addition with the IEEE-754
// default rounding (to nearest, ties to even).
//
//===----------------------------------------------------------------------===//
#include "libc/math.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

__static_yoink("huge_compiler_rt_license");

#define QUAD_PRECISION
#include "third_party/compiler_rt/fp_lib.inc"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#include "third_party/compiler_rt/fp_add_impl.inc"

COMPILER_RT_ABI long double __addtf3(long double a, long double b){
    return __addXf3__(a, b);
}

#endif

#endif /* long double is long */
