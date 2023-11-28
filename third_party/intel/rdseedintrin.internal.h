#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <rdseedintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _RDSEEDINTRIN_H_INCLUDED
#define _RDSEEDINTRIN_H_INCLUDED
#ifndef __RDSEED__
#pragma GCC push_options
#pragma GCC target("rdseed")
#define __DISABLE_RDSEED__
#endif
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdseed16_step (unsigned short *__p)
{
  return __builtin_ia32_rdseed_hi_step (__p);
}
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdseed32_step (unsigned int *__p)
{
  return __builtin_ia32_rdseed_si_step (__p);
}
#ifdef __x86_64__
extern __inline int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_rdseed64_step (unsigned long long *__p)
{
  return __builtin_ia32_rdseed_di_step (__p);
}
#endif
#ifdef __DISABLE_RDSEED__
#undef __DISABLE_RDSEED__
#pragma GCC pop_options
#endif
#endif
#endif
