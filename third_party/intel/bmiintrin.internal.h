#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <bmiintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _BMIINTRIN_H_INCLUDED
#define _BMIINTRIN_H_INCLUDED
#ifndef __BMI__
#pragma GCC push_options
#pragma GCC target("bmi")
#define __DISABLE_BMI__
#endif
extern __inline unsigned short __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__tzcnt_u16 (unsigned short __X)
{
  return __builtin_ia32_tzcnt_u16 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__andn_u32 (unsigned int __X, unsigned int __Y)
{
  return ~__X & __Y;
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__bextr_u32 (unsigned int __X, unsigned int __Y)
{
  return __builtin_ia32_bextr_u32 (__X, __Y);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_bextr_u32 (unsigned int __X, unsigned int __Y, unsigned __Z)
{
  return __builtin_ia32_bextr_u32 (__X, ((__Y & 0xff) | ((__Z & 0xff) << 8)));
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsi_u32 (unsigned int __X)
{
  return __X & -__X;
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsi_u32 (unsigned int __X)
{
  return __blsi_u32 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsmsk_u32 (unsigned int __X)
{
  return __X ^ (__X - 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsmsk_u32 (unsigned int __X)
{
  return __blsmsk_u32 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsr_u32 (unsigned int __X)
{
  return __X & (__X - 1);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsr_u32 (unsigned int __X)
{
  return __blsr_u32 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__tzcnt_u32 (unsigned int __X)
{
  return __builtin_ia32_tzcnt_u32 (__X);
}
extern __inline unsigned int __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_tzcnt_u32 (unsigned int __X)
{
  return __builtin_ia32_tzcnt_u32 (__X);
}
#ifdef __x86_64__
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__andn_u64 (unsigned long long __X, unsigned long long __Y)
{
  return ~__X & __Y;
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__bextr_u64 (unsigned long long __X, unsigned long long __Y)
{
  return __builtin_ia32_bextr_u64 (__X, __Y);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_bextr_u64 (unsigned long long __X, unsigned int __Y, unsigned int __Z)
{
  return __builtin_ia32_bextr_u64 (__X, ((__Y & 0xff) | ((__Z & 0xff) << 8)));
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsi_u64 (unsigned long long __X)
{
  return __X & -__X;
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsi_u64 (unsigned long long __X)
{
  return __blsi_u64 (__X);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsmsk_u64 (unsigned long long __X)
{
  return __X ^ (__X - 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsmsk_u64 (unsigned long long __X)
{
  return __blsmsk_u64 (__X);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__blsr_u64 (unsigned long long __X)
{
  return __X & (__X - 1);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_blsr_u64 (unsigned long long __X)
{
  return __blsr_u64 (__X);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__tzcnt_u64 (unsigned long long __X)
{
  return __builtin_ia32_tzcnt_u64 (__X);
}
extern __inline unsigned long long __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_tzcnt_u64 (unsigned long long __X)
{
  return __builtin_ia32_tzcnt_u64 (__X);
}
#endif
#ifdef __DISABLE_BMI__
#undef __DISABLE_BMI__
#pragma GCC pop_options
#endif
#endif
#endif
