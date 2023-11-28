//===-- lib/negsf2.c - single-precision negation ------------------*- C -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements single-precision soft-float negation.
//
//===----------------------------------------------------------------------===//

__static_yoink("huge_compiler_rt_license");

#define SINGLE_PRECISION
#include "third_party/compiler_rt/fp_lib.inc"

COMPILER_RT_ABI fp_t
__negsf2(fp_t a) {
    return fromRep(toRep(a) ^ signBit);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_fneg(fp_t a) {
  return __negsf2(a);
}
#else
AEABI_RTABI fp_t __aeabi_fneg(fp_t a) COMPILER_RT_ALIAS(__negsf2);
#endif
#endif
