#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <pkuintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _PKUINTRIN_H_INCLUDED
#define _PKUINTRIN_H_INCLUDED
#ifndef __PKU__
#pragma GCC push_options
#pragma GCC target("pku")
#define __DISABLE_PKU__
#endif
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdpkru_u32 (void)
{
  return __builtin_ia32_rdpkru ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_wrpkru (unsigned int __key)
{
  __builtin_ia32_wrpkru (__key);
}
#ifdef __DISABLE_PKU__
#undef __DISABLE_PKU__
#pragma GCC pop_options
#endif
#endif
#endif
