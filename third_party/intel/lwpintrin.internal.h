#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <lwpintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _LWPINTRIN_H_INCLUDED
#define _LWPINTRIN_H_INCLUDED
#ifndef __LWP__
#pragma GCC push_options
#pragma GCC target("lwp")
#define __DISABLE_LWP__
#endif
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__llwpcb (void *__pcbAddress)
{
  __builtin_ia32_llwpcb (__pcbAddress);
}
extern __inline void * __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__slwpcb (void)
{
  return __builtin_ia32_slwpcb ();
}
#ifdef __OPTIMIZE__
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lwpval32 (unsigned int __data2, unsigned int __data1, unsigned int __flags)
{
  __builtin_ia32_lwpval32 (__data2, __data1, __flags);
}
#ifdef __x86_64__
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lwpval64 (unsigned long long __data2, unsigned int __data1,
     unsigned int __flags)
{
  __builtin_ia32_lwpval64 (__data2, __data1, __flags);
}
#endif
#else
#define __lwpval32(D2, D1, F) (__builtin_ia32_lwpval32 ((unsigned int) (D2), (unsigned int) (D1), (unsigned int) (F)))
#ifdef __x86_64__
#define __lwpval64(D2, D1, F) (__builtin_ia32_lwpval64 ((unsigned long long) (D2), (unsigned int) (D1), (unsigned int) (F)))
#endif
#endif
#ifdef __OPTIMIZE__
extern __inline unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lwpins32 (unsigned int __data2, unsigned int __data1, unsigned int __flags)
{
  return __builtin_ia32_lwpins32 (__data2, __data1, __flags);
}
#ifdef __x86_64__
extern __inline unsigned char __attribute__((__gnu_inline__, __always_inline__, __artificial__))
__lwpins64 (unsigned long long __data2, unsigned int __data1,
     unsigned int __flags)
{
  return __builtin_ia32_lwpins64 (__data2, __data1, __flags);
}
#endif
#else
#define __lwpins32(D2, D1, F) (__builtin_ia32_lwpins32 ((unsigned int) (D2), (unsigned int) (D1), (unsigned int) (F)))
#ifdef __x86_64__
#define __lwpins64(D2, D1, F) (__builtin_ia32_lwpins64 ((unsigned long long) (D2), (unsigned int) (D1), (unsigned int) (F)))
#endif
#endif
#ifdef __DISABLE_LWP__
#undef __DISABLE_LWP__
#pragma GCC pop_options
#endif
#endif
#endif
