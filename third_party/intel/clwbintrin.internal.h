#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <clwbintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _CLWBINTRIN_H_INCLUDED
#define _CLWBINTRIN_H_INCLUDED
#ifndef __CLWB__
#pragma GCC push_options
#pragma GCC target("clwb")
#define __DISABLE_CLWB__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clwb (void *__A)
{
  __builtin_ia32_clwb (__A);
}
#ifdef __DISABLE_CLWB__
#undef __DISABLE_CLWB__
#pragma GCC pop_options
#endif
#endif
#endif
