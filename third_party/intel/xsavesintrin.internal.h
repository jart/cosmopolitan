#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <xsavesintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _XSAVESINTRIN_H_INCLUDED
#define _XSAVESINTRIN_H_INCLUDED
#ifndef __XSAVES__
#pragma GCC push_options
#pragma GCC target("xsaves")
#define __DISABLE_XSAVES__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsaves (void *__P, long long __M)
{
  __builtin_ia32_xsaves (__P, __M);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xrstors (void *__P, long long __M)
{
  __builtin_ia32_xrstors (__P, __M);
}
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xrstors64 (void *__P, long long __M)
{
  __builtin_ia32_xrstors64 (__P, __M);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsaves64 (void *__P, long long __M)
{
  __builtin_ia32_xsaves64 (__P, __M);
}
#endif
#ifdef __DISABLE_XSAVES__
#undef __DISABLE_XSAVES__
#pragma GCC pop_options
#endif
#endif
#endif
