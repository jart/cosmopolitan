#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _X86GPRINTRIN_H_INCLUDED
#error "Never use <usermsrintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _USER_MSRINTRIN_H_INCLUDED
#define _USER_MSRINTRIN_H_INCLUDED
#ifdef __x86_64__
#ifndef __USER_MSR__
#pragma GCC push_options
#pragma GCC target("usermsr")
#define __DISABLE_USER_MSR__
#endif
extern __inline unsigned long long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_urdmsr (unsigned long long __A)
{
  return (unsigned long long) __builtin_ia32_urdmsr (__A);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_uwrmsr (unsigned long long __A, unsigned long long __B)
{
  __builtin_ia32_uwrmsr (__A, __B);
}
#ifdef __DISABLE_USER_MSR__
#undef __DISABLE_USER_MSR__
#pragma GCC pop_options
#endif
#endif
#endif
#endif
