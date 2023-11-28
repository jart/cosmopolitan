#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <pconfigintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _PCONFIGINTRIN_H_INCLUDED
#define _PCONFIGINTRIN_H_INCLUDED
#ifndef __PCONFIG__
#pragma GCC push_options
#pragma GCC target("pconfig")
#define __DISABLE_PCONFIG__
#endif
#define __pconfig_b(leaf, b, retval) __asm__ __volatile__ ("pconfig\n\t" : "=a" (retval) : "a" (leaf), "b" (b) : "cc")
#define __pconfig_generic(leaf, b, c, d, retval) __asm__ __volatile__ ("pconfig\n\t" : "=a" (retval), "=b" (b), "=c" (c), "=d" (d) : "a" (leaf), "b" (b), "c" (c), "d" (d) : "cc")
extern __inline unsigned int
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_pconfig_u32 (const unsigned int __L, size_t __D[])
{
  enum __pconfig_type
  {
    __PCONFIG_KEY_PROGRAM = 0x01,
  };
  unsigned int __R = 0;
  if (!__builtin_constant_p (__L))
    __pconfig_generic (__L, __D[0], __D[1], __D[2], __R);
  else switch (__L)
    {
    case __PCONFIG_KEY_PROGRAM:
      __pconfig_b (__L, __D[0], __R);
      break;
    default:
      __pconfig_generic (__L, __D[0], __D[1], __D[2], __R);
    }
  return __R;
}
#ifdef __DISABLE_PCONFIG__
#undef __DISABLE_PCONFIG__
#pragma GCC pop_options
#endif
#endif
#endif
