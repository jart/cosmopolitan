#if !defined _X86INTRIN_H_INCLUDED && !defined _IMMINTRIN_H_INCLUDED
#error "Never use <lzcntintrin.h> directly; include <x86intrin.h> instead."
#endif

#ifndef _LZCNTINTRIN_H_INCLUDED
#define _LZCNTINTRIN_H_INCLUDED

#ifndef __LZCNT__
#pragma GCC push_options
#pragma GCC target("lzcnt")
#define __DISABLE_LZCNT__
#endif /* __LZCNT__ */

__funline unsigned short __lzcnt16(unsigned short __X) {
  return __builtin_ia32_lzcnt_u16(__X);
}

__funline unsigned int __lzcnt32(unsigned int __X) {
  return __builtin_ia32_lzcnt_u32(__X);
}

__funline unsigned int _lzcnt_u32(unsigned int __X) {
  return __builtin_ia32_lzcnt_u32(__X);
}

#ifdef __x86_64__
__funline unsigned long long __lzcnt64(unsigned long long __X) {
  return __builtin_ia32_lzcnt_u64(__X);
}

__funline unsigned long long _lzcnt_u64(unsigned long long __X) {
  return __builtin_ia32_lzcnt_u64(__X);
}
#endif

#ifdef __DISABLE_LZCNT__
#undef __DISABLE_LZCNT__
#pragma GCC pop_options
#endif /* __DISABLE_LZCNT__ */

#endif /* _LZCNTINTRIN_H_INCLUDED */
