#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <xtestintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _XTESTINTRIN_H_INCLUDED
#define _XTESTINTRIN_H_INCLUDED
#ifndef __RTM__
#pragma GCC push_options
#pragma GCC target("rtm")
#define __DISABLE_RTM__
#endif
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xtest (void)
{
  return __builtin_ia32_xtest ();
}
#ifdef __DISABLE_RTM__
#undef __DISABLE_RTM__
#pragma GCC pop_options
#endif
#endif
#endif
