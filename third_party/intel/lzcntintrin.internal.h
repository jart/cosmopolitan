#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <lzcntintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _LZCNTINTRIN_H_INCLUDED
#define _LZCNTINTRIN_H_INCLUDED
#ifndef __LZCNT__
#pragma GCC push_options
#pragma GCC target("lzcnt")
#define __DISABLE_LZCNT__
#endif
extern __inline unsigned short __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lzcnt16 (unsigned short __X)
{
  return __builtin_ia32_lzcnt_u16 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lzcnt32 (unsigned int __X)
{
  return __builtin_ia32_lzcnt_u32 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_lzcnt_u32 (unsigned int __X)
{
  return __builtin_ia32_lzcnt_u32 (__X);
}
#ifdef __x86_64__
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lzcnt64 (unsigned long long __X)
{
  return __builtin_ia32_lzcnt_u64 (__X);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_lzcnt_u64 (unsigned long long __X)
{
  return __builtin_ia32_lzcnt_u64 (__X);
}
#endif
#ifdef __DISABLE_LZCNT__
#undef __DISABLE_LZCNT__
#pragma GCC pop_options
#endif
#endif
#endif
