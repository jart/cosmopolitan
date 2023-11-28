#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _POPCNTINTRIN_H_INCLUDED
#define _POPCNTINTRIN_H_INCLUDED
#ifndef __POPCNT__
#pragma GCC push_options
#pragma GCC target("popcnt")
#define __DISABLE_POPCNT__
#endif
extern __inline int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_popcnt_u32 (unsigned int __X)
{
  return __builtin_popcount (__X);
}
#ifdef __x86_64__
extern __inline long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_popcnt_u64 (unsigned long long __X)
{
  return __builtin_popcountll (__X);
}
#endif
#ifdef __DISABLE_POPCNT__
#undef __DISABLE_POPCNT__
#pragma GCC pop_options
#endif
#endif
#endif
