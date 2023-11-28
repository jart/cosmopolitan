#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
# error "Never use <uintrintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _UINTRNTRIN_H_INCLUDED
#define _UINTRNTRIN_H_INCLUDED
#ifdef __x86_64__
#ifndef __UINTR__
#pragma GCC push_options
#pragma GCC target ("uintr")
#define __DISABLE_UINTR__
#endif
struct __uintr_frame
{
  unsigned long long rip;
  unsigned long long rflags;
  unsigned long long rsp;
};
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_clui (void)
{
  __builtin_ia32_clui ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_stui (void)
{
  __builtin_ia32_stui ();
}
extern __inline void
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_senduipi (unsigned long long __R)
{
  __builtin_ia32_senduipi (__R);
}
extern __inline unsigned char
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_testui (void)
{
  return __builtin_ia32_testui ();
}
#ifdef __DISABLE_UINTR__
#undef __DISABLE_UINTR__
#pragma GCC pop_options
#endif
#endif
#endif
#endif
