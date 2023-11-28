/* ===-- fixsfsi.c - Implement __fixsfsi -----------------------------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 */

__static_yoink("huge_compiler_rt_license");

#define SINGLE_PRECISION
#include "third_party/compiler_rt/fp_lib.inc"
typedef si_int fixint_t;
typedef su_int fixuint_t;
#include "third_party/compiler_rt/fp_fixint_impl.inc"

COMPILER_RT_ABI si_int
__fixsfsi(fp_t a) {
    return __fixint(a);
}

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI si_int __aeabi_f2iz(fp_t a) {
  return __fixsfsi(a);
}
#else
AEABI_RTABI si_int __aeabi_f2iz(fp_t a) COMPILER_RT_ALIAS(__fixsfsi);
#endif
#endif
