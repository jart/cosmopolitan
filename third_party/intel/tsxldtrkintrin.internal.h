#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <tsxldtrkintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _TSXLDTRKINTRIN_H_INCLUDED
#define _TSXLDTRKINTRIN_H_INCLUDED
#if !defined(__TSXLDTRK__)
#pragma GCC push_options
#pragma GCC target("tsxldtrk")
#define __DISABLE_TSXLDTRK__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xsusldtrk (void)
{
  __builtin_ia32_xsusldtrk ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_xresldtrk (void)
{
  __builtin_ia32_xresldtrk ();
}
#ifdef __DISABLE_TSXLDTRK__
#undef __DISABLE_TSXLDTRK__
#pragma GCC pop_options
#endif
#endif
#endif
