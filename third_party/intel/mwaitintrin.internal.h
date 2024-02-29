#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _MWAITINTRIN_H_INCLUDED
#define _MWAITINTRIN_H_INCLUDED
#ifndef __MWAIT__
#pragma GCC push_options
#pragma GCC target("mwait")
#define __DISABLE_MWAIT__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_monitor (void const * __P, unsigned int __E, unsigned int __H)
{
  __builtin_ia32_monitor (__P, __E, __H);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mwait (unsigned int __E, unsigned int __H)
{
  __builtin_ia32_mwait (__E, __H);
}
#ifdef __DISABLE_MWAIT__
#undef __DISABLE_MWAIT__
#pragma GCC pop_options
#endif
#endif
#endif
