#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
# error "Never use <avx512vpopcntdqintrin.h> directly; include <x86intrin.h> instead."
#endif
#ifndef _AVX512VPOPCNTDQINTRIN_H_INCLUDED
#define _AVX512VPOPCNTDQINTRIN_H_INCLUDED
#ifndef __AVX512VPOPCNTDQ__
#pragma GCC push_options
#pragma GCC target("avx512vpopcntdq")
#define __DISABLE_AVX512VPOPCNTDQ__
#endif
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_popcnt_epi32 (__m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountd_v16si ((__v16si) __A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_popcnt_epi32 (__m512i __W, __mmask16 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountd_v16si_mask ((__v16si) __A,
        (__v16si) __W,
        (__mmask16) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_popcnt_epi32 (__mmask16 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountd_v16si_mask ((__v16si) __A,
        (__v16si)
        _mm512_setzero_si512 (),
        (__mmask16) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_popcnt_epi64 (__m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountq_v8di ((__v8di) __A);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_popcnt_epi64 (__m512i __W, __mmask8 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountq_v8di_mask ((__v8di) __A,
       (__v8di) __W,
       (__mmask8) __U);
}
extern __inline __m512i
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_popcnt_epi64 (__mmask8 __U, __m512i __A)
{
  return (__m512i) __builtin_ia32_vpopcountq_v8di_mask ((__v8di) __A,
       (__v8di)
       _mm512_setzero_si512 (),
       (__mmask8) __U);
}
#ifdef __DISABLE_AVX512VPOPCNTDQ__
#undef __DISABLE_AVX512VPOPCNTDQ__
#pragma GCC pop_options
#endif
#endif
#endif
