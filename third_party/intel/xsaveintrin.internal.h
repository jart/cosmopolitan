#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <xsaveintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _XSAVEINTRIN_H_INCLUDED
#define _XSAVEINTRIN_H_INCLUDED
#ifndef __XSAVE__
#pragma GCC push_options
#pragma GCC target("xsave")
#define __DISABLE_XSAVE__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsave (void *__P, long long __M)
{
  __builtin_ia32_xsave (__P, __M);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xrstor (void *__P, long long __M)
{
  __builtin_ia32_xrstor (__P, __M);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsetbv (unsigned int __A, long long __V)
{
  __builtin_ia32_xsetbv (__A, __V);
}
extern __inline long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xgetbv (unsigned int __A)
{
  return __builtin_ia32_xgetbv (__A);
}
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsave64 (void *__P, long long __M)
{
  __builtin_ia32_xsave64 (__P, __M);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xrstor64 (void *__P, long long __M)
{
  __builtin_ia32_xrstor64 (__P, __M);
}
#endif
#ifdef __DISABLE_XSAVE__
#undef __DISABLE_XSAVE__
#pragma GCC pop_options
#endif
#endif
#endif
