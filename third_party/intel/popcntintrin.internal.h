#ifndef _POPCNTINTRIN_H_INCLUDED
#define _POPCNTINTRIN_H_INCLUDED
#ifdef __x86_64__

#ifndef __POPCNT__
#pragma GCC push_options
#pragma GCC target("popcnt")
#define __DISABLE_POPCNT__
#endif /* __POPCNT__ */

__funline int _mm_popcnt_u32(unsigned int __X) {
  return __builtin_popcount(__X);
}

#ifdef __x86_64__
__funline long long _mm_popcnt_u64(unsigned long long __X) {
  return __builtin_popcountll(__X);
}
#endif

#ifdef __DISABLE_POPCNT__
#undef __DISABLE_POPCNT__
#pragma GCC pop_options
#endif /* __DISABLE_POPCNT__ */

#endif /* __x86_64__ */
#endif /* _POPCNTINTRIN_H_INCLUDED */
