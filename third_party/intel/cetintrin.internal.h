#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <cetintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _CETINTRIN_H_INCLUDED
#define _CETINTRIN_H_INCLUDED
#ifndef __SHSTK__
#pragma GCC push_options
#pragma GCC target ("shstk")
#define __DISABLE_SHSTK__
#endif
#ifdef __x86_64__
extern __inline unsigned long long
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_get_ssp (void)
{
  return __builtin_ia32_rdsspq ();
}
#else
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_get_ssp (void)
{
  return __builtin_ia32_rdsspd ();
}
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_inc_ssp (unsigned int __B)
{
#ifdef __x86_64__
  __builtin_ia32_incsspq ((unsigned long long) __B);
#else
  __builtin_ia32_incsspd (__B);
#endif
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_saveprevssp (void)
{
  __builtin_ia32_saveprevssp ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rstorssp (void *__B)
{
  __builtin_ia32_rstorssp (__B);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wrssd (unsigned int __B, void *__C)
{
  __builtin_ia32_wrssd (__B, __C);
}
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wrssq (unsigned long long __B, void *__C)
{
  __builtin_ia32_wrssq (__B, __C);
}
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wrussd (unsigned int __B, void *__C)
{
  __builtin_ia32_wrussd (__B, __C);
}
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wrussq (unsigned long long __B, void *__C)
{
  __builtin_ia32_wrussq (__B, __C);
}
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_setssbsy (void)
{
  __builtin_ia32_setssbsy ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_clrssbsy (void *__B)
{
  __builtin_ia32_clrssbsy (__B);
}
#ifdef __DISABLE_SHSTK__
#undef __DISABLE_SHSTK__
#pragma GCC pop_options
#endif
#endif
#endif
