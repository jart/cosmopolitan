#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <fxsrintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _FXSRINTRIN_H_INCLUDED
#define _FXSRINTRIN_H_INCLUDED
#ifndef __FXSR__
#pragma GCC push_options
#pragma GCC target("fxsr")
#define __DISABLE_FXSR__
#endif
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_fxsave (void *__P)
{
  __builtin_ia32_fxsave (__P);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_fxrstor (void *__P)
{
  __builtin_ia32_fxrstor (__P);
}
#ifdef __x86_64__
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_fxsave64 (void *__P)
{
  __builtin_ia32_fxsave64 (__P);
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_fxrstor64 (void *__P)
{
  __builtin_ia32_fxrstor64 (__P);
}
#endif
#ifdef __DISABLE_FXSR__
#undef __DISABLE_FXSR__
#pragma GCC pop_options
#endif
#endif
#endif
