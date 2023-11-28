#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _CLZEROINTRIN_H_INCLUDED
#define _CLZEROINTRIN_H_INCLUDED
#ifndef __CLZERO__
#pragma GCC push_options
#pragma GCC target("clzero")
#define __DISABLE_CLZERO__
#endif
extern __inline void __attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clzero (void * __I)
{
  __builtin_ia32_clzero (__I);
}
#ifdef __DISABLE_CLZERO__
#undef __DISABLE_CLZERO__
#pragma GCC pop_options
#endif
#endif
#endif
