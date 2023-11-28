//===-- lib/extenddftf2.c - double -> quad conversion -------------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//

__static_yoink("huge_compiler_rt_license");

#define QUAD_PRECISION
#include "third_party/compiler_rt/fp_lib.inc"

#if defined(CRT_HAS_128BIT) && defined(CRT_LDBL_128BIT)
#define SRC_DOUBLE
#define DST_QUAD
#include "third_party/compiler_rt/fp_extend_impl.inc"

COMPILER_RT_ABI long double __extenddftf2(double a) {
    return __extendXfYf2__(a);
}

#endif
