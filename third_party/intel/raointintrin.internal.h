#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
#error "Never use <raointintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef __RAOINTINTRIN_H_INCLUDED
#define __RAOINTINTRIN_H_INCLUDED
#ifndef __RAOINT__
#pragma GCC push_options
#pragma GCC target("raoint")
#define __DISABLE_RAOINT__
#endif
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aadd_i32 (int *__A, int __B)
{
  __builtin_ia32_aadd32 ((int *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aand_i32 (int *__A, int __B)
{
  __builtin_ia32_aand32 ((int *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aor_i32 (int *__A, int __B)
{
  __builtin_ia32_aor32 ((int *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_axor_i32 (int *__A, int __B)
{
  __builtin_ia32_axor32 ((int *)__A, __B);
}
#ifdef __x86_64__
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aadd_i64 (long long *__A, long long __B)
{
  __builtin_ia32_aadd64 ((long long *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aand_i64 (long long *__A, long long __B)
{
  __builtin_ia32_aand64 ((long long *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_aor_i64 (long long *__A, long long __B)
{
  __builtin_ia32_aor64 ((long long *)__A, __B);
}
extern __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_axor_i64 (long long *__A, long long __B)
{
  __builtin_ia32_axor64 ((long long *)__A, __B);
}
#endif
#ifdef __DISABLE_RAOINT__
#undef __DISABLE_RAOINT__
#pragma GCC pop_options
#endif
#endif
#endif
