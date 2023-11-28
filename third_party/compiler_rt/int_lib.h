/* ===-- int_lib.h - configuration header for compiler-rt  -----------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file is a configuration header for compiler-rt.
 * This file is not part of the interface of this library.
 *
 * ===----------------------------------------------------------------------===
 */

#ifndef INT_LIB_H
#define INT_LIB_H
#define CRT_HAS_128BIT 1

/* Assumption: lool univac arithmetic */
/* Assumption: lool cray signed shift */
/* Assumption: lool pdp11 float byte order */

#if defined(__ELF__)
#define FNALIAS(alias_name, original_name) \
  void alias_name() __attribute__((__alias__(#original_name)))
#define COMPILER_RT_ALIAS(aliasee) __attribute__((__alias__(#aliasee)))
#else
#define FNALIAS(alias, name) _Pragma("GCC error(\"alias unsupported on this file format\")")
#define COMPILER_RT_ALIAS(aliasee) _Pragma("GCC error(\"alias unsupported on this file format\")")
#endif

/* ABI macro definitions */

#if __ARM_EABI__
# ifdef COMPILER_RT_ARMHF_TARGET
#   define COMPILER_RT_ABI
# else
#   define COMPILER_RT_ABI __attribute__((__pcs__("aapcs")))
# endif
#else
# define COMPILER_RT_ABI
#endif

#define AEABI_RTABI __attribute__((__pcs__("aapcs")))

#ifdef _MSC_VER
#define ALWAYS_INLINE __forceinline
#define NORETURN __declspec(noreturn)
#define UNUSED
#else
#define ALWAYS_INLINE __attribute__((__always_inline__))
#define NORETURN __attribute__((__noreturn__))
#define UNUSED __attribute__((__unused__))
#endif

#include "libc/literal.h"
#include "libc/math.h"
#include "libc/limits.h"

/* Include the commonly used internal type definitions. */
#include "third_party/compiler_rt/int_types.h"

/* Include internal utility function declarations. */
#include "third_party/compiler_rt/int_util.h"

COMPILER_RT_ABI si_int __paritysi2(si_int a);
COMPILER_RT_ABI si_int __paritydi2(di_int a);

COMPILER_RT_ABI di_int __divdi3(di_int a, di_int b);
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b);
COMPILER_RT_ABI su_int __udivsi3(su_int n, su_int d);

COMPILER_RT_ABI su_int __udivmodsi4(su_int a, su_int b, su_int* rem);
COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int* rem);
#ifdef CRT_HAS_128BIT
COMPILER_RT_ABI si_int __clzti2(ti_int a);
COMPILER_RT_ABI ti_int __divmodti4(ti_int a, ti_int b, tu_int *rem);
COMPILER_RT_ABI tu_int __udivmodti4(tu_int a, tu_int b, tu_int* rem);
#endif

/* Definitions for builtins unavailable on MSVC */
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>

uint32_t __inline __builtin_ctz(uint32_t value) {
  unsigned long trailing_zero = 0;
  if (_BitScanForward(&trailing_zero, value))
    return trailing_zero;
  return 32;
}

uint32_t __inline __builtin_clz(uint32_t value) {
  unsigned long leading_zero = 0;
  if (_BitScanReverse(&leading_zero, value))
    return 31 - leading_zero;
  return 32;
}

#if defined(_M_ARM) || defined(_M_X64)
uint32_t __inline __builtin_clzll(uint64_t value) {
  unsigned long leading_zero = 0;
  if (_BitScanReverse64(&leading_zero, value))
    return 63 - leading_zero;
  return 64;
}
#else
uint32_t __inline __builtin_clzll(uint64_t value) {
  if (value == 0)
    return 64;
  uint32_t msh = (uint32_t)(value >> 32);
  uint32_t lsh = (uint32_t)(value & 0xFFFFFFFF);
  if (msh != 0)
    return __builtin_clz(msh);
  return 32 + __builtin_clz(lsh);
}
#endif

#define __builtin_clzl __builtin_clzll
#endif /* defined(_MSC_VER) && !defined(__clang__) */

#endif /* INT_LIB_H */
