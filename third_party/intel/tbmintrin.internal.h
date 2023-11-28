#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <tbmintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _TBMINTRIN_H_INCLUDED
#define _TBMINTRIN_H_INCLUDED
#ifndef __TBM__
#pragma GCC push_options
#pragma GCC target("tbm")
#define __DISABLE_TBM__
#endif
#ifdef __OPTIMIZE__
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__bextri_u32 (unsigned int __X, const unsigned int __I)
{
  return __builtin_ia32_bextri_u32 (__X, __I);
}
#else
#define __bextri_u32(X, I) ((unsigned int)__builtin_ia32_bextri_u32 ((unsigned int)(X), (unsigned int)(I)))
#endif
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcfill_u32 (unsigned int __X)
{
  return __X & (__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blci_u32 (unsigned int __X)
{
  return __X | ~(__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcic_u32 (unsigned int __X)
{
  return ~__X & (__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcmsk_u32 (unsigned int __X)
{
  return __X ^ (__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcs_u32 (unsigned int __X)
{
  return __X | (__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsfill_u32 (unsigned int __X)
{
  return __X | (__X - 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsic_u32 (unsigned int __X)
{
  return ~__X | (__X - 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__t1mskc_u32 (unsigned int __X)
{
  return ~__X | (__X + 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__tzmsk_u32 (unsigned int __X)
{
  return ~__X & (__X - 1);
}
#ifdef __x86_64__
#ifdef __OPTIMIZE__
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__bextri_u64 (unsigned long long __X, const unsigned int __I)
{
  return __builtin_ia32_bextri_u64 (__X, __I);
}
#else
#define __bextri_u64(X, I) ((unsigned long long)__builtin_ia32_bextri_u64 ((unsigned long long)(X), (unsigned long long)(I)))
#endif
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcfill_u64 (unsigned long long __X)
{
  return __X & (__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blci_u64 (unsigned long long __X)
{
  return __X | ~(__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcic_u64 (unsigned long long __X)
{
  return ~__X & (__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcmsk_u64 (unsigned long long __X)
{
  return __X ^ (__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blcs_u64 (unsigned long long __X)
{
  return __X | (__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsfill_u64 (unsigned long long __X)
{
  return __X | (__X - 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsic_u64 (unsigned long long __X)
{
  return ~__X | (__X - 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__t1mskc_u64 (unsigned long long __X)
{
  return ~__X | (__X + 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__tzmsk_u64 (unsigned long long __X)
{
  return ~__X & (__X - 1);
}
#endif
#ifdef __DISABLE_TBM__
#undef __DISABLE_TBM__
#pragma GCC pop_options
#endif
#endif
#endif
