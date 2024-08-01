#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef _X86GPRINTRIN_H_INCLUDED
#error "Never use <cmpccxaddintrin.h> directly; include <x86gprintrin.h> instead."
#endif
#ifndef _CMPCCXADDINTRIN_H_INCLUDED
#define _CMPCCXADDINTRIN_H_INCLUDED
#ifdef __x86_64__
#ifndef __CMPCCXADD__
#pragma GCC push_options
#pragma GCC target("cmpccxadd")
#define __DISABLE_CMPCCXADD__
#endif
typedef enum {
    _CMPCCX_O,
    _CMPCCX_NO,
    _CMPCCX_B,
    _CMPCCX_NB,
    _CMPCCX_Z,
    _CMPCCX_NZ,
    _CMPCCX_BE,
    _CMPCCX_NBE,
    _CMPCCX_S,
    _CMPCCX_NS,
    _CMPCCX_P,
    _CMPCCX_NP,
    _CMPCCX_L,
    _CMPCCX_NL,
    _CMPCCX_LE,
    _CMPCCX_NLE,
} _CMPCCX_ENUM;
#ifdef __OPTIMIZE__
extern __inline int
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_cmpccxadd_epi32 (int *__A, int __B, int __C, const _CMPCCX_ENUM __D)
{
  return __builtin_ia32_cmpccxadd (__A, __B, __C, __D);
}
extern __inline long long
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_cmpccxadd_epi64 (long long *__A, long long __B, long long __C,
     const _CMPCCX_ENUM __D)
{
  return __builtin_ia32_cmpccxadd64 (__A, __B, __C, __D);
}
#else
#define _cmpccxadd_epi32(A,B,C,D) __builtin_ia32_cmpccxadd ((int *) (A), (int) (B), (int) (C), (_CMPCCX_ENUM) (D))
#define _cmpccxadd_epi64(A,B,C,D) __builtin_ia32_cmpccxadd64 ((long long *) (A), (long long) (B), (long long) (C), (_CMPCCX_ENUM) (D))
#endif
#ifdef __DISABLE_CMPCCXADD__
#undef __DISABLE_CMPCCXADD__
#pragma GCC pop_options
#endif
#endif
#endif
#endif
