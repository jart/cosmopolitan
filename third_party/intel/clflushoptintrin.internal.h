#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <clflushoptintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _CLFLUSHOPTINTRIN_H_INCLUDED
#define _CLFLUSHOPTINTRIN_H_INCLUDED
#ifndef __CLFLUSHOPT__
#pragma GCC push_options
#pragma GCC target("clflushopt")
#define __DISABLE_CLFLUSHOPT__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_clflushopt (void *__A)
{
  __builtin_ia32_clflushopt (__A);
}
#ifdef __DISABLE_CLFLUSHOPT__
#undef __DISABLE_CLFLUSHOPT__
#pragma GCC pop_options
#endif
#endif
#endif
