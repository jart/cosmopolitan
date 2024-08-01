#if defined(__x86_64__) && !(__ASSEMBLER__ + __LINKER__ + 0)
#if !defined _IMMINTRIN_H_INCLUDED
# error "Never use <avx5124fmapsintrin.h> directly; include <x86intrin.h> instead."
#endif
#ifndef _AVX5124FMAPSINTRIN_H_INCLUDED
#define _AVX5124FMAPSINTRIN_H_INCLUDED
#ifndef __AVX5124FMAPS__
#pragma GCC push_options
#pragma GCC target("avx5124fmaps,evex512")
#define __DISABLE_AVX5124FMAPS__
#endif
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_4fmadd_ps (__m512 __A, __m512 __B, __m512 __C,
    __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fmaddps ((__v16sf) __B,
        (__v16sf) __C,
        (__v16sf) __D,
        (__v16sf) __E,
        (__v16sf) __A,
        (const __v4sf *) __F);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_4fmadd_ps (__m512 __A, __mmask16 __U, __m512 __B,
         __m512 __C, __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fmaddps_mask ((__v16sf) __B,
      (__v16sf) __C,
      (__v16sf) __D,
      (__v16sf) __E,
      (__v16sf) __A,
      (const __v4sf *) __F,
      (__v16sf) __A,
      (__mmask16) __U);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_4fmadd_ps (__mmask16 __U,
   __m512 __A, __m512 __B, __m512 __C,
   __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fmaddps_mask ((__v16sf) __B,
      (__v16sf) __C,
      (__v16sf) __D,
      (__v16sf) __E,
      (__v16sf) __A,
      (const __v4sf *) __F,
      (__v16sf) _mm512_setzero_ps (),
      (__mmask16) __U);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_4fmadd_ss (__m128 __A, __m128 __B, __m128 __C,
        __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fmaddss ((__v4sf) __B,
        (__v4sf) __C,
        (__v4sf) __D,
        (__v4sf) __E,
        (__v4sf) __A,
        (const __v4sf *) __F);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_4fmadd_ss (__m128 __A, __mmask8 __U, __m128 __B, __m128 __C,
      __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fmaddss_mask ((__v4sf) __B,
      (__v4sf) __C,
      (__v4sf) __D,
      (__v4sf) __E,
      (__v4sf) __A,
      (const __v4sf *) __F,
      (__v4sf) __A,
      (__mmask8) __U);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_4fmadd_ss (__mmask8 __U, __m128 __A, __m128 __B, __m128 __C,
       __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fmaddss_mask ((__v4sf) __B,
      (__v4sf) __C,
      (__v4sf) __D,
      (__v4sf) __E,
      (__v4sf) __A,
      (const __v4sf *) __F,
      (__v4sf) _mm_setzero_ps (),
      (__mmask8) __U);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_4fnmadd_ps (__m512 __A, __m512 __B, __m512 __C,
     __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fnmaddps ((__v16sf) __B,
         (__v16sf) __C,
         (__v16sf) __D,
         (__v16sf) __E,
         (__v16sf) __A,
         (const __v4sf *) __F);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_mask_4fnmadd_ps (__m512 __A, __mmask16 __U, __m512 __B,
   __m512 __C, __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fnmaddps_mask ((__v16sf) __B,
       (__v16sf) __C,
       (__v16sf) __D,
       (__v16sf) __E,
       (__v16sf) __A,
       (const __v4sf *) __F,
       (__v16sf) __A,
       (__mmask16) __U);
}
extern __inline __m512
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm512_maskz_4fnmadd_ps (__mmask16 __U,
    __m512 __A, __m512 __B, __m512 __C,
    __m512 __D, __m512 __E, __m128 *__F)
{
  return (__m512) __builtin_ia32_4fnmaddps_mask ((__v16sf) __B,
       (__v16sf) __C,
       (__v16sf) __D,
       (__v16sf) __E,
       (__v16sf) __A,
       (const __v4sf *) __F,
       (__v16sf) _mm512_setzero_ps (),
       (__mmask16) __U);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_4fnmadd_ss (__m128 __A, __m128 __B, __m128 __C,
  __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fnmaddss ((__v4sf) __B,
         (__v4sf) __C,
         (__v4sf) __D,
         (__v4sf) __E,
         (__v4sf) __A,
         (const __v4sf *) __F);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_mask_4fnmadd_ss (__m128 __A, __mmask8 __U, __m128 __B, __m128 __C,
       __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fnmaddss_mask ((__v4sf) __B,
       (__v4sf) __C,
       (__v4sf) __D,
       (__v4sf) __E,
       (__v4sf) __A,
       (const __v4sf *) __F,
       (__v4sf) __A,
       (__mmask8) __U);
}
extern __inline __m128
__attribute__((__gnu_inline__, __always_inline__, __artificial__))
_mm_maskz_4fnmadd_ss (__mmask8 __U, __m128 __A, __m128 __B, __m128 __C,
        __m128 __D, __m128 __E, __m128 *__F)
{
  return (__m128) __builtin_ia32_4fnmaddss_mask ((__v4sf) __B,
       (__v4sf) __C,
       (__v4sf) __D,
       (__v4sf) __E,
       (__v4sf) __A,
       (const __v4sf *) __F,
       (__v4sf) _mm_setzero_ps (),
       (__mmask8) __U);
}
#ifdef __DISABLE_AVX5124FMAPS__
#undef __DISABLE_AVX5124FMAPS__
#pragma GCC pop_options
#endif
#endif
#endif
