#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _X86GPRINTRIN_H_INCLUDED
# error "Never use <prfchiintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _PRFCHIINTRIN_H_INCLUDED
#define _PRFCHIINTRIN_H_INCLUDED
#ifdef __x86_64__
#ifndef __PREFETCHI__
#pragma GCC push_options
#pragma GCC target("prefetchi")
#define __DISABLE_PREFETCHI__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_prefetchit0 (void* __P)
{
  __builtin_ia32_prefetchi (__P, 3);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_m_prefetchit1 (void* __P)
{
  __builtin_ia32_prefetchi (__P, 2);
}
#ifdef __DISABLE_PREFETCHI__
#undef __DISABLE_PREFETCHI__
#pragma GCC pop_options
#endif
#endif
#endif
#endif
